///////////////////////////////////////////////////////////
//  ConfigNotificationImpl.cpp
//  Implementation of the Class ConfigNotificationImpl
//  Created on:      15-Mrz-2017 09:58:50
//  Original author: Peter Zelger
///////////////////////////////////////////////////////////

#include "cctrl/Config.h"
#include "instregistry.h"
#include "DataReceiver.h"
#include "ConfigNotificationImpl.h"
#include "mv/Detectors.h"
#include "util/repdef.h"
#include <cstdarg>

using namespace sitraffic;

ConfigNotificationImpl::ConfigNotificationImpl() : pConfigDraft(0), pDetectorsDraft(0),
 fpcheck(0),
 fpactivate(0),
 fpcleanup(0), isDraftChanged(true)
  {
  }



ConfigNotificationImpl::~ConfigNotificationImpl()
{
    delete pConfigDraft;
    delete pDetectorsDraft;
}


void ConfigNotificationImpl::updateDrafts()
  {
    Report(CFG_DEBUG, "updateDrafts pdet=%p pConfig=%p", pDetectorsDraft, pConfigDraft);
    if(!pDetectorsDraft)
      pDetectorsDraft=new Detectors();
    if(!pConfigDraft)
      pConfigDraft=new Config();
    CLIENT *clnt=InstRegistry::Instance().getDataReceiver().getCCtrlClient("tcp");
    clnt=pDetectorsDraft->fill(clnt, true);
    clnt=pConfigDraft->fill(clnt, true);
    if(clnt)
      clnt_destroy(clnt);
    isDraftChanged=false;
  }


const Config & ConfigNotificationImpl::getDraftConfig() const
  {
    if(isDraftChanged)
      const_cast<ConfigNotificationImpl *>(this)->updateDrafts();
    return *pConfigDraft;
  }
const Detectors & ConfigNotificationImpl::getDraftDetectors() const
{
  if(isDraftChanged)
    const_cast<ConfigNotificationImpl *>(this)->updateDrafts();
  return *pDetectorsDraft;
}



void ConfigNotificationImpl::tacRegisterConfigNotifications(ta_checkConfig_t fcheck, ta_activateConfig_t factivate, ta_cleanupConfig_t fcleanup)
  {
    Report(CFG_EVENT, "tacRegisterConfigNotifications check=%p activate=%p cleanup=%p", fcheck, factivate, fcleanup);
    fpcheck=fcheck;
    fpactivate=factivate;
    fpcleanup=fcleanup;
  }

static CheckResult toCheckResult(TacCheckConfigResult tccr)
  {
   switch (tccr)
      {
      case TAC_CR_NOT_AFFECTED:  //<! config consumer was not affected by the configuration to check.
        return CR_NOT_AFFECTED;
      case TAC_CR_ON_THE_FLY:    //<! config consumer is able to activate the configuration to check on the fly, that is without restart or reset.
        return CR_ON_THE_FLY;
      case TAC_CR_TA_RESTART:    //<! TA needs restart to operate with new config
        return CR_TA_RESTART;
      /** config consumer has detected a problem inside the configuration to check that makes it impossible for this config consumer to operate with it.
        * returning this value prevents transition from draft to iunventory of the configuration to check. */
      default:
      case TAC_CR_REFUSE:
        return CR_REFUSE;
      }
  }

/**
 * bitfield of <font color="#3f7f5f"><b>ConfigType</b></font>
 */
void ConfigNotificationImpl::doCheckConfig(int configTypes, CheckRespondCctrl * pCheckRespondCctrl)
  {
// create draft instances--> lacy
    TacMsgList tml;
    enum TacCheckConfigResult ret=TAC_CR_NOT_AFFECTED;
    Report(CFG_EVENT, "doCheckConfig configTypes=%#x", configTypes);
    isDraftChanged=true;
    if(fpcheck)
      ret=(*fpcheck)(configTypes, &tml);
    else
      tml.tacMsgListAddMsg(TAC_MS_HINT, "no check registered %#x %s %f", 0x7fffffff, "Test String", 31.1);

    pCheckRespondCctrl->result=toCheckResult(ret);
    tml.moveMsgsTo(*pCheckRespondCctrl);

    Report(CFG_EVENT, "doCheckConfig configTypes=%#x = result=%d", configTypes, pCheckRespondCctrl->result);
  }

void ConfigNotificationImpl::doActivate(int sysJobId)
  {
    Report(CFG_EVENT, "activate %#x", sysJobId);
    if(fpactivate)
      (*fpactivate)();
  }
void ConfigNotificationImpl::doCleanupConfigCheck()
  {
    Report(CFG_EVENT, "cleanupConfigCheck");
    if(fpcleanup)
      (*fpcleanup)();
  }

