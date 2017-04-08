/*
 * reportDialog.cpp
 *
 *  Created on: 22.06.2016
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
#include <thread>
#include "cctrl/htmlHandler.h"
#include "cctrl/Config.h"
#include "cctrl/wrappedCctrlDtos.h"
#include "StateMessage.h"
#include "repdef.h"


#include "util/what.h"
DEFINE_WHATINFO;

#define Dprintf(...)
//#define Dprintf fflush(stdout);printf

namespace sitraffic
{
     class ScreenEntry
     {
       long    m_attr;
       char    m_time[80];
       int64_t m_tick;
       const char *m_func;
       char    m_text[400];

     public:
       ScreenEntry() : m_attr(0), m_tick(0), m_func(0)
         {
           strcpy(m_time, "....-..-.. ..:..:..,...\0");
           strcpy(m_text, "\0");
         }

       ScreenEntry(long attr,  const char * time, int64_t tick, const char * func, const char * text) : m_attr(attr), m_tick(tick), m_func(func)
         {
           snprintf(m_time, sizeof(m_time), "%s", time);
           snprintf(m_text, sizeof(m_text), "%s", text);
         }

       const char * getColor()
         {
           int color = R_EXTRACT_COL(m_attr);
           const char * colStr = ColorString(color);
           return colStr;
         }

       const char * getTime() {return m_time;}
       int64_t      getTick() {return m_tick;}

       const char * getFunc() {return m_func;}
       const char * getText() {return m_text;}
     };

   const int MAX_SCREEN = 200;
   static ScreenEntry sm_screen[MAX_SCREEN];
   static int sm_count = 0;


  void WriteToScreen(long attr, const char * time, int64_t ticks, const char * func, const char * text)
  {
     Dprintf("SCREEN: %s %s %s\n", time, func, text);
     int index = sm_count % MAX_SCREEN;
     sm_screen[index] = ScreenEntry(attr, time, ticks, func, text);
     sm_count++;
  }


  class Initializer
  {
     public:
      Initializer()
      {
        Dprintf("ctor reportDialog.Initializer\n");
        SetScreenPrint(WriteToScreen);
      };
  };
  static Initializer initializer;


  long AttributeFromString(std::string attrString)
  {
      long attr = 0;

      if(attrString.find("FILI")  != std::string::npos) attr |= R_FILI;
      if(attrString.find("FILE")  != std::string::npos) attr |= R_FILI;
      if(attrString.find("LINE")  != std::string::npos) attr |= R_FILI;
      if(attrString.find("FUNC")  != std::string::npos) attr |= R_FUNC;
      if(attrString.find("THREAD")!= std::string::npos) attr |= R_THREAD;
      if(attrString.find("PRIO")  != std::string::npos) attr |= R_PRIO;
      if(attrString.find("ERRNO") != std::string::npos) attr |= R_ERRNO;
      if(attrString.find("DATE")  != std::string::npos) attr |= R_DATE;
      if(attrString.find("TIME")  != std::string::npos) attr |= R_TIME;

      if(attrString.find("FILE1")   != std::string::npos) attr |= APP_FILE1;
      if(attrString.find("FILE2")   != std::string::npos) attr |= APP_FILE2;
      if(attrString.find("SCREEN")  != std::string::npos) attr |= APP_SCREEN;
      if(attrString.find("PRINTER") != std::string::npos) attr |= APP_PRINTER;
      if(attrString.find("HISTORY") != std::string::npos) attr |= APP_HISTORY;

      if(attrString[1] == ' ')
      {
        char cat = attrString[0];
        attr |= R_CAT(cat);
      }

     if(attrString.find("TRACE")  != std::string::npos)  attr |= 5;
     if(attrString.find("DEBUG")  != std::string::npos)  attr |= 4;
     if(attrString.find("INFO")   != std::string::npos)  attr |= 3;
     if(attrString.find("WARN")   != std::string::npos)  attr |= 2;
     if(attrString.find("EVENT")  != std::string::npos)  attr |= 2;
     if(attrString.find("ERROR")  != std::string::npos)  attr |= 1;
     if(attrString.find("STATE")  != std::string::npos)  attr |= 1;

      if(attrString.find("RED")    != std::string::npos)  attr |= R_RED;
      if(attrString.find("GREEN")  != std::string::npos)  attr |= R_GREEN;
      if(attrString.find("ORANGE") != std::string::npos)  attr |= R_ORANGE;
      if(attrString.find("BLUE")   != std::string::npos)  attr |= R_BLUE;
      if(attrString.find("GRAY")   != std::string::npos)  attr |= R_GRAY;
      if(attrString.find("MAGENTA")!= std::string::npos)  attr |= R_MAGENTA;
      if(attrString.find("CYAN")   != std::string::npos)  attr |= R_CYAN;

      return attr;
  }
  std::string showUsualReportPrios()
  {
    std::ostringstream ss;

    ss << "<BR><B>usual Report prios: </B>";
    ss << "0 = FATAL, 1 = ERROR or STATE, 2 = WARNING or EVENT, 3 = INFO, 4 = DEBUG, 5 = TRACE" << std::endl;

    return ss.str();
  }

std::string showAvailableReportCategories()
{
  std::ostringstream ss;

  ss << "<B>available Report Categories: </B> " << std::endl;

   for(char cat = 'A'; cat <= 'Z'; cat++)
   {
       const char * desc =  getReportGroupDescription(cat);
       if(desc != NULL && strcmp(desc, "reserved")!=0)
       {
         ss << cat << " = " << desc << ",  ";
       }

       char catLower = (char)(cat - 'A' + 'a');
       const char * descLower =  getReportGroupDescription(catLower);
       if(descLower != NULL && strcmp(descLower, "reserved")!=0)
       {
         ss << catLower << " = " << descLower << ", " ;
       }
   }
   ss << std::endl;
   return ss.str();
}

static std::string getHostAddress(EvHttpRequest & ehr)
{
  //std::string ipAdress;

  const char * host = ehr.getHost();
  if(host != NULL)
    return host;

  Report(DIALOG_INFO & ~APP_SCREEN, "EvHttpRequest.getHost() is NULL");

  sitraffic::DataReceiver & dr = sitraffic::InstRegistry::Instance().getDataReceiver();
  host = dr.getHost2subscribe();
  //const char * host = dr.getMyHostname();

  return host;

  /*
  try
  {
    ipAdress = InetAddress.getLocalHost().getHostAddress();
  }
  catch (UnknownHostException e)
  {
    ipAdress = "cbc";
  }
  */
 // return ipAdress;
}

