/*
 * api_Config.cpp
 *     config related api functions
 *
 *  Created on: 20.11.2015
 *      Author: zelgerpe
 */

#include "instregistry.h"
#include "Config.h"
#include "DataReceiver.h"
#include "cltDeviceValues.h"
#include "util/repdef.h"
#include <stdarg.h>
#include "ConfigNotificationImpl.h"

#include "util/what.h"
DEFINE_WHATINFO;


using namespace sitraffic;

/**
 * Syntax:      SLONG ZZWertLesen (UTINY bNr, UTINY ESig, UTINY RSig, SSHORT *pZZWert
 * Die Funktion liefert den ZZ-Wert (in 1/10 Sek.) der angegebenen Einfahrsignalgruppe
 * gegen die angegebene Raeumsignalgruppe aus der in bNr gew�nschten ZZ-Matrix.
 * Wird als bNr der Wert 0 �bergeben, so wird der Wert aus der aktuell aktiven Matrix �bergeben.
 * Bei nichtfeindlichen Signalgruppen wird der Wert 32767 (MAX_SSHORT) �bergeben
 * impl of api_va.h
 *
 * @param bNr      number of intergreentime matrix 0, 1 ... MAX_ZZ_MATRIZEN
 * @param ESig     number of entering signal group (1...MAX_SIGNALGRUPPEN)
 * @param RSig     number of clearing signal group (1...MAX_SIGNALGRUPPEN)
 * @param pZZWert  Pointer auf Zwischenzeitwert
 * @return SUCCESS bei Erfolg
 *         ERROR_PARAMETER bei unzul�ssiger bNr
 *         ERROR_SIG_UNBENUTZT     bei ung�ltiger Signalgruppe
 *
 */
SLONG ZZWertLesen (UTINY bNr, UTINY ESig, UTINY RSig, SSHORT* pZZWert)
{
  if(bNr > MAX_ZZ_MATRIZEN || !pZZWert)
    {
      Report(API_WARNING, "intergreentime matrix list %d or pZZWert %p invalid, returning ERROR_PARAMETER %d",(int)bNr, pZZWert, ERROR_PARAMETER);
      return ERROR_PARAMETER;
    }
  if(bNr==0)
    bNr = 1;    //TODO get current list
  {
      const Config & cfg(InstRegistry::Instance().getConfig());
      const int sgMin=cfg.getMinSgNo();
      const int sgMax=cfg.getMaxSgNo();
      if(ESig < sgMin || ESig >sgMax || RSig < sgMin || RSig >sgMax)
        {
          Report(API_WARNING, "invalid signal group %d returning ERROR_SIG_UNBENUTZT %d",(int)ESig, ERROR_SIG_UNBENUTZT);
          return ERROR_SIG_UNBENUTZT;
        }
      *pZZWert = cfg.getIgt(bNr).get(ESig, RSig);
      Report(API_TRACE, "ZZWERTLesen(%d, %d, %d, *%p=%d)=SUCCESS", (int)bNr, (int)ESig, (int)RSig, pZZWert, (int)(*pZZWert));
      return SUCCESS;
  }
}

