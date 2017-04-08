/*
 * awpWrapper.cpp
 *
 *  Created on: 01.02.2016
 *      Author: zelgerpe
 */

#include "start/awpWrapper.h"
#include "instregistry.h"
#include "DataReceiver.h"
#include "cctrl/Config.h"
#include "cctrl/cltDeviceValues.h"
#include "cctrl/wrappedCctrlDtos.h"
#include "cctrl/PublicTransport.h"
#include "statistics.h"
#include "util/repdef.h"
#include <util/StateMessage.h>
#include "evhttpadapter.h"
#include <chrono>
#include "cctrl/api_operating.h"//by Valentin

#include "util/what.h"
DEFINE_WHATINFO;

int sitraffic::AwpWrapper::TaCallOffsetMs=0;
int sitraffic::AwpWrapper::TaCallIntervallMs = 1000;


namespace sitraffic {
class AwpRequestHandler : public EvRequestHandler
{
	Histogramm<> jitterHistogramm;
	Histogramm<unsigned> callDurationHistogramm;
	bool isFirst;
public:
	AwpRequestHandler() : jitterHistogramm(1), callDurationHistogramm(5), isFirst(true) {}
	void addJitter(int j) { if(isFirst) isFirst=false; else jitterHistogramm.addValue(j/1000); }
	void addCallDuration(unsigned cd) { callDurationHistogramm.addValue(cd/1000); }
	virtual std::string
	doRequest(EvHttpRequest & ehr)
	{
		std::ostringstream ss;
		auto & pmap(ehr.getParams());
		if(pmap.find("clear")!=pmap.end())
		{
			ss << "<center>Histogramms cleared</center>" << std::endl;
			jitterHistogramm.clear(); callDurationHistogramm.clear();
		}
		else
		{
			ss << "<p>number of TA executions called versus delay jitter [ms]</p>" << std::endl;
			// generate page that displays the jitter histogram as bar chart by using Chart.js, see http://www.chartjs.org/
			// load Chart.js from lighttpd of device, that is where firmware is located, we (tac) are running at a different port
			ss << "<script language=\"JavaScript\">" << std::endl
					<< " document.write('<script src=\"http://' + window.location.hostname + '/js/chart/Chart.js\"></' + 'script>' );" << std::endl
					<< "</script>"
					<< "<div style=\"width: 70%\">"
					<< "<canvas id=\"canvas\" height=\"450\" width=\"600\"></canvas>"
					<< "</div>" << std::endl;
			ss << "<script>" << std::endl
					<< " var barChartData = {" << std::endl
					<< "  labels : [";
			jitterHistogramm.printBinsCsv(ss); ss << "]," << std::endl;
			ss << "  datasets: [" << std::endl
					<< "  { label: \"jitter us\"," << std::endl
					<< "    data : [";
			jitterHistogramm.printCountsCsv(ss); ss <<  "]" << std::endl;
			ss << "  } ] }" << std::endl;
			// add call DurationHistogramm data
			ss << " var callDurationHistogrammData = {" << std::endl
					<< "  labels : [";
			callDurationHistogramm.printBinsCsv(ss); ss << "]," << std::endl;
			ss << "  datasets: [" << std::endl
					<< "  { label: \"duration us\"," << std::endl
					<< "    fillColor : \"rgba(151,187,205,0.5)\"," << std::endl
					<< "    data : [";
			callDurationHistogramm.printCountsCsv(ss); ss <<  "]" << std::endl;

//			ss <<      "	options: {" << std::endl
//					<< "			   scales: { " << std::endl
//					<< "	                     yAxes: [{ " << std::endl
//					<< "	                               type: \"logarithmic\" " << std::endl
//					<< "	                           }] " << std::endl
//					<< "	                   } " << std::endl
//					<< "	         } " << std::endl;

			ss << "  } ] }" << std::endl;
			// generate onload function that instantiates both histograms
			ss << "window.onload = function(){" << std::endl
					<< "    var ctx = document.getElementById(\"canvas\").getContext(\"2d\");" << std::endl
					<< "    var ctxCD = document.getElementById(\"canvasCD\").getContext(\"2d\");" << std::endl
					<< "    window.myBar = new Chart(ctx).Bar(barChartData, {"<< std::endl
					<< "            responsive : true" << std::endl
					<< "    });"<< std::endl
					<< "    window.mycallDurationHistogramm = new Chart(ctxCD).Bar(callDurationHistogrammData, {"<< std::endl
					<< "            responsive : true" << std::endl
					<< "    });"<< std::endl

					<< "}" << std::endl;
			ss << "</script>" << std::endl;

			ss << "<center>" << jitterHistogramm.toString() << "</center>";

			ss << "<p>number of TA executions versus call duration [ms]</p>" << std::endl;
			// generate page that displays the jitter histogram as bar chart by using Chart.js, see http://www.chartjs.org/
			// load Chart.js from lighttpd of device, that is where firmware is located, we (tac) are running at a different port
			ss << "<div style=\"width: 70%\">"
					<< "<canvas id=\"canvasCD\" height=\"450\" width=\"600\"></canvas>"
					<< "</div>" << std::endl;

			ss << "<center>" << callDurationHistogramm.toString() << "</center>";

		}
		return ss.str();
	}

};

// AwpSettings, provide editing of TaCallOffsetMs for test purpose
class AwpWrapper::AwpSettings : public EvRequestHandler
{
	virtual std::string doRequest(EvHttpRequest & ehr)
	{
		const char *testSecondSlip="testSecondSlip";
		const char *cbIsMitigate="isMitigate";
		std::ostringstream ss;
		AwpWrapper & aw(InstRegistry::Instance().getAwpWrapper());
		auto & pmap(ehr.getParams());
		auto it=pmap.find("TaCallOffsetMs");
		if(it!=pmap.end())
		{
			AwpWrapper::TaCallOffsetMs = (int)strtoul(it->second, 0,0);
		}
		if(pmap.find(testSecondSlip)!=pmap.end())
		{
			aw.nextPlannedCall = aw.getTickOfNextCall(aw.nextPlannedCall);
			//ss << "slipped<br/>" << std::endl;
		}
		if((it=pmap.find(cbIsMitigate))!=pmap.end())
			aw.isMitigate = (strcmp("on", it->second)==0);


		ss << "<form action=\"Settings\" id=\"AwpSettings\" >";
		ss << " <label id=\"l1\" form=\"jitterBinSize\">TaCallOffsetMs</label>" << std::endl
				<< " <input type=\"number\" id=\"TaCallOffsetMs\" name=\"TaCallOffsetMs\" maxlength=\"3\" value=\""<< AwpWrapper::TaCallOffsetMs <<  "\"/>" << std::endl
				<< " <p>mitigate</p><fieldset><input type=\"radio\" name=\"" << cbIsMitigate << "\" value=\"on\"" << (aw.isMitigate?"checked":"") << "><label for=\"on\">on</label>" << std::endl
				<< "<input type=\"radio\" name=\"" << cbIsMitigate << "\" value=\"off\""<< (aw.isMitigate?"":"checked") << "><label for=\"off\">off</label> </fieldset>"
				<< " <button type=\"submit\">set</button>" << std::endl;
		ss << "</form>" << std::endl;
		ss << "<hr/><form action=\"Settings\" id=\"AwpSettings\" >"
				<< " <button type=\"submit\" name=\"" << testSecondSlip <<"\" value=\"1\">slip one second</button>" << std::endl;
		ss << "</form>" << std::endl;
		return ss.str();
	}
};

}



