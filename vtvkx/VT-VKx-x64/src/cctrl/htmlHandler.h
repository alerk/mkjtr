/*
 * htmlHandler.h
 *
 *  Created on: 26.11.2015
 *      Author: zelgerpe
 */

#ifndef CCTRL_HTMLHANDLER_H_
#define CCTRL_HTMLHANDLER_H_

#include <rpc/rpc.h>
#include <sstream>
#include "evhttpadapter.h"
#include "tac_kernel.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
  {
  class Config;


  class TableHelper
    {
      enum tstate { Initial, Header, Title, Items, finished} state;
      std::ostringstream ss;

    public:
      TableHelper() : state(Initial), ss() {}
      void appendHeader(const std::string & name, const std::string & id)
        { if(state==Initial) { ss << "<p><b>" << name << "</b></p><table id=\""<< id << "\" border=\"1\">" << std::endl; state=Header; } }
      void startTitle() { ss << "<tr>"; state = Title; }
      template <typename T> TableHelper & addTitleColumn(const T & tc) { ss << "<th>" << tc << "</th>"; return *this;}
      void endTitle() { ss << "</tr>" << std::endl;state = Items; }
      TableHelper & startRow() { ss << "<tr>"; return *this;}
      template <typename T> TableHelper & addItem(T item) { ss << "<td>" << item << "</td>"; return *this; }
      TableHelper & endRow() { ss << "</tr>" << std::endl; return *this; }
      void endTable() {  ss << "</table>" << std::endl; state=finished; }
      std::string toString() { return ss.str(); }
      static const char * formatBigValue(int64_t n);
      template <typename TTT>
      static std::string utinys2string(int anz, const TTT * put);
    };



  // provide html display of last received signal indication notification
    class SiRequestHandler : public EvRequestHandler
      {
        unsigned lastSgNr;
      public:
        SiRequestHandler() : lastSgNr(1) {}
        virtual std::string doRequest(EvHttpRequest & ehr);
      };

    class ConfigHtmlRequestHandler : public EvRequestHandler
      {
        Config & cfg;
      public:
        ConfigHtmlRequestHandler(Config & cfg_) : cfg(cfg_) { }
        virtual std::string doRequest(EvHttpRequest & /* ehr */);

        void interGreenTimeTables2html(std::ostringstream & ss);
        std::string Mindauer2html(const MINDAUERLISTE & mdl, const std::string & lname, unsigned index);
        std::string Siggruppe2html();
        std::string Spls2html();
        std::string Eckdaten2html();
        std::string Stammdaten2html();
      };

    class OperatingModeHtmlRequestHandler :public EvRequestHandler
    {
      public:
        OperatingModeHtmlRequestHandler() {}
        virtual std::string doRequest(EvHttpRequest & /* ehr */);
    };

    class StateMessageHtmlRequestHandler :public EvRequestHandler
      {
         // std::string utinys2string(int anz, const UTINY * put);
        public:
        StateMessageHtmlRequestHandler() {}
          virtual std::string doRequest(EvHttpRequest & /* ehr */);
      };

    class ReportHtmlRequestHandler :public EvRequestHandler
      {
        public:
        ReportHtmlRequestHandler() {}
          virtual std::string doRequest(EvHttpRequest & /* ehr */);
      };

    class WhatInfoHtmlRequestHandler :public EvRequestHandler
       {
         public:
        WhatInfoHtmlRequestHandler() {}
           virtual std::string doRequest(EvHttpRequest & /* ehr */);
       };
  }




#endif /* CCTRL_HTMLHANDLER_H_ */
