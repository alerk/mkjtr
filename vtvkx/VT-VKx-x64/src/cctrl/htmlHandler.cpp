/*
 * htmlHandler.cpp
 *
 *  Created on: 26.11.2015
 *      Author: zelgerpe
 */

#include "cctrl/htmlHandler.h"
#include "instregistry.h"
#include "CControl.h"
#include "cctrl/wrappedCctrlDtos.h"
#include "Config.h"

 #define __STDC_FORMAT_MACROS
 #include <inttypes.h>
 #include <stddef.h>

#include "util/what.h"
DEFINE_WHATINFO;


using namespace sitraffic;


// provide html display of last received signal indication notification
//  virtual
std::string SiRequestHandler::doRequest(EvHttpRequest & ehr)
  {
    SignalIndicationsWCctrl & si = InstRegistry::Instance().getSi();
    std::ostringstream ss;
    ss << "<center>";
    ss << "signalIndications utc:" << si.baseUtcMinusTicktime + si.baseTicktime << " tick:" << TableHelper::formatBigValue(si.baseTicktime) << " tx:" << si.tx
        << std::endl;
    ss << "<table border=\"true\">" << "<tr><th>SGNo</th><th>OCIT color</th><th>r/g state</th><th>since</th></tr>" << std::endl;
    for (auto it=si.getSgPatterns().cbegin(); it != si.getSgPatterns().cend(); ++it)
      {
        const SignalIndicationWCctrl &sic = it->second;
        ss << "<tr><td>" << sic.getSgNo() << "</td><td>" << sic.getOcitColor() << "</td><td>" << sic.getStateTxt() << "</td><td>" << sic.getColorStartTime() << "</td></tr>"
            << std::endl;
      }
    ss << "</table>" << std::endl;

    auto & pmap(ehr.getParams());
    auto isgNr=pmap.find("sgNr");
    if(isgNr != pmap.end())
      {
        const char *vsgNr=isgNr->second;
        unsigned int sgNr=(unsigned)strtoul(vsgNr, NULL, 0);
        if(sgNr!=0)
          lastSgNr=sgNr;
      }

    ss << "<form action=\"signalIndications\" id=\"SigZustandLesenSig\" >";
    ss << " <label id=\"l1\" form=\"SigZustandLesenSig\">sgNr</label>" << std::endl
       << " <input type=\"number\" id=\"sgNr\" name=\"sgNr\" maxlength=\"3\" value=\""<< lastSgNr << "\">" << std::endl
       << " <button type=\"submit\">show</button>" << std::endl;
    ss << "</form>" << std::endl;

    if(lastSgNr>0)
      {
        // form

        // display SigZustandLesenSig
        SIGSZ_STRUCT sz;
        SLONG szls = SigZustandLesenSig((UTINY) lastSgNr, &sz);
        if (szls == SUCCESS)
          {
            TableHelper t;
            t.appendHeader("SIGSZ_STRUCT", "idsz");
            t.startTitle();
            t.addTitleColumn("element").addTitleColumn("value").endTitle();
            t.startRow().addItem("wTX").addItem(sz.wTX).endRow();
            t.startRow().addItem("bZustand").addItem((int) sz.bZustand).endRow();
            t.startRow().addItem("bFarbe").addItem((int) sz.bFarbe).endRow();
            t.startRow().addItem("wDauer").addItem(sz.wDauer).endRow();
            t.startRow().addItem("bMindestzeitErfuellt").addItem((int) sz.bMindestzeitErfuellt).endRow();
            t.endTable();
            ss << t.toString();
          }
        else
          ss << "<p> SigZustandLesenSig()=" << szls << "</p>";
      }

    ss << "</center>" << std::endl;
    return ss.str();
  }