sitraffic::AwpWrapper::AwpWrapper(pAwpFunction_t pinit, pAwpFunction_t pfunction, bool withMitigation)
: init(pinit), function(pfunction), tickOfLastCall(-1),
  //    lastSentTick(0),
  nextTakeEffectTick(0), nextPlannedCall(-1), clnt(0), statusMods(), tpOfLastCall(), lastTaCallDuration(0), lastTaCallJitter(0), pAwpJmx(0),
  sgSwitchingEvents(), pinsert(sgSwitchingEvents.begin()), pLastSent(sgSwitchingEvents.begin()),
  initiatorP1({MPTI_INT64, 0}), sinitiator({ const_cast<char *>("pdmx%d"), { 1, &initiatorP1 } }),
  isMitigate(withMitigation), txMitigate(-1), txTaLastCalled(-1), sysJobId(0xc8400000)
{
}

// adjust times
//void sitraffic::AwpWrapper::getSwitchingEvents4Refresh(ExternalSgSwitchingEventArray & searray)
//  {
//     searray =
//      { static_cast<u_int>(pLastSent - sgSwitchingEvents.begin()), sgSwitchingEvents.begin() };
//    for(unsigned i=0; i<searray.ExternalSgSwitchingEventArray_len; i++)
//      {
//        searray.ExternalSgSwitchingEventArray_val[i].startTickTime = nextTakeEffectTick;
//        searray.ExternalSgSwitchingEventArray_val[i].initiator = sinitiatorRefresh;
//      }
//    pinsert=pLastSent;
//  }



