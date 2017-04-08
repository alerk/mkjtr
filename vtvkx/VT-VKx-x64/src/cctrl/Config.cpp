/*
 * Config.cpp
 *
 *  Created on: 09.10.2015
 *      Author: zelgerpe
 */

#include <cctrl/Config.h>
#include <cctrl/htmlHandler.h>
#include <cctrl/wrappedCctrlDtos.h>
#include "Archive.h"
#include <instregistry.h>
#include <DataReceiver.h>
#include <mv/Detectors.h>
#include <rpc/clnt.h>
#include <rpc/types.h>
#include <rpc/xdr.h>
#include <util/repdef.h>
#include <util/report.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <utility>

DEFINE_WHATINFO;


using namespace sitraffic;


//---------------CfgIgtTable----------------------
void CfgIgtTable::fill(unsigned cnt, const CfgInterGreenTimeElementCctrl * p)
{
  igt.clear();
  // insert all elements into our igt
  for (unsigned i = 0; i < cnt; i++)
    {
      Key k(p->enteringSgNo, p->clearingSgNo);
      igt[k] = (short) p->value;
      Report(CFG_DEBUG, "ZZ(e%2d l%2d)=%d", p->enteringSgNo, p->clearingSgNo, p->value);
      p++;
    }
}

short CfgIgtTable::get(int enteringSgNo, int clearingSgNo) const
{
  Key k(enteringSgNo, clearingSgNo);
  std::map<Key, short, Key::KeyCmp>::const_iterator i(igt.find(k));
  if(i==igt.end())
    return NOIGT;
  return i->second;
}
//----------------/CfgIgtTable---------------------

//----------------Config::SgInfo---------------------
Config::SgInfo::CfgSgStateTransition::CfgSgStateTransition()
	: no(-1), elements()
{
}
Config::SgInfo::CfgSgStateTransition::CfgSgStateTransition(const CfgSgStateTransitionCCtrl & o)
	: no(-1), elements()
{
	fill(o);
}

Config::SgInfo::CfgSgStateTransition & Config::SgInfo::CfgSgStateTransition::operator =(const CfgSgStateTransitionCCtrl & o)
{
	fill(o);
	return *this;
}
void Config::SgInfo::CfgSgStateTransition::fill(const CfgSgStateTransitionCCtrl & o)
{
	no = o.no;
	elements.clear();
	for(unsigned i=0; i < o.elements.elements_len; i++)
		elements.push_back(o.elements.elements_val[i]);
}

// store cctrl DTO into csst
void Config::SgInfo::fill(std::vector<CfgSgStateTransition>& csst, unsigned n, const CfgSgStateTransitionCCtrl * csstCCtrl)
{
	csst.clear();
	for(unsigned i=0; i<n; i++)
		csst.push_back(Config::SgInfo::CfgSgStateTransition(csstCCtrl[i]));
}

void Config::SgInfo::fill(const CfgSgInfo2CCtrl & cfgsgInfo)
{
  sgNo = cfgsgInfo.sgNo;
  piNo = cfgsgInfo.partialIntersectionNo;
  name.assign(cfgsgInfo.name);
  detailType = cfgsgInfo.sgType;
  isMainDirection = cfgsgInfo.isMainDirection;

  greenStateColors.clear();
  for(unsigned i=0; i<cfgsgInfo.greenStateColors.greenStateColors_len; i++)
    greenStateColors.push_back(cfgsgInfo.greenStateColors.greenStateColors_val[i]);
  redStateColors.clear();
  for(unsigned i=0; i<cfgsgInfo.redStateColors.redStateColors_len; i++)
    redStateColors.push_back(cfgsgInfo.redStateColors.redStateColors_val[i]);

  fill(transGnRd, cfgsgInfo.transGnRd.transGnRd_len, cfgsgInfo.transGnRd.transGnRd_val);
  fill(transRdGn, cfgsgInfo.transRdGn.transRdGn_len, cfgsgInfo.transRdGn.transRdGn_val);
  fill(minGreen, cfgsgInfo.minGreen.minGreen_len, cfgsgInfo.minGreen.minGreen_val);
  fill(minRed, cfgsgInfo.minRed.minRed_len, cfgsgInfo.minRed.minRed_val);

  fillSiggruppe();
}

