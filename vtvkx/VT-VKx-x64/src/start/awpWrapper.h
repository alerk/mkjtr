/*
 * awpWrapper.h
 *
 *  Created on: 26.11.2015
 *      Author: zelgerpe
 */

#ifndef CCTRL_AWPWRAPPER_H_
#define CCTRL_AWPWRAPPER_H_


#include <stdint.h>
#include <chrono>
#include <array>
#include <atomic>
#include "start/modonoffhelper.h"
#include "tac_kernel.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

struct SignalIndicationsCctrl;
struct CLIENT;

namespace sitraffic
{
class AwpRequestHandler;
class SignalIndicationsWCctrl;
typedef int (* pAwpFunction_t)();
class AwpWrapper
{
	class AwpSettings;
	typedef std::chrono::steady_clock         usedClock;
	typedef std::array<ExternalSgSwitchingEvent, 96>  sgSwitchingEvents_t;
	pAwpFunction_t init, function;
	int64_t tickOfLastCall;
	int64_t nextTakeEffectTick;       // OMC Tick when signalisation of current calculation shall be visible
	int64_t nextPlannedCall;

	CLIENT * clnt;
	ModOnOffHelper statusMods;     // used to send actual status-modifications only on change
	static int TaCallIntervallMs;  // set to system pulse ms at init
	static int TaCallOffsetMs;     // may be tuned for test and debug purpose
	static const int priority=44;  // used with switching events generated

	// used for log
	usedClock::time_point tpOfLastCall;
	uint32_t lastTaCallDuration;      // duration of last traffic actuation call in microseconds
	int32_t lastTaCallJitter;        // deviation of Call Interval duration from TaCallIntervallMs of last traffic actuation call in microseconds
	static const DEVICE_VALUE_TYPE_TAC sDevVar[];
	AwpRequestHandler * pAwpJmx;      // debug http page generator, defined in api_signalling.cpp

	// buffer of switching events
	sgSwitchingEvents_t sgSwitchingEvents;
	sgSwitchingEvents_t::iterator pinsert;//=sgSwitchingEvents.begin();
	sgSwitchingEvents_t::iterator pLastSent;//=sgSwitchingEvents.begin();
	MsgParameterValueCctrl initiatorP1;       // parameter for initiator sent with switching events
	const InitiatorCCtrl sinitiator;
	//TimelineHelper timelineHelper;
	std::atomic_bool isMitigate;
	int64_t txMitigate;     // if mitigating, the tx that has to be processed next by TA
	int64_t txTaLastCalled;
	int sysJobId;


	void registerDeviceValues();
	static int getDeviceValue(ULONG oitd32, int instanz, int *pvalue);
	static bool isEqual(ULONG oitd32, const DEVICE_VALUE_TYPE_TAC & dvtt);

	void onAwpFunctionCalled();
	void onAwpInitCalled();
	int64_t getTickOfNextCall(int64_t tickNow) const; // tick when next call of function should be
	bool checkLastSiSwitched(const SignalIndicationsWCctrl & si) ;
	void doTaExecutionDurationWarn(const SignalIndicationsWCctrl & si) const;

	//void getSwitchingEvents4Refresh(ExternalSgSwitchingEventArray & esarray);// is done inside VAlib/PDM

	AwpWrapper(const sitraffic::AwpWrapper&)=delete;
	AwpWrapper & operator=(const sitraffic::AwpWrapper&)=delete;

public:
	AwpWrapper(pAwpFunction_t pinit, pAwpFunction_t pfunction, bool withMitigation=true);
	~AwpWrapper();
	void onSignalIndication(const SignalIndicationsWCctrl & si);
	bool resetcyclemonitor();
	SLONG SigWunschSetzenSig(UTINY bSigNr, UTINY bZustand, UTINY bFarbe){
		return SigWunschSetzenSig(bSigNr, bZustand, bFarbe, nextTakeEffectTick);
	};
	SLONG SigWunschSetzenSig(UTINY bSigNr, UTINY bZustand, UTINY bFarbe, int64_t takeEffectTick);//by Valentin
	int64_t getTickOfNextTakeEffect() const; // start tick next signal indications are for
	int getTicksTillNextTakeEffectTick() const    // number of 100ms till nextTakeEffectTick
	{ return static_cast<int>(nextTakeEffectTick-tickOfLastCall)/100; }
        /** setTx at firmware, inclusive fixtime player */
        SLONG setTx(int txMs, const char *iprefix);

        SLONG setIntersectionMainState(TacIState tms, int durationMs);//by Valentin
        SLONG setSignalProgramNumber(UTINY bSigPrgNumber);//by Valentin
        SLONG archiveGetYoungestBySelector(ArchiveResultCctrl& arc, ArchiveSelectorCctrl selector);//by Valentin
	/**
	 * @return an udp rpc client handle associated with the firmware cctrl service,
	 * may be NULL if there is not (yet) a connection.
	 */
	CLIENT * getFwUdpClient() ;
};
}



#endif /* CCTRL_AWPWRAPPER_H_ */