void sitraffic::AwpWrapper::onSignalIndication(const SignalIndicationsWCctrl & si)
{
	if(this->tickOfLastCall<0)
	{
	    if(!InstRegistry::Instance().isReady4Init())
	      {
	        Report(AWP_WARNING, "waiting for isReady4Init");
	        return;
	      }
	    // set TaCallIntervallMs from configured system pulse millies
	    TaCallIntervallMs = InstRegistry::Instance().getConfig().getSystemPulseMs();
	    if(TaCallIntervallMs!=1000)
	      Report(AWP_WARNING, "set TaCallIntervallMs to SystemPulseMs=%d", TaCallIntervallMs);

		(*init)();
		Report(AWP_EVENT, "init called");
		// fake initial tickOfLastCall to get next call at desired tick
		nextPlannedCall = getTickOfNextCall(si.baseTicktime);
		tickOfLastCall = nextPlannedCall-TaCallIntervallMs;
		tpOfLastCall = usedClock::now();
		onAwpInitCalled();
	}
	else
	{
		if (si.baseTicktime >= nextPlannedCall)
		{
                    const ActualStatusWCctrl& as = sitraffic::InstRegistry::Instance().getActualStatus();
			tickOfLastCall=si.baseTicktime;
			nextTakeEffectTick = getTickOfNextTakeEffect();
			if(si.baseTicktime != nextPlannedCall)
			{
			    Report(AWP_WARNING, "%" PRId64" ms Tick slip expectedTick=%" PRId64" gotTick=%" PRId64, si.baseTicktime-nextPlannedCall, nextPlannedCall, si.baseTicktime);
				SSHORT paras[]={ VA_MESSAGE_Tick_Slip_detected, static_cast<SSHORT>(si.tx/100), static_cast<SSHORT>((si.baseTicktime-nextPlannedCall)/100) };
				//TabuMeldungAbsetzen (VA_MELDUNG, &paras[0], 3);
				sitraffic::UpdateCltMessage(VA_MELDUNG, &paras[0], 3, SMS_WARNING, VA_MESSAGE_Tick_Slip_detected_format);
			}
			else
			{
				SSHORT paras[]={ VA_MESSAGE_Tick_Slip_detected };
				sitraffic::UpdateCltMessage(VA_MELDUNG, &paras[0], 1, SMS_WARNING, NULL);
			}

			nextPlannedCall = getTickOfNextCall(si.baseTicktime);
			const bool lastSiSwitched=checkLastSiSwitched(si); // invoke always to get the switching event not realized message even if mitigation is off
			if(isMitigate)
			  {
                          if(!lastSiSwitched)
                            {
                              if(txMitigate<0) // start new mitigation with
                                {
                                  const Config& cfg(InstRegistry::Instance().getConfig());
                                  txMitigate = cfg.roundDownToSystemPulse(cfg.getNextProbableTxMs(as.signalProgram, txTaLastCalled));//si.tx - (si.tx % TaCallIntervallMs); // round tx down to whole system clock
                                }
                              //set tx back to txMitigate at nextTakeEffectTick
                              setTx(static_cast<int>(txMitigate), "tacMitigate");

                              return;   // mitigation, avoid next switchings prior last events are realized
                            }
                          else if(txMitigate>=0)
                            {
                              // we are still mitigating, waiting for tx to reach the value we have set previously
                              if(std::abs(si.tx - txMitigate) >= TaCallIntervallMs)
                                {
                                  setTx(static_cast<int>(txMitigate), "tacMitigateEnd");
                                  return;
                                }
                              Report(AWP_EVENT, "end mitigation si.tx=%" PRId64, si.tx);
                              txMitigate=-1; // end of evtl. mitigation
                            }
			  }

			if(as.signalProgram <= 0 && as.intersectionOnOffState == IS_ON)  // pdm will probably hang-up in TL code
			{
	          Report(AWP_ERROR|APP_HISTORY, "NO_signalProgram!!! globalFaultState:%d operatingMode:%d intersectionOnOffState:%d",
	                                                             as.globalFaultState, as.operatingMode, as.intersectionOnOffState);
	          // return;  ?
			}

			usedClock::time_point preFunction=usedClock::now();
			int ret=(*function)();
			usedClock::time_point postFunction=usedClock::now();
			std::chrono::microseconds duration=std::chrono::duration_cast<std::chrono::microseconds>(postFunction-preFunction);
			std::chrono::microseconds timeSinceLastCallJitter = std::chrono::duration_cast<std::chrono::microseconds>(preFunction-tpOfLastCall-std::chrono::milliseconds((int64_t)TaCallIntervallMs));
			txTaLastCalled=si.tx;
			tpOfLastCall = preFunction;
			lastTaCallDuration=static_cast<uint32_t>(duration.count());
			lastTaCallJitter=static_cast<int32_t>(timeSinceLastCallJitter.count());
			pAwpJmx->addJitter(lastTaCallJitter);
			pAwpJmx->addCallDuration(lastTaCallDuration);
			doTaExecutionDurationWarn(si);
			Report(AWP_DEBUG, "awpFunction tick=%" PRId64" tx=%" PRId64" returned %d took %" PRId64" us timeSinceLastCallJitter=%" PRId64" us", si.baseTicktime, si.tx, ret, duration.count(), timeSinceLastCallJitter.count());
			onAwpFunctionCalled();
			{

				// set VAState based on return value of AWP_Funktion
				ModOnOffHelperStateHandler & moohsh = InstRegistry::Instance().getModOnOffHelperStateHandler();
				moohsh.checkSend();

				if(!moohsh.isTaOnRequestedOrResponded() || EvHttpServer::isStopped()){
				        Report(AWP_EVENT, "stopping process due to %s !", EvHttpServer::isStopped()?" signal":"no more TA ON requested, responded");
					moohsh.setResponse(ModOnOffHelper(ModOnOffHelper::MOD_TARM_OFF));
					moohsh.checkSend();
					//exit(1);
					svc_exit();
				}
			}
		}
	}
}
// check lastTaCallDuration, warn if 300ms exceeded
void sitraffic::AwpWrapper::doTaExecutionDurationWarn(const SignalIndicationsWCctrl & si) const
{
	if(lastTaCallDuration >= 300000)
	{
		SSHORT paras[]={ VA_MESSAGE_TaExecutionDurationWarn, static_cast<SSHORT>(si.tx/100), static_cast<SSHORT>(lastTaCallDuration/1000) };
		// "message %d. No.: %d TX=%d: TA execution duration=%d ms"
		sitraffic::UpdateCltMessage(VA_MELDUNG, &paras[0], 3, SMS_WARNING, VA_MESSAGE_TaExecutionDurationWarn_format);
	}
	else
	{
		SSHORT paras[]={ VA_MESSAGE_TaExecutionDurationWarn };
		sitraffic::UpdateCltMessage(VA_MELDUNG, &paras[0], 1, SMS_WARNING, NULL);
	}
}