// allocate and fill pSiggruppe
void Config::SgInfo::fillSiggruppe()
{
	memset(&siggruppe, 0, sizeof (SIGGRUPPE));
	strncpy(siggruppe.aBezeichnung, name.c_str(), sizeof siggruppe.aBezeichnung);
	siggruppe.bTkrZuordnung = (UTINY)this->piNo;
	siggruppe.bTyp = detailType;
	siggruppe.bRichtung = this->isMainDirection?HAUPTRICHTUNG:NEBENRICHTUNG;
	UTINY i=0;
	for(int color : greenStateColors)
	{
		siggruppe.sVTZustand[i].color = (UTINY)color;
		siggruppe.sVTZustand[i].stateInfo = (UTINY)this->getLZ(color);
		i++;
	}
	for(int color : redStateColors)
	{
		siggruppe.sVTZustand[i].color = (UTINY)color;
		siggruppe.sVTZustand[i].stateInfo = (UTINY)this->getLZ(color);
		i++;
	}
	siggruppe.nColorInfo=i;
}

void Config::SgInfo::fillMd(std::vector<int> stateColors, const CfgSgStateTransition & minTime, bool hasTransition,
    MINDAUER & md) const
  {
    memset(&md, 0, sizeof md);

    int stdcolor = stateColors[0]; // standard red/green color
    const CfgSgStateTransitionElementCCtrl *pste = minTime.find(stdcolor);
    md.wDauerStandard = static_cast<SSHORT>(pste->duration_100ms); // lookup first entry at minTime list

    // DauerSpeziell = Mindestdauer der ersten �bergangsfarbe oder falls diese nicht vorhanden der ersten alternativ frei/sperr- Farbe
    if (stateColors.size() > 1)
      {
        stdcolor = stateColors[1];
        pste = minTime.find(stdcolor);
        if (pste)
          {
            md.wDauerSpeziell = static_cast<SSHORT>(pste->duration_100ms);
            md.bFarbeSpeziell = static_cast<UTINY>(pste->color);
          }
      }

    //zwingende Frei- bzw. Sperrfolge [DEF: NEIN][REF: CB0026] = 0 falls TransitioncolorList leer, sonst 1
    md.bZwangsfolge = hasTransition ? 1 : 0;
  }
// return true if there is a transition element with listNo
bool Config::SgInfo::hasTransitionSequence(unsigned listIndex) const
{
	if(listIndex<transGnRd.size() && !transGnRd[listIndex].empty())
		return true;
	if(listIndex<transRdGn.size() && !transRdGn[listIndex].empty())
		return true;

	return false;
}


bool Config::SgInfo::contains(const std::vector<int> & v, int what, unsigned startInd) const
{
	while(startInd < v.size())
	{
		if(v.at(startInd)==what)
			return true;
		else
			startInd++;
	}
	return false;
}
bool Config::SgInfo::contains(const std::vector<CfgSgStateTransition> & v, int color) const
{
	for(const CfgSgStateTransition & sst : v)
	{
		if(sst.contains(color))
			return true;
	}
	return false;
}

// lookup first element in elements that has color
// return 0 if none found
const CfgSgStateTransitionElementCCtrl * Config::SgInfo::CfgSgStateTransition::find(int color) const
{
	for(std::vector<CfgSgStateTransitionElementCCtrl>::const_iterator i = elements.begin();i!=elements.end(); ++i)
	{
		if( i->color == color)
			return &(*i);
	}
	return 0;
}

bool Config::SgInfo::CfgSgStateTransition::contains(int color) const
{
	return find(color)!=0;
}


