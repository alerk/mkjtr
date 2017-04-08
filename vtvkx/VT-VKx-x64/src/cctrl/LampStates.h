/*
 * LampStates.h
 *
 *  Created on: Jan 17, 2017
 *      Author: "Valentin Chernov"
 */

#ifndef CCTRL_LAMPSTATES_H_
#define CCTRL_LAMPSTATES_H_

#include <map>

namespace sitraffic {


class LampStatus {
public:
	LampStatus():
		sgNo(0),
		greenFault(false),
		redFault(false),
		amberFault(false)
		{};
	LampStatus(	int sgNo, bool greenFault, bool redFault, bool amberFault ):
		sgNo(sgNo),
		greenFault(greenFault),
		redFault(redFault),
		amberFault(amberFault)
		{};
	~LampStatus(){};

	int getSgNo()const{return sgNo;};
	bool isGreenHeadFault()const{return greenFault;};
	bool isRedHeadFault()const{return redFault;};
	bool isAmberHeadFault()const{return amberFault;};

private:
	int sgNo;
	bool greenFault;
	bool redFault;
	bool amberFault;
};

class LampStates {
public:
typedef std::map<int, LampStatus> NrToLampStatusMap;
    LampStates();
    virtual ~LampStates();

    void setLampState(int nr, const LampStatus& ls);
    bool getLampState(int nr, LampStatus& ls);
    NrToLampStatusMap::const_iterator getConstIteratorBegin(void){return nr2ls.begin();};
    NrToLampStatusMap::const_iterator getConstIteratorEnd(void){return nr2ls.end();};

    void erase(int nr){nr2ls.erase(nr2ls.find(nr));};

private:
    NrToLampStatusMap nr2ls;

};

} /* namespace sitraffic */

#endif /* CCTRL_LAMPSTATES_H_ */
