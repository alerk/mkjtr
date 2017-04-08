#include <sstream>
#include <iostream>

#include "CControl.h"
#include "start/awpWrapper.h"
#include "instregistry.h"
#include "cctrl/Config.h"
#include "cctrl/wrappedCctrlDtos.h"
#include "util/repdef.h"

#include "start/modonoffhelper.h"
#include "start/api_TaRunmode.h"

#include "util/what.h"
DEFINE_WHATINFO;

using namespace sitraffic;
using namespace std;

#define TO_MAP_ELEMENT(str) {str, #str}

static map<TaRunmode,string> taRunmodeStrings =
{
		TO_MAP_ELEMENT(TARM_OFF),
		TO_MAP_ELEMENT(TARM_REDUCED),
		TO_MAP_ELEMENT(TARM_IT),
		TO_MAP_ELEMENT(TARM_PT),
		TO_MAP_ELEMENT(TARM_FULLY),
};


static string getRunmodeString(TaRunmode rm)
{
	string& runmodeString = taRunmodeStrings.at(rm);

	return runmodeString;
}
/*
 * TaRunmode	Mod TA 	Mod IT	Mod PT
 * TARM_OFF 	0		x		x
 * TARM_REDUCED	1		0		0
 * TARM_IT		1		1		0
 * TARM_PT		1		0		1
 * TARM_FULLY	1		1		1
 */


/**
 * Get requested (by firmware) TA running mode
 *
 * @return    TA running mode
 */
TaRunmode getRequestTaRunmode()
{
	TaRunmode requestMode = TARM_OFF;

	const RequestedStatusWCctrl & rs = InstRegistry::Instance().getRequestedStatus();
	const TcTTVModOnOffStateCctrl * modTa = rs.getMod(RSMN_VAState);
	const TcTTVModOnOffStateCctrl * modPt = rs.getMod(RSMN_OEVState);
	const TcTTVModOnOffStateCctrl * modIv = rs.getMod(RSMN_IVState);

	if( modTa->value == RMS_OFF)
	{
		requestMode = TARM_OFF;
	}
	else
	{
		if     (modIv->value == RMS_OFF && modPt->value == RMS_OFF)
		{
			requestMode = TARM_REDUCED;
		}
		else if(modIv->value == RMS_ON  && modPt->value == RMS_OFF)
		{
			requestMode = TARM_IT;
		}
		else if(modIv->value == RMS_OFF && modPt->value == RMS_ON)
		{
			requestMode = TARM_PT;
		}
		else
		{
			requestMode = TARM_FULLY;
		}
	}

	return requestMode;
}

/**
 * check pmod for NULL, access value.
 * @param pmod
 * @return MOOS_OFF if pmod==0, else pmod->value
 */
static ModOnOffStateCctrl checkNull(const TTVModOnOffStateCctrl *pmod)
  {
    return pmod==0?MOOS_OFF:pmod->value;
  }

/**
 * Get actual (in firmware) TA running mode
 *
 * @return    TA running mode
 */
TaRunmode getActualTaRunmode()
{
	TaRunmode actualMode = TARM_OFF;

	const ActualStatusWCctrl & as = InstRegistry::Instance().getActualStatus();
        const ModOnOffStateCctrl modTa = checkNull(as.getMod(TOM_ModTD));
        const ModOnOffStateCctrl modPt = checkNull(as.getMod(TOM_ModPublicTransport));
	const ModOnOffStateCctrl modIv = checkNull(as.getMod(TOM_ModTDIndividualMkjtric));

	if( modTa == MOOS_OFF)
	{
		actualMode = TARM_OFF;
	}
	else
	{
		if     ( modIv == MOOS_OFF &&  modPt == MOOS_OFF)
		{
			actualMode = TARM_REDUCED;
		}
		else if(modIv == MOOS_ON  && modPt == MOOS_OFF)
		{
			actualMode = TARM_IT;
		}
		else if(modIv == MOOS_OFF && modPt == MOOS_ON)
		{
			actualMode = TARM_PT;
		}
		else
		{
			actualMode = TARM_FULLY;
		}
	}

	return actualMode;
}
/**
 * Convert api_va.h running mode to ModOnOffHelper running mode
 *
 * @param[in] runMode api_va.h TA running mode
 * @return    ModOnOffHelper TA running mode
 */
unsigned taRunModeToMod(TaRunmode runMode)
{
	static map<TaRunmode, unsigned> taRunmodeMap = {
			{TARM_OFF, ModOnOffHelper::MOD_TARM_OFF},
			{TARM_REDUCED, ModOnOffHelper::MOD_TARM_REDUCED},
			{TARM_IT, ModOnOffHelper::MOD_TARM_IT},
			{TARM_PT, ModOnOffHelper::MOD_TARM_PT},
			{TARM_FULLY, ModOnOffHelper::MOD_TARM_FULLY},
	};

	unsigned mod = ModOnOffHelper::MOD_TARM_OFF;

	auto it = taRunmodeMap.find(runMode);
	if(it != taRunmodeMap.end())
	{
		mod = it->second;
	}

	return mod;
}


/**
 * api_va.h, Get requested (by firmware) TA running mode
 *
 * @return    TA running mode
 */
TaRunmode tacGetRequestTaRunmode(void)
{
	TaRunmode requestMode = TARM_OFF;

	requestMode = getRequestTaRunmode();

	Report(RUNMODE_INFO, "return = %d = %s", requestMode, getRunmodeString(requestMode).c_str());

	return requestMode;
}

