//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#include "DriverUpdateParams.hpp"

#include "conf/ConfigManager.hpp"
#include "entities/roles/driver/DriverFacets.hpp"

using namespace sim_mob;

DriverUpdateParams::DriverUpdateParams() : UpdateParams(), isApproachingIntersection(false), hasStoppedForStopSign(false), isResponseReceived(false),
useIntAcc(false), isTargetLane(false), currLaneIndex(0), nextLaneIndex(0), status(0), flags(0), noOfLC(0), initialSpeed(0), parentId(0), currSpeed(0), desiredSpeed(0), elapsedSeconds(0),
trafficSignalStopDistance(0), perceivedFwdVelocity(0), perceivedLatVelocity(0), perceivedFwdVelocityOfFwdCar(0), perceivedLatVelocityOfFwdCar(0), 
perceivedAccelerationOfFwdCar(0), perceivedDistToFwdCar(0), perceivedDistToTrafficSignal(500), speedLimit(0), impatienceTimer(0), impatienceTimerStart(0), 
accessTime(0), gapBetnVehicles(0), accLeadVehicle(0), velocityLeadVehicle(0), spaceStar(0), distanceToNormalStop(0), distToStop(999), 
overflowIntoIntersection(0), stopVisibilityDistance(100), distanceToStoppingPt(999), stopTimeTimer(0), utilityLeft(0), utilityRight(0), utilityCurrent(0), 
headway(999), acc(0), density(0), FFAccParamsBeta(0), lateralVelocity(0), reactionTimeCounter(0), nextStepSize(0), maxAcceleration(0), normalDeceleration(0), 
maxDeceleration(0), laneChangeTime(200), lcMaxYieldingTime(0), maxLaneSpeed(0), acceleration(0), yieldTime(0,0), trafficColor(TrafficColor::TRAFFIC_COLOUR_INVALID),
perceivedTrafficColor(TrafficColor::TRAFFIC_COLOUR_INVALID), turningDirection(LaneChangeTo::LANE_CHANGE_TO_NONE),
stopPointState(STOP_POINT_NOT_FOUND), driver(NULL), currLane(NULL), leftLane(NULL), rightLane(NULL), leftLane2(NULL), rightLane2(NULL)
{
}

DriverUpdateParams::~DriverUpdateParams()
{
}

void DriverUpdateParams::setStatus(string name, StatusValue value, string whoSet)
{
    statusMgr.setStatus(name, value, whoSet);
}

StatusValue DriverUpdateParams::getStatus(string name)
{
    return statusMgr.getStatus(name);
}

void DriverUpdateParams::setStatusDoingLC(LaneChangeTo &laneChangingTo)
{
    if (laneChangingTo == LANE_CHANGE_TO_RIGHT)
    {
        setStatus(STATUS_LC_RIGHT);
    }
    else if (laneChangingTo == LANE_CHANGE_TO_LEFT)
    {
        setStatus(STATUS_LC_LEFT);
    }
}

void DriverUpdateParams::reset(timeslice now, const Driver &owner)
{
    UpdateParams::reset(now);

    //Set to the previous known buffered values
    currLane = owner.getCurrLane();

    if (currLane)
    {
        currLaneIndex = currLane->getLaneIndex();
    }

    //Current lanes to the left and right. May be null
    leftLane = NULL;
    rightLane = NULL;
    leftLane2 = NULL;
    rightLane2 = NULL;

    //Reset. These will be set before they are used; the values here represent either default
    //values or are unimportant.
    currSpeed = 0;
    perceivedFwdVelocity = 0;
    perceivedLatVelocity = 0;
    trafficColor = TRAFFIC_COLOUR_GREEN;
    elapsedSeconds = ConfigManager::GetInstance().FullConfig().baseGranMS() / 1000.0;
    perceivedFwdVelocityOfFwdCar = 0;
    perceivedLatVelocityOfFwdCar = 0;
    perceivedAccelerationOfFwdCar = 0;

    //Space to next car
    gapBetnVehicles = 0;

    //the acceleration of leading vehicle
    accLeadVehicle = 0;

    //the speed of leading vehicle
    velocityLeadVehicle = 0;

    //The distance which leading vehicle will move in next time step
    spaceStar = 0;

    distanceToNormalStop = 0;

    //Distance to where critical location where lane changing has to be made
    distToStop = 999;

    //If we've just moved into an intersection, this is set to the amount of overflow (e.g.,
    //how far into it we already are.)
    overflowIntoIntersection = 0;

    turningDirection = LANE_CHANGE_TO_NONE;

    density = 0;
}

void DriverUpdateParams::unsetStatus(unsigned int s)
{
    status &= ~s;
}

double DriverUpdateParams::lcMinGap(int type)
{
    std::vector<double> b = LC_GAP_MODELS[type];
    return b[2] * b[0];
}

void DriverUpdateParams::insertStopPoint(StopPoint &stopPt)
{
    std::map<unsigned int, std::vector<StopPoint> >::iterator it = stopPointPool.find(stopPt.segmentId);
    if (it != stopPointPool.end())
    {
        it->second.push_back(stopPt);
    }
    else
    {
        std::vector<StopPoint> v;
        v.push_back(stopPt);
        stopPointPool.insert(std::make_pair(stopPt.segmentId, v));
    }
}

