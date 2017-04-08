/*
 * Detectors.cpp
 *
 *  Created on: 29.09.2015
 *      Author: zelgerpe
 */
#include "Detectors.h"
#include "instregistry.h"
#include "cctrl/Config.h"
#include <stdexcept>
#include "evhttpadapter.h"
#include <sstream>
#include <stdarg.h>
#include <float.h>
#include "cctrl/htmlHandler.h"
#include "util/repdef.h"


#include "util/what.h"
DEFINE_WHATINFO;


// read all configured instances

using namespace sitraffic;

//const char * format(const char * format, ...) __attribute__((format(gnu_printf, 1, 2)));


const char * getCounterName(UTINY detCounter)
{
  static char buf[30];
  switch(detCounter)
  {
    case  DET_VALUE_SS: return "SS";
    case  DET_VALUE_LS: return "LS";
    case  DET_VALUE_LU: return "LU";
    case  DET_VALUE_ZS: return "ZS";
    default:
      sprintf(buf, "unknown Counter %d", detCounter);
      return buf;
  }
}

const char * getResultString(SLONG ret)
{
  static char buf[40];
  switch(ret)
  {
    case  ERROR_BEREICHSPARAMETER: return "ERROR_BEREICHSPARAMETER";
    case  ERROR_DETEKTORART: return "ERROR_DETEKTORART";
    case  SUCCESS: return "SUCCESS";
    default:
      sprintf(buf, "unknown return value %d", ret);
      return buf;
  }
}

inline double min(double a, double b) {return (a < b) ? a : b;}
inline double max(double a, double b) {return (a > b) ? a : b;}

class DetectorsHandler : public EvRequestHandler
{
public:
  std::string
  doRequest(EvHttpRequest & ehr)
  {
    Detectors & dets = InstRegistry::Instance().getDetectors();
    double alpha1_min = 99.9;
    double alpha2_min = 99.9;
    double alpha1_max = 0.0;
    double alpha2_max = 0.0;
    std::ostringstream ss;
    ss << "<center>";
    ss << "Detectors:"  << std::endl << " (... compiled at " << __DATE__ << " " << __TIME__<< " with " << __FILE__ << " from " << __TIMESTAMP__ << " )" << std::endl;
    ss << "<table border=\"true\">" << "<tr><th>DetNo</th> <th>AU</th> <th>ZS</th> <th>SS</th> <th>LS</th> <th>LU</th> <th>BU</th> <th>BA</th> <th>Quality</th> <th>Error</th> </tr>" << std::endl;
    for (std::map<int, Detector>::const_iterator it =  dets.getDets().begin(); it != dets.getDets().end(); it++)
    {
      Detector det = it->second;

      ss << "<tr><td>" <<  it->first
      << "</td><td>" << det.getDmd().wAU
      << "</td><td>" << det.getDmd().wZS
      << "</td><td>" << det.getDmd().wSS
      << "</td><td>" << det.getDmd().wLS
      << "</td><td>" << det.getDmd().wLU
      << "</td><td>" << det.getDmd().wBU
      << "</td><td>" << det.getDmd().wBA // = smoothed BU
      << "</td><td>" << det.getDmd().wQuality
      << "</td><td>" << det.getDmd().wError

      << "</td></tr>" << std::endl;

     alpha1_min = min(alpha1_min, det.getAlpha1());
     alpha2_min = min(alpha2_min, det.getAlpha2());
     alpha1_max = max(alpha1_max, det.getAlpha1());
     alpha2_max = max(alpha2_max, det.getAlpha2());
    }
    ss << "</table>" << std::endl;
    ss << "last change Tick: " << TableHelper::formatBigValue(dets.getTick()) << std::endl;
   // ss << "</center>" << std::endl;

// #pragma warning (disable : 1572 )
#define IDENTICAL_FLOAT(x, y)  ( (x) <= (y) && (x) >= (y) )

    if(IDENTICAL_FLOAT(alpha1_min, alpha1_max))
      ss << "<p>alpha1=" << alpha1_min << "";
    else
      ss << "<p>alpha1=" << alpha1_min << "-" << alpha1_max << "";

    if(IDENTICAL_FLOAT(alpha2_min, alpha2_max))
      ss << " alpha2=" << alpha2_min << "</p>";
    else
      ss << " alpha2=" << alpha2_min << "-" << alpha2_max << "</p>";


    ss << std::endl << std::endl;

    // form
    ss << "<p>Counter: ZS="<<DET_VALUE_ZS << ", SS="<<DET_VALUE_SS << ", LS="<<DET_VALUE_LS << ", LU="<<DET_VALUE_LU  << std::endl;
    ss << "<form action=\"Detectors\" id=\"ResetDet\" >";
    ss << " <label id=\"l1\" form=\"ResetDet\">DetNo</label>" << std::endl
       << " <input type=\"number\" id=\"detNo\" name=\"detNo\" maxlength=\"3\">" << std::endl;
    ss << " <label id=\"l2\" form=\"ResetDet\">Counter</label>" << std::endl
       << " <input type=\"number\" id=\"counter\" name=\"counter\" maxlength=\"2\">" << std::endl
       << " <button type=\"submit\">reset</button>" << std::endl;
    ss << "</form>" << std::endl;

    auto & pmap(ehr.getParams());
    auto idetNr=pmap.find("detNo");
    if(idetNr != pmap.end())
    {
      const char *vdetNr = idetNr->second;
      USHORT detNr= (USHORT)strtoul(vdetNr, NULL, 0);
      if(detNr != 0)
      {
        auto idetCounter=pmap.find("counter");
        if(idetCounter != pmap.end())
        {
          const char *vdetCounter = idetCounter->second;
          UTINY detCounter= (UTINY)strtoul(vdetCounter, NULL, 0);
          if(detCounter != 0)
          {
            SLONG ret = DetectorValueSet (detNr, detCounter, 0);
            ss <<  "reset Detector '" << (int)detNr << "' Counter='" << getCounterName(detCounter) << "' returned " << getResultString(ret);
          }
          else
          {
              // empty attribute: "...&counter="
            DetectorValueSet (detNr, DET_VALUE_SS, 0);
            DetectorValueSet (detNr, DET_VALUE_LS, 0);
            DetectorValueSet (detNr, DET_VALUE_LU, 0);
            SLONG ret = DetectorValueSet (detNr, DET_VALUE_ZS, 0);

            ss <<  "reset detector '" << detNr << "' returned " << getResultString(ret);
          }
         }
         else
         {
             // no attribute: "&counter=..."
           DetectorValueSet (detNr, DET_VALUE_SS, 0);
           DetectorValueSet (detNr, DET_VALUE_LS, 0);
           DetectorValueSet (detNr, DET_VALUE_LU, 0);
           SLONG ret = DetectorValueSet (detNr, DET_VALUE_ZS, 0);

           ss <<  "reset detector '" << detNr << "' returned " << getResultString(ret);
         }
      }
    }

    ss << "</center>" << std::endl;
    // ss << ehr.toString().c_str() <<  std::endl ;
    return ss.str();
  }
};


