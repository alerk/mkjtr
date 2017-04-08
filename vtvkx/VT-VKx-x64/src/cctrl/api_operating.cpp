/*
 * implemenmtation of operating state related api functions of api_va.h
 */

#include "start/awpWrapper.h"
#include "instregistry.h"
#include "cctrl/Config.h"
#include "CControl.h"
#include "wrappedCctrlDtos.h"
#include "start/modonoffhelper.h"
#include "util/repdef.h"
#include "tac_kernel.h"

#include "DataReceiver.h"
#include "util/what.h"
DEFINE_WHATINFO;

static bool isOff(IntersectionStateCctrl is)
  {
    switch (is)
      {
      case IS_NONE:
      case IS_OFF_DEFAULT:
      case IS_OFF_FLASH_SECONDARY_DIRECTION:
      case IS_OFF_DARK:
      case IS_OFF_FLASH_ALL:
        return true;
      case IS_MANUAL_CYCLIC:
      case IS_MANUAL_ACYCLIC:
      case IS_FATAL_ERROR:
      case IS_RECOVER_FROM_EMERGENCY_MODE:
      case IS_ON:
        return false;
      }
    Report(RUNMODE_WARNING, "unexpected IntersectionStateCctrl:%d, assuming off", is);
    return true;
  }

///*
// * @return bitfield, 2^0 ModTD on, 2^1 mod public transport, 2^2 mod individual traffic
// */
//static  unsigned ActualStatusCCtrl_getModBits(const ActualStatusCctrl & as)
//  {
//    static const TypeOfModificationCctrl mods[]={TOM_ModTDIndividualMkjtric, TOM_ModPublicTransport, TOM_ModTD };
//    unsigned ret=0;
//    for(TypeOfModificationCctrl mod: mods)
//      {
//        const TTVModOnOffStateCctrl * pmod=sitraffic::ModOnOffHelper::ActualStatusCCtrl_getMod(as, mod);
//        ret <<= 1;
//        if(!pmod) Report(RUNMODE_DEBUG("ActualStatusCCtrl_getModBits mod %d not found, assuming off", mod);
//        if(pmod && pmod->value==MOOS_ON)
//          ret |= 1;
//      }
//    return ret;
//  }

//------------------------------- tacGetOperatingState related functions ----------------------------------

static TacOperator toOperator(const TimeConstrainedTransactionTaggedIntValueCctrl & intersectionState)
  {
    //RequestedStatusPriorityCctrl
    if(intersectionState.priority < RSP_CentralSystem)
      return TAC_OP_JAUT;
    if(intersectionState.priority < RSP_Manual)
      return TAC_OP_CENTER;
    const unsigned originatorSubtype = (intersectionState.sysJobId >> (32-2-4-4))&0xf;
    switch(originatorSubtype)
      {
      case 1: //BAZ
        return TAC_OP_BAZ;
      case 2: //DeviceGui
      default:
        return TAC_OP_GUI;
      case 3: //LocalServicePC
        return TAC_OP_RPC;
      }
  }

static bool isPiSwitchingOnOff(const sitraffic::ActualStatusWCctrl & as)
  {
    for(unsigned pii=0; pii<as.subIntersectionStates.subIntersectionStates_len; pii++)
      switch(as.subIntersectionStates.subIntersectionStates_val[pii].value)
        {
        case SDS_WAIT4SYNC_ON:
        case SDS_SWITCHON_PATTERN:
        case SDS_WAIT4_OFF:
        case SDS_SWITCHOFF_PATTERN:
          return true;
        default:
          ;//continue
        }
    return false;
  }

/*
 * not yet set:
 *  TAC_MS_MA_CO_AC_TRANSITION,                 // state = stage transition by acyclic manual control is running, !!state is currently not suported by sX-controller
 *  TAC_MS_SP_CHANGE_GSP,                       // todo to be added to cctrl OR derived from requested status; state = signal program change by GSP method, from the request of signalplan changing until GSP. Also by changing to cyclic manual control, if a signalplan changing is necessary
 *  TAC_MS_SYNC_GSP,                            // todo to be added to cctrl; state = synchronization by GSP method, -> when controller should run synchonized and is currently not synchronized. The state could be set for several cycletimes.(dependent to the parameter 'number of cycletimes until controller is synchronized)
 *  TAC_MS_CHANGE_TO_FIXED_TIME_CONTROL,        // state = Changing to fixed time mode / TA from modes like all-red, acycl. Manual control -> state is currently not suported by sX-controller.
 *  TAC_MS_CHANGE_TO_MANUAL_CONTROL_ACYCLIC,    // state = change to manual control acyclic -> state is currently not suported by sX-controller.
 *  TAC_MS_SP_CHANGE_STRETCH,                   // no stretch implemented at sX yet
 *  TAC_MS_SYNC_STRETCH,                        // no stretch implemented at sX yet
 *  TAC_MS_DELAY_TIME,                          // todo to be added to cctrl, for now NONE is mapped ro delay time
 *  TAC_MS_CHANGE_TO_ALL_RED,                   // all red not yet implemented at sX
 *  TAC_MS_ALL_RED,                             // all red not yet implemented at sX
 *
 */
