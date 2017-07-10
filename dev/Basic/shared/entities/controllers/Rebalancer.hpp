/*
 * Rebalancer.hpp
 *
 *  Created on: Jun 1, 2017
 *      Author: araldo
 */
#pragma once
#include <queue>
#include "entities/Person.hpp"

// jo{

#include "Types.hpp" // hash functor
#include "message/Message.hpp"
#include "message/MobilityServiceControllerMessage.hpp"
#include "MobilityServiceController.hpp"
#include "entities/mobilityServiceDriver/MobilityServiceDriver.hpp"
#include "database/predaydao/ZoneCostSqlDao.hpp"

// #include "MobilityServiceController.hpp"

#include <map>
#include <set>
#include <sstream>

#include "glpk.h"
// }jo

namespace sim_mob {

class OnCallController; //forward declaration

class Rebalancer
{
public:
	Rebalancer(const OnCallController* parentController_);
	virtual ~Rebalancer();

	virtual void rebalance(const std::vector<const Person*>& availableDrivers,
			const timeslice currTick)=0;

	void onRequestReceived(const Node* startNode);
	// jo{ need these functions to get supply/demand by zone ID
	//private:
		// get demand by Zone
	virtual int getNumCustomers(int TazId);

		// get supply by Zone
	virtual int getNumVehicles(const std::vector<const Person*>& availableDrivers,
			int TazId);
	// }jo

protected:
	std::vector<const Node*> latestStartNodes;
	const OnCallController* parentController;

};

class SimpleRebalancer : public Rebalancer
{
	using Rebalancer::Rebalancer;

	void rebalance(const std::vector<const Person*>& availableDrivers,
			const timeslice currTick);
};

class LazyRebalancer : public Rebalancer
{
	using Rebalancer::Rebalancer;

	void rebalance(const std::vector<const Person*>& availableDrivers,
			const timeslice currTick);
};

class KasiaRebalancer : public Rebalancer
{
	using Rebalancer::Rebalancer;

	void rebalance(const std::vector<const Person*>& availableDrivers,
			const timeslice currTick);

};





} /* namespace sim_mob */

