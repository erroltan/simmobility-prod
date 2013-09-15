//Copyright (c) 2013 Singapore-MIT Alliance for Research and Technology
//Licensed under the terms of the MIT License, as described in the file:
//   license.txt   (http://opensource.org/licenses/MIT)

#include "geo10-pimpl.hpp"

using namespace sim_mob::xml;


void sim_mob::xml::fwdBckSegments_t_pimpl::pre ()
{
	model.clear();
}

std::vector<sim_mob::RoadSegment*> sim_mob::xml::fwdBckSegments_t_pimpl::post_fwdBckSegments_t ()
{
	//Register these for lookup later
	for (std::vector<sim_mob::RoadSegment*>::iterator it=model.begin(); it!=model.end(); it++) {
		book.addSegment(*it);
	}

	return model;
}

void sim_mob::xml::fwdBckSegments_t_pimpl::Segment (sim_mob::RoadSegment* value)
{
	model.push_back(value);
}