bool sitraffic::AwpWrapper::checkLastSiSwitched(const SignalIndicationsWCctrl & si)
{   // check if last switching events are realized
	ExternalSgSwitchingEventArray searray =
	{ static_cast<u_int>(pLastSent - sgSwitchingEvents.begin()), sgSwitchingEvents.begin() };
	int para[3];
	if( ! InstRegistry::Instance().getConfig().isLastSiSwitched(searray, si, &para[0]))
	{
		Report(AWP_WARNING, "TX=%" PRId64"/%" PRId64" switchingEvents not realized as expected, sg:%d expectedColor:%d gotColor:%d !", si.tx, si.baseTicktime, para[0], para[1], para[2]);
		SSHORT paras[]={ VA_MESSAGE_switchingEvents_not_realized_as_expected, static_cast<SSHORT>(si.tx/100),
		    static_cast<SSHORT>(para[0]), static_cast<SSHORT>(para[1]), static_cast<SSHORT>(para[2]) };
		//TabuMeldungAbsetzen (VA_MELDUNG, &paras[0], 2); // be aware that this causes a network roundtrip to firmware
		sitraffic::UpdateCltMessage(VA_MELDUNG, &paras[0], sizeof(paras)/sizeof(paras[0]), SMS_WARNING, VA_MESSAGE_switchingEvents_not_realized_as_expected_format);
		return false;
	}
	else
	{
		SSHORT paras[]={ VA_MESSAGE_switchingEvents_not_realized_as_expected };
		sitraffic::UpdateCltMessage(VA_MELDUNG, &paras[0], 1, SMS_WARNING, NULL);
	}
	return true;
}

// start tick next signal indications are for
// based on tickOfLastCall
// @return tick of next second change
int64_t sitraffic::AwpWrapper::getTickOfNextTakeEffect() const
{
	return tickOfLastCall + (TaCallIntervallMs - tickOfLastCall % TaCallIntervallMs);
}
int64_t sitraffic::AwpWrapper::getTickOfNextCall(int64_t tickNow) const
{
	return tickNow - tickNow % TaCallIntervallMs + TaCallIntervallMs + TaCallOffsetMs;
}