// (re) read config from ccontrol service
// clnt has to be setup !=0
CLIENT * Detectors::fill(CLIENT *clnt, bool isDraft)
{
  SelectedConfig sc;
  if(!clnt)
    return clnt;
  memset(&sc, 0, sizeof sc); // !! necessary to avoid heap corruption by xdr_array !!
  enum clnt_stat ret = Config::getselectedcfg(isDraft, CS_CfgDetectorParamCctrl, &sc, clnt);//getselectedcfg_8(CS_CfgSgDetParam, &sc, clnt);
  if (ret != RPC_SUCCESS || sc.selector != CS_CfgDetectorParamCctrl)
    {
      Report(DET_ERROR, "%s ret=%d selector=%d", clnt_sperror(clnt, "getselectedcfg_8 CS_CfgSgDetParam call failed"), ret, sc.selector);
      clnt_destroy(clnt);
      clnt=0;
    }
  else
    {
      std::map<int,bool> toRemove;
      for(std::map<int, Detector>::const_iterator it=nr2det.begin(); it!=nr2det.end(); it++)
        {
          toRemove[it->first]=true;
        }
      for (unsigned i = 0;
          i < sc.SelectedConfig_u.detectorParams.detectorParams_len; i++)
        {
          const CfgDetectorParamCctrl * pDet =
              &sc.SelectedConfig_u.detectorParams.detectorParams_val[i];
          Report(DET_INFO,"D%2d %s %s %f %f", pDet->base.cfgInfoBaseDet.no, pDet->base.cfgInfoBaseDet.shortname, pDet->base.cfgInfoBaseDet.name,
              pDet->SmothingFactorUp, pDet->SmothingFactorDown);
          if(nr2det.find(pDet->base.cfgInfoBaseDet.no)==nr2det.end())
            nr2det[pDet->base.cfgInfoBaseDet.no] = Detector(pDet->base.cfgInfoBaseDet.no, pDet->SmothingFactorUp, pDet->SmothingFactorDown);
          toRemove[pDet->base.cfgInfoBaseDet.no]=false;
        }
      // remove not now filled instances
      for(std::map<int, bool>::const_iterator it=toRemove.begin(); it!=toRemove.end(); it++)
        if(it->second)
          nr2det.erase(nr2det.find(it->first));

      clnt_freeres(clnt, (xdrproc_t)xdr_SelectedCfgInfo, (caddr_t)&sc);
    }

  return fillDataFromDetectorCctrl(clnt);
}