static TacMainState toMainState(const sitraffic::ActualStatusWCctrl & as)
  {
    if(as.globalFaultState == SA_ALARM_CAUSING_SWITCH_OFF)
      return TAC_MS_ERR_OFF;

    /** special intervention number. if no special intervention is engaged specialIntervention==0 */
    if(as.specialIntervention)
      return TAC_MS_SI_FIRE;

    switch(as.intersectionOnOffState)
      {
      default:
        Report(RUNMODE_WARNING, "unexpected intersectionOnOffState %d, assuming IS_ON", as.intersectionOnOffState);
      /** main intersection is on.<br/>
       *  note the intersection is only running a signalplan if main intersection and sub intersection is on and a signal plan is defined. */
       // fall through --> no break
      case IS_ON:
      // TODO access remaining mainstates via cctrl
        if(isPiSwitchingOnOff(as))
          return TAC_MS_PI_ON_OFF_WHILE_FIXED_TIME;

        if(as.signalProgramSysJobId & 0xC0000000)   // was it an intersection controller initiated job ?
          return TAC_MS_LOCAL_SELECTED_SP;
        else
          return TAC_MS_CENTER_SELECTED_SP;

      case IS_NONE:
        // ??? is map correct ???
        return TAC_MS_DELAY_TIME;

      /** main intersection is in default off.
       *  signal groups chosen by configuration (typically vehicle signal groups of secondary direction) are flashing normally, the remaining signal groups are dark */
      case IS_OFF_DEFAULT:
      /** intersection is switched off flashing secondary direction and for all active sub intersections applies respectively:<br/>
       *  vehicle signal groups of secondary direction are flashing, the remaining signal groups are dark */
      case IS_OFF_FLASH_SECONDARY_DIRECTION:
      /** intersection is to be switched off dark and all signal groups of active sub intersections are dark respectively */
      case IS_OFF_DARK:
      /** intersection is to be switched off flashing all and all signal groups of active sub intersections are flashing respectively */
      case IS_OFF_FLASH_ALL:
        return isPiSwitchingOnOff(as) ? TAC_MS_PI_ON_OFF_WHILE_FIXED_TIME : TAC_MS_OFF;

      /** intersection controller is manually controlled. the stages sequence is taken of configuration, they are continued manually. */
      case IS_MANUAL_CYCLIC:
        return TAC_MS_MANUAL_CONTROL_CYCLIC;
      /** intersection controller is manually controlled. the stage sequence and the continuation of stages is controlled manually. */
      case IS_MANUAL_ACYCLIC:
        return  isPiSwitchingOnOff(as) ? TAC_MS_PI_ON_OFF_WHILE_MC_ACYCLIC : TAC_MS_MANUAL_CONTROL_ACYCLIC;

      /** OMC is in an only via restart recoverable error condition. e.g. no ticks from OMC received. */
      case IS_FATAL_ERROR:
        return TAC_MS_ERR_OFF;
      /** the cbu is running in emergency mode, the omc is switching the intersection back into normal operation. */
      case IS_RECOVER_FROM_EMERGENCY_MODE:
        return TAC_MS_RECOVER_FROM_EMERGENCY_MODE;

      }
  }

// by Valentin
RequestedMainNodeStateCctrl toRequestedMainNodeState(TacIState tis)
{
    switch(tis)
    {
    case TAC_IS_ON:
        return RMNS_ON;
    case TAC_IS_OFF_FLASH_SECONDARY_DIRECTION:
        return RMNS_OFF_FLASH_SECONDARY_DIRECTION;
    case TAC_IS_OFF_DARK:
        return RMNS_OFF_DARK;
    case TAC_IS_OFF_FLASH_ALL:
        return RMNS_OFF_FLASH_ALL;
    case TAC_IS_MANUAL_CYCLIC:
        return RMNS_MANUAL_CYCLIC;
    case TAC_IS_MANUAL_ACYCLIC:
        return RMNS_MANUAL_ACYCLIC;
    default:
        break;
    }
    return RMNS_OFF_DEFAULT;
}

