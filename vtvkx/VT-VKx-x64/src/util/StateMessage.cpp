/*
 * StateMessage.cpp
 *
 *  Created on: 29.10.2015
 *      Author: philippf
 */

#include <map>
#include "CControl.h"
#include "instregistry.h"
#include "DataReceiver.h"
#include <stdexcept>
#include "evhttpadapter.h"
#include <sstream>
#include <chrono>
#include "cctrl/htmlHandler.h"
#include "cctrl/Config.h"
#include "cctrl/wrappedCctrlDtos.h"
#include "cctrl/cltDeviceValues.h"
#include "util/TestAndDiagnostics.h"
#include "StateMessage.h"
#include "util/repdef.h"

#include "util/what.h"
DEFINE_WHATINFO

#define Dprintf(...)
// #define Dprintf fflush(stdout);printf
#define DFLUSH
// #define DFLUSH fflush(stdout);

struct MessageParams
  {
     MsgParameterValueCctrl p[7];
     bool status;
  };

static bool initialized = false;
static const int maxMsg = 20;
static int msgCount = 0; // "watermark" in msgCache
static CfgMessageCctrl msgCache[maxMsg];
static MessageParams paramCache[maxMsg];
static bool invalidated = false;
static char versions[300];


namespace sitraffic
{

enum SeverityCctrl getSeverity(SSHORT wErrorID);
int64_t getCurrentTicks();
int fillSysJobId(int system, int type, int subType, int instance, int jobID);
SLONG sendMessages();


const char * getResultString(SLONG ret);
std::string sendAllTestMessages();
std::string paramAsText(MsgParameterValueCctrl& p);

void WriteReportToDetailStateHistory(long attr, const char * file, const char * func, const char * text);
class InitializerWTH
{
   public:
    InitializerWTH()
    {
       Dprintf("ctor StateMessage.InitializerWTH\n");
       SetHistoryAppender(WriteReportToDetailStateHistory);
    };
};
static InitializerWTH initializer;



// temp. def.
//const SSHORT TL_FEHLER = 85;
//const SSHORT VA_MELDUNG = 88;
//const SSHORT TL_MELDUNG = 90;
//const SSHORT TL_PRIO_FEHLER = 91;
const SSHORT U_LED_ROT = 103;
const SSHORT U_LED_GELB = 111;
const SSHORT U_LED_GRUEN = 112;


// temp. def.
#define COMP_CORE "CC" // control core ("Steuerkern")
#define COMP_PDM  "PDM"
#define COMP_PDMe "PDMe"
#define COMP_S_L  "S_L"
#define COMP_S_Le "S_Le"


#define VALID_COMPS1(a)         a "_"
#define VALID_COMPS2(a,b)       a "_" b "_"
#define VALID_COMPS3(a,b,c)     a "_" b "_" c "_"
#define VALID_COMPS4(a,b,c,d)   a "_" b "_" c "_" d "_"
#define VALID_COMPS5(a,b,c,d,e) a "_" b "_" c "_" d "_" e "_"


bool matchComponent(const char * comp, const char * validComps)
{
   char needle[80];
   strcpy(needle, comp);
   strcat(needle, "_");
   return strstr(validComps, needle) != NULL;
}


enum SeverityCctrl getSeverity(SSHORT wErrorID)
{
    switch(wErrorID)
    {
      case VA_FEHLER:      return SMS_ERROR;
      case VA_MELDUNG:     return SMS_HINT;
      case VA_PRIO_FEHLER: return SMS_FAULT;
      case TL_MELDUNG:     return SMS_HINT;
      case TL_PRIO_FEHLER: return SMS_ERROR;
      case U_LED_ROT:      return SMS_WARNING;
      case U_LED_GELB:     return SMS_WARNING;
      case U_LED_GRUEN:    return SMS_WARNING;
      default:             return SMS_UNKNOWN;
    }
}

const char * getSeverityString(SeverityCctrl severity)
{
    switch(severity)
    {
      case    SMS_UNKNOWN : return  "UNKNOWN";
      case    SMS_OK      : return       "OK";
      case    SMS_HINT    : return     "HINT";
      case    SMS_WARNING : return  "WARNING";
      case    SMS_ERROR   : return    "ERROR";
      case    SMS_FAULT   : return    "FAULT";
      default: return "???";
    }
}

int64_t getCurrentTicks()
{
        static SignalIndicationsWCctrl * si = NULL;
        if(si == NULL)
          si = & sitraffic::InstRegistry::Instance().getSi();
        return si->baseTicktime;
}

int getFnr()
{
    static int fnr = -1;
    if(fnr < 0)
      {
        const Config & cfg = sitraffic::InstRegistry::Instance().getConfig();
        fnr = cfg.getFnr();
      }

    return fnr;
}

long getParam(const std::map<std::string, const char *> pmap, const char * paramKey)
{
   long paramValue = 0;
   auto iParam = pmap.find(paramKey);
   if(iParam != pmap.end())
   {
     const char * vParam = iParam->second;
    paramValue = strtol(vParam, NULL, 0);
   }
   return paramValue;
}

const char * getResultString(SLONG ret)
{
  static char buf[40];
  switch(ret)
  {
    case  NO_SUCCESS: return "<B>NO_SUCCESS</B>";
    case  SUCCESS:    return "SUCCESS";
    default:
      snprintf(buf, sizeof(buf), "unknown return value %d", ret);
      return buf;
  }
}

std::string paramAsText(MsgParameterValueCctrl& p)
  {
    std::ostringstream convert;
     switch(p.typeInfo)
     {
       case 0:           convert << "-"; return convert.str(); // no support of MPTI_BYTE here
       case MPTI_SHORT:  convert << p.MsgParameterValueCctrl_u.intValue; return convert.str();
       case MPTI_ENUM:   convert << p.MsgParameterValueCctrl_u.enumValue.name << '{' << p.MsgParameterValueCctrl_u.enumValue.ordinal << '}'; return convert.str();
       case MPTI_STRING: convert << "'" << p.MsgParameterValueCctrl_u.strValue << "'"; return convert.str();
       default:  convert << "?? type:" << (int)p.typeInfo << " ??"; return convert.str();
     }
  }

#define PARAM_AS_TEXT(a) ((a).typeInfo==MPTI_SHORT ? reinterpret_cast<const char *>((a).MsgParameterValueCctrl_u.intValue) : paramAsText(a).c_str())


void printTableOfSentMessages(std::ostringstream& ss)
{
   TableHelper t;
   char buf[100];
   snprintf(buf, sizeof(buf), "sent Messages: %d", msgCount);
   t.appendHeader(buf, "idsz");
   t.startTitle();
   t.addTitleColumn("Ticks").addTitleColumn("sysjobid").addTitleColumn("Severity").addTitleColumn("Message").addTitleColumn("P0(Nr)").addTitleColumn("P1").addTitleColumn("P2").addTitleColumn("P3").addTitleColumn("P4").addTitleColumn("P5").addTitleColumn("P6").endTitle();
   for(int i = 0; i < msgCount; i++)
   {
	 CfgMessageCctrl& msg = msgCache[i];
	 MessageParams & params = paramCache[i];
	 const char * sev = getSeverityString(msg.severity);
	 char text[200];
	 snprintf(text, sizeof(text), msg.msg.format, PARAM_AS_TEXT(params.p[0]), PARAM_AS_TEXT(params.p[1]), PARAM_AS_TEXT(params.p[2]), PARAM_AS_TEXT(params.p[3]), PARAM_AS_TEXT(params.p[4]), PARAM_AS_TEXT(params.p[5]), PARAM_AS_TEXT(params.p[6]));

	 const char * tickString = TableHelper::formatBigValue(msg.ticks);
	 t.startRow().addItem(tickString).addItem(msg.sysjobid).addItem(sev).addItem(text);
	 if(&params != 0)
	   t.addItem(paramAsText(params.p[0])).addItem(paramAsText(params.p[1])).addItem(paramAsText(params.p[2])).addItem(paramAsText(params.p[3])).addItem(paramAsText(params.p[4])).addItem(paramAsText(params.p[5])).addItem(paramAsText(params.p[6])).endRow();
   }
   t.endTable();
   ss << t.toString();
}

std::string StateMessageHtmlRequestHandler::doRequest(EvHttpRequest & ehr)
{
    std::ostringstream ss;
    ss << "<center>";

    ss << "<B>TabuMeldungAbsetzen</B>";

    ss << "<p>(" << __FILE__ << " from " << __TIMESTAMP__ << ")</p>";

    ss << "<p>ErrorID: VA_FEHLER ="<<VA_FEHLER << ", VA_MELDUNG="<<VA_MELDUNG << ", VA_PRIO_FEHLER="<<VA_PRIO_FEHLER << ", TL_MELDUNG="<<TL_MELDUNG<< ", TL_PRIO_FEHLER="<<TL_PRIO_FEHLER  << std::endl;

    ss << "<form action=\"StateMessage\" id=\"StateMessage\" >";
    ss << " <label id=\"l1\" form=\"StateMessage\"> ErrorID</label>" << std::endl
       << " <input type=\"number\" id=\"id\" name=\"id\" maxlength=\"3\">" << std::endl;
    ss << " <label id=\"l2\" form=\"StateMessage\"> Nr</label>" << std::endl
       << " <input type=\"number\" id=\"nr\" name=\"nr\" maxlength=\"3\">" << std::endl;
    ss << " <label id=\"l3\" form=\"StateMessage\"> p1</label>" << std::endl
       << " <input type=\"number\" id=\"p1\" name=\"p1\" maxlength=\"3\">" << std::endl;
    ss << " <label id=\"l4\" form=\"StateMessage\"> p2</label>" << std::endl
       << " <input type=\"number\" id=\"p2\" name=\"p2\" maxlength=\"3\">" << std::endl;
    ss << " <label id=\"l5\" form=\"StateMessage\"> p3</label>" << std::endl
       << " <input type=\"number\" id=\"p3\" name=\"p3\" maxlength=\"3\">" << std::endl;
    ss << " <label id=\"l6\" form=\"StateMessage\"> p4</label>" << std::endl
       << " <input type=\"number\" id=\"p4\" name=\"p4\" maxlength=\"3\">" << std::endl;
    ss << " <label id=\"l7\" form=\"StateMessage\"> p5</label>" << std::endl
         << " <input type=\"number\" id=\"p5\" name=\"p5\" maxlength=\"3\">" << std::endl;

    ss << " <button type=\"submit\">send</button>" << std::endl;
    ss << "</form>" << std::endl;


   auto & pmap(ehr.getParams());

   SSHORT wErrorID = (SSHORT)getParam(pmap, "id");
   if(wErrorID > 0)
   {
     SSHORT p[6];

     p[0] = (SSHORT)getParam(pmap, "nr");
     p[1] = (SSHORT)getParam(pmap, "p1");
     p[2] = (SSHORT)getParam(pmap, "p2");
     p[3] = (SSHORT)getParam(pmap, "p3");
     p[4] = (SSHORT)getParam(pmap, "p4");
     p[5] = (SSHORT)getParam(pmap, "p5");

     SLONG ret = TabuMeldungAbsetzen (wErrorID, &p[0], sizeof(p)/sizeof(p[0]));

     ss << "TabuMeldungAbsetzen '" << (int)wErrorID << "' Nr='" << p[0] << "' p1='" << p[1] <<"' p2='" << p[2] <<"' p3='" << p[3] <<"' p4='" << p[4] <<"' p5='" << p[5] << "' returned " << getResultString(ret);
   }
   else if(wErrorID == -44)
   {
      std::string ret = sitraffic::sendAllTestMessages();
      ss << "sendAllTestMessages() returned: " << ret;
   }
   else if(wErrorID < 0) // reset all messages
   {

     msgCount = 0;
     SLONG ret = sendMessages();
     ss << "sendMessages (msgCount = 0) returned " << getResultString(ret);
   }

   printTableOfSentMessages(ss);

   ss << "</center>" << std::endl;
   return ss.str();
}

int fillSysJobId(int system, int type, int subType, int instance, int jobID)
{
   return (((((((((system&0x03) << 4 ) | (type&0x0F)) << 4 ) | (subType&0x0F)) << 16 ) | (instance&0xFFFF)) << 6)) | (jobID&0x3F);
}

// wErrorID, nr=P0,   P1,      P2,      P3,      P4,      P5
//           ^-pTabuPar											  max. anzPar=6
// i:                 1        2        3        4        5
// pars[0],  pars[1], pars[2], pars[3], pars[4], pars[5], pars[6]
//
void fillParameters(SSHORT wErrorID, SSHORT nr, const SSHORT* pTabuPar, USHORT anzPar, const char ** stringPars, const char ** enumPars, MsgParameterValueCctrl * pars, InitiatorCCtrl * msgi)
{
  pars[0].MsgParameterValueCctrl_u.intValue = wErrorID;
  pars[0].typeInfo = MPTI_SHORT;
  pars[1].MsgParameterValueCctrl_u.intValue = nr;
  pars[1].typeInfo = MPTI_SHORT;
  for(int i = 1; i < anzPar; i++)
  {
    if(enumPars != NULL && enumPars[i] != NULL)
    {
   	  const char * enumPar = enumPars[i];
   	  MsgParamTypeEnumCctrl& ev = pars[i+1].MsgParameterValueCctrl_u.enumValue;
   	  ev.ordinal  = pTabuPar[i];
   	 // ev.name.name_len  = (u_int)strlen(enumPar);
   	 // ev.name.name_val  = const_cast<char *>(enumPar);
   	  ev.name = const_cast<char *>(enumPar);
   	  pars[i+1].typeInfo = MPTI_ENUM;

     // Report(MSG_WARNING, "fillParameters: i=%d ordinal=%d name=%s", i, ev.ordinal, ev.name);
    }
    else if(stringPars != NULL && stringPars[i] != NULL)
    {
   	  const char * stringPar = stringPars[i];
   	  pars[i+1].MsgParameterValueCctrl_u.strValue = const_cast<char *>(stringPar);
   	  pars[i+1].typeInfo = MPTI_STRING;

   	 // Report(MSG_WARNING, "fillParameters: i=%d string=%s", i, stringPar);
    }
    else
    {
    	if(pTabuPar == 0)
    	  Report(MSG_DEBUG, "fillParameters: wErrorID=%d, nr=%d, i=%d, anzPar=%d, pTabuPar==NULL", wErrorID, nr, i, anzPar);
    	else
    	{
	  pars[i+1].MsgParameterValueCctrl_u.intValue  = pTabuPar[i];
	  pars[i+1].typeInfo = MPTI_SHORT;
	  //Report(MSG_WARNING, "fillParameters: wErrorID=%d, nr=%d, i=%d, anzPar=%d par=%d", wErrorID, nr, i, anzPar, pTabuPar[i]);
    	}
    }
  }

  msgi->params.params_val = & pars[0];
  msgi->params.params_len = anzPar + 1;
}

int64_t tickOffset = 0;
void fillMessage(CfgMessageCctrl * msg, SSHORT wErrorID, SSHORT nr, const SSHORT* pTabuPar, USHORT anzPar, const char ** stringPars, const char ** enumPars, SeverityCctrl severity, const char * format, MsgParameterValueCctrl * pars)
{
   msg->severity = severity;
   static int count = 1;
   int system = 3; // intersection controller
   int type = 2; // TA logic
   int subType = 1; // legacy - TA
   int instance = getFnr(); // field device No.
   msg->sysjobid = fillSysJobId(system, type, subType, instance, count++);
   msg->ticks = getCurrentTicks() + tickOffset;
   InitiatorCCtrl * msgi = & msg->msg;
   msgi->format = const_cast<char*>(format);

  //  MsgParameterValueCctrl pars[anzPar + 1];
   fillParameters(wErrorID, nr, pTabuPar, anzPar, stringPars, enumPars, pars, msgi);

  // MsgParameterValueCctrl * p = msgi->params.params_val;
  //const char * wErrorIDs = paramAsText(p[0]).c_str();
  //const char * nrs       = paramAsText(p[1]).c_str();
  //Report(MSG_WARNING, "fillMessage: '%s' wErrorIDs:%s nr:%s p1:%s p2:%s p3:%s p4:%s p5:%s p6:%s", msgi->format, paramAsText(p[0]).c_str(), paramAsText(p[1]).c_str(),
  //		  paramAsText(p[2]).c_str(), paramAsText(p[3]).c_str(), paramAsText(p[4]).c_str(), paramAsText(p[5]).c_str(), paramAsText(p[6]).c_str(), paramAsText(p[7]).c_str() );
}

/**
 * avoid sending the state messages during runtime of TA
 */
SLONG flushStateMessages()
{
	SLONG ret = SUCCESS;
	if(invalidated)
	{
	  ret = sendMessages();
	  Dprintf("flushStateMessages: sent Messages: msgCount=%d\n", msgCount);
	}
	return ret;
}



SLONG sendMessages()
{
    SLONG ret = SUCCESS;
    int versionIdx = -1;

    if(msgCount == 0)
    {
		if(TestAndDiagnosticsHttpRequestHandler::isDemoVersion())
		   fillMessage(&msgCache[msgCount], 0, 0, NULL, 0, NULL, NULL, SMS_OK, "TacKernel DEMO active", paramCache[msgCount].p);
		else
		   fillMessage(&msgCache[msgCount], 0, 0, NULL, 0, NULL, NULL, SMS_OK, "PDMx fehlerfrei", paramCache[msgCount].p);
		msgCount++;
    }

    if(sitraffic::TestAndDiagnosticsHttpRequestHandler::isVersionInvalidated() )
    {
      // append versions once, respectively after change
      const char * stringPar[TABU_ANZ_ERR_PAR];
      memset(&stringPar[0], 0, sizeof(stringPar));

      snprintf(versions, sizeof(versions), "%s", sitraffic::TestAndDiagnosticsHttpRequestHandler::getTacVersionsHTML().c_str());
      MessageParams * mp = &paramCache[msgCount];

      stringPar[1] = versions;
      mp->status = false;
      fillMessage(&msgCache[msgCount], 1, 2, NULL, 3, (const char **)&stringPar, NULL, SMS_OK, "versions: %3$s", mp->p);
      sitraffic::TestAndDiagnosticsHttpRequestHandler::setVersionInvalidated(false);
      versionIdx = msgCount;
      msgCount++;
      Report(MSG_EVENT, "msgCount:%d filled version:%s", msgCount, versions);
    }

    sitraffic::DataReceiver & dr = sitraffic::InstRegistry::Instance().getDataReceiver();

    const char * host = dr.getMyHostname();
    int port = dr.getSvcPort();
    CltStatusMsgsCctrl status = CltStatusMsgsCctrl();
    status.unit = UID_TRAFFIC_ACTUACTED_CCONTROL;

    status.msgs.msgs_len = msgCount;
    status.msgs.msgs_val = msgCache;

    enum RetCodeCctrl clnt_res = RetCodeCctrl();
    CLIENT * clnt = dr.getCCtrlClient();


//    Report(MSG_WARNING, "sendMessages: host=%s port=%d status=%p", host, port, &status);

//    for(unsigned int i = 0; i< status.msgs.msgs_len; i++)
//      {
//        CfgMessageCctrl& m = status.msgs.msgs_val[i];
//       Report(MSG_WARNING, "sendMessages: i=%d ticks=%lld sev=%d sysjobid=%d format=%s params=%d:", i, m.ticks, m.severity, m.sysjobid, m.msg.format, m.msg.params.params_len);
//        for(unsigned int j = 0; j < m.msg.params.params_len; j++)
//        {
//           MsgParameterValueCctrl& p =  m.msg.params.params_val[j];
//           switch(p.typeInfo)
//           {
//             case MPTI_SHORT: Report(MSG_WARNING, "sendMessages: j=%d MPTI_SHORT intValue=%d", j, p.MsgParameterValueCctrl_u.intValue); break;
//             case MPTI_ENUM:  Report(MSG_WARNING, "sendMessages: j=%d MPTI_ENUM ordinal=%d name=%s", j, p.MsgParameterValueCctrl_u.enumValue.ordinal,
//                                            p.MsgParameterValueCctrl_u.enumValue.name); break;
//             default    :     Report(MSG_WARNING, "sendMessages: j=%d type=%d", j, p.typeInfo);
//           }
//        }
//      }

    auto start = std::chrono::high_resolution_clock::now();
    enum clnt_stat rpcRet = setcltstatus_8(const_cast<char*>(host), port, status, &clnt_res,  clnt);
    auto finish = std::chrono::high_resolution_clock::now();

   // std::cout << std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count();
    Report(MSG_INFO, "sendMessages: msgCount = %d duration = %" PRId64" nanos, versionIdx = %d",
    		msgCount, std::chrono::duration_cast<std::chrono::nanoseconds>(finish-start).count(), versionIdx);

    if( versionIdx >= 0) // remove above sent version message (assume version msg is the last with the highest index)
	{
    	msgCount--;
	}

    if(rpcRet != RPC_SUCCESS)
      ret = NO_SUCCESS;

    if(clnt_res != RC_OK)
      ret = NO_SUCCESS;

    clnt_destroy(clnt);

    if(ret != SUCCESS)
      Report(MSG_WARNING, "setcltstatus_8 with rpc error: %d client error: %d", rpcRet, clnt_res);

    invalidated = false;

    return ret;
}


SSHORT testMessageArray[][7] =
{
    {VA_FEHLER,999, 1, 2, 3, 4, 5},
    {VA_FEHLER,  1, 1, 0, 0, 0, 0},
    {VA_FEHLER,  1, 2, 0, 0, 0, 0},
    {VA_FEHLER,  1, 3, 0, 0, 0, 0},

    {VA_FEHLER,  2, 1, 0, 0, 0, 0},
    {VA_FEHLER,  2, 2, 0, 0, 0, 0},
    {VA_FEHLER,  2, 3, 0, 0, 0, 0},

    {VA_FEHLER,  3, 1, 0, 0, 0, 0},
    {VA_FEHLER,  3, 2, 0, 0, 0, 0},
    {VA_FEHLER,  3, 2, 1, 0, 0, 0},
    {VA_FEHLER,  3, 2, 2, 0, 0, 0},
    {VA_FEHLER,  3, 2, 11, 0, 0, 0},
    {VA_FEHLER,  3, 2, 12, 0, 0, 0},
    {VA_FEHLER,  3, 2, 13, 0, 0, 0},
    {VA_FEHLER,  3, 2, 14, 0, 0, 0},
    {VA_FEHLER,  3, 2, 15, 0, 0, 0},
    {VA_FEHLER,  3, 2, 16, 0, 0, 0},
    {VA_FEHLER,  3, 2, 17, 0, 0, 0},
    {VA_FEHLER,  3, 2, 18, 0, 0, 0},
    {VA_FEHLER,  3, 2, 30, 0, 0, 0},
    {VA_FEHLER,  3, 2, 50, 0, 0, 0},
    {VA_FEHLER,  3, 2, 51, 0, 0, 0},
    {VA_FEHLER,  3, 2, 52, 0, 0, 0},
    {VA_FEHLER,  3, 2, 53, 0, 0, 0},
    {VA_FEHLER,  3, 2, 60, 0, 0, 0},

    {VA_FEHLER,  3, 31, 12, 1, 2, 0},

  {VA_FEHLER,  21, 0, 0, 0, 0, 0},
  {VA_FEHLER,  22, 0, 0, 0, 0, 0},
  {VA_FEHLER,  23, 0, 0, 0, 0, 0},
  {VA_FEHLER,  24, 0, 0, 0, 0, 0},


    {VA_FEHLER, 150, 30, 1, 7, 1,  0},
    {VA_FEHLER, 150, 30, 2, 7, 2,  0},
    {VA_FEHLER, 150, 30, 2, 7, 3,  0},
    {VA_FEHLER, 150, 30, 2, 7, 4,  0},
    {VA_FEHLER, 150, 30, 2, 7, 5,  0},
    {VA_FEHLER, 150, 30, 2, 7, 6,  0},
    {VA_FEHLER, 150, 30, 2, 7, 7,  0},
    {VA_FEHLER, 150, 30, 2, 7, 8,  0},
    {VA_FEHLER, 150, 30, 2, 7, 9,  0},
    {VA_FEHLER, 150, 30, 2, 7, 10, 0},
    {VA_FEHLER, 150, 30, 2, 7, 11, 0},

  {VA_FEHLER, 160, 0, 0, 0, 0, 0},
  {VA_FEHLER, 161, 0, 0, 0, 0, 0},
  {VA_FEHLER, 162, 0, 0, 0, 0, 0},

    {VA_FEHLER,    170, 1, 2, 3, 4, 5},

	  {VA_MELDUNG,   170, 1, 6, 0, 0, 0},
	  {VA_MELDUNG,   999, 1, 2, 3, 4, 5},
//    {VA_MELDUNG,   3, 1, 2, 3, 4, 5},
//    {VA_MELDUNG,   3, 2, 1, 0, 0, 0},
  //  {VA_MELDUNG,   3, 2, 2, 3, 4, 5},
   // {VA_MELDUNG,   3, 3, 2, 3, 4, 5},

    {VA_PRIO_FEHLER, 8, 1, 2, 3, 4, 5},

    {TL_MELDUNG,     8, 1, 2, 3, 4, 5},

    {TL_PRIO_FEHLER, 8, 1, 2, 3, 4, 5},

    {99,           999, 1, 2, 3, 4, 5},
};



std::string sendAllTestMessages()
{
  int count = 0;
  int available = sizeof(testMessageArray) / sizeof(testMessageArray[0]);
  tickOffset = 0; // overcome the message filter in StateCounter.hasChanges() of sX firmware if format string (==nr) is identical to previous format string

  for(int i = 0; i < available; i++, tickOffset++)
  {
    SSHORT * pars = testMessageArray[i];

    SSHORT wErrorID = pars[0];
    SSHORT* pTabuPar = &pars[1];

    SLONG ret = TabuMeldungAbsetzen (wErrorID, pTabuPar, 6);

    if(ret != SUCCESS)
    {
       return  std::string("TabuMeldungAbsetzen returned ") + sitraffic::getResultString(ret);
    }

    count ++;

    if( (i+1)%maxMsg == 0)
    {
      flushStateMessages();
      resetStatus(); // make space for the next bunch of test messages
    }
  }

  tickOffset = 0;

  return std::string("count=") + std::to_string(count);
}

const char * getEnumPar_150_30_P2(int ordinal)
{
    switch(ordinal)
    {
      case 1:  return "Objekt Streckenparameter";
      case 2:  return "Objekt Zeitparameter";
      default: return "???";
    }
}

const char * getEnumPar_150_30_P4(int ordinal)
{
    switch(ordinal)
    {
      case 1:  return "Parameterpointer == NULL";
      case 3:  return "SG- bzw. VS-Nummer falsch";
      case 4:  return "Notanmeldedetektor bei Impuls falsch";
      case 5:  return "Notanmeldedetektor bei Belegung falsch";
      case 6:  return "Notanmeldedetektor bei Luecke falsch";
      case 7:  return "Funktion fuer Notanmeldung falsch";
      case 8:  return "Nummer fuer TW-Timer falsch";
      case 9:  return "MP-Nummer bei paralleler Meldung falsch";
      case 10: return "Funktion des letzten Meldepunktes falsch";
      case 11: return "Positve und negative Linien-Routennummern parametriert";
      default: return "???";
    }
}

const char * getEnumPar_3_32_P2(int ordinal)
{
    switch(ordinal)
    {
      case 1:  return "Steuerungskern";
      case 2:  return "TL-Anwenderparameter";

      case 11:  return "VS-PLUS";
      case 12:  return "PDM";
      case 13:  return "OEV";
      case 14:  return "SDM";
      case 15:  return "NORRA";
      case 16:  return "SPIRIT";
      case 17:  return "FESA";
      case 18:  return "STRIDE";

      case 30:  return "OML";

      case 50:  return "Gruppensteuerung";
      case 51:  return "S-X";
      case 52:  return "S-L";
      case 53:  return "M-X";

      case 60:  return "MW-PLUS";

      default: return "???";
    }
}


SSHORT getErrorID(int prio, int color)
{

    switch(prio)
    {
      case 0:      return VA_PRIO_FEHLER;
      case 1:      return color == R_BLUE ? VA_MELDUNG : VA_FEHLER;
      case 2:      return color == R_GREEN ? VA_MELDUNG : VA_MELDUNG;
      case 3:      return VA_MELDUNG;
      case 4:      return VA_MELDUNG;
      case 5:      return VA_MELDUNG;
      default:     return VA_MELDUNG;
    }

}

enum SeverityCctrl getSeverity(int prio, int color)
{
    switch(prio)
    {
      case 0:      return SMS_FAULT;
      case 1:      return color == R_BLUE ? SMS_HINT : SMS_ERROR;
      case 2:      return color == R_GREEN ? SMS_HINT : SMS_WARNING;
      case 3:      return SMS_OK;
      default:     return SMS_UNKNOWN;
    }
}


static char loc_text[500];
void WriteReportToDetailStateHistory(long attr, const char * file, const char * func, const char * text)
{
    Dprintf("*** WriteReportToDetailStateHistory: attr: %ld format: %s\n", attr, text);

    if (attr & APP_HISTORY)
    {
        int prio = Ri_GET_PRIO(attr);
        int color = R_EXTRACT_COL(attr);
        SSHORT wErrorID = getErrorID(prio, color);
        const SeverityCctrl& severity = getSeverity(prio, color);

        strcpy(loc_text, file);
        strcat(loc_text, " ");
        strcat(loc_text, func);
        strcat(loc_text, " ");
        strcat(loc_text, text);

        SSHORT  pars[7] = {IdReportAsDetailState, 0, 0, 0, 0, 0, 0}; //(const SSHORT *) parptr;
        USHORT anzPar = 5; // TODO

        Dprintf("WriteReportToDetailStateHistory:UpdateCltMessage: wErrorID=%d severity=%d\n", wErrorID, severity);

        sitraffic::UpdateCltMessage (wErrorID, pars, anzPar, severity, loc_text, false);
    }
}


SLONG UpdateCltMessageEnum(SSHORT wErrorID, const SSHORT * pTabuPar, USHORT anzPar, const char ** stringPars, const char ** enumPars, const SeverityCctrl& severity, const char * format, bool status)
{
  SLONG ret = SUCCESS;
  SSHORT nr =  pTabuPar[0];

  if(format != NULL) // add message
  {
    //const char * enumPar[TABU_ANZ_ERR_PAR];
    //memset(&enumPar[0], 0, sizeof(enumPar));

    time_t rawtime; time(&rawtime);
    Dprintf("1. UpdateCltMessage: msgCount=%d format=%s %s", msgCount, format, ctime(&rawtime));DFLUSH

    CfgMessageCctrl * dsMsg = NULL;
    MessageParams * dsParams = NULL;
    if( ! invalidated) // prevent that messages with the same format string gets overwritten in the same time slot (before flush)
    {
		for(int i = 0; i < msgCount; i++)
		{
		  CfgMessageCctrl * msg = &msgCache[i];
		  MessageParams * par = &paramCache[i];
		  if(msg->msg.format == format || par->status == false)
		  {
			 dsMsg = msg;
			 dsParams = par;
			 break;
		  }
		}
    }

    Dprintf("2. UpdateCltMessage: msgCount=%d dsMsg=%p\n", msgCount, dsMsg);DFLUSH
    if(dsMsg == NULL)
    {
      if(msgCount < maxMsg - 1)
      {
        dsMsg = &msgCache[msgCount];
        dsParams = &paramCache[msgCount];
        msgCount++;
      }
      else // overwrite first message
      {
        dsMsg = &msgCache[0];
        dsParams = &paramCache[0];
      }
    }

    Dprintf("3. UpdateCltMessage: status=%d\n",status);DFLUSH
    dsParams->status = status;

    Dprintf("4. UpdateCltMessage: fillMessage: msgCount=%d cfgMsg=%p status=%d\n", msgCount, dsMsg, status);DFLUSH
    fillMessage(dsMsg, wErrorID, nr, pTabuPar, anzPar, stringPars, enumPars, severity, format, dsParams->p);
    Dprintf("5. UpdateCltMessage:filled\n");DFLUSH
  }
  else // remove message
  {
    int i;
    for(i = 0; i < msgCount; i++)
    {
      //CfgMessageCctrl * msg = &msgCache[i];
      MessageParams * dsParams = &paramCache[i];
      if(dsParams->p[0].MsgParameterValueCctrl_u.intValue == wErrorID
          && dsParams->p[1].MsgParameterValueCctrl_u.intValue == nr
          && (anzPar <= 1 || dsParams->p[2].MsgParameterValueCctrl_u.intValue == pTabuPar[1]) )
      {
         break;
      }
    }

    if(i < msgCount) // message to remove was found
    {
      for(; i < msgCount - 1; i++)
      {
        msgCache[i] = msgCache[i+1];
        paramCache[i] = paramCache[i+1];
      }
      --msgCount;
    }
    else
      return ret;       // no message removed, so don't send messages

  }

  invalidated = true;
 // ret = sendMessages(msgCount, msgCache, paramCache);

 // Dprintf("sent Messages: msgCount=%d\n", msgCount);
  return ret;
}

SLONG UpdateCltMessage (SSHORT wErrorID, const SSHORT * pTabuPar, USHORT anzPar, const SeverityCctrl& severity, const char * format, bool status)
{
	return UpdateCltMessageEnum(wErrorID, pTabuPar, anzPar, NULL, NULL, severity, format, status);
}

/**
 * please call after initializing tacKernel (maybe PDM sent already some errors)
 */
void initStateMessages()
{
  Report(MSG_EVENT, "msgCount:%d", msgCount);
  invalidated = true;
  initialized = true;
}


}