// register at cctrl
CLIENT * ConfigNotificationImpl::registerAtCctrl(CLIENT * clnt, const DataReceiver & dr)
  {
    if(!clnt)
      return clnt;
    const int configTypeSet=  CT_BASE|CT_TACODE|CT_TAPARAM;
    enum RetCodeCctrl retCode;
    if(registerasconfigconsumer_8(const_cast<char *>(dr.getMyHostname()), dr.getSvcPort(), configTypeSet, UID_TRAFFIC_ACTUACTED_CCONTROL, &retCode, clnt) != RPC_SUCCESS || retCode != RC_OK)
      {
        clnt_destroy(clnt);
        clnt = 0;
        Report(RPC_ERROR, "registerasconfigconsumer_8() failed retCode=%d", retCode);
      }
    else
      Report(CFG_EVENT, "ConfigNotificationImpl registered at Cctrl");
    return clnt;
  }

static SeverityCctrl toSeverityCctrl(TacMsgSeverity severity)
  {
    switch (severity)
      {
      case TAC_MS_OK:    //<! to be used with informations if used with status marked with green symbol
        return SMS_OK;
      case TAC_MS_HINT:  //<! to be used with hints if used with status marked with blue symbol
        return SMS_HINT;
      case TAC_MS_WARN:  //<! to be used with warnings if used with status marked with yellow symbol
        return SMS_WARNING;
      case TAC_MS_ERROR:  //<! to be used with error if used with status marked with orange symbol
        return SMS_ERROR;
      default:
        return SMS_UNKNOWN;
      }

  }

static bool isflag(char c)
{
        switch(c)
        {
        case '-':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '+':
        case ' ':
        case '#':
        case '.':
        case 'L':
        case 'z':
        case 'j':
        case 't':
        case 'l':
            return true;
        }
        return false;
}

static const char * parseType(const char *format, MsgParameterValueCctrl & mpv,  std::va_list & mpList)
{
        const char *cp=format;
    start:
        while(*cp && *cp!='%')
                cp++;
        if(*cp) cp++; // skip %
        if(*cp && *cp=='%')
                {
                cp++;
                goto start;
                }
        while(*cp && isflag(*cp) )
                { cp++; }
        //ret.u.e=cp;
        switch(*cp)
        {
        case 'd':
        case 'u':
        case 'o':
        case 'i':
        case 'x':
        case 'X':
          if(cp[-1]=='l')
            {
              if(cp[-2]=='l')
                {
                  mpv.typeInfo=MPTI_INT64;
                  mpv.MsgParameterValueCctrl_u.int64Value=va_arg(mpList, int64_t);
                }
              mpv.typeInfo=MPTI_LONG;
              mpv.MsgParameterValueCctrl_u.intValue=va_arg(mpList, int32_t);
            }
          else
              {
                mpv.typeInfo=MPTI_LONG;
                mpv.MsgParameterValueCctrl_u.intValue=va_arg(mpList, int);
              }
                break ;
        case 'f':
        case 'F':
        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
                mpv.typeInfo=MPTI_DOUBLE;
                mpv.MsgParameterValueCctrl_u.dValue=va_arg(mpList, double);
                break;
        case 's':
                mpv.typeInfo=MPTI_STRING;
                mpv.MsgParameterValueCctrl_u.strValue = strdup(va_arg(mpList, char*));
                break;
        default:
          Report(CFG_WARNING, "unexpected format specifier %c in %s ignored", *cp, format);
          break;
        }
    return cp;
}

void TacMsgList::tacMsgListAddMsg(TacMsgSeverity severity, const char * format, ...)
  {
    va_list mpList;
    va_start(mpList, format);
    CfgMessageCctrl msg;
    msg.sysjobid=4711;
    msg.severity=toSeverityCctrl(severity);
    msg.msg.format=strdup(format);
    const int maxparam=8;
    msg.msg.params.params_val= new MsgParameterValueCctrl[maxparam];
    msg.msg.params.params_len=0;
    const char * cp=format;
    while(msg.msg.params.params_len < maxparam)
      {
        cp = parseType(cp, msg.msg.params.params_val[msg.msg.params.params_len], mpList);
        if(*cp==0)
          break;
        msg.msg.params.params_len++;
      }

    //TODO parse, convert parameters

    msgs.push_back(msg);
    va_end(mpList);
  }

void TacMsgList::moveMsgsTo(CheckRespondCctrl & chkRespond)
  {
    if(msgs.size())
      {
      chkRespond.Messages.Messages_val=new CfgMessageCctrl[msgs.size()];
      for(TacMsgList::msgs_t::iterator it=msgs.begin(); it!= msgs.end(); it++)
        {
          chkRespond.Messages.Messages_val[chkRespond.Messages.Messages_len]=*it;
          chkRespond.Messages.Messages_len++;
        }
      msgs.clear();
      }
  }