static TacPIState toPiState(SubIntersectionDetailStateCctrl sids)
  {
    switch(sids)
      {
      //Teilkreuzung im AUS-Zustand durch Bediener
      // SX: ActualStatus subIntersectionStates == SDS_OFF_*
      case SDS_OFF_DEFAULT:
      case SDS_OFF_DARK:
      case SDS_OFF_FLASH_ALL:
      case SDS_OFF_FLASH_SECONDARY_DIRECTION:
        return TAC_PI_NOT_ACTIVE;
      //Teilkreuzung im AUS-Zustand durch SISI
      case SDS_FAULT_OFF:
      case SDS_NONE:
      default:
        return TAC_PI_ERROR_OFF;
      //Teilkreuzung in der Einschaltung (Einschaltprogramm l�uft)
      case SDS_SWITCHON_PATTERN:
      case SDS_RECOVER_FROM_EMERGENCY_MODE:
        return TAC_PI_ACTIVATION_ROUTINE;//AL_AI_TKR_EINSCHALTUNG;
      //Teilkreuzung im EIN-Zustand (Festzeit, VA, Handsipl, etc)
      case SDS_ON:
        return TAC_PI_ACTIVE; //AL_AI_TKR_AKTIV;
      //Teilkreuzung in der Ausschaltung (Ausschaltprogramm l�uft)
      case SDS_SWITCHOFF_PATTERN:
        return TAC_PI_DEACTIVATION_ROUTINE;//AL_AI_TKR_AUSSCHALTUNG;
      case SDS_WAIT4SYNC_ON:
        return TAC_PI_WAITING_FOR_START_ACTIVATION_ROUTINE; //AL_AI_TKR_WARTEN_AUF_EZP;
      case SDS_WAIT4_OFF:
        return TAC_PI_WAITING_FOR_DEACTIVATION_TIME; //AL_AI_TKR_WARTEN_AUF_AZP;
      }
  }

static TacPIRqState matchToActualPIState (TacPIRqState rqPIState, TacPIState actPIState)
{
	// requested (already mapped) PI state is set to TAC_PIRQ_MATCH_ACTUAL_STATE if
	// actual PI state is semantical equal to requested PI state
	switch (rqPIState)
	{
	  case TAC_PIRQ_OFF_DEFAULT:
	  case TAC_PIRQ_OFF_FLASH_SECONDARY_DIRECTION:
	  case TAC_PIRQ_OFF_DARK:
	  case TAC_PIRQ_OFF_FLASH_ALL:
		return (actPIState == TAC_PI_NOT_ACTIVE || actPIState ==  TAC_PI_WAITING_FOR_START_ACTIVATION_ROUTINE) ? TAC_PIRQ_MATCH_ACTUAL_STATE : rqPIState;
	  case TAC_PIRQ_ON:
		return (actPIState == TAC_PI_ACTIVE  || actPIState == TAC_PI_ACTIVATION_ROUTINE || actPIState == TAC_PI_WAITING_FOR_DEACTIVATION_TIME) ? TAC_PIRQ_MATCH_ACTUAL_STATE : rqPIState;
	  case TAC_PIRQ_FAULT_SHUTDOWN:
	    return (actPIState == TAC_PI_ERROR_OFF) ? TAC_PIRQ_MATCH_ACTUAL_STATE : rqPIState;
	  default:
		// unused or already matching
		return rqPIState;
	}
}