// get color state, see LZ_ defines at api_va.h
int Config::SgInfo::getLZ(int color) const
{
	if(greenStateColors.at(0)==color)
		return LZ_STD_FREI;
	if(redStateColors.at(0)==color)
		return LZ_STD_SPERR;
	if(contains(greenStateColors, color, 1))
	{
		if(contains(transGnRd, color))
			return LZ_UEB_FS_VT_FREI;
		if(contains(transRdGn, color))
			return LZ_UEB_SF_VT_FREI;
		// here it is an alternative green color
		return LZ_FREI;
	}
	if(contains(redStateColors, color, 1))
	{
		if(contains(transGnRd, color))
			return LZ_UEB_FS_VT_SPERR;
		if(contains(transRdGn, color))
			return LZ_UEB_SF_VT_SPERR;
		return LZ_SPERR;
	}

	return LZ_UNZULAESSIG;
}

void Config::SgInfo::getSiggruppe(SIGGRUPPE *p) const
{
	*p = siggruppe;
}

// if no not found, return first entry
const Config::SgInfo::CfgSgStateTransition & Config::SgInfo::find(const std::vector<Config::SgInfo::CfgSgStateTransition> & trans, int no) const
  {
    for(const CfgSgStateTransition & t : trans)
      {
        if(t.getNo()==no)
          return t;
      }
    return *trans.begin();
  }

// calculate traffic state from ocitColor code
UTINY Config::SgInfo::getColorToState(int ocitColor) const
  {
    if(contains(transGnRd, ocitColor))
      return AL_AI_FS;
    else
    if(contains(transRdGn, ocitColor))
      return AL_AI_SF;
    else
    if(contains(greenStateColors, ocitColor, 0))
      return AL_AI_FREIGEBEN;
    else
    if(contains(redStateColors, ocitColor, 0))
      return AL_AI_SPERREN;
    else
      {
        Report(CFG_ERROR, "sg %d: got unexpected color %d setting bZustand=0xff", sgNo, ocitColor);
        return 0xff; //undefined traffic state
      }
  }

Config::SgInfo::~SgInfo()
{
	//delete pSiggruppe;
}
//----------------/Config::SgInfo---------------------


//-------------Config------------------------
CLIENT * Config::fill(CLIENT * clnt, bool isDraft)
{
  // read from ccontrol and store intergreentime tables
  //CLIENT *
  if(clnt==0)
    return 0;

  SelectedConfig sc;
  memset(&sc, 0, sizeof sc); // !! necessary to avoid heap corruption by xdr_array !!
  enum clnt_stat ret = getselectedcfg(isDraft, CS_CfgInterGreenTimeTables, &sc, clnt);
  if (ret != RPC_SUCCESS)
    {
      Report(CFG_ERROR, "%s", clnt_sperror(clnt, "getselectedcfg_8 CS_CfgInterGreenTimeTables call failed"));
      clnt_destroy(clnt);
      return 0;
    }
  else
    {
      // store intergreentime table

      for (unsigned i = 0;
          i < sc.SelectedConfig_u.igTables.igTables_len; i++)
        {
          const CfgInterGreenTimeTableCctrl * pigtCctrl =
              &sc.SelectedConfig_u.igTables.igTables_val[i];
          //TODO range check
          igts[pigtCctrl->igTableNo -1].fill(pigtCctrl->elements.elements_len, pigtCctrl->elements.elements_val);
          Report(CFG_INFO, "filled igtTable[%d]", pigtCctrl->igTableNo);
        }
    }
  // release memory allocated by xdr system
  clnt_freeres(clnt, (xdrproc_t) xdr_SelectedConfig, (caddr_t)&sc);

  memset(&sc, 0, sizeof sc); // !! necessary to avoid heap corruption by xdr_array !!
  ret = getselectedcfg(isDraft, CS_CfgSgInfo2CCtrl, &sc, clnt);
  if (ret != RPC_SUCCESS)
    {
      Report(CFG_ERROR, "%s", clnt_sperror(clnt, "getselectedcfg_8 CS_CfgSgInfo2CCtrl call failed"));
      clnt_destroy(clnt);
      return 0;
    }
  else
    {

      // store sgInfo table

      for (unsigned i = 0;
          i < sc.SelectedConfig_u.sgInfos2.sgInfos2_len; i++)
        {
          const CfgSgInfo2CCtrl * psgInfo =
              &sc.SelectedConfig_u.sgInfos2.sgInfos2_val[i];
          //TODO range check
          sgInfo[psgInfo->sgNo].fill(*psgInfo);
          Report(CFG_INFO, "filled sgInfo[%d]", psgInfo->sgNo);
        }
    }

  clnt_freeres(clnt, (xdrproc_t) xdr_SelectedConfig, (caddr_t)&sc);

  clnt=fillGeneralParam(clnt, isDraft);

  // fill Archive values
  Archive & archive=InstRegistry::Instance().getArchive();
  clnt = archive.fill(clnt);

  if(isDraft)
    {
      Detectors dets;
      clnt=dets.fill(clnt, isDraft);
      fillEckdaten(dets);
    }
  else
  fillEckdaten(InstRegistry::Instance().getDetectors());

  fillMindauern();

  clnt = fillSpls(clnt, isDraft);

  return clnt;
}

