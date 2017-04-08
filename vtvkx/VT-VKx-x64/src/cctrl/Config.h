/*
 * Config.h
 * manages configuration access, caching and check
 *
 *  Created on: 09.10.2015
 *      Author: zelgerpe
 */

#ifndef CONFIG_H_
#define CONFIG_H_
#include <map>
#include <vector>
#include <string>
#include "CControl.h"
#include "tac_kernel.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

//#include <cstdint>
struct CfgInterGreenTimeElementCctrl;
struct CLIENT;
struct CfgSgInfo;

namespace sitraffic
{
  class Detectors;
  class SignalIndicationsWCctrl;
  // stores a single inter greentime table
  class CfgIgtTable
  {
    class Key
    {
      const int a,b;
    public:
      Key(int aa, int bb):a(aa), b(bb){}
      struct KeyCmp
      {
        bool operator () ( Key const & x, Key const & y) const
        {
          if(x.a < y.a) return true;
          if(x.a == y.a) return x.b < y.b;
          return false;
        }
      };
    };
    // Key(enteringSgNo, leavingSgNo)
    // value intergreentime value in 100ms units
    std::map<Key, short, Key::KeyCmp> igt;
  public:
    CfgIgtTable() : igt() {}
    static const short NOIGT=0x7fff;
    void fill(unsigned cnt, const CfgInterGreenTimeElementCctrl * p);
    short get(int enteringSgNo, int clearingSgNo) const;
    bool isEmpty() const { return igt.empty(); }
  };
  //------------------
  //const int MAX_ZZ_MATRIZEN=3;


  class Config
  {
    class SgInfo
    {
      class CfgSgStateTransition
	  {
    	  int no;
    	  std::vector<CfgSgStateTransitionElementCCtrl> elements;
    	  void fill(const CfgSgStateTransitionCCtrl & o);
	  public:
    	  CfgSgStateTransition();
    	  CfgSgStateTransition(const CfgSgStateTransitionCCtrl & o);
    	  CfgSgStateTransition & operator =(const CfgSgStateTransitionCCtrl & o);
    	  bool contains(int color) const;
    	  int getDuration100ms(unsigned ind=0) const { return elements[ind].duration_100ms; }
    	  const CfgSgStateTransitionElementCCtrl * find(int color) const;
    	  int getNo() const {return no;}
    	  bool empty() const { return elements.empty(); }
	  };

      int sgNo;
      int piNo;	// partial intersection number
      std::string name;
      SignalGroupDetailTypeCctrl detailType;
      bool isMainDirection;
      std::vector<int> greenStateColors;
      std::vector<int> redStateColors;
      std::vector<CfgSgStateTransition> transGnRd;
      std::vector<CfgSgStateTransition> transRdGn;
      std::vector<CfgSgStateTransition> minGreen;
      std::vector<CfgSgStateTransition> minRed;
      SIGGRUPPE  siggruppe;
      void fill(const CfgSgInfo2CCtrl & cfgsgInfo);
      void fillSiggruppe();
      void fill(std::vector<CfgSgStateTransition>& csst, unsigned n, const CfgSgStateTransitionCCtrl * csstCCtrl) ;

      friend class Config;
      bool contains(const std::vector<int> & v, int what, unsigned startInd) const;
      bool contains(const std::vector<CfgSgStateTransition> & v, int color) const;
      bool hasTransitionSequence(unsigned listIndex) const;
      void fillMd(std::vector<int> stateColors, const CfgSgStateTransition & minTime, bool trans, MINDAUER & md) const;
      const CfgSgStateTransition & find(const std::vector<CfgSgStateTransition> & trans, int no) const;

    public:
      SgInfo() : sgNo(0), piNo(-1), name("?"), detailType(SGDT_NO_TYPE), isMainDirection(false),
	  greenStateColors(), redStateColors(), transGnRd(), transRdGn(), minGreen(), minRed(), siggruppe() {}
      ~SgInfo();
      int getSgNo() const { return sgNo; }
      int getLZ(int color) const;
      void getSiggruppe(SIGGRUPPE *p) const;

      void fillMinGreenDauer(MINDAUER & md, unsigned listIndex) const { fillMd(greenStateColors, minGreen[listIndex], hasTransitionSequence(listIndex), md); }
      void fillMinRedDauer(MINDAUER & md, unsigned listIndex) const   { fillMd(redStateColors, minRed[listIndex], hasTransitionSequence(listIndex), md); }
      size_t getMinGreenSize() const { return minGreen.size();}
      size_t getMinRedSize() const   { return minRed.size();}
      unsigned getMinGreenListNo(unsigned index) const { return minGreen[index].getNo();}
      unsigned getMinRedListNo(unsigned index) const   { return minRed[index].getNo();}
      UTINY getColorToState(int color) const;
    };

