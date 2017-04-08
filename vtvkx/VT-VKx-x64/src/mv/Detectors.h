
#include <map>
#include "CControl.h"
#include "tac_kernel.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

// class that handles multiple Detectors
namespace sitraffic
{

#define TYPE_BUTTON_3 "BUTTON_3"

  static const double alpha1Default =  0.5;
  static const double alpha2Default =  0.25;

  class Detector
  {
    friend class Detectors;
    const int no;
    DET_MEASURED_DATA dmd;
    /**
     * AU       Aktueller Detektorzustand
     * Ext:
     *  0 = nicht belegt
     *  1 = belegt
     *  Int: ???
        0 = nicht belegt
        1 = belegt -> nicht belegt
        2 = nicht belegt
        -> belegt
        4 = Dauerbelegt
        255 = Fehler    100 ms
     *
     */
//    unsigned short wAU;
//    unsigned short wZS;

    int64_t lastRisingEdgeTick;
    double alpha1 = alpha1Default;
    double alpha2 = alpha2Default;
    int64_t lastBuCalcTick;
    int bu10ms;

    void setAU(bool_t occupied);
    void every100msAfterAU(int64_t ticks);
    void calcBuBa(int64_t tick, bool_t occupied);
    USHORT msDiffTo10ms(int64_t tick, int64_t tick2) const;
    USHORT addUSHORTs(USHORT u1, USHORT u2) const;
  public:
    Detector();
    Detector(int nr, double al1, double al2);
    Detector(const Detector & o);
    Detector& operator=(const sitraffic::Detector& o);
    void onNewValue100ms(const DetValuesPerChannel & dvpc);
    const DET_MEASURED_DATA & getDmd() const { return dmd; }
    unsigned short getAU() const { return dmd.wAU; }
    void setStatus(const DetStatusCctrl & stat);
    double getAlpha1() {return alpha1;}
    double getAlpha2() {return alpha2;}
  };

  class Detectors
  {
    std::map<int, Detector> nr2det;
    int64_t devTick;
  public:
    Detectors();
    CLIENT * fill(CLIENT *clnt, bool isDraft=false);
    // called by c-control data receiver service, every 100ms tick.
    void onNewValues100ms(const DetRawValuesExchg & drve);
    const std::map<int, Detector> & getDets() const { return nr2det; }
    const Detector * getDet(int nr) { std::map<int, Detector>::const_iterator it = nr2det.find(nr); if(it==nr2det.end()) return NULL; else return &(it->second); }
    int64_t getTick() const { return devTick; }

    //all the rest by Valentin
    unsigned int getPushButtonsNumber()const{return pushButtonsNumber;};
    unsigned int getVehDetectorsNumber()const{return vehDetectorsNumber;};

  private:
    CLIENT * fillDataFromDetectorCctrl(CLIENT *clnt);

    unsigned int pushButtonsNumber;
    unsigned int vehDetectorsNumber;
  };
}
