//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#pragma once

#include "Driver.hpp"
#include "BusDriverFacets.hpp"
#include "entities/misc/BusTrip.hpp"
#include "entities/roles/passenger/Passenger.hpp"
/*
 * BusDriver.hpp
 *
 *  Created on: May 6, 2013
 *      Author: zhang huai peng
 *      		melani
 */

namespace sim_mob {

namespace medium
{
class BusDriverBehavior;
class BusDriverMovement;
class BusStopAgent;

class BusDriver : public sim_mob::medium::Driver {
public:
	BusDriver(Agent* parent, MutexStrategy mtxStrat, sim_mob::medium::BusDriverBehavior* behavior = nullptr, sim_mob::medium::BusDriverMovement* movement = nullptr);
	virtual ~BusDriver();

	virtual sim_mob::Role* clone(sim_mob::Person* parent) const;

	//Virtual overrides
	virtual void make_frame_tick_params(timeslice now);
	virtual std::vector<sim_mob::BufferedBase*> getSubscriptionParams();
	virtual sim_mob::DriverRequestParams getDriverRequestParams();

	/**
	 * gets stops list from bus route
	 * @return constant pointer to bus stops list
	 */
	const std::vector<const sim_mob::BusStop*>* getBusStopsVector() const;

	/**
	 * insert a new passenger to bus driver when a passenger
	 * decide to board on this bus
	 *
	 * @param passenger is the pointer to the person who will board on this bus
	 *
	 * @return boolean value, if boarding success, the value is true;
	 * otherwise this value is false.
	 */
	bool insertPassenger(sim_mob::medium::Passenger* passenger);

	/**
	 * alight passengers when those want to alight at next bus stop
	 * @param bus stop agent is the agent which wrap bus stop and waiting people
	 * @return the number of alighting people
	 */
	int alightPassenger(sim_mob::medium::BusStopAgent* busStopAgent);

	/**
	 * enter the bus stop
	 * @param bus stop agent is the agent which wrap bus stop and waiting people
	 */
	void enterBusStop(sim_mob::medium::BusStopAgent* busStopAgent);

	/**
	 * predict arrival at next bus stop in next frame tick
	 * @param preArrivalTime is predicted arrival time at next bus stop
	 * @param bus stop agent is the agent which wrap next bus stop
	 */
	void predictArrivalAtBusStop(double preArrivalTime, sim_mob::medium::BusStopAgent* busStopAgent);

	/**
	 * get current waiting time at bus stop
	 */
	double getWaitingTimeAtBusStop() {
		return waitingTimeAtbusStop;
	}

private:
	/**passengers list*/
	std::list<sim_mob::medium::Passenger*> passengerList;
	/**last visited bus stop*/
	Shared<const BusStop*> visitedBusStop;
	/**last visited bus stop sequence number*/
	Shared<int> visitedBusStopSequenceNo;
	/**real arrival time set by bus driver*/
	Shared<double> arrivalTime;
	/**current bus stop real times including departure and arrival time*/
	Shared<BusStop_RealTimes>* busStopRealTimes;
	/** dwell time set by bus driver*/
	Shared<double> dwellTime;
	/**holding time set by bus controller*/
	Shared<double> holdingTime;
	/**get bus line information*/
	Shared<std::string> visitedBusLine;
	/**sequence number for current bus trip*/
	Shared<int> visitedBusTripSequenceNo;
	/**request mode for holding strategy*/
	Shared<int> requestMode;
	/**final waiting time at bus stop*/
	double waitingTimeAtbusStop;

protected:
	friend class BusDriverBehavior;
	friend class BusDriverMovement;
};

//#endif /* BUSDRIVER_HPP_ */

}
}


