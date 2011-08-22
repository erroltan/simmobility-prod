/* Copyright Singapore-MIT Alliance for Research and Technology */

#include "Pedestrian.hpp"
#include "../../Person.hpp"
#include "../driver/Driver.hpp"
#include "../../../geospatial/Node.hpp"

using namespace sim_mob;


double Pedestrian::collisionForce = 20;
double Pedestrian::agentRadius = 0.25; //Shoulder width of a person is about 0.5 meter


sim_mob::Pedestrian::Pedestrian(Agent* parent) : Role(parent)
{
	//Check non-null parent. Perhaps references may be of use here?
	if (!parent) {
		std::cout <<"Role constructed with no parent Agent." <<std::endl;
		throw 1;
	}

	//Defaults
	//Init start positions of agents ????

	currPhase = 3; //Green phase by default
//	phaseCounter = 0;
//	sig.initializeSignal();
	currentStage=0;
	setGoal(currentStage);
	startToCross=false;

	//Set random seed
	srand(parent->getId());

	//Set default speed in the range of 0.9m/s to 1.4m/s
	speed = 0.9+(double(rand()%5))/10;

	//TEMP: Needed to speed things up a little:
//	speed *= 5;

	xVel = 0;
	yVel = 0;

	xCollisionVector = 0;
	yCollisionVector = 0;

}

//Main update functionality
void sim_mob::Pedestrian::update(frame_t frameNumber)
{
	//Set the initial goal of agent (next intersection)
//	if(!isGoalSet){
//		setGoal(0);
//		isGoalSet = true;
//	}

	//update signal information
//	updatePedestrianSignal();

//	checkForCollisions();

	//Check if the agent has reached the destination
	if(isDestReached()){

		if(!parent->isToBeRemoved()){
			//Output (temp)
			{
				boost::mutex::scoped_lock local_lock(BufferedBase::global_mutex);
				std::cout <<"(Agent " <<parent->getId() <<" has reached the destination)" <<std::endl;
			}
			parent->setToBeRemoved(true);
		}
		return;
	}

	if(isGoalReached()){
		currentStage++;
		setGoal(currentStage); //Set next goal
	}

	if(currentStage==0||currentStage==2){
		updateVelocity(1);
		updatePosition();
	}
	else if(currentStage==1){

		//Check whether to start to cross or not
		if(!startToCross){
			if(currPhase == 3)  //Green phase
				startToCross = true;
			else if(currPhase == 1){ //Red phase
				if(checkGapAcceptance()==true)
					startToCross=true;
			}
		}

		if(startToCross){
			if(currPhase==3)
				updateVelocity(1);
			else if (currPhase ==1)
				updateVelocity(2);
			updatePosition();
		}
	}

//	//Continue checking if the goal has not been reached.
//	if(reachStartOfCrossing()) {
//		if(currPhase == 3){ //Green phase
//			updateVelocity(1);
//			updatePosition();
//		} else if (currPhase == 1) { //Red phase
//			//Waiting, do nothing now
//			//Output (temp)
//			checkGapAcceptance();
//			{
//				boost::mutex::scoped_lock local_lock(BufferedBase::global_mutex);
//				std::cout <<"(Agent " <<parent->getId() <<" is waiting at crossing at frame "<<frameNumber<<")" <<std::endl;
//			}
//		}
//	} else {
//		if(currPhase==1&&onCrossing())
//			updateVelocity(2);
//		else
//			updateVelocity(1);
//		updatePosition();
//	}

	//Output (temp)
	{
		boost::mutex::scoped_lock local_lock(BufferedBase::global_mutex);
		std::cout <<"(" <<parent->getId() <<"," <<frameNumber<<","<<parent->xPos.get()<<"," <<this->parent->yPos.get()<<","<<currPhase<<")" <<std::endl;
	}
}

/*---------------------Perception-related functions----------------------*/

void sim_mob::Pedestrian::setGoal(int flag) //0-to the next intersection, 1-to the crossing end, 2-to the destination
{
	//goal.xPos = this->xPos.get();
	//goal.yPos = topLeftCrossing.yPos + double(rand()%5) + 1;;

	//Give every agent the same goal.
	//goal.xPos = 1100;
	if(flag==0){
		//To be changed
		goal = Point2D(37218351,14335255);
	}
	else if(flag==1){

		//???? How to get position of crossings
		//Set the agent's position at the start of crossing and set the goal to the end of crossing
		RoadNetwork& network = ConfigParams::GetInstance().getNetwork();
		startToCross=false;

	}
	else if(flag==2){
		goal = Point2D(parent->destNode->location->getX(),parent->destNode->location->getY());
	}

}

bool sim_mob::Pedestrian::isDestReached()
{
	//Simple manhatten distance check
	double dX = abs((double)(parent->destNode->location->getX()/100) - parent->xPos.get());
	double dY = abs((double)(parent->destNode->location->getY()/100) - parent->yPos.get());
//	int dX = abs(goal.getX() - parent->xPos.get());
//	int dY = abs(goal.getY() - parent->yPos.get());
	return dX+dY < agentRadius;

	//return (parent->yPos.get()>=goal.yPos);
}

bool sim_mob::Pedestrian::isGoalReached()
{
	//Simple manhatten distance check
	int dX = abs(goal.getX()/100 - parent->xPos.get());
	int dY = abs(goal.getY()/100 - parent->yPos.get());
	return dX+dY < agentRadius;

	//return (parent->yPos.get()>=goal.yPos);
}