static TacPIRqState toPiRqState(RequestedNodeStateCctrl rqPIState, RequestedMainNodeStateCctrl rqMIState, TacPIState actPIState)
  {
    switch (rqPIState)
      {
      case RNS_ON:
        {
          // If PI state is set to "on" means, that PI state follows main state by definition
          // set requested PI state to requested main state
    	  switch (rqMIState)
    	  {
    	    case RMNS_OFF_DEFAULT:
    	      return matchToActualPIState(TAC_PIRQ_OFF_DEFAULT, actPIState);
    	    case RMNS_OFF_FLASH_SECONDARY_DIRECTION:
    	      return matchToActualPIState (TAC_PIRQ_OFF_FLASH_SECONDARY_DIRECTION, actPIState);
    	    case RMNS_OFF_DARK:
    	      return matchToActualPIState(TAC_PIRQ_OFF_DARK, actPIState);
    	    case RMNS_OFF_FLASH_ALL:
    	      return matchToActualPIState(TAC_PIRQ_OFF_FLASH_ALL, actPIState);
    	    default:
    	      // requested main state is either on or in cyclic/acyclic manual mode
    	      return matchToActualPIState(TAC_PIRQ_ON, actPIState);
    	  }
        }
        //no break at end of case
      case RNS_OFF_DEFAULT:
        return matchToActualPIState(TAC_PIRQ_OFF_DEFAULT, actPIState);
      case RNS_OFF_FLASH_SECONDARY_DIRECTION:
        return matchToActualPIState(TAC_PIRQ_OFF_FLASH_SECONDARY_DIRECTION, actPIState);
      case RNS_OFF_DARK:
        return matchToActualPIState(TAC_PIRQ_OFF_DARK, actPIState);
      case RNS_OFF_FLASH_ALL:
        return matchToActualPIState(TAC_PIRQ_OFF_FLASH_ALL, actPIState);
      case RNS_FAULT_SHUTDOWN:
        return matchToActualPIState(TAC_PIRQ_FAULT_SHUTDOWN, actPIState);
      default:
        return matchToActualPIState(TAC_PIRQ_NOT_USED, actPIState);
      }
  }

/**
 * retrieve operating state from controller.
 * replaces "BetriebszustandLesen"
 * @param psTacOpState adress where to store the result, must not be null.
 * @return SUCCESS
 */
SLONG tacGetOperatingState (TacOperatingState *psTacOpState)
  {
    const sitraffic::InstRegistry & instReg(sitraffic::InstRegistry::Instance());
    const sitraffic::SignalIndicationsWCctrl & si(instReg.getSi());
    const sitraffic::ActualStatusWCctrl & as(instReg.getActualStatus());
    const sitraffic::RequestedStatusWCctrl & rs(instReg.getRequestedStatus());
    const sitraffic::Config & co(instReg.getConfig());
    psTacOpState->wTX = static_cast<SSHORT>(si.tx/100);
    psTacOpState->eOperator = toOperator(rs.intersectionState);
    psTacOpState->eMainState = toMainState(as);

    //by Valentin
    psTacOpState->eIState = (TacIState)as.intersectionOnOffState;
    psTacOpState->eIOpMode = (TacIOpMode)as.operatingMode;
    psTacOpState->eReqIOpMode = (TacReqIOpMode)rs.intersectionState.value;

    psTacOpState->wSpNrRequested = static_cast<SSHORT>( (rs.intersectionState.value==RMNS_ON) && (as.signalProgram!=rs.signalProgram.value)  ? rs.signalProgram.value:-1);
    psTacOpState->wSpNrActual = static_cast<SSHORT>(as.signalProgram>0 ? as.signalProgram : -1);
    psTacOpState->eSpecialIntervention = TAC_SI_NOT_ACTIVE; //TODO with adding feature special intervention

    psTacOpState->wGsp = ( (psTacOpState->wTX) == (co.getGsp(psTacOpState->wSpNrActual)) ) ? 1 : 0 ;//by Valentin

    psTacOpState->eRequestedSync = TAC_RS_NO; //TODO
    psTacOpState->wStageNr=0;   //TODO
    psTacOpState->wStageNrRequested=0; //TODO

    // fill in defaults for unconfigured partial intersections
    for(unsigned i=0; i < MAX_PARTIAL_INTERSECTIONS; ++i)
      {
        psTacOpState->ePIState[i] = TAC_PI_NOT_USED;
      }
    for(unsigned i=0; i<as.subIntersectionStates.subIntersectionStates_len; ++i)
      {
        const TTVSubIntersectionDetailStateCctrl sids(as.subIntersectionStates.subIntersectionStates_val[i]);
        psTacOpState->ePIState[(sids.subintersectionNo - 1) % MAX_PARTIAL_INTERSECTIONS] = toPiState(sids.value);

        // synchronize recent received signal indication with actual PI state only exactly at ezp
        // at ezp actual PI state should change from activation pattern to active
        if ((psTacOpState->ePIState[(sids.subintersectionNo - 1) % MAX_PARTIAL_INTERSECTIONS] == TAC_PI_ACTIVATION_ROUTINE) && (psTacOpState->wTX == co.getEzp((UTINY)psTacOpState->wSpNrActual)))
        {
          psTacOpState->ePIState[(sids.subintersectionNo - 1) % MAX_PARTIAL_INTERSECTIONS] = TAC_PI_ACTIVE;
        }
      }

    for(unsigned i=0; i < MAX_PARTIAL_INTERSECTIONS; ++i)
      {
        psTacOpState->ePIStateRequested[i] = TAC_PIRQ_NOT_USED;
      }
    for(unsigned i=0; i < ((sizeof rs.subIntersectionState) / (sizeof rs.subIntersectionState[0])) && i< as.subIntersectionStates.subIntersectionStates_len; ++i)
      {
        const TimeConstrainedTransactionTaggedIntValueCctrl sids(rs.subIntersectionState [i]);
        psTacOpState->ePIStateRequested[i % MAX_PARTIAL_INTERSECTIONS] = toPiRqState((RequestedNodeStateCctrl)sids.value, (RequestedMainNodeStateCctrl)rs.intersectionState.value, psTacOpState->ePIState[i % MAX_PARTIAL_INTERSECTIONS]);
      }

    Report(RUNMODE_INFO, "wTX:%d, eOperator:%d eMainState:%d wSpNrActual:%d wSpNrRequested:%d ePIState:%d %d %d %d ePIStateRequested:%d %d %d %d",
        psTacOpState->wTX, psTacOpState->eOperator, psTacOpState->eMainState, psTacOpState->wSpNrActual, psTacOpState->wSpNrRequested,
        psTacOpState->ePIState[0], psTacOpState->ePIState[1], psTacOpState->ePIState[2], psTacOpState->ePIState[3],
        psTacOpState->ePIStateRequested[0], psTacOpState->ePIStateRequested[1], psTacOpState->ePIStateRequested[2], psTacOpState->ePIStateRequested[3]);

    return SUCCESS;
  }