/**
 * Je nach uebergebenen Parametern werden unterschiedliche Objekte ausgelesen.
 * Die Werte f�r die Zwischenzeiten werden in 1/10 Sekunden erwartet.
 * Bei nichtfeindlichen Signalgruppen wird der Wert 32767 (MAX_SSHORT) uebergeben.
 *<ol><li>
 * 1)      Anforderung der kompletten aktuellen ZZ Matrix:
 * ESig  =  0;
 * RSig  = 0;
 * bNr    =  0;
 * *pObjekt vom Typ (SSHORT) * MAX_SIGNALGRUPPEN * MAX_SIGNALGRUPPEN
 *</li><li>
 * 2)      Anforderung einer kompletten versorgten ZZ Matrix (1..3)
 * ESig  =  0;
 * RSig  = 0;
 * bNr    =  1..3
 * *pObjekt vom Typ (SSHORT) * MAX_SIGNALGRUPPEN * MAX_SIGNALGRUPPEN
 *</li><li>
 * 3)      Anforderung einer Zeile in der aktuellen aktiven ZZ Matrix
 * ESig  =  0;
 * RSig  = 1..... MAX_SIGNALGRUPPEN;
 * bNr    =  0;
 * *pObjekt Gr��e: (SSHORT * MAX_SIGNALGRUPPEN);
 *</li><li>
 * 4)      Anforderung einer Zeile aus einer versorgten ZZ Matrix (1..3)
 * ESig  =  0;
 * RSig  = 1..... MAX_SIGNALGRUPPEN;
 * bNr    =  1..3;
 * *pObjekt  Gr��e: (SSHORT * MAX_SIGNALGRUPPEN);
 *</li><li>
 * 5)      Anforderung einer Spalte in der aktuellen aktiven ZZ Matrix
 * ESig  =  1..... MAX_SIGNALGRUPPEN;
 * RSig  =  0;
 * bNr    =  0;
 * *pObjekt  Gr��e:  (SSHORT * MAX_SIGNALGRUPPEN);
 *</li><li>
 * 6)      Anforderung einer Spalte aus einer versorgten ZZ Matrix (1..3)
 * ESig  =  1..... MAX_SIGNALGRUPPEN;
 * RSig  =  0;
 * bNr    =  1..3;
 * *pObjekt Gr��e: (SSHORT * MAX_SIGNALGRUPPEN);
 *</li><li>
 * 7)      Anforderung eines Wertes in der aktuellen aktiven ZZ Matrix (1..3)
 * ESig  =  1..... MAX_SIGNALGRUPPEN;
 * RSig  =  1..... MAX_SIGNALGRUPPEN;
 * bNr    =  0;
 * *pObjekt vom Typ SSHORT;
 *</li><li>
 * 8)      Anforderung eines Wertes aus einer versorgten ZZ Matrix (1..3)
 * ESig  =  1..... MAX_SIGNALGRUPPEN;
 * RSig  =  1..... MAX_SIGNALGRUPPEN;
 * bNr    =  1..3;
 * *pObjekt vom Typ SSHORT;
 *</li></ol>
 *
 * @param bNr      number of intergreentime matrix 0, 1 ... MAX_ZZ_MATRIZEN
 * @param ESig     number of entering signal group 0, 1 ... MAX_SIGNALGRUPPEN
 * @param RSig     number of clearing signal group 0, 1 ... MAX_SIGNALGRUPPEN
 * @param pZZObject       address of result, caller has to make shure that there is enough space
 * @return SUCCESS              bei Erfolg
 *         ERROR_PARAMETER      bei unzulaessiger bNr
 *         ERROR_SIG_UNBENUTZT  bei ungueltiger Signalgruppe
 *
 */
SLONG ZZObjekteLesen (UTINY bNr, UTINY ESig, UTINY RSig, void* pZZObject)
{
    if(bNr > MAX_ZZ_MATRIZEN || !pZZObject )
      {
        Report(API_WARNING, "intergreentime matrix list %d or pZZWert %p invalid, returning ERROR_PARAMETER %d",(int)bNr, pZZObject, ERROR_PARAMETER);
        return ERROR_PARAMETER;
      }

    if(ESig > MAX_SIGNALGRUPPEN || RSig > MAX_SIGNALGRUPPEN)
      {
        Report(API_WARNING, "intergreentime ESig %d or RSig %d invalid, returning ERROR_SIG_UNBENUTZT %d",(int)ESig, (int)RSig, ERROR_SIG_UNBENUTZT);
        return ERROR_SIG_UNBENUTZT;
      }

    if(bNr==0)
      bNr = 1;    //TODO get current list
    const Config & cfg(InstRegistry::Instance().getConfig());
    const CfgIgtTable & igt(cfg.getIgt(bNr));
    SSHORT * pZZWert=static_cast<SSHORT *>(pZZObject);

    const int estart=ESig?ESig:1; const int eend=ESig?(ESig+1):(MAX_SIGNALGRUPPEN+1);
    const int rstart=RSig?RSig:1; const int rend=RSig?(RSig+1):(MAX_SIGNALGRUPPEN+1);

    for(int e=estart; e<eend; e++)
      for(int r=rstart; r<rend; r++)
        *pZZWert++ = igt.get(e,r);

    Report(API_TRACE, "ZZObjekteLesen(%d, %d, %d, *%p)=SUCCESS", (int)bNr, (int)ESig, (int)RSig, pZZWert);
    return SUCCESS;
}

// impl of api_va.h
SLONG VsgDatenLesen (UTINY bObjID, UTINY bIndex, void* pObject)
{
    const Config & cfg(InstRegistry::Instance().getConfig());
    SLONG ret;
        switch(bObjID)
        {
        case VD_STAMMDATEN:
                ret=cfg.getStammdaten(pObject);
                break;
        case VD_ECKDATEN:
                ret = cfg.getEckdaten(bIndex, pObject);
                break;
        case VD_SIG_BESCHREIBUNG:
                ret = cfg.getSiggruppe(bIndex, pObject);
                break;
        case VD_MINDESTFREIGABELISTE:
        case VD_MINDESTSPERRLISTE:
                ret = cfg.getMindauer(bObjID, bIndex, reinterpret_cast<MINDAUERLISTE *>(pObject));
                break;
        case VD_SIGNALPLAN:
                ret = cfg.getSpl(bIndex,  reinterpret_cast<SIGNALPLAN *>(pObject));
                break;

        default:
                ret = AI_VSG_DATEN_LESEN_ERG1;
        }

        if(ret != AI_VSG_DATEN_LESEN_ERG0)
          Report(CFG_WARNING, "VsgDatenLesen(%u, %u, %p) returning %d", bObjID, bIndex, pObject, ret);
        else
          Report(CFG_INFO, "VsgDatenLesen(%u, %u, %p) returning %d", bObjID, bIndex, pObject, ret);

        return ret;
}