    class MyCfgGeneralParamCctrl
      {
        std::string name, centerType;
        enum CfgIntersectionControllerTypeCctrl controllerType;
        int tccNo;
        int guiNo;
        int fnr;
        int systemPulse_ms;

        public:
        MyCfgGeneralParamCctrl() : name(""), centerType(""), controllerType(ICT_UNKNOWN), tccNo(0), guiNo(0), fnr(-1), systemPulse_ms(1000)
          { }
        void fill(const CfgGeneralParamCctrl & o);
        CfgIntersectionControllerTypeCctrl getControllerType() const { return controllerType; }
        const std::string & getName() const {return name;}
        const std::string & getCenterType() const {return centerType;}
        int getTccNo() const { return tccNo; }
        int getGuiNo() const { return guiNo; }
        int getSystemPulse_ms() const { return systemPulse_ms; }
        int getFnr() const { return fnr; }
        int getFnrOrGuiNo() const { return fnr >-1 ? fnr:guiNo; }
      };
    //std::array<CfgIgtTable, MAX_ZZ_MATRIZEN> igts;
    CfgIgtTable igts[MAX_ZZ_MATRIZEN];
    std::map<int, SgInfo> sgInfo;       // key signal group number
    const SgInfo * getSgInfo(int sgNo) const;
    CONFIG_BUF eckdaten;
    std::map<unsigned, MINDAUERLISTE> mdlGruen, mdlRed;
    void fillEckdaten(const Detectors &dets);
    void fillMindauern();
    std::vector<SIGNALPLAN> spls;
    CLIENT * fillSpls(CLIENT * clnt, bool isDraft);
    const SIGNALPLAN * getSpl(UTINY bSiplNr) const;

    MyCfgGeneralParamCctrl generalParam;
    CLIENT * fillGeneralParam(CLIENT * clnt, bool isDraft);

    static const SignalIndicationCctrl * lookup(const SignalIndicationsCctrl & si, int sigNr);


  public:
    Config();
    CLIENT * fill(CLIENT *clnt, bool isDraft=false);
    int getMinSgNo() const;
    int getMaxSgNo() const;
    int getSgSize() const;//by Valentin
    bool isSgConfigured(int sgNo) const { return getSgInfo(sgNo)!=0; }
    const CfgIgtTable & getIgt(int no) const;
    SLONG getSiggruppe( UTINY sgNo, void * p) const;
    SLONG getEckdaten( UTINY bIndex, void * p) const;
    SLONG getMindauer(UTINY bObjID, UTINY bIndex, MINDAUERLISTE * p) const;
    SLONG getSpl(UTINY bIndex, SIGNALPLAN *p) const;
    SLONG getStammdaten(void *) const;
    int   getGeneralParam(TacCfgGeneralParam *pGeneralParam) const;
    SSHORT getEzp(UTINY bSpNo) const;
    SSHORT getGsp(UTINY bSpNo) const;//by Valentin
    UTINY VA_SgZustandBerechnen(UTINY bSgInd, UTINY ocitColor) const;
    SLONG getSigZustand(UTINY bSigNr, SIGSZ_STRUCT *pSZ, const SignalIndicationsWCctrl & si, int splNo, int diff2NextTakeEffect=0) const;
    SLONG getSigZustandPi(UTINY bTkrNr, TKRSZ_STRUCT *pSZ, const SignalIndicationsWCctrl & si, int splNo, int diff2NextTakeEffect=0) const;
    /** @return field device number or gui number in case no control center with fnr configured */
    int getFnr() const { return generalParam.getFnrOrGuiNo(); }
    bool isLastSiSwitched(const ExternalSgSwitchingEventArray & esa, const SignalIndicationsWCctrl & si, int *para) const;
    int getSystemPulseMs() const { return generalParam.getSystemPulse_ms(); }
    /** @return probable next tx in milliseconds */
    int64_t getNextProbableTxMs(int splNo, int64_t lastTxMs) const;
    int64_t roundDownToSystemPulse(int64_t tx) const { return tx - (tx % generalParam.getSystemPulse_ms()); }
    static enum clnt_stat getselectedcfg(bool isDraft, enum CfgSelector arg1, SelectedConfig *clnt_res,  CLIENT *clnt);
  };
}

#endif /* CONFIG_H_ */