// e.g.: http://ent0045g.dev.its/guiprovider/CommonBuild.html?userid=jump&role=developer&job=user&omctick=83524557#Szp:null
static std::string urlForCallingSZP(int64_t tick, EvHttpRequest & ehr)
{
//  log.info("Calling SZP at omcTick=" + omcTick);
    std::string locale="de";
    std::string device="notebook";
    std::string userid="Report";
    std::string role="developer"; //Role ..getCurrentRole().name();
    std::string job="user";
    std::string mode="&mode=debug";
    std::string host = getHostAddress(ehr); // + ":8080";
    std::stringstream  omcTick; omcTick << tick;

  return std::string("http://") + host + "/guiprovider/CommonBuild.html?userid="+userid+"&role="+role+"&job="+job+mode+"&omctick="+omcTick.str()+"#Szp:null";
}


std::string htmlLinkForCallingSZP(int64_t tick, EvHttpRequest & ehr)
{
  return std::string("<a href=\"" +urlForCallingSZP(tick, ehr) + "\" target=\"szp\"> " + TableHelper::formatBigValue(tick) + "</a>"); // target=\"_blank\" --> opens always in new tab
}

std::string withColor(const char * color, const char * s)
 {
   if(color == NULL)
     return s;
   return std::string("<font color=") + color + ">" + s + "</font>";
 }

const char * currentTime()
{
  static char timeBuf[20];
  timeval tv;
  gettimeofday(&tv, NULL);
  struct tm tmpTm;
  time_t now = tv.tv_sec;

#ifdef WIN32
 #define localtime_r(x, y) localtime_s((y), (x))
#endif
  localtime_r(&now, &tmpTm);

  strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", &tmpTm);

  return timeBuf;
}


std::string showInvokedScreenReports(EvHttpRequest & ehr)
{
  std::ostringstream ss;

  int64_t currentTick = getCurrentReportTicks();

  TableHelper t;
  char buf[100];
  snprintf(buf, sizeof(buf), "current Ticks: %s Time: %s Invoked SCREEN Reports: %d",
          TableHelper::formatBigValue(currentTick), currentTime(), sm_count);
  t.appendHeader(buf, "-----");
  t.startTitle();
  t.addTitleColumn("Nr").addTitleColumn("Ticks -->STP").addTitleColumn("Time Nr: [Thread] PRIO (File:Line)").addTitleColumn("Function: Text").endTitle();
  for(int i = 0; i < MAX_SCREEN; i++)
  {
    int idx = (sm_count - 1 - i + MAX_SCREEN) % MAX_SCREEN; // start with last entry, reverse order, incl. wrap around
    ScreenEntry& se = sm_screen[idx];
    if(se.getFunc() != 0)
    {
      t.startRow();
      std::string  text = std::string(se.getFunc()) + ": " + withColor(se.getColor(), se.getText());
      t.addItem(sm_count-i).addItem(htmlLinkForCallingSZP(se.getTick(), ehr)).addItem(se.getTime()).addItem(text);
      t.endRow();
    }
  }
  t.endTable();
  ss << t.toString();

 return ss.str();
}


