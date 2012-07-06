#pragma once

#include "GenConfig.h"
#include "util/LangHelpers.hpp"

#include <sstream>
#include <string>

#ifndef SIMMOB_DISABLE_MPI
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/list.hpp>
#include <boost/serialization/set.hpp>
#include <boost/serialization/map.hpp>
#endif


namespace unit_tests {
class PackUnpackUnitTests;
}


namespace sim_mob {

class BoundaryProcessor;



/**
 * \author Xu Yan
 * Function:
 * PackageUtils is used in sender side to pack basic data type (like: vector<int>) and some SimMobility data type (like: Node).
 *
 * \note
 * PackageUtils/UnPackageUtils have matching functions, if you add/edit/remove one function in this class, you need to check class UnPackageUtils
 *
 * \note
 * If the flag SIMMOB_DISABLE_MPI is defined, then this class is completely empty. It still exists as a friend class to anything
 * which can be serialized so that we can avoid lots of #idefs elsewhere in the code. ~Seth
 */
class PackageUtils {

public:
	PackageUtils() CHECK_MPI_THROW ;
	~PackageUtils() CHECK_MPI_THROW ;
public:
	/**
	 * DATA_TYPE can be:
	 * (1)Basic Data Type: int {unsigned, signed}, long, short, float, double, char, bool.
	 * (2)STL Data Type: list, array, set.
	*/
	template<class DATA_TYPE>
	void operator<<(DATA_TYPE& value) CHECK_MPI_THROW ;

	/**
	 * xuyan:
	 * double value is processed specially, because sometimes the double value is NaN.
	 */
	void operator<<(double value) CHECK_MPI_THROW ;

private:
	std::string getPackageData() CHECK_MPI_THROW ;

private:
	friend class BoundaryProcessor;
	friend class unit_tests::PackUnpackUnitTests;

#ifndef SIMMOB_DISABLE_MPI
	std::stringstream buffer;
	boost::archive::text_oarchive* package;
#endif

};


//Template definitions. These are essentially source, so they are #ifdef'd like everything else.

#ifndef SIMMOB_DISABLE_MPI

template<class DATA_TYPE>
inline void sim_mob::PackageUtils::operator<<(DATA_TYPE& value) {
	(*package) & value;
}

#endif


}