Config::Config()
  :  sgInfo(), eckdaten(), mdlGruen(), mdlRed(), spls(), generalParam()
{
InstRegistry::Instance().getEvHttpServer().add("/config",
    new ConfigHtmlRequestHandler(*this));
}

int Config::getMinSgNo() const
{
  return sgInfo.begin()->first;
}
int Config::getMaxSgNo() const
{
  return sgInfo.rbegin()->first;
}
//by Valentin
int Config::getSgSize() const
{
  return sgInfo.size();
}
const CfgIgtTable & Config::getIgt(int no) const
{
  return igts[(no-1)%MAX_ZZ_MATRIZEN];
}
const Config::SgInfo * Config::getSgInfo(int sgNo) const
{
	std::map<int, SgInfo>::const_iterator i=sgInfo.find(sgNo);
	if(i!=sgInfo.end())
		return &(i->second);
	return 0;
}
SLONG Config::getSiggruppe( UTINY bIndex, void * p) const
{
	const Config::SgInfo *psgInfo=getSgInfo(bIndex);
	if(!psgInfo)
		return AI_VSG_DATEN_LESEN_ERG2;
	psgInfo->getSiggruppe((SIGGRUPPE*)p);
	return AI_VSG_DATEN_LESEN_ERG0;
}

void Config::fillEckdaten(const Detectors &dets)
{
	memset(&eckdaten, 0, sizeof eckdaten);
	unsigned piMask=0;
	for(std::map<int, Config::SgInfo>::iterator sgi=sgInfo.begin(); sgi!=sgInfo.end(); sgi++)
	{
		const Config::SgInfo & e=sgi->second;
		eckdaten.bAnzSG++;
		if(e.sgNo > eckdaten.bMaxSgNo)
		    eckdaten.bMaxSgNo=(UTINY)(e.sgNo);
		piMask |= (1 << (e.piNo));
		if(e.piNo > eckdaten.bMaxTkNo)
		  eckdaten.bMaxTkNo = (UTINY)e.piNo;
	}
	while(piMask)
		{
		if(piMask & 1)
		  eckdaten.bAnzTK++;
		piMask >>= 1;
		}
	eckdaten.bInputCnt = (UTINY)dets.getDets().size();
	for(const std::map<int, Detector>::value_type & pair:dets.getDets())
	  {
	    if(pair.first > eckdaten.bMaxInputNo)
	      eckdaten.bMaxInputNo = (UTINY)pair.first;
	  }
	//TODO provide bOutputCnt
}
SLONG Config::getEckdaten( UTINY bIndex, void * p) const
{
	if(bIndex > 1) // pdm passes 1 in case there is only one instance
		return AI_VSG_DATEN_LESEN_ERG2;

	 * static_cast<CONFIG_BUF *>(p)=eckdaten;
	return AI_VSG_DATEN_LESEN_ERG0;
}