void sitraffic::AwpWrapper::onAwpInitCalled()
{
	nextTakeEffectTick = getTickOfNextTakeEffect();
	registerDeviceValues();
	sitraffic::InstRegistry & ir(sitraffic::InstRegistry::Instance());
	ir.getEvHttpServer().add("/AwpWrapper", pAwpJmx=new AwpRequestHandler()); // EvHttpServer takes over memory responsibility
	ir.getEvHttpServer().add("/AwpWrapper/Settings", new AwpSettings()); // EvHttpServer takes over memory responsibility
	ir.getCltDeviceValueProviders().registerAtCctrl(ir.getDataReceiver());
	// updateStateMessages(FALSE);  --> better after first call of awp function

	//  MODOFF
		ModOnOffHelperStateHandler & moohsh = InstRegistry::Instance().getModOnOffHelperStateHandler();
		moohsh.setResponse(ModOnOffHelper(ModOnOffHelper::MOD_TARM_FULLY));
}

static int awpFunctionCallCount = 0;

// nextStartTick already points to nextStart that is assumed to be the time when this switching events shall occur
void sitraffic::AwpWrapper::onAwpFunctionCalled()
{
	awpFunctionCallCount++;
	// send all stored switching events
	ExternalSgSwitchingEventArray searray =
	{ static_cast<u_int>(pinsert - sgSwitchingEvents.begin()), sgSwitchingEvents.begin() };

	// if nothing to send, check for refresh,
	// currently refresh is done by PDM.
	// if it should be done at TAC kernel, we would need to keep track of switching events per signal group
	//if(searray.ExternalSgSwitchingEventArray_len==0 && (nextStartTick-lastSentTick)>SWITCHING_EVENT_REFRESH_INTERVAL)
	//   getSwitchingEvents4Refresh(searray);

	if (!clnt)
		clnt = InstRegistry::Instance().getDataReceiver().getCCtrlClient();

	// only send if there are any events in buffer
	if (searray.ExternalSgSwitchingEventArray_len)
	{

		if(clnt)
		{
		    Report(AWP_DEBUG, "sending %d SG switching events", searray.ExternalSgSwitchingEventArray_len);
			clnt_stat cs = adddesiredsgswitchingevents_8(searray, NULL, clnt);
			if (cs != RPC_SUCCESS)
			{
			    Report(AWP_ERROR, "adddesiredsgswitchingevents_8(..) failed %s", clnt_sperror(clnt, ""));
				clnt_destroy(clnt);
				clnt = 0;
			}
		}
		// reset pinsert, adjust nextStartTick
		//lastSentTick = nextStartTick; only necessary if TAC kernel handles refresh
		pLastSent = pinsert;
		pinsert = sgSwitchingEvents.begin();
	}

	InstRegistry::Instance().getPublicTransport().sendAmlis();

	clnt = InstRegistry::Instance().getCltDeviceValueProviders().sendValues(clnt, nextTakeEffectTick);

	if(awpFunctionCallCount == 1)
	  initStateMessages();
}

/*static*/ const DEVICE_VALUE_TYPE_TAC sitraffic::AwpWrapper::sDevVar[] = {
		{sitraffic::AwpWrapper::getDeviceValue, "lTaCallDur", "lastTaCallDuration", "duration of last traffic actuation call in milliseconds", 60, 850, TADVI_Fd, 0, 0, 0, 0, -1 },
		{sitraffic::AwpWrapper::getDeviceValue, "lTaCallJit", "lastTaCallJitter", "deviation of Call Interval duration from TaCallIntervallMs of last traffic actuation call in milliseconds", 60, 851, TADVI_Fd, 0, 0, 0, 0, -1 }
};

//static
bool sitraffic::AwpWrapper::isEqual(ULONG oitd32, const DEVICE_VALUE_TYPE_TAC & dvtt)
{
	if((oitd32 & 0xffff) != (unsigned)dvtt.otype)
		return false;
	if((oitd32>>16) != (unsigned)dvtt.member)
		return false;
	return true;
}

//static
int sitraffic::AwpWrapper::getDeviceValue(ULONG oitd32, int /* instanz */, int *pvalue)
{
	if(isEqual(oitd32, sDevVar[0]))
		*pvalue=InstRegistry::Instance().getAwpWrapper().lastTaCallDuration/1000;
	else
		if(isEqual(oitd32, sDevVar[1]))
			*pvalue=InstRegistry::Instance().getAwpWrapper().lastTaCallJitter/1000;
	return 0;
}

