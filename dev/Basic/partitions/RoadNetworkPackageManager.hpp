/*
 * RoadNetworkPackageManager.hpp
 * Target:
 * 1. package road networks and others (like trip chain)
 * 2. The modeler should define what to package and send;
 * 3. The modeler should define how to process the received data;
 */

#pragma once

#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>

#include "geospatial/RoadNetwork.hpp"
#include "geospatial/Node.hpp"
#include "geospatial/Link.hpp"
#include "geospatial/Lane.hpp"
#include "geospatial/Point2D.hpp"

#include "conf/simpleconf.hpp"
#include "geospatial/RoadNetwork.hpp"
#include "geospatial/Node.hpp"
#include "geospatial/Link.hpp"
#include "geospatial/Lane.hpp"
#include "geospatial/Point2D.hpp"
#include "geospatial/StreetDirectory.hpp"
#include "geospatial/RoadSegment.hpp"
#include "geospatial/Crossing.hpp"
#include "geospatial/StreetDirectory.hpp"

#include "entities/Signal.hpp"
#include "entities/vehicle/Vehicle.hpp"
#include "entities/misc/TripChain.hpp"
#include "util/RelAbsPoint.hpp"
#include "util/GeomHelpers.hpp"
#include "perception/FixedDelayed.hpp"

#include "entities/roles/driver/Driver.hpp"

#include <list>
#include <vector>

namespace sim_mob {

class RoadNetworkPackageManager
{
public:
	static RoadNetworkPackageManager &
	instance()
	{
		return instance_;
	}

private:
	static RoadNetworkPackageManager instance_;

public:
	template<class Archive>
	void packageNode(Archive & ar, const Node* one_node);

	template<class Archive>
	const Node* unpackageNode(Archive & ar);

	template<class Archive>
	void packageRoadSegment(Archive & ar, const RoadSegment* roadsegment);

	template<class Archive>
	const RoadSegment* unpackageRoadSegment(Archive & ar);

	template<class Archive>
	void packageLink(Archive & ar, const Link* one_link);

	template<class Archive>
	const Link* unpackageLink(Archive & ar);

	template<class Archive>
	void packageLane(Archive & ar, const Lane* one_lane);

	template<class Archive>
	const Lane* unpackageLane(Archive & ar);

	template<class Archive>
	void packageTripActivity(Archive & ar, const TripActivity* tripActivity);

	template<class Archive>
	TripActivity* unpackageTripActivity(Archive & ar);

	template<class Archive>
	void packageTripChain(Archive & ar, const TripChain* tripChain);

	template<class Archive>
	TripChain* unpackageTripChain(Archive & ar);

	template<class Archive>
	void packageSignal(Archive & ar, const Signal* one_signal);

	template<class Archive>
	const Signal* unpackageSignal(Archive & ar);

	template<class Archive>
	void packageSignalContent(Archive & ar, const Signal* one_signal);

	template<class Archive>
	void unpackageSignalContent(Archive & ar);

	template<class Archive>
	void packageVehicle(Archive & ar, const Vehicle* one_vehicle);

	template<class Archive>
	Vehicle* unpackageVehicle(Archive & ar);

	template<class Archive>
	void packageRelAbsPoint(Archive & ar, const RelAbsPoint* attributes);

	template<class Archive>
	RelAbsPoint* unpackageRelAbsPoint(Archive & ar);

	template<class Archive>
	void packageFixedDelayedPointer2D(Archive & ar, FixedDelayed<Point2D*>& one_delay);

	template<class Archive>
	FixedDelayed<Point2D*>& unpackageFixedDelayedPointer2D(Archive & ar);

	template<class Archive>
	void packageFixedDelayedInt(Archive & ar, FixedDelayed<int32_t>& one_delay);

	template<class Archive>
	FixedDelayed<int32_t>& unpackageFixedDelayedInt(Archive & ar);

	template<class Archive>
	void packageLaneAndIndexPair(Archive & ar, const StreetDirectory::LaneAndIndexPair* lane_pair);