static MINDAUERLISTE & getOrCreate(unsigned listNo, std::map<unsigned, MINDAUERLISTE> & mdlMap)
{
	std::map<unsigned, MINDAUERLISTE>::iterator i=mdlMap.find(listNo);
	if(i == mdlMap.end())
	{
	    mdlMap.insert(std::pair<unsigned, MINDAUERLISTE>(listNo, MINDAUERLISTE()));
		MINDAUERLISTE & ret(mdlMap[listNo]);
		memset(&ret, 0, sizeof ret );
		sprintf(ret.aName, "L%d", listNo);
		return ret;
	}
	return i->second;
}

void Config::fillMindauern()
  {
    mdlGruen.clear();
    mdlRed.clear();

    // loop over all configured signalgroups (SX)
    for (std::map<int, SgInfo>::const_iterator i = sgInfo.begin(); i != sgInfo.end(); ++i)
      {
        int sgNo = i->first;
        if (sgNo < MAX_SIGNALGRUPPEN)
          {
            for (unsigned listIndex = 0; listIndex < i->second.getMinGreenSize(); listIndex++)
              {
                unsigned listNo = i->second.getMinGreenListNo(listIndex) % 100; // list numbers are sgNo*100 + listNo
                MINDAUERLISTE & mdl(getOrCreate(listNo, mdlGruen));
                i->second.fillMinGreenDauer(mdl.sDauer[i->first-1], listIndex);
              }
            for (unsigned listIndex = 0; listIndex < i->second.getMinRedSize(); listIndex++)
              {
                unsigned listNo = i->second.getMinRedListNo(listIndex) % 100; // SX-list-numbers are sgNo*100 + listNo
                MINDAUERLISTE & mdl(getOrCreate(listNo, mdlRed));
                i->second.fillMinRedDauer(mdl.sDauer[i->first-1], listIndex);
              }
          }
      }
  }
SLONG Config::getMindauer(UTINY bObjID, UTINY bIndex, MINDAUERLISTE * p) const
  {
    const std::map<unsigned, MINDAUERLISTE> * mdlMap=0;
    switch(bObjID)
      {
      case VD_MINDESTFREIGABELISTE:
        mdlMap = &mdlGruen;
        break;
      case VD_MINDESTSPERRLISTE:
        mdlMap = &mdlRed;
        break;
      default:
        return AI_VSG_DATEN_LESEN_ERG1;
      }
    std::map<unsigned, MINDAUERLISTE>::const_iterator i = mdlMap->find(bIndex);
    if(i == mdlMap->end())
      return AI_VSG_DATEN_LESEN_ERG2;
    *p = i->second;
    return AI_VSG_DATEN_LESEN_ERG0;
  }

static unsigned findFirstDefaultColor(const UTINY * p, unsigned n)
  {
    for(unsigned i=0; i<n; i++)
      if(p[i]==MAX_UTINY)
        return i;
    return n-1;
  }

