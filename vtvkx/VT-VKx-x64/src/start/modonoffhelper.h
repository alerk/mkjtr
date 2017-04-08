/*
 * modonoffhelper.h
 *
 *  Created on: 07.12.2015
 *      Author: zelgerpe
 */

#ifndef START_MODONOFFHELPER_H_
#define START_MODONOFFHELPER_H_
#include "CControl.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
{

class ModOnOffHelper
{
public:
	static const unsigned MOD_TA = 0x1;
	static const unsigned MOD_IT = 0x2;
	static const unsigned MOD_PT = 0x4;

	static const unsigned MOD_TARM_OFF = 0x0;
	static const unsigned MOD_TARM_REDUCED = MOD_TA;
	static const unsigned MOD_TARM_IT = MOD_IT | MOD_TA;
	static const unsigned MOD_TARM_PT = MOD_PT | MOD_TA;
	static const unsigned MOD_TARM_FULLY = MOD_PT | MOD_IT | MOD_TA;
private:
	static const unsigned nmof = 3;
	TTVModOnOffStateCctrlA modOnOffA;
	TTVModOnOffStateCctrl mof[nmof];

public:
	ModOnOffHelper() :
		modOnOffA({ 0, &mof[0] })
        {
        }
	ModOnOffHelper(unsigned PtItTa, bool force=false) :
		ModOnOffHelper()
	{
		setPtItTa(PtItTa, force);
	}

	ModOnOffHelper & operator=(const ModOnOffHelper&);
	bool checkSend(const ModOnOffHelper & o);
	static bool isTaOnRequested()  ;
	bool isTaOn() const;
	static ModOnOffStateCctrl toModOnOffState(RequestedModificationStateCctrl rsm);

private:
	bool operator ==(const ModOnOffHelper & o) const;
	void setPtItTa(unsigned PtItTa);
	void setPtItTa(unsigned PtItTa, bool force);
	bool send();

	/* return first modification with mod found in rs.modifications */
	//static const TcTTVModOnOffStateCctrl * RequestedStatusCctrl_getMod(const RequestedStatusCctrl & rs,
	//    RequestedStatusModNoCctrl mod);
	/* return first modification with mod found in as.modifications */
	//static const TTVModOnOffStateCctrl * ActualStatusCCtrl_getMod(const ActualStatusCctrl & as, TypeOfModificationCctrl mod);
};

class ModOnOffHelperStateHandler
{
private:
	ModOnOffHelper _response; //!< set by PDM as response to firmware
	ModOnOffHelper _actual; //!< actual already sent to/known by firmware
public:
	ModOnOffHelperStateHandler():
		_response {},
		_actual {}
	{
	}
	ModOnOffHelperStateHandler(unsigned PtIvTa):
		_response {PtIvTa},
		_actual {PtIvTa}
	{
	}
	void setResponse(const ModOnOffHelper &response);
	bool checkSend();
	bool isTaOnRequestedOrResponded() const;
};

}

#endif /* START_MODONOFFHELPER_H_ */
