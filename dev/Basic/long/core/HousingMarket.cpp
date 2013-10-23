//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

/* 
 * File:   HousingMarket.cpp
 * Author: Pedro Gandola <pedrogandola@smart.mit.edu>
 * 
 * Created on March 11, 2013, 4:13 PM
 */

#include "HousingMarket.hpp"
#include "workers/Worker.hpp"
#include "event/LT_EventArgs.hpp"
#include "message/MessageBus.hpp"

using namespace sim_mob::long_term;
using namespace sim_mob::event;
using namespace sim_mob::messaging;
using sim_mob::Entity;
using std::vector;

HousingMarket::HousingMarket() : UnitHolder(-1), Entity(-1), firstTime(true) {
}

HousingMarket::~HousingMarket() {
}

bool HousingMarket::Add(Unit* unit, bool reOwner) {
    // no re-parent the unit to the market.
    bool retVal = UnitHolder::Add(unit, false);
    if (retVal) {
        MessageBus::PublishEvent(LTEID_HM_UNIT_ADDED, this,
            MessageBus::EventArgsPtr(new HM_ActionEventArgs(unit->GetId())));
    }
    return retVal;
}

Unit* HousingMarket::Remove(UnitId id, bool reOwner) {
    // no re-parent the unit to the market.
    Unit* retVal = UnitHolder::Remove(id, false);
    if (retVal) {
        MessageBus::PublishEvent(LTEID_HM_UNIT_REMOVED, this, 
            MessageBus::EventArgsPtr(new HM_ActionEventArgs(id)));
    }
    return retVal;
}

Entity::UpdateStatus HousingMarket::update(timeslice now) {
    Setup();
    return Entity::UpdateStatus(Entity::UpdateStatus::RS_CONTINUE);
}

bool HousingMarket::isNonspatial() {
    return false;
}

void HousingMarket::buildSubscriptionList(vector<BufferedBase*>& subsList) {
}

void HousingMarket::Setup() {
    if (firstTime) {
        //setup first things inside the entity.
        firstTime = false;
    }
}