CLIENT * Config::fillSpls(CLIENT *clnt, bool isDraft)
  {
    if(!clnt)
      return clnt;
    spls.clear();

    SelectedConfig sc;
    memset(&sc, 0, sizeof sc); // !! necessary to avoid heap corruption by xdr_array !!
    enum clnt_stat ret = getselectedcfg(isDraft, CS_CfgSignalplanInfoCCtrl, &sc, clnt);
    if (ret != RPC_SUCCESS)
      {
        Report(CFG_ERROR, "%s", clnt_sperror(clnt, "getselectedcfg_8 CS_CfgSignalplanInfoCCtrl call failed"));
        clnt_destroy(clnt);
        return 0;
      }
    else
      {
        // store signal programs

        for (unsigned i = 0;
            i < sc.SelectedConfig_u.signalplanInfos.signalplanInfos_len; i++)
          {
            const CfgSignalplanInfoCCtrl * pspl =
                &sc.SelectedConfig_u.signalplanInfos.signalplanInfos_val[i];
            SIGNALPLAN spl;
            memset(&spl, 0, sizeof spl);
            spl.bSiplNr = static_cast<UTINY>(pspl->splNo);
            strncpy(spl.aSignalplanName, pspl->name, sizeof spl.aSignalplanName);
            spl.wEzp = pspl->activationPointOfTime_100ms;
            spl.wAzp = pspl->deactivationPointOfTime_100ms;
            spl.wGsp = pspl->gsp_100ms;
            spl.wTu = pspl->tu_100ms;
            spl.bNrMinFreigabeListe = static_cast<UTINY>(pspl->minGreenRefs.minGreenRefs_val[0] % 100);
            spl.bNrMinSperrListe = static_cast<UTINY>(pspl->minRedRefs.minRedRefs_val[0] % 100);
            spl.bZZ_Nr = static_cast<UTINY>(pspl->itRef);
            for(unsigned ii=0; ii<MAX_SIGNALGRUPPEN; ii++)
              {
                spl.sSiGruDefs[ii] = {{MAX_SSHORT,MAX_SSHORT,MAX_SSHORT,MAX_SSHORT},{MAX_UTINY,MAX_UTINY,MAX_UTINY,MAX_UTINY}};
              }
            // fill in SCHALTZEITEN
            for(unsigned ii=0; ii<pspl->switchingTimeList.switchingTimeList_len; ii++)
              {
                const CfgStageTransitionSwitchingTime & st(pspl->switchingTimeList.switchingTimeList_val[ii]);
                const unsigned iii=findFirstDefaultColor(spl.sSiGruDefs[st.sgNo-1].bFarbe, MAX_ZEITEN_PRO_UMLAUF);
                spl.sSiGruDefs[st.sgNo-1].bFarbe[iii] = static_cast<UTINY>(st.signalIndication);
                spl.sSiGruDefs[st.sgNo-1].wZeit[iii] = static_cast<SSHORT>(st.time_100ms);
              }

            spls.push_back(spl);
          }
        eckdaten.bAnzSipl= static_cast<UTINY>(sc.SelectedConfig_u.signalplanInfos.signalplanInfos_len);
      }
    // release memory allocated by xdr system
    clnt_freeres(clnt, (xdrproc_t) xdr_SelectedConfig, (caddr_t)&sc);

    return clnt;
  }

const SIGNALPLAN * Config::getSpl(UTINY bSiplNr) const
  {
    // lookup spl
     for(const SIGNALPLAN & spl:spls)
       {
         if(spl.bSiplNr == bSiplNr)
           {
             return &spl;
           }
       }
     return 0;
  }

SLONG Config::getSpl(UTINY bIndex, SIGNALPLAN *p) const
  {
    // lookup spl
    const SIGNALPLAN *pspl = getSpl(bIndex);
    if (p && pspl)
      {
        *p = *pspl;
        return AI_VSG_DATEN_LESEN_ERG0;
      }
    return AI_VSG_DATEN_LESEN_ERG1;
  }

SSHORT Config::getEzp(UTINY bSpNo) const
  {
    // lookup spl
    const SIGNALPLAN *pspl = getSpl(bSpNo);
    if (pspl)
      {
        return pspl->wEzp;
      }
    return -1;
  }

//by Valentin
SSHORT Config::getGsp(UTINY bSpNo) const
  {
    // lookup spl
    const SIGNALPLAN *pspl = getSpl(bSpNo);
    if (pspl)
      {
        return pspl->wGsp;
      }
    return -1;
  }