/**
 * sets the TA locks in firmware
 * for example this can be a lock which is held in firmware while TA is running
 * until the control is released back to firmware
 * @param lock structure carrying the locks state
 * @return SUCCESS, ERROR_PARAM or NO_SUCCESS in case of general failure
 */
SLONG tacSetLockSwitchOff (TacLockSwitchOff *lock)
{
    struct TaLocksCctrl ccTaLock;
    sitraffic::DataReceiver &dr = sitraffic::InstRegistry::Instance().getDataReceiver();

    CLIENT *clnt=sitraffic::InstRegistry::Instance().getAwpWrapper().getFwUdpClient();
    if(!clnt)
      return NO_SUCCESS;

    const char *hostname = dr.getMyHostname();
    int port = dr.getSvcPort();
    enum RetCodeCctrl clnt_res = RetCodeCctrl();

    static char oldLocks[MAX_PARTIAL_INTERSECTIONS+1];
    char locks[MAX_PARTIAL_INTERSECTIONS+1];

    int i;
    for (i = 0; i < MAX_PARTIAL_INTERSECTIONS; i++) {
        switch (lock->lockPI[i]) {
            case TAC_LS_LOCK:
                    ccTaLock.subIntersection[i] = TLS_LOCK;
                    locks[i] = 'L';
                break;
            case TAC_LS_UNLOCK:
                    ccTaLock.subIntersection[i] = TLS_UNLOCK;
                    locks[i] = 'U';
                break;
            case TAC_LS_DO_NOT_CARE:
                    ccTaLock.subIntersection[i] = TLS_DO_NOT_CARE;
                    locks[i] = '.';
                break;
            default:
				ccTaLock.subIntersection[i] = TLS_DO_NOT_CARE;
				locks[i] = '?';
				Report(LOCK_ERROR, "unkown TacLockState enum value %s in tacLockRequest()", std::to_string((int)(lock->lockPI[i])).c_str());
			    return NO_SUCCESS;
        }
    }
    locks[MAX_PARTIAL_INTERSECTIONS] = '\0';

    Report(LOCK_DEBUG, "PI locks: %s", locks);
    if(strcmp(locks, oldLocks) == 0)
      return SUCCESS; // already sent

    clnt_stat stat = settalock_8(const_cast<char*>(hostname), port, ccTaLock, &clnt_res, clnt);

    if (stat != RPC_SUCCESS || clnt_res != RC_OK) {
        Report(LOCK_ERROR, "settalock_8(%s,%d, %p...)=%d", hostname, port, &ccTaLock, clnt_res);
        return NO_SUCCESS;
    }

    Report(LOCK_INFO, "SUCCESS: settalock_8(%s)", locks);
    strcpy(oldLocks, locks);

    return SUCCESS;
}


