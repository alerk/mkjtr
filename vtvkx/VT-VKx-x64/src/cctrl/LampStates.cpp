/*
 * LampStates.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: "Valentin Chernov"
 */

#include "LampStates.h"

namespace sitraffic {

LampStates::LampStates() {
    // TODO Auto-generated constructor stub

}

LampStates::~LampStates() {
    // TODO Auto-generated destructor stub
}

void LampStates::setLampState(int nr, const LampStatus& ls)
{
	NrToLampStatusMap::iterator it = nr2ls.find(nr);
	if(it != nr2ls.end())
		it->second = ls;
	else
		nr2ls[nr]=ls;
}

bool LampStates::getLampState(int nr,LampStatus& ls)
{
	if(nr2ls.find(nr) != nr2ls.end())
	{
		ls = nr2ls[nr];
		return true;
	}
	return false;
}


} /* namespace sitraffic */