CLIENT * Config::fillGeneralParam(CLIENT * clnt, bool isDraft)
  {
    if(!clnt)
      return clnt;
    SelectedConfig sc;
    memset(&sc, 0 , sizeof(sc));
    enum clnt_stat ret = getselectedcfg(isDraft, CS_CfgGeneralParamCctrl, &sc, clnt);

    if (ret != RPC_SUCCESS)
      {
        Report(CFG_ERROR, "%s", clnt_sperror(clnt, "getselectedcfg_8 CS_CfgGeneralParamCctrl call failed"));
        clnt_destroy(clnt);
        return 0;
      }
    else
      {
        if(sc.selector == CS_CfgGeneralParamCctrl)
          generalParam.fill(sc.SelectedConfig_u.generalParams);
        else
          Report(CFG_ERROR, "getselectedcfg_8 CS_CfgGeneralParamCctrl unexpected selector: %d", sc.selector);
      }
    // release memory allocated by xdr system
    clnt_freeres(clnt, (xdrproc_t) xdr_SelectedConfig, (caddr_t)&sc);
    return clnt;
  }

void Config::MyCfgGeneralParamCctrl::fill(const CfgGeneralParamCctrl & o)
  {
    this->name = o.name;
    this->centerType = o.centerType;
    this->controllerType = o.controllerType;
    this->tccNo = o.tccNo;
    this->guiNo = o.guiNo;
    this->fnr = o.fnr;
    this->systemPulse_ms = o.systemPulse_ms;
  }

// read selected config from draft or inventory
enum clnt_stat Config::getselectedcfg(bool isDraft, enum CfgSelector selector, SelectedConfig *psc,  CLIENT *clnt)
  {
    if(isDraft)
      {
        const DataReceiver & dr(InstRegistry::Instance().getDataReceiver());
        return getdraftselectedcfg_8(const_cast<char *>(dr.getMyHostname()), dr.getSvcPort(), selector, psc, clnt);
      }
    else
     return getselectedcfg_8(selector, psc, clnt);
  }



UTINY Config::VA_SgZustandBerechnen(UTINY bSgInd, UTINY ocitColor) const
  {
    const SgInfo * psgInfo = getSgInfo(bSgInd+1);
    if(!psgInfo)
      return LZ_UNZULAESSIG;    // signal group with bSgInd not configured
    if(psgInfo->contains(psgInfo->greenStateColors, ocitColor, 0))
      return FREIGEBEN;
    if(psgInfo->contains(psgInfo->redStateColors, ocitColor, 0))
      return SPERREN;
    return LZ_UNZULAESSIG;      // no rd/green state configured for that signal group and ocitColor
  }

const SignalIndicationCctrl * Config::lookup(const SignalIndicationsCctrl & si, int sigNr)
  {
    for (unsigned i = 0; i < si.sgPatterns.sgPatterns_len; i++)
      {
        if (si.sgPatterns.sgPatterns_val[i].sgNo == sigNr)
          {
            return &si.sgPatterns.sgPatterns_val[i];
          }
      }
    return 0;
  }

static bool logNotSwitched(const ExternalSgSwitchingEvent & esa, const SignalIndicationWCctrl & si, int * para)
  {
    if(para)
      {
        para[0]=esa.signalGroupNumber; para[1]=esa.signalIndication; para[2]=si.getOcitColor();
      }
    Report(SIGNALGROUP_DEBUG, "notSwitched sg:%d expected:%d gotOcitColor:%d", esa.signalGroupNumber, esa.signalIndication, si.getOcitColor());
    return false;
  }
/**
 * check if current signal indication honors last sent switching events.
 * @param esa   last second sent signal indication switching events
 * @param si    this second realized signal indications
 * @param para  pointer at least 3 ints that are set to sgNo, expected signalIndication, get signalIndication in case of ret=false
 * @return     true if si matches with esa
 */