//-----------------------------------------
std::string ConfigHtmlRequestHandler::doRequest(EvHttpRequest & /* ehr */)
  {
    std::ostringstream ss;
    ss << "<center>";
    ss << "<p>fnr=<b>" << cfg.getFnr() << "</b></p>" << std::endl;
    interGreenTimeTables2html(ss);

    ss << Siggruppe2html();

    MINDAUERLISTE mdl;
    for (UTINY ind = 1; cfg.getMindauer(VD_MINDESTFREIGABELISTE, ind, &mdl) == AI_SYS_DATEN_LESEN_ERG0; ++ind)
      ss << Mindauer2html(mdl, "MinGreen", ind);

    for (UTINY ind = 1; cfg.getMindauer(VD_MINDESTSPERRLISTE, ind, &mdl) == AI_SYS_DATEN_LESEN_ERG0; ++ind)
      ss << Mindauer2html(mdl, "MinRed", ind);

    ss << Spls2html();

    ss << Eckdaten2html();

    ss << Stammdaten2html();

    ss << "</center>" << std::endl;
    return ss.str();
  }

void ConfigHtmlRequestHandler::interGreenTimeTables2html(std::ostringstream & ss)
  {
    const int minSg = cfg.getMinSgNo();
    const int maxSg = cfg.getMaxSgNo();

    for (int zzNo = 1; zzNo <= MAX_ZZ_MATRIZEN; zzNo++)
      {
        TableHelper t;
        const CfgIgtTable & igt(cfg.getIgt(zzNo));
        if (igt.isEmpty())
          continue;
        std::ostringstream id;
        id << "interGreenTimeTable_" << zzNo;
        t.appendHeader(id.str(), id.str());
        t.startTitle();
        t.addTitleColumn("c\\e");
        for (int sg = minSg; sg <= maxSg; sg++)
          if(cfg.isSgConfigured(sg)) t.addTitleColumn(sg);

        t.endTitle();
        for (int lsg = minSg; lsg <= maxSg; lsg++)
          {
            if(!cfg.isSgConfigured(lsg))
              continue;
            t.startRow();
            t.addItem(lsg);
            for (int esg = minSg; esg <= maxSg; esg++)
              {
                if(!cfg.isSgConfigured(esg))
                  continue;
                const short val = igt.get(esg, lsg);
                if (val == CfgIgtTable::NOIGT)
                  t.addItem("&nbsp;");
                else
                  t.addItem((int) val);
              }
            t.endRow();
          }
        t.endTable();
        ss << t.toString();
      }
  }

std::string ConfigHtmlRequestHandler::Mindauer2html(const MINDAUERLISTE & mdl, const std::string & lname, unsigned index)
  {
    TableHelper t;
    std::ostringstream id;
    id << lname << "_" << index;
    t.appendHeader(lname + " " + mdl.aName, id.str());
    t.startTitle();
    t.addTitleColumn("sgNo").addTitleColumn("wDauerStandard").addTitleColumn("wDauerSpeziell").addItem("bFarbeSpeziell").addItem(
        "bZwangsfolge").endTitle();
    for (int sg = cfg.getMinSgNo(); sg <= cfg.getMaxSgNo(); ++sg)
      {
        if(!cfg.isSgConfigured(sg))
          continue;
        const MINDAUER & md(mdl.sDauer[sg-1]);
        t.startRow();
        t.addItem(sg).addItem(md.wDauerStandard).addItem(md.wDauerSpeziell).addItem((int) md.bFarbeSpeziell).addItem(
            (int) md.bZwangsfolge);
        t.endRow();
      }
    t.endTable();
    return t.toString();
  }
std::string ConfigHtmlRequestHandler::Siggruppe2html()
  {
    TableHelper t;
    t.appendHeader("SIGGRUPPEN", "SIGGRUPPEN");
    t.startTitle();
    t.addTitleColumn("sgNo").addTitleColumn("Bezeichnung").addTitleColumn("Tkr").addTitleColumn("Typ").addTitleColumn("HR").addTitleColumn(
        "VTZustand").endTitle();
    for (int sg = cfg.getMinSgNo(); sg <= cfg.getMaxSgNo(); ++sg)
      {
        SIGGRUPPE s;
        if(cfg.getSiggruppe(static_cast<UTINY>(sg), &s) != AI_VSG_DATEN_LESEN_ERG0)
          continue;
        std::ostringstream ss;
        ss << "<ul>";
        for (unsigned i = 0; i < s.nColorInfo; ++i)
          {
            SG_ColorInfo & ci(s.sVTZustand[i]);
            ss << "<li>" << (int) ci.color << "--" << (int) ci.stateInfo << "</li>";
          }
        t.startRow();
        t.addItem(sg).addItem(s.aBezeichnung).addItem((int) s.bTkrZuordnung).addItem((int) s.bTyp).addItem((int) s.bRichtung).addItem(
            ss.str());
        t.endRow();
      }
    t.endTable();
    return t.toString();
  }