std::string ReportHtmlRequestHandler::doRequest(EvHttpRequest & ehr)
{
    static int nr;
    std::ostringstream ss;
    ss << "<center>";


    auto & pmap(ehr.getParams());

    auto iParam = pmap.find("level");
    if(iParam != pmap.end())
    {
        const char * levelNew = iParam->second;
        const char * processedNewLevels = LevelReport(levelNew);       // invoke before title "ReportLevels: ..."
        Report(DIALOG_EVENT, "set new levels %s", processedNewLevels);
    }

    const char * currentLevels = LevelReport(NULL);
    const char * persistedLevels = getPersistedLevels();

    ss << "<B>ReportLevels: </B>" << currentLevels << "<BR>" << std::endl;

    if(iParam != pmap.end()) // again after title "ReportLevels: ..."
     {
        const char * levelNew = iParam->second;
         ss << "set new report levels: " << levelNew;
         ss << " ==> " << currentLevels << "<BR>" << std::endl;
     }

    ss << "<BR>" << std::endl;

     ss << "<form action=\"Report\" id=\"ReportLevel\" >" << std::endl;
     ss << " <label id=\"l1\" form=\"ReportLevel\"> new Level for (e.g. 'S3' to set INFO for category 'Signal groups' or '3' for all groups or '+3' for all except individual groups) </label>" << std::endl;
     ss  << " <input type=\"string\" id=\"level\" name=\"level\" maxlength=\"20\">" << std::endl;
     ss << " <button value=\"4\">set</button>" << std::endl;

     ss  << "  persist" << " <input type=\"checkbox\" id=\"level2\" name=\"pers\" value=\"valPer\" label=\"labPer\">" << std::endl;
     if(persistedLevels != NULL && strcmp(currentLevels, persistedLevels))
     {
        ss  << "  (persistent: " << persistedLevels << ")" << std::endl;
     }

    ss << "</form>";


     auto persParam = pmap.find("pers");
     if(persParam != pmap.end())
     {
         ss << "persist " << currentLevels << std::endl;
         persistLevelReport();
         ss << "<BR>" << std::endl;
         Report(DIALOG_EVENT, "persisted levels %s", currentLevels);
     }



    ss << showAvailableReportCategories() << std::endl;
    ss << showUsualReportPrios() << "<BR><BR>" << std::endl;


    if(Report(DIALOG_INFO, NULL) > 0)
    {
       ss << "<form action=\"Report\" id=\"SetReport\" >" << std::endl;
       ss << " <label id=\"set\" form=\"SetReport\"> invoke dummy Report with flags (e.g. SCREEN or HISTORY) </label>" << std::endl
          << " <input type=\"string\" id=\"flags\" name=\"flags\" maxlength=\"30\">" << std::endl;

       ss << " <label id=\"count\" form=\"SetReport\"> count</label>" << std::endl
            << " <input type=\"number\" id=\"count\" name=\"count\" maxlength=\"5\">" << std::endl;

       ss << " <button type=\"submit\">invoke</button>" << std::endl;
       ss << "</form>" << std::endl;

    auto flagsParam = pmap.find("flags");
    if(flagsParam != pmap.end())
    {
        std::string flags = std::string(flagsParam->second);
        ss << "invoked Report(" << flags << ", ";

        std::ostringstream format; format << "dummy test " << ++nr;
        const char * text = format.str().c_str();


        long attr = APP_SCREEN | APP_FILE1 | R_THREAD |  R_FILI | R_FUNC | R_DATI | 0;
        Dprintf("ReportHtmlRequestHandler:       attr = o%lo = %s\n", attr, AttributeToString(attr));
        long attrNew = AttributeFromString(flags);
        Dprintf("ReportHtmlRequestHandler:   new attr = o%lo = %s\n", attrNew, AttributeToString(attrNew));
        attr |= attrNew;
        Dprintf("ReportHtmlRequestHandler: merg. attr = o%lo = %s\n", attr, AttributeToString(attr));

        static char attrOctal[20];
        sprintf(attrOctal, "oct%lo", attr);

        ss <<  attrOctal << " = " << AttributeToString(attr) << ", " << text << "); ";

        auto countParam = pmap.find("count");
        if(countParam != pmap.end())
        {
            const char * countS = countParam->second;
            ss << countS <<  " times";
            int count = (int)strtol(countS, NULL, 0);
            if(count > 1)
            {
              Report(DIALOG_EVENT, "Start invoking %d Reports", count);
              for(int i = 0; i < count; i++)
              {
                Report(REPORT_LOCATION, attr, "%s", text);
              }
              Report(DIALOG_EVENT, "Invoked %d Reports", count);
            }
            else
            {
              int done = Report(REPORT_LOCATION, attr, "%s", text);
              ss << " with result = " << done;
            }
        }
        else
        {
          int done = Report(REPORT_LOCATION, attr, "%s", text);
          ss << " with result = " << done;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(200)); // wait until report is processed
    }
   }

   ss << showInvokedScreenReports(ehr);
   ss << "</center>" << std::endl;
   // puts(ss.str().c_str());

   return ss.str();
}



}