bool Config::isLastSiSwitched(const ExternalSgSwitchingEventArray & esa, const SignalIndicationsWCctrl & si, int * para) const
  {
    const unsigned onPiMask=InstRegistry::Instance().getActualStatus().getOnPiMask();
    // walk through all switching events
    for(unsigned i=0; i<esa.ExternalSgSwitchingEventArray_len; i++)
      {
        const ExternalSgSwitchingEvent & ese(esa.ExternalSgSwitchingEventArray_val[i]);
        // ignore switching events with duration==0
        if(ese.validDuration==0)
          continue;
        // ignore switching events that are expired
        if(ese.startTickTime + ese.validDuration <= si.baseTicktime)
          continue;
        // lookup si
        const SignalIndicationWCctrl *psi = si.lookup(ese.signalGroupNumber);
        if(!psi)
          {
            Report(CFG_WARNING, "isLastSiSwitched: SG %2d expected at signal indications, ignoring", ese.signalGroupNumber);
            continue;
          }
        const SgInfo * psgInfo = getSgInfo( ese.signalGroupNumber);
        if(!psgInfo)
          {
            Report(CFG_WARNING, "isLastSiSwitched: SG %2d config info not found, ignoring", ese.signalGroupNumber);
            continue;
          }
        if(!ActualStatusWCctrl::isPiOn(onPiMask, psgInfo->piNo))
          {
            Report(CFG_INFO, "isLastSiSwitched: SG %2d ignored due to not switched ON PI %d", ese.signalGroupNumber, psgInfo->piNo);
            continue;
          }
        const UTINY lastState=psgInfo->getColorToState(ese.signalIndication);
        const UTINY aktState=psgInfo->getColorToState(psi->getOcitColor());
        switch(lastState)
          {
          case AL_AI_SF:
          case AL_AI_FREIGEBEN:
            if(aktState == AL_AI_SPERREN || aktState == AL_AI_FS)
              return logNotSwitched(ese, *psi, para);
            break;
          case AL_AI_FS:
          case AL_AI_SPERREN:
            if(aktState == AL_AI_SF || aktState == AL_AI_FREIGEBEN)
              return logNotSwitched(ese, *psi, para);
            break;
          }
      }
    return true;
  }

/** @return probable next tx in milliseconds */
int64_t  Config::getNextProbableTxMs(int splNo, int64_t lastTxMs) const
  {
    int64_t ret=lastTxMs + getSystemPulseMs();
    // calc cycle time of spl
    const SIGNALPLAN * pspl=getSpl(static_cast<UTINY>(splNo));
    if(!pspl)
      return ret; // no valid spl
    const int cycleTimeMs= 100 * (int)pspl->wTu;
    ret = ret % cycleTimeMs;
    return ret;
  }

// see TFS 51787
SLONG Config::getStammdaten(void *vps) const
  {
    STAMMDATEN *ps = (STAMMDATEN *)vps;
    memset(ps, 0, sizeof(STAMMDATEN));
    ps->bTyp = (UTINY)generalParam.getControllerType();
    generalParam.getName().copy(&ps->aGeraetename[0], sizeof ps->aGeraetename);
    ps->wAmtsNr = static_cast<USHORT>(generalParam.getTccNo());
    ps->wKnotenNr = static_cast<USHORT>(generalParam.getGuiNo());
    ps->bSystemTakt = static_cast<UTINY>(generalParam.getSystemPulse_ms()/100);
    generalParam.getCenterType().copy((char *)ps->bZentralenTyp, sizeof ps->bZentralenTyp);

    return AI_VSG_DATEN_LESEN_ERG0;
  }

int   Config::getGeneralParam(TacCfgGeneralParam *pGeneralParam) const
  {
    memset(pGeneralParam, 0, sizeof(TacCfgGeneralParam));
    pGeneralParam->controllerType = (TacCfgIntersectionControllerType)generalParam.getControllerType();
    generalParam.getName().copy(&pGeneralParam->name[0], sizeof pGeneralParam->name);
    pGeneralParam->tccNo = generalParam.getTccNo();
    pGeneralParam->guiNo = generalParam.getGuiNo();
    pGeneralParam->fnr = generalParam.getFnr();
    pGeneralParam->systemPulse_ms = generalParam.getSystemPulse_ms();
    generalParam.getCenterType().copy(&pGeneralParam->centerType[0], sizeof pGeneralParam->centerType);
    return 0;
  }