//by Valentin
CLIENT * Detectors::fillDataFromDetectorCctrl(CLIENT *clnt)
{
	  SelectedCfgInfo sc;
	  memset(&sc, 0, sizeof sc); // !! necessary to avoid heap corruption by xdr_array !!
	  enum clnt_stat ret = getselectedcfginfo_8(CS_CfgInfoDetCctrl, &sc, clnt);//getselectedcfg_8(CS_CfgSgDetParam, &sc, clnt);
	  if (ret != RPC_SUCCESS)
	    {
	      Report(DET_ERROR, "%s", clnt_sperror(clnt, "getselectedcfg_8 CS_CfgInfoDetCctrl call failed"));
	      clnt_destroy(clnt);
	      clnt=0;
	    }
	  else
	    {
		  pushButtonsNumber = 0;
		  vehDetectorsNumber = 0;
	      for (unsigned i = 0;
	          i < sc.SelectedCfgInfo_u.cfgDets.cfgDets_len; i++)
	        {
	          const CfgInfoDetectorCctrl * pDetCtrl =
	              &sc.SelectedCfgInfo_u.cfgDets.cfgDets_val[i];

	          //	          Report(DET_INFO,"D%2d %s %s", pDet->no, pDet->shortname, pDet->name);

	          if(strncmp(pDetCtrl->modelType, TYPE_BUTTON_3, sizeof(TYPE_BUTTON_3))==0)
	          {
	        	  pushButtonsNumber++;
	          }
	          else
	          {
	        	  vehDetectorsNumber++;
	          }
	        }

	      clnt_freeres(clnt, (xdrproc_t)xdr_SelectedCfgInfo, (caddr_t)&sc);
	    }
	  return clnt;
}

Detectors::Detectors() :  nr2det(), devTick(0), pushButtonsNumber(0), vehDetectorsNumber(0)
{
  const std::string  DetectorsPath("/Detectors");
  // members are filled with fill(CLIENT*)

  // register html request handler only once

  if(InstRegistry::Instance().getEvHttpServer().getRhs().find(DetectorsPath)==InstRegistry::Instance().getEvHttpServer().getRhs().end())
    InstRegistry::Instance().getEvHttpServer().add(DetectorsPath, new DetectorsHandler());
}



Detector::Detector() : no(-1), dmd(), lastRisingEdgeTick(0), alpha1(alpha1Default), alpha2(alpha2Default), lastBuCalcTick(0), bu10ms(0) { memset(&dmd,0,sizeof dmd); }
Detector::Detector(int nr, double al1, double al2): no(nr), dmd(), lastRisingEdgeTick(0), alpha1(al1), alpha2(al2), lastBuCalcTick(0), bu10ms(0) { memset(&dmd,0,sizeof dmd); }
Detector::Detector(const Detector & o) : no(o.no), dmd(o.dmd), lastRisingEdgeTick(o.lastRisingEdgeTick), alpha1(o.alpha1), alpha2(o.alpha2), lastBuCalcTick(o.lastBuCalcTick), bu10ms(o.bu10ms) {  }
Detector& Detector::operator=(const sitraffic::Detector& o)
  { *(const_cast<int *>(&no))=o.no; dmd=o.dmd; lastRisingEdgeTick=o.lastRisingEdgeTick; alpha1=o.alpha1; alpha2=o.alpha2; lastBuCalcTick=o.lastBuCalcTick; bu10ms=o.bu10ms; return *this;}