SLONG tacCfgGetGeneralParam(int isDraft, TacCfgGeneralParam *pGeneralParam)
  {
    if(isDraft)
      return 1;//TODO retrieve draft config instance
    const Config & cfg(InstRegistry::Instance().getConfig());
    return cfg.getGeneralParam(pGeneralParam);
  }

/**
 * check if signal group with number sgNo is configured.
 * @param sgNo  signal group number to check
 * @return !=0 if signal group sgNo is configured
 *         ==0 if there is no signal group with sgNo configured
 */
int tacConfigIsSgConfigured(int sgNo)
{
  const Config & cfg(InstRegistry::Instance().getConfig());
  return cfg.isSgConfigured(sgNo);
}

//------------------------------------------------------------------------------------------
// retrieve inventory ta parameter blob.
// @param pData address where pointer to param blob is stored
//              memory responsibility is passed to the caller,
//              if caller does not need memory anymore, it has to be released with GibSpeicherFrei
// @param nData address where function stores number of parameter bytes read.
// @param isDraft if true draft component params are read, ==false inventory parameter are read.
// @return 0 OK, !=0 Error, no data read
static int tacGetComponentParams_(void ** pData, unsigned * nData, bool isDraft)
{
        static const char * taName=InstRegistry::Instance().getCltDeviceValueProviders().getName().c_str();//"pdmx";
        const DataReceiver & dr(InstRegistry::Instance().getDataReceiver());
        CLIENT *clnt=dr.getCCtrlClient("tcp");
        MkjtricActuationComponentParamsResult res;
        memset(&res, 0, sizeof res);
        *pData=0; *nData=0;
        clnt_stat stat=isDraft?
            getdraftcomponentparams_8(const_cast<char *>(taName), const_cast<char *>(dr.getMyHostname()), dr.getSvcPort(), &res, clnt)
            :
            getcomponentparams_8(const_cast<char *>(taName), &res, clnt);
        if(stat!=RPC_SUCCESS)
        {
            Report(CFG_ERROR, "get%scomponentparams_8(%s) RPC returned %s", isDraft?"draft":"", taName, clnt_sperrno(stat));
                clnt_destroy(clnt);
                return -1;
        }
        if(res.retCode != RC_OK)
        {
            Report(CFG_ERROR, "get%scomponentparams_8(%s) returned %d", isDraft?"draft":"", taName, res.retCode);
                clnt_freeres(clnt, (xdrproc_t) xdr_MkjtricActuationComponentParamsResult, (caddr_t)&res);
                clnt_destroy(clnt);
                return -2;
        }
        *pData = res.MkjtricActuationComponentParamsResult_u.tacp.componentParams.componentParams_val;
        *nData = res.MkjtricActuationComponentParamsResult_u.tacp.componentParams.componentParams_len;
        // don't call clnt_freeres, memory is passed to caller !
        // only works if clnt_freeres uses same allocationb strategy as GibSpeicherFrei
        clnt_destroy(clnt);
        return 0;
}
int tacGetComponentParams(void ** pData, unsigned * nData)      { return tacGetComponentParams_(pData, nData, false); }
int tacGetDraftComponentParams(void ** pData, unsigned * nData) { return tacGetComponentParams_(pData, nData, true);  }

/**
 * @param bSgInd        signal group index = signal group number -1
 * @param ocitColor     ocit color code the red/green state is requested for.
 * @return
 *      SPERREN         red state
 *      FREIGEBEN       green state
 *      LZ_UNZULAESSIG  signal group undefined or no red/green state configured for refered signal group
 */
UTINY VA_SgZustandBerechnen(UTINY bSgInd, UTINY ocitColor)
  {
    return InstRegistry::Instance().getConfig().VA_SgZustandBerechnen(bSgInd, ocitColor);
  }


void tacRegisterConfigNotifications(unsigned configTypeSet, ta_checkConfig_t fcheck, ta_activateConfig_t factivate, ta_cleanupConfig_t fcleanup)
  {
    Report(CFG_INFO,"tacRegisterConfigNotifications(%#x, %p, %p, %p)", configTypeSet, fcheck, factivate, fcleanup);
    InstRegistry::Instance().getConfigNotification().tacRegisterConfigNotifications(fcheck, factivate, fcleanup);
  }

void tacMsgListAddMsg(struct TacMsgList* pMsgList, enum TacMsgSeverity severity, const char * format, ...)
  {
    va_list myArgs;
    va_start(myArgs, format);
    Report(CFG_INFO,"tacMsgListAddMsg(%p, %d, %s ...) ", pMsgList, severity, format);
    pMsgList->tacMsgListAddMsg(severity, format, myArgs);
    va_end(myArgs);
  }
