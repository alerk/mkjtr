/*
 * api_signalling.cpp
 *
 *  Created on: 03.12.2015
 *      Author: zelgerpe
 */

#include "start/awpWrapper.h"
#include "instregistry.h"
#include "cctrl/Config.h"
#include "cctrl/wrappedCctrlDtos.h"
#include "util/repdef.h"

#include "util/what.h"
DEFINE_WHATINFO;

//by Valentin
SLONG setIntersectionMainState(TacIState tis, int durationMs)
  {
    Report(SIGNALGROUP_INFO, "setIntersectionState(%d)", (int)tis);
    SLONG ret = sitraffic::InstRegistry::Instance().getAwpWrapper().setIntersectionMainState(tis, durationMs);
    Report(SIGNALGROUP_DEBUG, "setIntersectionState(...)  ret=%ld", ret);
    return ret;
  }

//by Valentin
SLONG setSignalProgramNumber(UTINY bSigPrgNumber)
  {
    Report(SIGNALGROUP_INFO, "setSignalProgramNumber(%d)", (int)bSigPrgNumber);
    SLONG ret = sitraffic::InstRegistry::Instance().getAwpWrapper().setSignalProgramNumber(bSigPrgNumber);
    Report(SIGNALGROUP_DEBUG, "setSignalProgramNumber(...)  ret=%ld", ret);
    return ret;
  }

//by Valentin
SLONG SigWunschSetzenSigAtTick(UTINY bSigNr, UTINY bZustand, UTINY bFarbe, int64_t takeEffectTick)
  {
    Report(SIGNALGROUP_INFO, "SigWunschSetzenSig(%d, %d, %d)", (int)bSigNr, (int)bZustand, (int)bFarbe);
    SLONG ret = sitraffic::InstRegistry::Instance().getAwpWrapper().SigWunschSetzenSig(bSigNr, bZustand, bFarbe, takeEffectTick);
    Report(SIGNALGROUP_DEBUG, "SigWunschSetzenSig(...)  ret=%d", ret);
    return ret;
  }


/**
 * store signal event to be sent after end of current AWP_Funktion().
 * @param bSigNr        signal group number (1..MAX_SIGNALGRUPPEN)
 * @param bZustand      one of: AL_AI_SGS_SPERREN (0) AL_AI_SGS_FREIGEBEN (1) AL_AI_SGS_SIG_UNVERAENDERT (2)
 * @param bFarbe        ocit color code
 * @return
 *      SUCCESS (0)                     stored to pass to firmware
 *      ERROR_SIG_UNBENUTZT (-15)       signal group not defined bei Fehler in bSigNr
 *      ERROR_PARAMETER (2)             undefined bZustand/ bFarbe
 */
SLONG SigWunschSetzenSig(UTINY bSigNr, UTINY bZustand, UTINY bFarbe)
  {
    Report(SIGNALGROUP_INFO, "SigWunschSetzenSig(%d, %d, %d)", (int)bSigNr, (int)bZustand, (int)bFarbe);
    SLONG ret = sitraffic::InstRegistry::Instance().getAwpWrapper().SigWunschSetzenSig(bSigNr, bZustand, bFarbe);
    Report(SIGNALGROUP_DEBUG, "SigWunschSetzenSig(...)  ret=%d", ret);
    return ret;
  }


/**
 * provide current signal group state
 * @param bSigNr        signal group number (1..MAX_SIGNALGRUPPEN)
 * @param pSZ           address where this function stores the result:
 *      wTX             cycle time counter in 100ms units
 *      bZustand        traffic state (red-/greenstate) of signal group AL_AI_SPERREN(0), AL_AI_FREIGEBEN(1), AL_AI_SF(2), AL_AI_FS(3)
 *      bFarbe          ocit color code of signal group
 *      wDauer          duration this sg has been at bZustand in 100 ms units (will be at bZustand at nextTakeEffectTick)
 *      wRestDauer      not defined
 *      bMindestzeitErfuellt    minimum red or green reached, TRUE/FALSE
 * @return
 *      SUCCESS         OK
 *      ERROR_SIG_UNBENUTZT in case of undefined bSigNr
 */
SLONG SigZustandLesenSig (UTINY bSigNr, SIGSZ_STRUCT *pSZ)
  {
    Report(SIGNALGROUP_DEBUG, "SigZustandLesenSig(%d, ...)", (int)bSigNr);
    const sitraffic::InstRegistry & ir(sitraffic::InstRegistry::Instance());
    const sitraffic::SignalIndicationsWCctrl & si(ir.getSi());
    const sitraffic::AwpWrapper & aw(ir.getAwpWrapper());
    SLONG ret = ir.getConfig().getSigZustand(bSigNr, pSZ, si, ir.getActualStatus().signalProgram, aw.getTicksTillNextTakeEffectTick());
    Report(SIGNALGROUP_INFO, "SigZustandLesenSig(%d,) sp=%d TicksTillNextTakeEffectTick=%d [100ms] ret=%d",
        (int)bSigNr, ir.getActualStatus().signalProgram, aw.getTicksTillNextTakeEffectTick(), ret);
    return ret;
  }



/**
 * retrieves the signal state of all signal groups belonging to partial intersection with number bTkrNr.
 * @param bTkrNr        number of partial intersection the signal group states are to be reported.
 * @param pSZ           address to store the results
 * @return
 *      SUCCESS(0)      OK, results stored
 *      ERROR_TKR_FALSCHER_ZUSTAND (-17)        wrong partial intersection number
 */
SLONG SigZustandLesenTkr (UTINY bTkrNr, TKRSZ_STRUCT *pSZ)
  {
    Report(SIGNALGROUP_DEBUG, "SigZustandLesenTkr(%d, ...)", (int)bTkrNr);
    const sitraffic::InstRegistry & ir(sitraffic::InstRegistry::Instance());
    const sitraffic::SignalIndicationsWCctrl & si(ir.getSi());
    const sitraffic::AwpWrapper & aw(ir.getAwpWrapper());
    SLONG ret =  ir.getConfig().getSigZustandPi(bTkrNr, pSZ, si, ir.getActualStatus().signalProgram, aw.getTicksTillNextTakeEffectTick());
    Report(SIGNALGROUP_INFO, "SigZustandLesenTkr(%d,) sp=%d TicksTillNextTakeEffectTick=%d [100ms] ret=%d",
        (int)bTkrNr, ir.getActualStatus().signalProgram, aw.getTicksTillNextTakeEffectTick(), ret);
    return ret;
  }



SLONG UmlaufzeitRuecksetzen (void)
  {
    SLONG ret = NO_SUCCESS;
    Report(SIGNALGROUP_DEBUG, "UmlaufzeitRuecksetzen()");
    if(sitraffic::InstRegistry::Instance().getAwpWrapper().resetcyclemonitor())
      ret = SUCCESS;
    Report(SIGNALGROUP_INFO, "UmlaufzeitRuecksetzen() = %s",ret == SUCCESS ? "SUCCESS" : "NO_SUCCESS");
    return ret;
  }