/** release control of TA (after reaching GSP)
 *
 * @param void
 * @return SUCCESS, NO_SUCCESS
 */
extern SLONG tacReleaseControl()
{
	sitraffic::AwpWrapper &awp = sitraffic::InstRegistry::Instance().getAwpWrapper();

//	CLIENT *clnt=awp.getFwUdpClient();
//	if(!clnt)
//	  return NO_SUCCESS;
//	enum RetCodeCctrl clnt_res = RetCodeCctrl();
//
	int sp = sitraffic::InstRegistry::Instance().getActualStatus().signalProgram;
        SIGNALPLAN spl;
	sitraffic::InstRegistry::Instance().getConfig().getSpl((unsigned char) sp, &spl);
	int txMs = spl.wGsp * 100; // GSP
//	int64_t atTick = awp.getTickOfNextTakeEffect();
//	int durationMs = INT_MAX; // FOREVER
//	int priority = 0; // --> restart FixTimePlayer
//	MsgParameterValueCctrl initiatorP1({MPTI_UTC, 0});
//	InitiatorCCtrl initiator;
//	initiator.format = const_cast<char *>("pdmx%d");
//	initiator.params.params_len = 1;
//	initiator.params.params_val = &initiatorP1;
//	// ({ const_cast<char *>("pdmx%d"), { 1, &initiatorP1 } });
//	int sysJobId = 0;

	Report(TAC_EVENT, "tacReleaseControl() sp=%d gsp=%d", sp, spl.wGsp);

	return awp.setTx(txMs, "pdmx");

//	clnt_stat stat = setsplcycletimecounterto_8(sp, txMs, atTick, durationMs, priority, initiator, sysJobId, &clnt_res,  clnt);
//
//	if (stat != RPC_SUCCESS || clnt_res != RC_OK)
//	{
//		Report(TAC_ERROR, "setsplcycletimecounterto_8(%d, %d, %" PRId64", ...) = %d", sp, txMs, atTick, stat);
//		return NO_SUCCESS;
//	}
//
//	return SUCCESS;
}


/** this function is used to set the TX of the firmware player
 *
 * @param tx as int in seconds
 * @return SUCCESS, NO_SUCCESS
 */
extern SLONG tacSetTX(int tx)
{
        CLIENT *clnt=sitraffic::InstRegistry::Instance().getAwpWrapper().getFwUdpClient();
        if(!clnt)
          return NO_SUCCESS;
	enum RetCodeCctrl clnt_res = RetCodeCctrl();

	int sp = sitraffic::InstRegistry::Instance().getActualStatus().signalProgram;
	SIGNALPLAN spl;
	sitraffic::InstRegistry::Instance().getConfig().getSpl((unsigned char) sp, &spl);
	//int gsp = spl.wGsp * 100; // GSP
	int txMs = tx * 1000;
	const sitraffic::AwpWrapper& awp = sitraffic::InstRegistry::Instance().getAwpWrapper();
	int64_t atTick = awp.getTickOfNextTakeEffect();
	int durationMs = INT_MAX; // FOREVER
	int priority = 0; // --> restart FixTimePlayer
	MsgParameterValueCctrl initiatorP1({MPTI_UTC, 0});
	InitiatorCCtrl initiator;
	initiator.format = const_cast<char *>("pdmx%d");
	initiator.params.params_len = 1;
	initiator.params.params_val = &initiatorP1;
	// ({ const_cast<char *>("pdmx%d"), { 1, &initiatorP1 } });
	int sysJobId = 0;

	Report(TAC_EVENT, "tacSetTX() sp=%d tx=%d TickOfNextTakeEffect=%" PRId64, sp, tx, atTick);

	clnt_stat stat = setsplcycletimecounterto_8(sp, txMs, atTick, durationMs, priority, initiator, sysJobId, &clnt_res,  clnt);

	if (stat != RPC_SUCCESS || clnt_res != RC_OK)
	{
		Report(TAC_ERROR, "setsplcycletimecounterto_8(%d, %d, %" PRId64", ...) = %d", sp, txMs, atTick, stat);
		return NO_SUCCESS;
	}

	return SUCCESS;
}
