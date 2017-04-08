/*
 * Archive.cpp
 *
 *  Created on: Jan 17, 2017
 *      Author: "Valentin Chernov"
 */

#include "Archive.h"
#include "LampStates.h"
//#include "CControl.h"

#include "start/awpWrapper.h"
#include "instregistry.h"
#include "cctrl/Config.h"
#include "cctrl/wrappedCctrlDtos.h"
#include "util/repdef.h"

#include "util/what.h"
DEFINE_WHATINFO;

namespace sitraffic {

Archive::Archive():
		youngestLampStates()
{
	// TODO Auto-generated constructor stub

}

Archive::~Archive() {
	// TODO Auto-generated destructor stub
}

CLIENT * Archive::fill(CLIENT *clnt)
{
	CLIENT *_clnt = clnt;

	if(!_clnt)
      return _clnt;


	_clnt = fillLampStates(_clnt);

	return _clnt;
}

CLIENT * Archive::fillLampStates(CLIENT *clnt)
{
	SelectedConfig sc;
	memset(&sc, 0, sizeof sc); // !! necessary to avoid heap corruption by xdr_array !!

	enum clnt_stat ret = getselectedcfg_8(CS_CfgSgInfo2CCtrl, &sc, clnt);
	if (ret != RPC_SUCCESS)
	{
		Report(CFG_ERROR, "%s", clnt_sperror(clnt, "getselectedcfg_8 CS_CfgSgInfo2CCtrl call failed"));
		clnt_destroy(clnt);
		return 0;
	}
	else
	{
		std::map<int,bool> toRemove;

		LampStates::NrToLampStatusMap::const_iterator cit = youngestLampStates.getConstIteratorBegin();

		for(; cit!=youngestLampStates.getConstIteratorEnd(); cit++)
		{
			toRemove[cit->first]=true;
		}

		// store empty LampStatusWCctrl table
		for (unsigned i = 0; i < sc.SelectedConfig_u.sgInfos2.sgInfos2_len; i++)
		{
			const CfgSgInfo2CCtrl * psgInfo =
					&sc.SelectedConfig_u.sgInfos2.sgInfos2_val[i];

			const LampStatus lampStatus(psgInfo->sgNo, false,false,false);

			youngestLampStates.setLampState(psgInfo->sgNo, lampStatus );
			toRemove[psgInfo->sgNo]=false;
		}

		// remove not now filled instances
		for(std::map<int, bool>::const_iterator it=toRemove.begin(); it!=toRemove.end(); it++)
			if(it->second)
				youngestLampStates.erase(it->first);

		updateLampStates();
	}

	clnt_freeres(clnt, (xdrproc_t) xdr_SelectedConfig, (caddr_t)&sc);
	return clnt;
}

void Archive::onNewValues(enum ArchiveSelectorCctrl selector)
{
	Report(RUNMODE_EVENT,"Archive::onNewValues");

	switch(selector)
	{
	case AS_LampStates:
		updateLampStates();
		break;
	default:
		break;
	}
}

void Archive::updateLampStates(void)
{


	ArchiveResultCctrl arc;

	//TODO Ask Siemens about possible memory leakes in "arc" argument
	SLONG ret = sitraffic::InstRegistry::Instance().getAwpWrapper().archiveGetYoungestBySelector(arc, AS_LampStates);

	if(ret != SUCCESS)
	{
		return;
	}

	int len = arc.ArchiveResultCctrl_u.dto.ArchiveDtoCctrl_u.lsDto.lampStates.lampStates_len;
	for (int i=0;i<len;i++)
	{
		const LampStatusWCctrl &lampState =  (LampStatusWCctrl&)arc.ArchiveResultCctrl_u.dto.ArchiveDtoCctrl_u.lsDto.lampStates.lampStates_val[i];
		const LampStatus lampStatus(lampState.getSgNo(), lampState.isGreenHeadFault(), lampState.isRedHeadFault(), lampState.isAmberHeadFault() );
		youngestLampStates.setLampState(lampState.getSgNo(), lampStatus);
	}

	//next line removes possible memory leak
	clnt_freeres(sitraffic::InstRegistry::Instance().getAwpWrapper().getFwUdpClient(), (xdrproc_t) xdr_ArchiveResultCctrl, (caddr_t)&arc);

	Report(RUNMODE_EVENT,"updateLampStates SUCCESS");

}

} /* namespace sitraffic */