static std::string schaltzeiten2html(const Config &cfg, const SIGRUDEFS * sgsz)
  {
    TableHelper t;
    t.appendHeader("switching events", "sgszid");
    t.startTitle();
    t.addTitleColumn("sg").addTitleColumn("Zeiten").addTitleColumn("Farben").endTitle();
    for(unsigned i=0; i<MAX_SIGNALGRUPPEN && i< (unsigned)cfg.getMaxSgNo(); i++)
      {
        if(cfg.isSgConfigured(i+1))
          {
          std::ostringstream s1,s2;
          t.startRow();
          t.addItem(i+1);
          for(unsigned ii=0; ii<MAX_ZEITEN_PRO_UMLAUF && sgsz[i].bFarbe[ii]!=MAX_UTINY; ii++)
            {
              s1 << sgsz[i].wZeit[ii] << ", ";
              s2 << static_cast<int>(sgsz[i].bFarbe[ii]) << ", ";
            }
          t.addItem(s1.str()).addItem(s2.str()).endRow();
          }
      }
    t.endTable();
    return t.toString();
  }

std::string ConfigHtmlRequestHandler::Spls2html()
  {
    SIGNALPLAN spl;
    TableHelper t;
    t.appendHeader("SIGNALPLAN", "SIGNALPLAN");
    t.startTitle();
    t.addTitleColumn("splNo").addTitleColumn("Name").addTitleColumn("Ezp").addTitleColumn("Azp").addTitleColumn("Gsp").addTitleColumn(
        "Tu").addTitleColumn("MinFreiListe").addTitleColumn("MinSperrListe").addTitleColumn("ZZ").addTitleColumn("SiGruDefs").endTitle();

    CONFIG_BUF eckdaten; cfg.getEckdaten(0, &eckdaten);
    for (UTINY ind = 1; ind < 0xff; ++ind)
      if(cfg.getSpl(ind, &spl) == AI_SYS_DATEN_LESEN_ERG0)
        {
        t.startRow();
        t.addItem((int) spl.bSiplNr).addItem(spl.aSignalplanName).addItem(spl.wEzp).addItem(spl.wAzp).addItem(spl.wGsp).addItem(
            spl.wTu);
        t.addItem((int) spl.bNrMinFreigabeListe).addItem((int) spl.bNrMinSperrListe).addItem((int) spl.bZZ_Nr);
        t.addItem(schaltzeiten2html(cfg, &spl.sSiGruDefs[0]));
        t.endRow();
        }
    t.endTable();
    return t.toString();
  }


#define TABLE_ROW(x)    t.startRow().addItem(#x).addItem(static_cast<unsigned>(eckdaten.x)).endRow()