	template<class Archive>
	StreetDirectory::LaneAndIndexPair* unpackageLaneAndIndexPair(Archive & ar);

	template<class Archive>
	void packageCrossing(Archive & ar, const Crossing* cross);

	template<class Archive>
	const Crossing* unpackageCrossing(Archive & ar);
};

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageNode(Archive & ar, const Node* one_node)
{
	bool hasSomthing = true;
	if (!one_node)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	ar & (*(one_node->location));
}

template<class Archive>
const Node* sim_mob::RoadNetworkPackageManager::unpackageNode(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	sim_mob::Point2D location;
	ar & location;

	sim_mob::RoadNetwork& rn = ConfigParams::GetInstance().getNetwork();
	return rn.locateNode(location, true);
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageRoadSegment(Archive & ar, const RoadSegment* roadsegment)
{
	bool hasSomthing = true;
	if (!roadsegment)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	ar & (*roadsegment->getStart()->location);
	ar & (*roadsegment->getEnd()->location);
}

template<class Archive>
const RoadSegment* sim_mob::RoadNetworkPackageManager::unpackageRoadSegment(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	sim_mob::Point2D point_1;
	sim_mob::Point2D point_2;

	ar & point_1;
	ar & point_2;

	return sim_mob::getRoadSegmentBasedOnNodes(&point_1, &point_2);
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageLink(Archive & ar, const Link* one_link)
{
	bool hasSomthing = true;
	if (!one_link)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	ar & (*(one_link->getStart()->location));
	ar & (*(one_link->getEnd()->location));
}

template<class Archive>
const Link* sim_mob::RoadNetworkPackageManager::unpackageLink(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	sim_mob::Point2D point_1;
	sim_mob::Point2D point_2;

	ar & point_1;
	ar & point_2;

	return sim_mob::getLinkBetweenNodes(&point_1, &point_2);
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageLane(Archive & ar, const Lane* one_lane)
{
	bool hasSomthing = true;
	if (!one_lane)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	sim_mob::Point2D* start = one_lane->getRoadSegment()->getStart()->location;
	sim_mob::Point2D* end = one_lane->getRoadSegment()->getEnd()->location;
	int lane_id = one_lane->getLaneID();

	ar & (*start);
	ar & (*end);
	ar & lane_id;
}

template<class Archive>
const Lane* sim_mob::RoadNetworkPackageManager::unpackageLane(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	sim_mob::Point2D start;
	sim_mob::Point2D end;
	int lane_id;

	ar & start;
	ar & end;
	ar & lane_id;

	const sim_mob::RoadSegment* roadSegment = sim_mob::getRoadSegmentBasedOnNodes(&start, &end);

	const std::vector<sim_mob::Lane*>& lanes = roadSegment->getLanes();
	std::vector<sim_mob::Lane*>::const_iterator it = lanes.begin();

	for (; it != lanes.end(); it++)
	{
		if ((*it)->getLaneID() == lane_id)
			return (*it);
	}

	return NULL;
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageTripActivity(Archive & ar, const TripActivity* tripActivity)
{
	bool hasSomthing = true;
	if (!tripActivity)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	ar & (tripActivity->description);
	packageNode(ar, tripActivity->location);
}

template<class Archive>
TripActivity* sim_mob::RoadNetworkPackageManager::unpackageTripActivity(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	TripActivity* activity = new TripActivity();
	ar & (activity->description);

	activity->location = const_cast<sim_mob::Node*> (unpackageNode(ar));
	return activity;
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageTripChain(Archive & ar, const TripChain* tripChain)
{
	bool hasSomthing = true;
	if (!tripChain)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	packageTripActivity(ar, &(tripChain->from));
	packageTripActivity(ar, &(tripChain->to));

	ar & (tripChain->primary);
	ar & (tripChain->flexible);
	ar & (tripChain->startTime);
	ar & (tripChain->mode);
}

template<class Archive>
TripChain* sim_mob::RoadNetworkPackageManager::unpackageTripChain(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	TripChain* tripChain = new TripChain();
	tripChain->from = *(unpackageTripActivity(ar));
	tripChain->to = *(unpackageTripActivity(ar));

	ar & (tripChain->primary);
	ar & (tripChain->flexible);
	ar & (tripChain->startTime);
	ar & (tripChain->mode);

	return tripChain;
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageSignal(Archive & ar, const Signal* one_signal)
{
	//std::cout << "packageOneCrossDriver Pdesttrains 2.8.4.4:" << std::endl;
	bool hasSomthing = true;

	if (!one_signal)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	//	std::cout << "packageOneCrossDriver Pdesttrains 2.8.4.4:" << hasSomthing << std::endl;
	//	std::cout << "packageOneCrossDriver Pdesttrains 2.8.4.4:" << one_signal->getId() << std::endl;
	//	std::cout << "packageOneCrossDriver Pdesttrains 2.8.4.4:" << one_signal->getNode().location << std::endl;
	//	std::cout << "packageOneCrossDriver Pdesttrains 2.8.4.4:" << one_signal->getNode().location->getX() << std::endl;

	ar & (*(one_signal->getNode().location));
}

template<class Archive>
const Signal* sim_mob::RoadNetworkPackageManager::unpackageSignal(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	sim_mob::Point2D point_1;
	ar & point_1;

	//std::cout << "packageOneCrossDriver Pdesttrains 2.8.4.4.2:" << point_1.getX() << std::endl;

	return sim_mob::getSignalBasedOnNode(&point_1);
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageSignalContent(Archive & ar, const Signal* one_signal)
{
	bool hasSomthing = true;
	if (!one_signal)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	//std::cout << "1111111111" << std::endl;

	packageSignal(ar, one_signal);

	//	int id = one_signal->getId();
	//	ar & id;
	ar & (one_signal->currCL);
	ar & (one_signal->currSplitPlan);
	ar & (one_signal->currOffset);
	ar & (one_signal->currPhase);
	ar & (one_signal->currSplitPlanID);
	ar & (one_signal->phaseCounter);

	//std::cout << "22222" << std::endl;

	//very dangerous, suggest to change
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			//int value = one_signal->TC_for_Driver[i][j];
			int value = one_signal->buffered_TC.get().TC_for_Driver[i][j];
			//	std::cout << "value:" << value << std::endl;
			ar & value;
		}
	}

	//std::cout << "33333333" << std::endl;

	for (int i = 0; i < 4; i++)
	{
		int value = one_signal->buffered_TC.get().TC_for_Pedestrian[i];
		ar & value;
	}

	//	std::string value = "test_main";
	//	ar & value;
	//std::cout << "444444444444" << std::endl;
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::unpackageSignalContent(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return;

	const Signal* signal = unpackageSignal(ar);
	Signal* one_signal = const_cast<Signal*> (signal);

	//	int id;
	//	ar & (id);

	//	one_signal->id = id;
	ar & (one_signal->currCL);
	ar & (one_signal->currSplitPlan);
	ar & (one_signal->currOffset);
	ar & (one_signal->currPhase);
	ar & (one_signal->currSplitPlanID);
	ar & (one_signal->phaseCounter);

	//	int[][] TC_for_Driver = const_cast<int[][]> (one_signal->TC_for_Driver);

	//std::cout << "66666666666666" << std::endl;

	SignalStatus buffered_signal;

	//very dangerous, suggest to change
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			//			int value;
			//ar & (one_signal->TC_for_Driver[i][j]);
			ar & (buffered_signal.TC_for_Driver[i][j]);
			//			TC_for_Driver[i][j] = value;
		}
	}

	//std::cout << "7777777777777" << std::endl;

	//	int[] TC_for_Pedestrian = const_cast<int[]> (one_signal->TC_for_Pedestrian);
	for (int i = 0; i < 4; i++)
	{
		//		int value;
		//ar & (one_signal->TC_for_Pedestrian[i]);
		ar & (buffered_signal.TC_for_Pedestrian[i]);
		//		TC_for_Pedestrian[i] = value;
	}

	one_signal->buffered_TC.force(buffered_signal);

	//	std::string value;
	//	ar & value;
	//	std::cout << "Testing:" << value << std::endl;
	//std::cout << "888888888888" << std::endl;
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageVehicle(Archive & ar, const Vehicle* one_vehicle)
{

	bool hasSomthing = true;
	if (!one_vehicle)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	ar & (one_vehicle->length);
	ar & (one_vehicle->width);
	ar & (one_vehicle->timeStep);

	ar & (one_vehicle->xPos);
	ar & (one_vehicle->yPos);
	ar & (one_vehicle->xPos_);
	ar & (one_vehicle->yPos_);

	packageRelAbsPoint(ar, &(one_vehicle->velocity));
	packageRelAbsPoint(ar, &(one_vehicle->accel));
}

template<class Archive>
Vehicle* sim_mob::RoadNetworkPackageManager::unpackageVehicle(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	Vehicle* one_vehicle = new Vehicle();

	ar & (one_vehicle->length);
	ar & (one_vehicle->width);
	ar & (one_vehicle->timeStep);
	ar & (one_vehicle->xPos);
	ar & (one_vehicle->yPos);
	ar & (one_vehicle->xPos_);
	ar & (one_vehicle->yPos_);

	one_vehicle->velocity = *(unpackageRelAbsPoint(ar));
	one_vehicle->accel = *(unpackageRelAbsPoint(ar));

	return one_vehicle;
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageRelAbsPoint(Archive & ar, const RelAbsPoint* attributes)
{
	bool hasSomthing = true;
	if (!attributes)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	ar & (attributes->abs.x);
	ar & (attributes->abs.y);

	ar & (attributes->rel.x);
	ar & (attributes->rel.y);

	ar & (attributes->scaleDir.x);
	ar & (attributes->scaleDir.y);
}

template<class Archive>
RelAbsPoint* sim_mob::RoadNetworkPackageManager::unpackageRelAbsPoint(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	RelAbsPoint* attributes = new RelAbsPoint();

	ar & (attributes->abs.x);
	ar & (attributes->abs.y);

	ar & (attributes->rel.x);
	ar & (attributes->rel.y);

	ar & (attributes->scaleDir.x);
	ar & (attributes->scaleDir.y);

	return attributes;
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageFixedDelayedPointer2D(Archive & ar, FixedDelayed<Point2D*>& one_delay)
{
	int list_size = one_delay.history.size();
	ar & list_size;

	std::list<FixedDelayed<Point2D*>::HistItem>::const_iterator itr = one_delay.history.begin();
	for (; itr != one_delay.history.end(); itr++)
	{
		FixedDelayed<Point2D*>::HistItem one = (*itr);
		Point2D* value = one.item;
		int value2 = one.observedTime;

		ar & (*value);
		ar & value2;
	}

//	std::cout << "Send one_delay(Pointer2D)->history:" << one_delay.history.size() << std::endl;
}

template<class Archive>
FixedDelayed<Point2D*>& sim_mob::RoadNetworkPackageManager::unpackageFixedDelayedPointer2D(Archive & ar)
{
	size_t reaction_time = Driver::reactTime;
	FixedDelayed<Point2D*>* one_delay = new FixedDelayed<Point2D*> (reaction_time, true);

	int list_size;
	ar & list_size;

	for (int i = 0; i < list_size; i++)
	{
		Point2D value;
		int value2;

		ar & value;
		ar & value2;

		Point2D* buffer_value = new Point2D(value.getX(), value.getY());
		uint32_t ut_value = value2;

		FixedDelayed<Point2D*>::HistItem one(buffer_value, ut_value);
		one_delay->history.push_back(one);
	}

//	std::cout << "one_delay(Pointer2D)->history:" << one_delay->history.size() << std::endl;
	return (*one_delay);
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageFixedDelayedInt(Archive & ar, FixedDelayed<int32_t>& one_delay)
{
	int list_size = one_delay.history.size();
	ar & list_size;

	std::list<FixedDelayed<int32_t>::HistItem>::const_iterator itr = one_delay.history.begin();
	for (; itr != one_delay.history.end(); itr++)
	{
		FixedDelayed<int32_t>::HistItem one = (*itr);
		int value = one.item;
		int value2 = one.observedTime;

		ar & value;
		ar & value2;
	}

//	std::cout << "Send one_delay(int)->history:" << one_delay.history.size() << std::endl;
}

template<class Archive>
FixedDelayed<int32_t>& sim_mob::RoadNetworkPackageManager::unpackageFixedDelayedInt(Archive & ar)
{
	size_t reaction_time = Driver::reactTime;
	FixedDelayed<int32_t>* one_delay = new FixedDelayed<int32_t> (reaction_time, false);

	int list_size;
	ar & list_size;

	for (int i = 0; i < list_size; i++)
	{
		int value;
		int value2;

		ar & value;
		ar & value2;

		int32_t t_value = value;
		uint32_t ut_value = value2;

		FixedDelayed<int32_t>::HistItem one(t_value, ut_value);
		one_delay->history.push_back(one);
	}

//	std::cout << "one_delay(int)->history:" << one_delay->history.size() << std::endl;
	return (*one_delay);
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageLaneAndIndexPair(Archive & ar,
		const StreetDirectory::LaneAndIndexPair* lane_pair)
{

	bool hasSomthing = true;
	if (!lane_pair)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	packageLane(ar, lane_pair->lane_);
	ar & lane_pair->startIndex_;
	ar & lane_pair->endIndex_;
}

template<class Archive>
StreetDirectory::LaneAndIndexPair* sim_mob::RoadNetworkPackageManager::unpackageLaneAndIndexPair(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	StreetDirectory::LaneAndIndexPair* lane_pair = new StreetDirectory::LaneAndIndexPair();
	lane_pair->lane_ = unpackageLane(ar);
	ar & lane_pair->startIndex_;
	ar & lane_pair->endIndex_;

	return lane_pair;
}

template<class Archive>
void sim_mob::RoadNetworkPackageManager::packageCrossing(Archive & ar, const Crossing* cross)
{
	bool hasSomthing = true;
	if (!cross)
	{
		hasSomthing = false;
		ar & hasSomthing;
		return;
	}
	else
	{
		ar & hasSomthing;
	}

	ar & (cross->nearLine.first);
	ar & (cross->nearLine.second);

	ar & (cross->farLine.first);
	ar & (cross->farLine.second);

	//	std::cout << "Point 1:" << cross->farLine.first.getX() << "," << cross->farLine.first.getY() << std::endl;
	//	std::cout << "Point 2:" << cross->farLine.second.getX() << "," << cross->farLine.second.getY() << std::endl;
	//	std::cout << "Point 3:" << cross->nearLine.first.getX() << "," << cross->nearLine.first.getY() << std::endl;
	//	std::cout << "Point 4:" << cross->nearLine.second.getX() << "," << cross->nearLine.second.getY() << std::endl;

	//	ar & cross->start;
	//	ar & cross->end;
}

template<class Archive>
const Crossing* sim_mob::RoadNetworkPackageManager::unpackageCrossing(Archive & ar)
{
	bool hasSomthing;
	ar & hasSomthing;

	if (hasSomthing == false)
		return NULL;

	sim_mob::Point2D near_1;
	sim_mob::Point2D near_2;
	sim_mob::Point2D far_1;
	sim_mob::Point2D far_2;

	ar & near_1;
	ar & near_2;
	ar & far_1;
	ar & far_2;

	return sim_mob::getCrossingBasedOnNode(&near_1, &near_2, &far_1, &far_2);
}

}