void sitraffic::AwpWrapper::registerDeviceValues()
{
	sitraffic::InstRegistry::Instance().getCltDeviceValueProviders().add(2, &sDevVar[0]);
}

sitraffic::AwpWrapper::~AwpWrapper()
{
	if(this->clnt)
		clnt_destroy(clnt);
}



SLONG sitraffic::AwpWrapper::SigWunschSetzenSig(UTINY bSigNr, UTINY bZustand, UTINY bFarbe, int64_t takeEffectTick)
{
	if(bZustand > AL_AI_SGS_SIG_UNVERAENDERT)
		return ERROR_PARAMETER;

	if(!sitraffic::InstRegistry::Instance().getConfig().isSgConfigured(bSigNr))
		return ERROR_SIG_UNBENUTZT;

	if(pinsert >= sgSwitchingEvents.end())
	{
	   Report(AWP_WARNING, "end of buffer for sg switching events reached ignoring switching sg %u to %u !", (unsigned) bSigNr, (unsigned)bFarbe);
	   return NO_SUCCESS;
	}
	Report(AWP_DEBUG, "SigWunschSetzenSig sg=%2u bZustand=%u color=%3u %#x", bSigNr, bZustand, bFarbe, bFarbe);

	// lookup entry for sgno, in order to only store only one event per signal group
	std::array<ExternalSgSwitchingEvent, 96>::iterator i=sgSwitchingEvents.begin();
	while(i!=pinsert && i->signalGroupNumber!=bSigNr)
		++i;

	i->isWithCorrection=true;
	i->isWithTransition=false;//true;
	i->priority=RSP_TA;//priority;
	i->signalGroupNumber=bSigNr;
	i->signalIndication=bFarbe;
	i->validDuration= bZustand==AL_AI_SGS_SIG_UNVERAENDERT ? 0 : SWITCHING_EVENT_DEFAULT_VALID_DURATION;
	i->initiator = sinitiator; initiatorP1.MsgParameterValueCctrl_u.int64Value=takeEffectTick;
	i->startTickTime = takeEffectTick;

	if(i==pinsert)
		++pinsert;

	return SUCCESS;
}


static SSHORT limit2short(int v)
{
	if(v > SHRT_MAX) return SHRT_MAX;
	if(v < SHRT_MIN) return SHRT_MIN;
	return static_cast<SSHORT>(v);
}

SLONG sitraffic::Config::getSigZustand(UTINY bSigNr, SIGSZ_STRUCT *pSZ, const SignalIndicationsWCctrl & si, int splNo, int diff2NextTakeEffect) const
{
	const SgInfo * psgInfo = getSgInfo(bSigNr);
	if(!psgInfo)
		return ERROR_SIG_UNBENUTZT;

	pSZ->wTX = static_cast<SSHORT>(si.tx/100);
	const SignalIndicationWCctrl * psi=si.lookup(bSigNr);
	if(!psi)
		return ERROR_SIG_UNBENUTZT;

	const int ocitColor=psi->getOcitColor();

	pSZ->bZustand = psi->hasState() ? psi->getState() : psgInfo->getColorToState(ocitColor);
	pSZ->bFarbe = static_cast<UTINY>(ocitColor);
	pSZ->wDauer = limit2short( psi->getColorStartTime()+diff2NextTakeEffect);
	// determine mindauer, dependant of spl
	const CfgSgStateTransitionElementCCtrl *pSgStateTrans=0;
	const SIGNALPLAN * pspl=getSpl((UTINY)splNo);
	UTINY mindauerListNo;
	switch(pSZ->bZustand)
	{
	case AL_AI_FREIGEBEN:
		mindauerListNo=pspl?pspl->bNrMinFreigabeListe:1;
		if(psgInfo->minGreen.size())
			pSgStateTrans = psgInfo->find(psgInfo->minGreen, mindauerListNo).find(ocitColor);
		break;
	case AL_AI_SPERREN:
		mindauerListNo=pspl?pspl->bNrMinSperrListe:1;
		if(psgInfo->minRed.size())
			pSgStateTrans = psgInfo->find(psgInfo->minRed, mindauerListNo).find(ocitColor);
		break;
	}

	if(pSgStateTrans)
	{
		pSZ->bMindestzeitErfuellt = pSZ->wDauer >= pSgStateTrans->duration_100ms ? TRUE: FALSE;
	}
	else
		pSZ->bMindestzeitErfuellt = FALSE;

	return SUCCESS;
}