bool sim_mob::Pedestrian::reachStartOfCrossing()
{


	return false;

//	int lowerRightCrossingY = ConfigParams::GetInstance().crossings["lowerright"].getY();
//
//	if(parent->yPos.get()<=lowerRightCrossingY){
//		double dist = lowerRightCrossingY - parent->yPos.get();
//		if(dist<speed*1)
//			return true;
//		else
//			return false;
//	}
//	else
//		return false;
}

bool sim_mob::Pedestrian::onCrossing()
{

		return false;
}

int sim_mob::Pedestrian::getCurrentCrossing()
{

	if(onCrossing())
		return curCrossingID;
	else
		return -1;
}

void sim_mob::Pedestrian::updatePedestrianSignal()
{

//	currPhase = sig.get_Pedestrian_Light(0);
//	if(phaseCounter==60){ //1 minute period for switching phases (testing only)
//		phaseCounter=0;
//		if(currPhase==0)
//			currPhase = 1;
//		else
//			currPhase = 0;
//	}
//	else
//		phaseCounter++;
}

/*---------------------Decision-related functions------------------------*/

bool sim_mob::Pedestrian::checkGapAcceptance(){

	//Search for the nearest driver on the current link
	Agent* a = nullptr;
	for (size_t i=0; i<Agent::all_agents.size(); i++) {
		//Skip self
		a = Agent::all_agents[i];
		if (a->getId()==parent->getId()) {
			a = nullptr;
			continue;
		}

	   Person* p = dynamic_cast<Person*>(a);
	   if (dynamic_cast<Driver*>(p->getRole())) {
//		   (dynamic_cast<Driver*>(p->getRole()))->
			//It's a driver
	   } else if (dynamic_cast<Pedestrian*>(p->getRole())) {
		   //It's a pedestrian
		   //etc.
	   }
	   p = nullptr;
	   a = nullptr;
	}

	return false;
}

/*---------------------Action-related functions--------------------------*/

void sim_mob::Pedestrian::updateVelocity(int flag) //0-on sidewalk, 1-on crossing green, 2-on crossing red
{
	//Set direction (towards the goal)
	double scale;
	xVel = goal.getX() - parent->xPos.get();
	yVel = goal.getY() - parent->yPos.get();
	//Normalize
	double length = sqrt(xVel*xVel + yVel*yVel);
	xVel = xVel/length;
	yVel = yVel/length;
	//Set actual velocity
	if(flag==0||flag==1)
		scale = 1;
	else if (flag==2)
		scale = 1.8;
	xVel = xVel*speed*scale;
	yVel = yVel*speed*scale;

//	//Set direction (towards the goal)
//	double xDirection = goal.getX() - parent->xPos.get();
//	double yDirection = goal.getY() - parent->yPos.get();
//
//	//Normalize
//	double magnitude = sqrt(xDirection*xDirection + yDirection*yDirection);
//	xDirection = xDirection/magnitude;
//	yDirection = yDirection/magnitude;
//
//	//Set actual velocity
//	xVel = xDirection*speed;
//	yVel = yDirection*speed;
}

void sim_mob::Pedestrian::updatePosition()
{
//	//Factor in collisions
//	double xVelCombined = xVel + xCollisionVector;
//	double yVelCombined = yVel + yCollisionVector;

	//Compute
//	double newX = parent->xPos.get()+xVelCombined*1; //Time step is 1 second
//	double newY = parent->yPos.get()+yVelCombined*1;
	double newX = parent->xPos.get()+xVel*1; //Time step is 1 second
	double newY = parent->yPos.get()+yVel*1;

	//Decrement collision velocity
//	if (xCollisionVector != 0) {
//		xCollisionVector -= ((0.1*collisionForce) / (xCollisionVector/abs(xCollisionVector)) );
//	}
//	if (yCollisionVector != 0) {
//		yCollisionVector -= ((0.1*collisionForce) / (yCollisionVector/abs(yCollisionVector)) );
//	}

	//Set
	parent->xPos.set(newX);
	parent->yPos.set(newY);
}

//Simple implementations for testing

void sim_mob::Pedestrian::checkForCollisions()
{
	//For now, just check all agents and get the first positive collision. Very basic.
	Agent* other = nullptr;
	for (size_t i=0; i<Agent::all_agents.size(); i++) {
		//Skip self
		other = Agent::all_agents[i];
		if (other->getId()==parent->getId()) {
			other = nullptr;
			continue;
		}

		//Check.
		double dx = other->xPos.get() - parent->xPos.get();
		double dy = other->yPos.get() - parent->yPos.get();
		double distance = sqrt(dx*dx + dy*dy);
		if (distance < 2*agentRadius) {
			break; //Collision
		}
		other = nullptr;
	}

	//Set collision vector. Overrides previous setting, if any.
	if (other) {
		//Get a heading.
		double dx = other->xPos.get() - parent->xPos.get();
		double dy = other->yPos.get() - parent->yPos.get();

		//If the two agents are directly on top of each other, set
		//  their distances to something non-crashable.
		if (dx==0 && dy==0) {
			dx = other->getId() - parent->getId();
			dy = parent->getId() - other->getId();
		}

		//Normalize
		double magnitude = sqrt(dx*dx + dy*dy);
		if (magnitude==0) {
			dx = dy;
			dy = dx;
		}
		dx = dx/magnitude;
		dy = dy/magnitude;

		//Set collision vector to the inverse
		xCollisionVector = -dx * collisionForce;
		yCollisionVector = -dy * collisionForce;
	}
}

/*---------------------Other helper functions----------------------------*/