using namespace sitraffic;




// 2.6.5         Betriebstagebuchmeldung absetzen
// Syntax:      SLONG TabuMeldungAbsetzen (USHORT       wErrorID,
//              UTINY   bSource,
//              UTINY * pTabuPar );
// Parameter:   wErrorID :      moegliche Fehlerkennungen
//              TL_FEHLER (85)
//              VA_MELDUNG (88)
//              TL_MELDUNG (90)
//              TL_PRIO_FEHLER (91)
//                      U_LED_ROT' (103)
//                 'U_LED_GELB' (111)
//                 'U_LED_GRUEN' (112)
//      bSource :       Task in der der Fehler aufgetreten ist bzw. die den Fehler meldet, bei
//              Lizensierung nur VA moeglich
//      pTabuPar :      Pointer auf Parameterfeld, welche den Fehler naeher charakterisieren
//              (max. TABU_ANZ_ERR_PAR (8) werden dem Tagebuch uebergeben)
// Rueckgabe:   SUCCESS bei Erfolg
//      ERROR_NO_SOURCE         bei ungueltigem Absender in bSource
// Beschreibung:        Diese Funktion gibt eine Tagebuchmeldung aus. Datum und Uhrzeit werden hierbei ergaenzt.


// 1f_Fehlerhandbuch:
// z.B.:
// 4.3 Priorer VA-Fehler...
//      Nr. Par1 Par2 Par3 Par4 Par5    Bedeutung                                        Komponente    Fehlerart
//      151                        0    S-X Parameterfehler                              S-X           Parameter
//             3                        fehlerhafter Parameter im Modul ,SXSgParameter'
//                  1                   Freigabezeiten nicht aufsteigend
//                       X              Instanz
//                            Y         Gruppennummer