std::string ConfigHtmlRequestHandler::Eckdaten2html()
  {
    TableHelper t;
    CONFIG_BUF eckdaten;
    cfg.getEckdaten(0, &eckdaten);
    t.appendHeader("ECKDATEN", "ECKDATEN");
    t.startTitle();
    t.addTitleColumn("element").addTitleColumn("value").endTitle();
    TABLE_ROW( bCfgValid);          // 0: Konfiguration wurde geaendert 255: Versorgungseingriff aktiv  -> nicht bei Lizenzierung
    TABLE_ROW( bAnzSG);             // count of signal group instances
    TABLE_ROW( bMaxSgNo);           // greatest signal group number
    TABLE_ROW( bAnzTK);             // count of hardware part intersections                             -> nicht bei Lizenzierung
    TABLE_ROW( bMaxTkNo);           // greatest of hardware part intersection number                    -> nicht bei Lizenzierung
    TABLE_ROW( bAnzSipl);           // count of signal plan instances                                   -> nicht bei Lizenzierung
    TABLE_ROW( bMaxSplNo);          // greatest signal plan number                                      -> nicht bei Lizenzierung
    TABLE_ROW( bInputCnt);          // count of detector instances (0-250)
    TABLE_ROW( bMaxInputNo);        // greatest detector number    (0-250)
    TABLE_ROW( bOutputCnt);         // count of output instances   (0-148)
    TABLE_ROW( bMaxOutputNo);       // greatest output number      (0-148)
    TABLE_ROW( bAnzPhasen);         // count of stage instances                                         -> nicht bei Lizenzierung
    TABLE_ROW( bMaxStageNo);        // greatest stage number                                            -> nicht bei Lizenzierung
    TABLE_ROW( bAnzPhasenUeb);      // count of stage transition instances                              -> nicht bei Lizenzierung
    TABLE_ROW( bMaxStageTrans);     // greatest stage transition number                                 -> nicht bei Lizenzierung
    TABLE_ROW( bAnzZZMatrizen);     // count of intergreen time matrices                                -> nicht bei Lizenzierung
    TABLE_ROW( bMaxZZMatrizenNo);   // greatest number of intergreen time matrices                      -> nicht bei Lizenzierung
    TABLE_ROW( bAnzVZBBMatrizen);   // count of offset time matrices begin begin                        -> nicht bei Lizenzierung
    TABLE_ROW( bMaxVZBBMatrizenNo); // greatest number of offset time matrices begin begin              -> nicht bei Lizenzierung
    TABLE_ROW( bAnzVZEEMatrizen);   // count of offset time matrices end end                            -> nicht bei Lizenzierung
    TABLE_ROW( bMaxVZEEMatrizenNo); // greatest number of offset time matrices end end                  -> nicht bei Lizenzierung
    TABLE_ROW( bAnzEFolgen);        // count of switch on sequences                                     -> nicht bei Lizenzierung
    TABLE_ROW( bMaxSwitchOnSequNo); // greatest switch on sequence number                               -> nicht bei Lizenzierung
    TABLE_ROW( bAnzAFolgen);        // count of switch off sequences                                    -> nicht bei Lizenzierung
    TABLE_ROW( bMaxSwitchOffSequNo);// greatest switch off sequence number                              -> nicht bei Lizenzierung
    TABLE_ROW( bAnzUeseqSF);        // count of transition sequences (stop to free) instances           -> nicht bei Lizenzierung
    TABLE_ROW( bMaxUeseqSFNo);      // greatest number of transition sequences (stop to free)           -> nicht bei Lizenzierung
    TABLE_ROW( bAnzUeseqFS);        // count of transition sequences (free to stop) instances           -> nicht bei Lizenzierung
    TABLE_ROW( bMaxUeseqFS);        // greatest number of transition sequences (free to stop)           -> nicht bei Lizenzierung
    TABLE_ROW( bAnzFarben);         // maximale Anzahl Farbkombinationen                                -> nicht bei Lizenzierung
    t.startRow().addItem("bSiplUmlaufzeit").addItem(t.utinys2string(MAX_SIGNALPLAENE, eckdaten.bSiplUmlaufzeit)).endRow();   // Umlaufzeiten pro Signalplan                     -> nicht bei Lizenzierung
    t.startRow().addItem("bFeuerwehrplaene").addItem(t.utinys2string(MAX_SONDERPLAENE, eckdaten.bFeuerwehrplaene)).endRow(); // Umlaufzeiten pro Sonderplan                     -> nicht bei Lizenzierung
    t.endTable();
    return t.toString();
  }

std::string ConfigHtmlRequestHandler::Stammdaten2html()
  {
    TableHelper t;
    STAMMDATEN stammdaten;
    cfg.getStammdaten(&stammdaten);
    t.appendHeader("Stammdaten", "Stammdaten");
    t.startTitle();
    t.addTitleColumn("element").addTitleColumn("value").endTitle();
    t.startRow().addItem("bTyp").addItem(static_cast<unsigned>(stammdaten.bTyp)).endRow();
    t.startRow().addItem("aGeraetename").addItem(std::string(stammdaten.aGeraetename)).endRow();
    t.startRow().addItem("wAmtsNr").addItem(stammdaten.wAmtsNr).endRow();
    t.startRow().addItem("wKnotenNr").addItem(stammdaten.wKnotenNr).endRow();
    t.startRow().addItem("bSystemTakt").addItem(static_cast<unsigned>(stammdaten.bSystemTakt)).endRow();
    t.startRow().addItem("bZentralenTyp").addItem(stammdaten.bZentralenTyp).endRow();
    t.endTable();
    return t.toString();
  }