SLONG sitraffic::Config::getSigZustandPi(UTINY bTkrNr, TKRSZ_STRUCT *pSZ, const SignalIndicationsWCctrl & si, int splNo, int diff2NextTakeEffect) const
{
	if(bTkrNr>eckdaten.bAnzTK)
		return ERROR_TKR_FALSCHER_ZUSTAND;

	UTINY ind=0;
	for(auto i=sgInfo.cbegin(); i!=sgInfo.cend(); i++)
	{
		if(ind >= MAX_SIGNALGRUPPEN)
			break;
		if(static_cast<UTINY>(i->second.piNo)==bTkrNr)
		{
			SIGSZ_STRUCT sigz;
			getSigZustand(static_cast<UTINY>(i->second.sgNo), &sigz, si, splNo, diff2NextTakeEffect);
			SIGSZ &sz (pSZ->sSigZustand[ind]);
			sz.bSigNr=static_cast<UTINY>(i->second.sgNo);
			sz.bZustand=sigz.bZustand;
			sz.bFarbe=sigz.bFarbe;
			sz.wDauer=sigz.wDauer;
			sz.wRestDauer=sigz.wRestDauer;
			sz.bMindestzeitErfuellt=sigz.bMindestzeitErfuellt;
			ind++;
		}
	}
	pSZ->bAnzSig=ind;
	pSZ->wTX = static_cast<SSHORT>(si.tx/100);
	return SUCCESS;
}


/**
 * call resetcyclemonitor_8 at firmware for all partial intersections.
 * {@link #client} is used and may be altered
 * @return true if call was successful, false otherwise,
 */
bool sitraffic::AwpWrapper::resetcyclemonitor()
{
	const int ResetCyclemonitorAllPartialIntersections=0xff;
	if (!clnt)
		clnt = InstRegistry::Instance().getDataReceiver().getCCtrlClient();

	if (clnt)
	{
		enum RetCodeCctrl ret;
		clnt_stat cs = resetcyclemonitor_8(ResetCyclemonitorAllPartialIntersections, &ret, clnt);
		if (cs != RPC_SUCCESS)
		{
		        Report(AWP_ERROR, "resetcyclemonitor_8(..) failed %s", clnt_sperror(clnt, ""));
			clnt_destroy(clnt);
			clnt = 0;
			return false;
		}
		if(ret==RC_OK)
			return true;
		Report(AWP_ERROR, "resetcyclemonitor_8(..) failed ret=%d", (int)ret);
	}
	Report(AWP_WARNING, "resetcyclemonitor_8 skipped due to: no connection to firmware");
	return false;
}


CLIENT * sitraffic::AwpWrapper::getFwUdpClient()
{
  if (!clnt)
    clnt = InstRegistry::Instance().getDataReceiver().getCCtrlClient();
  return clnt;
}

/**
 * send set tx to firmware, set tx atTick including the fixtime player.
 * @param txMs tx value in ms (starting at 0 ) to set at firmware
 * @param iprefix initiator prefix text, used for debug at firmware
 * @return SUCCESS, NO_SUCCESS in case of rpc or firmware errors
 */
SLONG sitraffic::AwpWrapper::setTx(int txMs, const char *iprefix )
  {

    CLIENT *_clnt=getFwUdpClient();
    if(!_clnt)
      return NO_SUCCESS;
    enum RetCodeCctrl clnt_res = RetCodeCctrl();

    int64_t atTick=nextTakeEffectTick;
    int sp = sitraffic::InstRegistry::Instance().getActualStatus().signalProgram;
    int durationMs = INT_MAX; // FOREVER
    int priority = 0; // --> restart FixTimePlayer
    MsgParameterValueCctrl _initiatorP1;
    _initiatorP1.typeInfo=MPTI_UTC; _initiatorP1.MsgParameterValueCctrl_u.timeStamp=atTick;
    InitiatorCCtrl initiator;
    std::string iformat(iprefix);
    iformat += "%d";
    initiator.format = const_cast<char *>(iformat.c_str());
    initiator.params.params_len = 1;
    initiator.params.params_val = &_initiatorP1;

    Report(TAC_EVENT, "%s sp=%d tx=%d TickOfNextTakeEffect=%" PRId64, iprefix, sp, txMs, atTick);

    clnt_stat stat = setsplcycletimecounterto_8(sp, txMs, atTick, durationMs, priority, initiator, sysJobId, &clnt_res,  _clnt);

    if (stat != RPC_SUCCESS || clnt_res != RC_OK)
    {
            Report(TAC_ERROR, "setsplcycletimecounterto_8(%d, %d, %" PRId64",...) = %s %d -->NO_SUCCESS", sp, txMs, atTick, stat==RPC_SUCCESS?"":clnt_sperrno(stat), clnt_res);
            return NO_SUCCESS;
    }
    sysJobId = ((sysJobId+1) & 0x7f) | (sysJobId & 0xffffffc0);

    return SUCCESS;
  }