SLONG TabuMeldungAbsetzen (SSHORT wErrorID, SSHORT* pTabuPar, USHORT anzPar)
{
  SLONG ret = SUCCESS;

  if(anzPar >= 6)
    Report(MSG_EVENT, "wErrorID:%d anzPar:%d nr:%d, p1:%d p2:%d p3:%d p4:%d p5:%d", wErrorID, anzPar, pTabuPar[0], pTabuPar[1], pTabuPar[2], pTabuPar[3], pTabuPar[4], pTabuPar[5]);
  else
    Report(MSG_WARNING, "wErrorID:%d anzPar:%d nr:%d", wErrorID, anzPar, pTabuPar[0]);

  const char * comp = InstRegistry::Instance().getCltDeviceValueProviders().getName().c_str();

  SSHORT nr =  pTabuPar[0];

  const char * stringPar[TABU_ANZ_ERR_PAR];
  memset(&stringPar[0], 0, sizeof(stringPar));
  const char * enumPar[TABU_ANZ_ERR_PAR];
  memset(&enumPar[0], 0, sizeof(enumPar));

  const char * format = "Unknown message %d. No.: %d. Parameters of TL[5] = %d, %d, %d, %d, %d";
  bool status = true;

  SeverityCctrl severity = getSeverity(wErrorID);

  switch(wErrorID)
  {
    case VA_FEHLER:
      format = "TA error (%d). No.: %d. Parameters of TL[5] = %d, %d, %d, %d, %d";
      if(strcmp(comp, "pdmx")==0)
      {
		switch(nr)
		{
		  case 1:   // Fehler bei Komponentenanmeldung
			switch(pTabuPar[1])
			{
			   case 1: format = "VA-Fehler(%d) bei Komponentenanmeldung: (%d)(%d) Zu viele Komponenten: %d";	break;
			   case 2: format = "VA-Fehler(%d) bei Komponentenanmeldung: (%d)(%d) Komponentendaten nicht schluessig: %d";	break;
			   case 3: format = "VA-Fehler(%d) bei Komponentenanmeldung: (%d)(%d) Komponente benutzt bei der Anmeldung eine Anmeldestruktur, die der Kern noch nicht unterstuetzt : %d.%d";	break;
			}
			break;
		  case 2:   // Message fehlerhaft
			switch(pTabuPar[1])
			{
			   case 1: format = "VA-Fehler(%d) Message fehlerhaft: (%d)(%d) Unbekannte Version des Infoheaders: %d.%d";	break;
			   case 2: format = "VA-Fehler(%d) Message fehlerhaft: (%d)(%d) Keinen dynamischen Speicher erhalten (bei extended MSG)";	break;
			   case 3: format = "VA-Fehler(%d) Message fehlerhaft: (%d)(%d) MSG-Block fehlt (bei extended MSG)";	break;
			}
			break;
		 case 3:   // Parameteruebertragung
		   switch(pTabuPar[1])
		   {
			  case 1:
				  format = "VA-Fehler(%d) Parameteruebertragung (%d)(%d): Parameterdatei nicht vorhanden.";
			  break;
			  case 2:
				if(pTabuPar[3] > 0 || pTabuPar[4] > 0)
				  format = "VA-Fehler(%d) Parameteruebertragung (%d)(%d) Komponente nicht gefunden: (%s) Version: %d.%d";
				else
				  format = "VA-Fehler(%d) Parameteruebertragung (%d)(%d) Komponente nicht gefunden: (%s)";
				enumPar[2] = getEnumPar_3_32_P2(pTabuPar[2]);
			  break;
			  case 20:
				  format = "VA-Fehler(%d) Parameteruebertragung (%d)(%d): Keine Parameterdatei";
			  break;
			  case 32:
				  format = "VA-Fehler(%d) Parameteruebertragung (%d)(%d): Die in der Parameterdatei angegebene Version fuer die Komponente stimmt nicht mit der geladenen Version der Komponente ueberein."
						  " ID: %s Version: %d.%d";
				  enumPar[2] = getEnumPar_3_32_P2(pTabuPar[2]);
			  break;
		    }
		    break;

		 case 150:   // Fehlerhafter Parameter
		   switch(pTabuPar[1])
		   {
			 case 30:
			   format = "VA-Fehler(%d) Fehlerhafter OeV-Parameter: (%d)(%d): %s: Parameterinstanz: %d Detail: %s OeV-Gruppe: %d";
			   enumPar[2] = getEnumPar_150_30_P2(pTabuPar[2]);
			   enumPar[4] = getEnumPar_150_30_P4(pTabuPar[4]);
			   break;
		   }
		   break;
		 case 170:
		   switch(pTabuPar[1])
		   {
			 case 1:
			   format = "Fehler(%d) Parameterfkt(%d)(%d) SgNr:%d SG-Zustand bei Wechsel von Festzeit in VA-Betrieb stimmt mit Einschaltphase nicht ueberein  (%d) (%d)"; // jetzt VA_MELDUNG(88) beim sX
			   break;
		   }
		   break;
		}
    }
    break;
    case VA_MELDUNG:
      format = "TA message (%d). No.: %d. Parameters of TL[5] = %d, %d, %d, %d, %d";
      if(strcmp(comp, "pdmx")==0)
      {
		  switch (nr)
			{
			  case 3:
			  /*~+:VA_MELDUNG (85) 3 1 x 0 0 0  -> OEVRingpufferAnlegen  -> Ringpuffer konnte nicht angelegt werden*/
			  /*~+:VA_MELDUNG (85) 3 2 1 0 0 0  -> OEVSchreibeOevEintrag -> size of structure OEV_SP_SATZ and bytes to copy don't fit together*/
			  /*~+:VA_MELDUNG (85) 3 2 y x 0 0  -> OEVSchreibeOevEintrag -> pt record could not be written to pt memory (x = record index in pt memory)*/
			  /*~+:VA_MELDUNG (85) 3 3 y x 0 0  -> OEVLeseOevEintrag     -> pt record could not be read from pt memory  (x = record index in pt memory)*/
				switch (pTabuPar[1])
				{
				case 1:
					format = "VA-Meldung(%d) OEVRingpufferAnlegen(%d)(%d) Fehler im OeV-Speicher: Ringpuffer konnte nicht angelegt werden";
					break;
				case 2:
					if(pTabuPar[3] == 0)
						format = "VA-Meldung(%d) OEVSchreibeOevEintrag(%d)(%d) size of structure OEV_SP_SATZ and bytes to copy do not fit together";
					else
						format = "VA-Meldung(%d) OEVSchreibeOevEintrag(%d)(%d) Fehler im OeV-Speicher: Ein Datensatz konnte nicht in den Ringpuffer "
								"eingetragen werden (indexMSB: %d, indexLSB: %d)";
					break;
				case 3:
					format = "VA-Meldung(%d) OEVLeseOevEintrag(%d)(%d) Fehler im OeV-Speicher: Ein Datensatz konnte nicht aus dem Ringpuffer "
							"ausgelesen werden (indexMSB: %d, indexLSB: %d)";
					break;
				}


			  break;
			 case 170:
			   switch(pTabuPar[1])
			   {
				 case 1:
					format = "Fehler(%d) Parameterfkt(%d)(%d) SgNr:%d SG-Zustand bei Wechsel von Festzeit in VA-Betrieb stimmt mit Einschaltphase nicht ueberein  (%d) (%d)"; // jetzt VA_MELDUNG(88) beim sX - oder doch nicht?
				   break;
			   }
			   break;

			case VA_MESSAGE_switchingEvents_not_realized_as_expected:
			  format = VA_MESSAGE_switchingEvents_not_realized_as_expected_format;//"message %d. No.: %d TX=%d: switchingEvents not realized as expected !";
			  severity = SMS_WARNING;
			  break;
			case VA_MESSAGE_Tick_Slip_detected:
			  format = VA_MESSAGE_Tick_Slip_detected_format;//"message %d. No.: %d TX=%d: %d 100ms Ticks slipped !";
			  severity = SMS_WARNING;
			  break;
			}
		  break;

		case VA_PRIO_FEHLER:
		  format = "TA prio error (%d). No.: %d. Parameters of TL[5] = %d, %d, %d, %d, %d";
		  break;
		case TL_MELDUNG:
		  format = "TL message (%d). No.: %d. Parameters of TL[5] = %d, %d, %d, %d, %d";
		  break;
		case TL_PRIO_FEHLER:
		  format = "Priority TL error (%d). No.: %d. Parameters of TL[5] = %d, %d, %d, %d, %d";
		  break;
		default:
		  format = "unknown message %d. Error No.: %d. Parameters of TL[5] = %d, %d, %d, %d, %d";
	  //		  return SMS_UNKNOWN;
	  }
  }

  ret = UpdateCltMessageEnum(wErrorID, pTabuPar, anzPar, (const char **)&stringPar, (const char **)&enumPar, severity, format, status);
  return ret;
}

// reset all TA (detail) states
void resetStatus()
{
  if(! initialized) // avoid sending initial status before tacKernel is fully initiated (unfortunately PDMx calls the method too early)
  {
	  Report(MSG_WARNING, "ignore resetStatus during initializing of TacKernel");
	  return;
  }

  Report(MSG_STATE, "reset all states; msgCount:%d", msgCount);
  if(msgCount > 0)
  {
	 msgCount = 0;
    // sitraffic::updateStateMessages(TRUE);
  }
  invalidated = true;  // send OK message at next flush
}