/**
 * api_va.h, Get requested (by firmware) TA running mode
 *
 * In general tacGetModifiedRequestTaRunmode() is equal to tacGetRequestedTaRunmode()with one exception:
 * If TA is running in FULLY/IT/PT mode and requested status is “OFF”, the function getModifiedRequestTaRunmode() returns “REDUCED” as (modified) requested mode as long as TA is also returning “REDUCED” as current state.
 * When TA finally returns “REDUCED” mode, function forwards original requested mode “OFF” to TA when called.
 * (TA can not be switched from FULLY to ON directly, but FULLY->REDUCED->OFF).
 *
 * @return    TA (modified) running mode
 */
TaRunmode tacGetModifiedRequestTaRunmode(void)   // TA-off via TA-reduced
{
	TaRunmode modifiedMode = TARM_OFF;

	const TaRunmode requestMode = tacGetRequestTaRunmode();
	const TaRunmode actualMode = getActualTaRunmode();

	if( requestMode == TARM_OFF && (actualMode == TARM_FULLY || actualMode == TARM_IT || actualMode==TARM_PT) )
	{
		modifiedMode = TARM_REDUCED;
	}
	else
	{
		modifiedMode = requestMode;
	}

	Report(RUNMODE_INFO, "return = %d = %s", modifiedMode, getRunmodeString(modifiedMode).c_str());

	return modifiedMode;
}


/**
 * api_va.h, Set response (processed) running mode to firmware
 *
 * @param[in] rm TA running mode to set in firmware
 */
void tacSetResponseTaRunmode(TaRunmode rm)
{
	Report(RUNMODE_DEBUG, "TaRunmode = %d", (int)rm);
	sitraffic::ModOnOffHelperStateHandler & moohsh = InstRegistry::Instance().getModOnOffHelperStateHandler();

	unsigned mod = taRunModeToMod(rm);
	ModOnOffHelper helper(mod, true);

	moohsh.setResponse(helper);
	Report(RUNMODE_DEBUG, "TaRunmode %s() called with value %d mod=%u", __FUNCTION__, (int)rm, mod);

}

/**
 * (JMX) HTML interface to test implementation of api_va.h functions
 *
 * @param[in] ehr TA running mode to set in firmware
 * @return    Generated HTML (JMX) string
 */
string TaRunmodeHtmlRequestHandler::doRequest(EvHttpRequest & ehr)
{


	{
		// tacSetResponseTaRunmode() execution
		auto & pmap(ehr.getParams());
		auto rm=pmap.find("rm");

		if(rm != pmap.end())
		{
			const char *vRm = rm->second;
			if(vRm != nullptr && strlen(vRm) > 0)
			{
				TaRunmode mode = static_cast<TaRunmode>(stoi(vRm, nullptr, 0));
				tacSetResponseTaRunmode(mode);
			}
		}
	}
	ostringstream ss;
	{
		ss << "<a href=\"/\">Back</a>" << "<hr>" << endl;
	}
	{
		// print table of values
		ss << "<table border=\"1\">" << endl;
		ss << "<tr>" << endl;
		ss << "<td colspan=\"2\" align=\"center\"><b>TaRunmode</b></td>" << endl;
		ss << "</tr>" << endl;
		ss << "<tr>" << endl;
		ss << "<td>Decimal</td><td>Enumeration</td>" << endl;
		ss << "</tr>" << endl;
		for (auto element:taRunmodeStrings)
		{
			ss << "<tr>" << endl;
			ss << "<td align=\"right\">" << (int)element.first << "</td><td>" << element.second << "</td>" << endl;
			ss << "</tr>" << endl;
		}
		ss << "</table>" << endl;
		ss << "<hr>" << endl;
	}
	{
		// tacGetRequestTaRunmode()
		TaRunmode mode = tacGetRequestTaRunmode();

		ss << "tacGetRequestTaRunmode: ";
		ss << (int)mode << " (" << getRunmodeString(mode) << ")<br>" << endl;
		ss << "<hr>" << endl;
	}
	{
		// tacGetModifiedRequestTaRunmode()
		TaRunmode mode = tacGetModifiedRequestTaRunmode();
		ss << "tacGetModifiedRequestTaRunmode: ";
		ss << (int)mode << " (" << getRunmodeString(mode) << ")<br>" << endl;
		ss << "<hr>" << endl;
	}
	{
		// tacSetResponseTaRunmode()
		ss << "<form action=\"" << TaRunmodeHtmlRequestHandler::NAME << "\" id=\"SetResponseTaRunmodeId\" >" << endl;
		ss << " <label id=\"l1\" form=\"SetResponseTaRunmodeId\">rm</label>" << endl
				<< " <input type=\"number\" id=\"rm\" name=\"rm\" maxlength=\"3\" value=\"\">" << endl
				<< " <button type=\"submit\">tacSetResponseTaRunmode(rm)</button>" << endl;
		ss << "</form>" << endl;
	}

	return ss.str();
}

/**
 * (JMX) get path to html page
 *
 * @return    Path to HTML (JMX) page
 */
string TaRunmodeHtmlRequestHandler::getPath()
{
	ostringstream ss;

	ss << "/" << TaRunmodeHtmlRequestHandler::NAME;

	return ss.str();
}

