/* Copyright Singapore-MIT Alliance for Research and Technology */

#include "WorkGroup.hpp"

//For debugging
#include <stdexcept>
#include <boost/thread.hpp>
#include "util/OutputUtil.hpp"

#include "conf/simpleconf.hpp"

#include "entities/Agent.hpp"

using std::map;
using std::vector;
using boost::barrier;
using boost::function;

using namespace sim_mob;




/**
 * Template function must be defined in the same translational unit as it is declared.
 */

void sim_mob::WorkGroup::initWorkers(Worker::ActionFunction* action, EntityLoadParams* loader)
{
	this->loader = loader;

	for (size_t i=0; i<total_size; i++) {
		workers.push_back(new Worker(this, shared_barr, external_barr, action, endTick, tickStep, auraManagerActive));
	}
}



//////////////////////////////////////////////
// These also need the temoplate parameter,
// but don't actually do anything with it.
//////////////////////////////////////////////



sim_mob::WorkGroup::WorkGroup(size_t size, unsigned int endTick, unsigned int tickStep, bool auraManagerActive) :
		shared_barr(size+1), external_barr(size+1), nextWorkerID(0), endTick(endTick), tickStep(tickStep), total_size(size), auraManagerActive(auraManagerActive),
		nextTimeTickToStage(0), loader(nullptr)
{
}



sim_mob::WorkGroup::~WorkGroup()
{
	for (size_t i=0; i<workers.size(); i++) {
		workers[i]->join();  //NOTE: If we don't join all Workers, we get threading exceptions.
		delete workers[i];
	}
}



void sim_mob::WorkGroup::startAll()
{
	//Stage any Agents that will become active within the first time tick (in time for the next tick)
	nextTimeTickToStage = 0;

	//Start all workers
	tickOffset = tickStep;
	for (size_t i=0; i<workers.size(); i++) {
		workers[i]->start();
	}
}


#ifndef SIMMOB_DISABLE_DYNAMIC_DISPATCH
void sim_mob::WorkGroup::scheduleEntForRemoval(Entity* ag)
{
	if (!loader) {
		throw std::runtime_error("Attempting to remove an entity from a WorkGroup that doesn't allow it.");
	}

	//May be accessed by child workers; requires locking.
	boost::mutex::scoped_lock local_lock(loader->entity_dest_lock);
	entToBeRemoved.push_back(ag);
}

void sim_mob::WorkGroup::stageEntities()
{
	if (!loader) {
		return;
	}

	//Keep assigning the next entity until none are left.
	unsigned int nextTickMS = nextTimeTickToStage*ConfigParams::GetInstance().baseGranMS;
	while (!loader->pending_source.empty() && loader->pending_source.top()->getStartTime() <= nextTickMS) {
		//Remove it.
		Entity* ag = loader->pending_source.top();
		loader->pending_source.pop();

		if (sim_mob::Debug::WorkGroupSemantics) {
			std::cout <<"Staging agent ID: " <<ag->getId() <<" in time for tick: " <<nextTimeTickToStage <<"\n";
		}

		//Add it to our global list. Requires locking.
		{
			//TODO: This shouldn't actually require locking. Leaving it in here for now to be safe.
			//boost::mutex::scoped_lock local_lock(loader->entity_dest_lock);
			loader->entity_dest.push_back(ag);
		}

		//Find a worker to assign this to and send it the Entity to manage.
		assignAWorker(ag);
	}
}
#endif


void sim_mob::WorkGroup::assignAWorker(Entity* ag)
{
#ifndef SIMMOB_DISABLE_DYNAMIC_DISPATCH
	workers.at(nextWorkerID++)->scheduleForAddition(ag);
#else
	workers.at(nextWorkerID++)->scheduleEntityNow(ag);
#endif
	nextWorkerID %= workers.size();
}



size_t sim_mob::WorkGroup::size()
{
	return workers.size();
}




sim_mob::Worker* sim_mob::WorkGroup::getWorker(int id)
{
	if (id<0) {
		return nullptr;
	}
	return workers.at(id);
}



void sim_mob::WorkGroup::wait()
{
	if (--tickOffset>0) {
		return;
	}
	tickOffset = tickStep;

	//Stay in sync with the workers.
	nextTimeTickToStage += tickStep;

	shared_barr.wait();

	//Stage Agent updates based on nextTimeTickToStage
#ifndef SIMMOB_DISABLE_DYNAMIC_DISPATCH
	stageEntities();
#endif

	//Remove any Agents staged for removal.
#ifndef SIMMOB_DISABLE_DYNAMIC_DISPATCH
	if (loader) {
		for (std::vector<Entity*>::iterator it=entToBeRemoved.begin(); it!=entToBeRemoved.end(); it++) {
			//TODO: This shouldn't actually require locking. Leaving it in here for now to be safe.
			//boost::mutex::scoped_lock local_lock(loader->entity_dest_lock);
			std::vector<Entity*>::iterator it2 = std::find(loader->entity_dest.begin(), loader->entity_dest.end(), *it);
			if (it2!=loader->entity_dest.end()) {
				loader->entity_dest.erase(it2);
			}

			//Delete this entity
			delete *it;
		}
		entToBeRemoved.clear();
	}
#endif

	external_barr.wait();
}


void sim_mob::WorkGroup::removeAgentFromWorker(Entity* ag)
{
	ag->currWorker->migrateOut(*(ag));
//	ag->currWorker->remEntity(ag);
//	ag->currWorker = NULL;
//	scheduleEntForRemoval(ag);
}


void sim_mob::WorkGroup::addAgentInWorker(Entity * ag)
{
	int free_worker_id = getTheMostFreeWorkerID();
	getWorker(free_worker_id)->migrateIn(*(ag));

	//assignAWorker(ag);
}


int sim_mob::WorkGroup::getTheMostFreeWorkerID()
{
	int minimum_task = std::numeric_limits<int>::max();
	int minimum_index = 0;

	for (size_t i = 0; i < workers.size(); i++) {
		if (workers[i]->getAgentSize() < minimum_task) {
			minimum_task = workers[i]->getAgentSize();
			minimum_index = i;
		}
	}

	return minimum_index;
}



void sim_mob::WorkGroup::waitExternAgain()
{
	if (!auraManagerActive) {
		throw std::runtime_error("Aura manager must be active for waitExternAgain()");
	}
	external_barr.wait();
}



void sim_mob::WorkGroup::interrupt()
{
	for (size_t i=0; i<workers.size(); i++)
		workers[i]->interrupt();
}