// called by c-control data receiver service, every 100ms tick.
void Detectors::onNewValues100ms(const DetRawValuesExchg & drve)
{
  Report(DET_DEBUG,"onNewValues100ms tick %" PRId64 "%u detDatas, %lu detStatus", drve.absTimeDevTicks, drve.detDatas.detDatas_len, (unsigned long)drve.detStatus.detStatus_len);
  devTick = drve.absTimeDevTicks;

  for (unsigned i = 0; i < drve.detStatus.detStatus_len; i++)
    {
      const DetStatusCctrl & detStat(drve.detStatus.detStatus_val[i]);
      auto it = nr2det.find(detStat.channel);
      if(it==nr2det.end())
        Report(DET_WARNING,"ignoring detStatus channel=%d quality=%d error=%d", detStat.channel, (int)detStat.quality, (int)detStat.errorCode);
      else
        it->second.setStatus(detStat);
    }

  for (unsigned i = 0; i < drve.detDatas.detDatas_len; i++)
    {
      const DetValuesPerChannel & dvpc(drve.detDatas.detDatas_val[i]);
      try
        {
          Detector & det(nr2det.at(dvpc.nr));
          det.onNewValue100ms(dvpc);
        }
      catch (const std::out_of_range & e)
        {
          Report(DET_ERROR,"ignoring detdata for det%d, not configured", dvpc.nr);
        }
    }
  // invoke each 100ms, status are coming in between
  if(!drve.detStatus.detStatus_len)
    {
    for (std::map<int, Detector>::iterator it =nr2det.begin(); it != nr2det.end(); it++)
      it->second.every100msAfterAU(devTick);
    }


}

void Detector::setAU(bool_t occupied)
{
  if(dmd.wAU == occupied)
    return;     // no change

  if(!dmd.wAU && occupied)  // rising edge
    {
      if(dmd.wZS < 255)
        dmd.wZS++;
      // end of gap
      dmd.wLU = dmd.wLS;
      dmd.wLS = 0;
    }

  dmd.wAU = occupied ? 4 : 0;
}


void Detector::calcBuBa(int64_t tick, bool_t /*occupied*/)
 {
/*
   if(occupied)
   {
     USHORT diff = msDiffTo10ms(tick, lastRisingEdgeTick); // add beginning of current occupancy
     bu10ms = bu10ms + diff;
     lastRisingEdgeTick = tick; // take rest of current occupancy for next BU calculation
   }
*/

   double alpha = (dmd.wBU > dmd.wBA) ? alpha1 : alpha2;
   double baCalc = (double)dmd.wBA * (1.0 - alpha) + (double)bu10ms * alpha;

   dmd.wBA = (USHORT)baCalc;

   dmd.wBU = (USHORT)bu10ms; // TODO : 0 .. 100   12 Bit ganzzahlig   4 Bit Nachkomma

   // start again
   bu10ms = 0;

   lastBuCalcTick = tick;
 }

USHORT Detector::msDiffTo10ms(int64_t tick, int64_t tick2) const
{
  int64_t diff10ms = (tick - tick2) / 10;
  if(diff10ms < USHRT_MAX)
    return (USHORT)diff10ms;

  Report(DET_WARNING,"try to subtract: %" PRId64"ms - %" PRId64"ms = %" PRId64"10ms --> USHRT_MAX", tick, tick2, diff10ms);
  return USHRT_MAX ;
}

USHORT Detector::addUSHORTs(USHORT u1, USHORT u2) const
{
  ULONG sum = (ULONG)u1 + (ULONG)u2;
  if(sum < USHRT_MAX)
    return (USHORT)sum;

  Report(DET_WARNING,"try to add: %d + %d = %d --> USHRT_MAX", u1, u2, sum);
  return USHRT_MAX ;
}


void Detector::every100msAfterAU(int64_t ticks)
{
  if(dmd.wAU==0) // gap
     {
       if(dmd.wLS <600)        // Limit to max Value
         dmd.wLS ++;
       else
         dmd.wLU = dmd.wLS;
     }
   else // occupied
     {
       //Report(DET_DEBUG,"incrementing D%d SS=%d tick=%s", this->no, dmd.wSS, formatBigValue(ticks));
       if(dmd.wSS<65535)
         dmd.wSS++;
     }

  bool_t occupied = dmd.wAU > 0;

  if(occupied)
  {
    bu10ms += 10;
  }

  if(ticks - lastBuCalcTick >= 1000)
  {
    calcBuBa(ticks, occupied);
  }
}

void Detector::onNewValue100ms(const DetValuesPerChannel & dvpc)
{
  setAU(dvpc.value);

  // loop over all edges reported within this interval
  for(unsigned i=0; i< dvpc.detDatas.detDatas_len; i++)
  {
    const DetDataStruct & dds=dvpc.detDatas.detDatas_val[i];
    Report(DET_INFO,"D%02u %d %d", no, dds.value, dds.subTicks);
    setAU(dds.value);
  }

}

void Detector::setStatus(const DetStatusCctrl & stat)
  {
    dmd.wQuality = stat.quality;
    dmd.wError = (USHORT)stat.errorCode; //TODO convert
  }