//by Valentin
SLONG sitraffic::AwpWrapper::setIntersectionMainState(TacIState tis, int durationMs)
{
    // TODO Fix as suggested by Siemens
    // CLIENT *_clnt=getFwUdpClient();
    static CLIENT *_clnt = NULL;
    if (!_clnt)
        _clnt = InstRegistry::Instance().getDataReceiver().getCCtrlClient();

    if(!_clnt)
      return NO_SUCCESS;
    enum RetCodeCctrl clnt_res = RetCodeCctrl();

    RequestedMainNodeStateCctrl value = toRequestedMainNodeState(tis);
    int64_t atTick=-1;//nextTakeEffectTick; // -1 means as soon as possible
//    int durationMs = 10*1000;//INT_MAX; // FOREVER

    Report(TAC_EVENT, "TickOfNextTakeEffect=%" PRId64, atTick);

    clnt_stat stat;// = setsplcycletimecounterto_8(sp, txMs, atTick, durationMs, priority, initiator, sysJobId, &clnt_res,  _clnt);
    stat = setintersectionstate_8(RSP_TA, value, sysJobId, durationMs, &clnt_res,  _clnt);
    if (stat != RPC_SUCCESS || clnt_res != RC_OK)
    {
        Report(TAC_ERROR, "setintersectionstate_8(%d, %" PRId64",...) = %s %d -->NO_SUCCESS", value, atTick, stat==RPC_SUCCESS?"":clnt_sperrno(stat), clnt_res);

        // TODO Fix as suggested by Siemens
        if (stat != RPC_SUCCESS)
        {
            clnt_destroy(_clnt);
            _clnt = 0;
        }

        return NO_SUCCESS;
    }
    sysJobId = ((sysJobId+1) & 0x7f) | (sysJobId & 0xffffffc0);

    return SUCCESS;
}

//by Valentin
SLONG sitraffic::AwpWrapper::setSignalProgramNumber(UTINY bSigPrgNumber)
{
    CLIENT *_clnt=getFwUdpClient();
    if(!_clnt)
      return NO_SUCCESS;
    enum RetCodeCctrl clnt_res = RetCodeCctrl();

    int64_t atTick=nextTakeEffectTick;
    int durationMs = INT_MAX; // FOREVER

    Report(TAC_EVENT, "sp=%d TickOfNextTakeEffect=%" PRId64, bSigPrgNumber, atTick);

    clnt_stat stat;
    stat = setsignalprogram_8(RSP_TA, bSigPrgNumber, sysJobId, durationMs, &clnt_res,  _clnt);
    if (stat != RPC_SUCCESS || clnt_res != RC_OK)
    {
            Report(TAC_ERROR, "setsignalprogram_8(%d, %" PRId64",...) = %s %d -->NO_SUCCESS", bSigPrgNumber, atTick, stat==RPC_SUCCESS?"":clnt_sperrno(stat), clnt_res);
            return NO_SUCCESS;
    }
    sysJobId = ((sysJobId+1) & 0x7f) | (sysJobId & 0xffffffc0);

    return SUCCESS;
}

//by Valentin
SLONG sitraffic::AwpWrapper::archiveGetYoungestBySelector(ArchiveResultCctrl& arc, enum ArchiveSelectorCctrl selector)
{
    CLIENT *_clnt=getFwUdpClient();
    if(!_clnt)
      return NO_SUCCESS;

    clnt_stat stat;// = setsplcycletimecounterto_8(sp, txMs, atTick, durationMs, priority, initiator, sysJobId, &clnt_res,  _clnt);
    memset(&arc, 0, sizeof(ArchiveResultCctrl));

    stat = archivegetyoungest_8(selector, &arc, _clnt);
    if (stat != RPC_SUCCESS )
    {
			Report(RUNMODE_ERROR,"archivegetyoungest_8 NO_SUCCESS");
            return NO_SUCCESS;
    }

	Report(RUNMODE_EVENT,"archivegetyoungest_8 SUCCESS");
    return SUCCESS;
}
