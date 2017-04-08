/*
 * modonoffhelper.cpp
 *
 *  Created on: 07.12.2015
 *      Author: zelgerpe
 */
#include "CControl.h"
#include "cctrl/wrappedCctrlDtos.h"
#include "instregistry.h"
#include "DataReceiver.h"
#include "modonoffhelper.h"
#include "util/repdef.h"

#include "util/what.h"
DEFINE_WHATINFO;


using namespace sitraffic;

//static
ModOnOffStateCctrl ModOnOffHelper::toModOnOffState(RequestedModificationStateCctrl rsm)
{
	switch(rsm)
	{
	default:
	case      RMS_NONE:
		return MOOS_NONE;
	case      RMS_OFF:
		return MOOS_OFF;
	case      RMS_ON:
		return MOOS_ON;
	}
}

bool ModOnOffHelper::operator ==(const ModOnOffHelper & o) const
    				  {
	if(modOnOffA.TTVModOnOffStateCctrlA_len != o.modOnOffA.TTVModOnOffStateCctrlA_len)
		return false;
	if(modOnOffA.TTVModOnOffStateCctrlA_len >= nmof)
		return false;
	for(unsigned i=0; i< modOnOffA.TTVModOnOffStateCctrlA_len; i++)
	{
		if(mof[i].modType != o.mof[i].modType ||
				mof[i].sysJobId != mof[i].sysJobId ||
				mof[i].value != mof[i].value)
			return false;
	}
	return true;
    				  }
/*
 * Set PT, IV, and TA mode regarding to Requested status
 */
void ModOnOffHelper::setPtItTa(unsigned PtIvTa, bool force)
{
	unsigned i = 0;
	const RequestedStatusWCctrl & rs = sitraffic::InstRegistry::Instance().getRequestedStatus();

	auto setup = [this, PtIvTa, force, &i, rs]
				  (RequestedStatusModNoCctrl rqMod, TypeOfModificationCctrl modType, unsigned setMod)
				  {
		const TcTTVModOnOffStateCctrl *prmod = rs.getMod(rqMod);
		if(prmod)
		{
			ModOnOffStateCctrl value = MOOS_OFF;
			if((PtIvTa & setMod) == setMod)
			{
				if(force)
				{
					value = MOOS_ON;
				}
				else
				{
					value = toModOnOffState(prmod->value);
				}
			}
			else
			{
				value =  MOOS_OFF;
			}

			mof[i].modType = modType;
			mof[i].sysJobId = prmod->sysJobId;
			mof[i].value = value;
			mof[i].modNo = 0;
			++i;
		}
				  };

	setup(RSMN_VAState, TOM_ModTD, MOD_TA);
	setup(RSMN_IVState, TOM_ModTDIndividualMkjtric, MOD_IT);
	setup(RSMN_OEVState, TOM_ModPublicTransport, MOD_PT);

	modOnOffA.TTVModOnOffStateCctrlA_len = i;
}
void ModOnOffHelper::setPtItTa(unsigned PtIvTa)
{
	ModOnOffHelper::setPtItTa(PtIvTa, false);
}

bool ModOnOffHelper::send()
{
	RetCodeCctrl retCode;
	sitraffic::DataReceiver & dr(sitraffic::InstRegistry::Instance().getDataReceiver());
	CLIENT * clnt=dr.getCCtrlClient();
	enum clnt_stat ret=settaactualstatus_8(const_cast<char *>(dr.getMyHostname()), dr.getSvcPort(), modOnOffA , &retCode, clnt);
	if (ret != RPC_SUCCESS || retCode != RC_OK)
	{
		Report(RUNMODE_ERROR, "settaactualstatus_8(...%d) failed %s", retCode, clnt_sperror(clnt, ""));
		clnt_destroy(clnt);
		clnt = 0;
		return false;
	}
	else
		clnt_destroy(clnt);
	return true;
}

ModOnOffHelper & ModOnOffHelper::operator=(const ModOnOffHelper & o)
{
	for(unsigned i=0; i < o.modOnOffA.TTVModOnOffStateCctrlA_len && i<nmof; i++)
		mof[i] = o.mof[i];
	modOnOffA.TTVModOnOffStateCctrlA_len = o.modOnOffA.TTVModOnOffStateCctrlA_len;
	return *this;
}

// check for changes, if there are any, send and store o into *this
bool ModOnOffHelper::checkSend(const ModOnOffHelper & o)
{
	bool sent = false;
	if(! this->operator ==(o))
	{
		sent = const_cast<ModOnOffHelper &>(o).send();
		if(sent)
		{
			this->operator =(o);
		}
	}
	return sent;
}

// static
bool ModOnOffHelper::isTaOnRequested()
{
	const TcTTVModOnOffStateCctrl * pmod=InstRegistry::Instance().getRequestedStatus().getMod(RSMN_VAState);
	return pmod && pmod->value == RMS_ON;
}

/** @return true if this ModOnOffHelper contains a  TOM_ModTD with value MOOS_ON, or contains no TOM_ModTD at all. */
bool ModOnOffHelper::isTaOn() const
{
  //for (TTVModOnOffStateCctrl m : mof)
  for(unsigned i=0; i < modOnOffA.TTVModOnOffStateCctrlA_len && i<nmof; i++)
    {
      const TTVModOnOffStateCctrl & m(mof[i]);
      if (m.modType == TOM_ModTD)
        return m.value == MOOS_ON;
    }
  return true;
}

void ModOnOffHelperStateHandler::setResponse(const ModOnOffHelper &response)
{
    Report(RUNMODE_DEBUG, "isTaOn=%d", response.isTaOn());
	_response = response;
}
bool ModOnOffHelperStateHandler::checkSend()
{
	bool sent = false;
	sent = _actual.checkSend(_response);
	return sent;
}

/**
 * determine if TA process should keep on running.
 * @return true if traffic actuation ON is requested OR traffic actuation ON was reported, last passed to setRespond()
 */
bool ModOnOffHelperStateHandler::isTaOnRequestedOrResponded() const
{
    Report(RUNMODE_DEBUG, "isTaOnRequested=%d _response.isTaOn=%d",  ModOnOffHelper::isTaOnRequested(), _response.isTaOn());
    return ModOnOffHelper::isTaOnRequested() || _response.isTaOn();
}
