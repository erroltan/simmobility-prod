/* Copyright Singapore-MIT Alliance for Research and Technology */

#pragma once

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <stdexcept>
#include <sstream>

#include "GenConfig.h"
#include "util/LangHelpers.hpp"
#include "metrics/Frame.hpp"
#include "buffering/BufferedDataManager.hpp"
#include "entities/misc/TripChain.hpp"

namespace sim_mob
{

class Worker;
class WorkGroup;

#ifndef SIMMOB_DISABLE_MPI
class PartitionManager;
#endif



/**
 * Base class of all agents and other "decision-making" entities.
 * \author Seth N. Hetu
 * \author LIM Fung Chai
 * \author Xu Yan
 */
class Entity {
public:
	///Construct an entity with an immutable ID
	explicit Entity(unsigned int id);
	virtual ~Entity();


	/**
	 * Value returned from update() and used to signal changes to the parent Worker class.
	 */
	struct UpdateStatus {
		///Return status of the update() function.
		enum RET_STATUS {
			RS_CONTINUE,     ///< Continue processing next time tick.
			RS_DONE          ///< Done; remove from the simulation.
		};

		///Helper variable: represents a simple "Done" state with no changed variables.
		static const UpdateStatus Continue;
		static const UpdateStatus Done;

		///Construction requires at least the return code
		///More complex construction takes two vectors of variables and extracts which ones are old/new.
		explicit UpdateStatus(RET_STATUS status, const std::vector<BufferedBase*>& currTickVals=std::vector<BufferedBase*>(), const std::vector<BufferedBase*>& nextTickVals=std::vector<BufferedBase*>());

		///The return status
		RET_STATUS status;

		///BufferedBase* items to remove from the parent worker after this time tick.
		std::set<BufferedBase*> toRemove;

		///BufferedBase* items to add to the parent worker for the next time tick.
		std::set<BufferedBase*> toAdd;
	};


	/**
	 * Update function. This will be called each time tick (at the entity type's granularity; see
	 * simpleconf.hpp for more information), and will update the entity's state. During this phase,
	 * the entity may call any Buffered data type's "get" method, but may only "set" its own
	 * Buffered data. Flip is called after each update phase.
	 *
	 * Return value is specified by the UpdateStatus class, which allows for an Entity to "Continue"
	 *  processing, be considered "Done" processing, and optionally to manually register and deregister
	 *  several buffered types.
	 *
	 * If this function returns Done, the Entity should be considered finished with its work
	 *   and may be removed from the Simulation and deleted. Buffered types should all be considered
	 *   moot at this point.
	 */
	virtual UpdateStatus update(frame_t frameNumber) = 0;

	virtual Link* getCurrLink() = 0;
	virtual void setCurrLink(Link* link)= 0;

	virtual void setStartTime(unsigned int value) { startTime = value; }
	virtual unsigned int getStartTime() const { return startTime; }

	virtual void setActivities(std::vector<TripActivity*> value) { activities = value; }
	virtual std::vector<TripActivity*> getActivities() const { return activities; }

	virtual void setNextActivity(TripActivity* value) { nextActivity = value; }
	virtual TripActivity* getNextActivity() const { return nextActivity; }


protected:
	/**
	 * Build the list of Buffered<> types this entity subscribes to. Any subclass of
	 * Entity should override this method. The first thing to do is call the immediate base
	 * class's buildSubscriptionList() method, which will construct the subscription list up to
	 * this point. Then, any Buffered types in the current class should be added to subscriptionList_cached.
	 */
	virtual void buildSubscriptionList(std::vector<sim_mob::BufferedBase*>& subsList) = 0;
	//std::vector<sim_mob::BufferedBase*> subscriptionList_cached;

protected:
	unsigned int id;
	//bool isSubscriptionListBuilt;

	//When (in ms) does this Entity start?
	unsigned int startTime;

	std::vector<TripActivity*> activities;

	TripActivity* nextActivity;

	Link* currLink;

	//temp setting, I set Worker group to be friend class, but worker group still can not get it;
public:
	///Who is currently managing this Entity?
	Worker* currWorker;

	//Only the WorkGroup can retrieve/set the currWorker flag. I'm doing this through a
	// friend class, since get/set methods have the potential for abuse (currWorker can't be declared const*)
	friend class Worker;
	friend class WorkerGroup;

//Some near-trivial functions
public:
	///Retrieve this Entity's id. An entity's ID must be unique within a given machine.
	unsigned int getId() const { return id; }

	///Retrieve this Entity's global id. An entity's Global ID must be unique across machine boundaries,
	///  and is currently made by concatenating the result of getId() to the current machine's IP address.
	///
	///\note
	///This function currently assumes one machine (localhost); it should be modified once our MPI code is
	//   finished.
	std::string getGlobalId() const;


	/**
	 * Returns a list of pointers to each Buffered<> type that this entity managed.
	 * Entity sub-classes should override buildSubscriptionList() to help with
	 * this process.
	 */
	std::vector<BufferedBase*> getSubscriptionList();

	bool isFake;
	bool receiveTheFakeEntityAgain;

	friend class PartitionManager;

};


}