//--------------------------------------------------------------------
// invoke BetriebszustandLesen and display result
#undef TABLE_ROW
#define TABLE_ROW(x)    t.startRow().addItem(#x).addItem(static_cast<unsigned>(bz.x)).endRow()
//virtual
std::string OperatingModeHtmlRequestHandler::doRequest(EvHttpRequest & /* ehr */)
  {
    std::string ret;
//    {
//    TableHelper t;
//    BZSTRUCT bz;
//    BetriebszustandLesen(&bz);
//    t.appendHeader("Betriebszustand", "BZSTRUCT");
//    t.startTitle();
//    t.addTitleColumn("element").addTitleColumn("value").endTitle();
//    //t.startRow().addItem("wTX").addItem(bz.wTX).endRow();
//    TABLE_ROW(wTX);
//    TABLE_ROW(bSteuermodus);
//    TABLE_ROW(bBedienmodus);
//    TABLE_ROW(bAwpState);
//    t.startRow().addItem("bTkrZustand").addItem(t.utinys2string(MAX_TKR, bz.bTkrZustand)).endRow();
//    TABLE_ROW(bSiplNrAktuell);
//    TABLE_ROW(bSiplNrGefordert);
//    TABLE_ROW(bZplNr);
//    TABLE_ROW(bSondereingriff);
//    TABLE_ROW(bHerkunftSteuermodus);
//    t.startRow().addItem("bHerkunftTkrState").addItem(t.utinys2string(MAX_TKR, bz.bHerkunftTkrState)).endRow();
//    TABLE_ROW(bHerkunftSiplNrAktuell);
//    TABLE_ROW(bAderbruch);
//    TABLE_ROW(bGsp);
//    TABLE_ROW(bSy);
//    TABLE_ROW(bSyncWunsch);
//    TABLE_ROW(bMainState);
//    TABLE_ROW(bPhasenNr);
//    TABLE_ROW(bPhaseGefordert);
//    TABLE_ROW(bSumiStarted);
//
//    t.endTable();
//    ret = t.toString();
//    }
    {
    TableHelper t;
    TacOperatingState bz;
    tacGetOperatingState(&bz);
    t.appendHeader("Betriebszustand", "TacOperatingState");
    t.startTitle();
    t.addTitleColumn("element").addTitleColumn("value").endTitle();
    //t.startRow().addItem("wTX").addItem(bz.wTX).endRow();
    TABLE_ROW(wTX);
    TABLE_ROW(eOperator);
    TABLE_ROW(eMainState);
    t.startRow().addItem("ePIState").addItem(t.utinys2string(MAX_TKR, bz.ePIState)).endRow();
    TABLE_ROW(wSpNrActual);
    TABLE_ROW(wSpNrRequested);
    TABLE_ROW(eSpecialIntervention);
    TABLE_ROW(wGsp);
    TABLE_ROW(eRequestedSync);
    TABLE_ROW(wStageNr);
    TABLE_ROW(wStageNrRequested);
    t.startRow().addItem("ePIStateRequested").addItem(t.utinys2string(MAX_TKR, bz.ePIStateRequested)).endRow();

    t.endTable();
    ret += "\n" + t.toString();
    }

    return ret;
  }



template <typename TTT>
std::string TableHelper::utinys2string(int anz, const TTT * put)
  {
    std::ostringstream ss;
    while(anz)
      {
        ss << ((int)*put) << ", ";
        ++put; --anz;
      }
    return ss.str();
  }

const char * TableHelper::formatBigValue(int64_t n)
     {
         static char buf[30];
         static char buf2[40];
         sprintf(buf, "%" PRId64, n);
         unsigned int len = (unsigned int)strlen(buf);
         unsigned int j = 0;
         unsigned int i = 0;
         for(; i < len % 3; i++)
         {
            buf2[j++] = buf[i];
         }

         unsigned int first = i;
         for( ; i < len; i++)
         {
           if(i%3==first)
             buf2[j++] = ',';

            buf2[j++] = buf[i];
         }

         return buf2;
     }
