/*
 * ANNOUNCE_Message.h
 *
 *  Created on: May 9, 2013
 *      Author: vahid
 */

#pragma once

//#include "entities/commsim/communicator/message/base/Message.hpp"
//#include "ANNOUNCE_Handler.hpp"
#include "entities/commsim/communicator/message/derived/roadrunner/RoadrunnerMessage.hpp"

namespace sim_mob {
namespace roadrunner {

class MSG_ANNOUNCE : public sim_mob::roadrunner::RoadrunnerMessage {
	//...
public:
	Handler * newHandler();
	MSG_ANNOUNCE(msg_data_t data_);
};

}/* namespace roadrunner */
} /* namespace sim_mob */