void DriverUpdateParams::insertConflictTurningDriver(const TurningConflict *conflict, double distance, const Driver *driver)
{
    NearestVehicle nearestVehicle;
    nearestVehicle.distance = distance;
    nearestVehicle.driver = driver;

    //Find turning conflict
    std::map<const TurningConflict*, std::set<NearestVehicle, compare_NearestVehicle> >::iterator it = conflictVehicles.find(conflict);

    if (it != conflictVehicles.end())
    {
        std::set<NearestVehicle, compare_NearestVehicle>& nearestVehicles = it->second;
        nearestVehicles.insert(nearestVehicle);
    }
    else
    {
        std::set<NearestVehicle, compare_NearestVehicle> nearestVehicles;
        nearestVehicles.insert(nearestVehicle);
        conflictVehicles.insert(std::make_pair(conflict, nearestVehicles));
    }
}

void DriverUpdateParams::addTargetLanes(set<const Lane *> tgtLanes)
{
    set<const Lane*> newTargetLanes;
    set<const Lane*>::iterator it;

    //find the lane in both tgtLanes and targetLanes
    for (it = tgtLanes.begin(); it != tgtLanes.end(); ++it)
    {
        const Lane* l = *it;
        set<const Lane*>::iterator itFind = targetLanes.find(l);
        if (itFind != targetLanes.end())
        {
            newTargetLanes.insert(l);
        }
    }

    targetLanes = newTargetLanes;
}

void DriverUpdateParams::buildDebugInfo()
{
    std::stringstream s;
    
    s << "            " << parentId << ":" << accSelect << ":" << acc;
    s << ":speed:" << perceivedFwdVelocity;
    
#if 0
    //Debug lane changing

    string lc = "lc-s";
    if (getStatus(STATUS_LC_LEFT))
    {
        lc = "lc-l";
    }
    else if (getStatus(STATUS_LC_RIGHT))
    {
        lc = "lc-r";
    }

    s<<":"<<lcd;
    s<<":"<<lc;
    s<<"=="<<lcDebugStr.str();

#endif

#if 0
    //Debug car following
    
    int fwdcarid = -1;
    char fwdnvdis[30] = "\0";
    if (this->nvFwd.exists())
    {
        Driver* fwd_driver_ = const_cast<Driver*> (nvFwd.driver);
        fwdcarid = fwd_driver_->getParams().parentId;
        sprintf(fwdnvdis, "fwdnvdis:%03.1f", nvFwd.distance);
    }
    else if (this->nvFwdNextLink.exists())
    {
        Driver* fwd_driver_ = const_cast<Driver*> (nvFwdNextLink.driver);
        fwdcarid = fwd_driver_->getParams().parentId;
        sprintf(fwdnvdis, "fwdnv_nxtlnkdis:%03.1f", nvFwdNextLink.distance);
    }

    int backcarid = -1;
    char backnvdis[30] = "\0";
    if (this->nvBack.exists())
    {
        Driver* back_driver_ = const_cast<Driver*> (nvBack.driver);
        backcarid = back_driver_->getParams().parentId;
        sprintf(backnvdis, "backnvdis:%03.1f", nvBack.distance);
    }
    s << ":fwd:" << fwdcarid << ":" << fwdnvdis;
    s << ":back:" << backcarid << ":" << backnvdis; 
    
    int lftFwdId = -1;
    char leftFwdDis[30] = "\0";
    if (this->nvLeftFwd.exists())
    {
        Driver* leftFwdDrv = const_cast<Driver*> (nvLeftFwd.driver);
        lftFwdId = leftFwdDrv->getParams().parentId;
        sprintf(leftFwdDis, "leftFwdDis:%03.1f", nvLeftFwd.distance);
    }
    
    int rtFwdId = -1;
    char rtFwdDis[30] = "\0";
    if (this->nvRightFwd.exists())
    {
        Driver* rtFwdDrv = const_cast<Driver*> (nvRightFwd.driver);
        rtFwdId = rtFwdDrv->getParams().parentId;
        sprintf(rtFwdDis, "rtFwdDis:%03.1f", nvRightFwd.distance);
    }
    s << ":lftFwd:" << lftFwdId << ":" << leftFwdDis;
    s << ":rtFwd:" << rtFwdId << ":" << rtFwdDis;
    
    int lftBkId = -1;
    char leftBkDis[30] = "\0";
    if (this->nvLeftBack.exists())
    {
        Driver* leftBkDrv = const_cast<Driver*> (nvLeftBack.driver);
        lftBkId = leftBkDrv->getParams().parentId;
        sprintf(leftBkDis, "leftBkDis:%03.1f", nvLeftBack.distance);
    }
    
    int rtBkId = -1;
    char rtBkDis[30] = "\0";
    if (this->nvRightBack.exists())
    {
        Driver* rtBkDrv = const_cast<Driver*> (nvRightBack.driver);
        rtBkId = rtBkDrv->getParams().parentId;
        sprintf(rtBkDis, "rtBkDis:%03.1f", nvRightBack.distance);
    }
    s << ":lftBk:" << lftBkId << ":" << leftBkDis;
    s << ":rtBk:" << rtBkId << ":" << rtBkDis;
#endif
    
#if 0
    //Debug intersection driving    
    s << intDebugStr;
#endif

    debugInfo = s.str();
}
