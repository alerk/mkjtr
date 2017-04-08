/*
 * cltDeviceValues.h
 *
 *  Created on: 08.12.2015
 *      Author: zelgerpe
 */

#ifndef CCTRL_CLTDEVICEVALUES_H_
#define CCTRL_CLTDEVICEVALUES_H_
#include <string>
#include <map>
#include "tac_kernel.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;


//// neue api funktion, implementiert im tac kernel
//struct DeviceValueTypeTac {
//        int (*getValue)(int member, int otype, int instanz, int *pvalue);
//        const char *name;
//        const char *longName;
//        const char *description;
//        int member;
//        int otype;
//        enum DeviceValueIndexType dvit;
//        int minIndex;
//        int maxIndex;
//        int minValue;
//        int maxValue;
//};

namespace sitraffic {
class DataReceiver;

  class CltDeviceValueProviders
  {
    class Oitd32
      {
        protected:
          uint32_t oitd;
        public:
        Oitd32(int member, int otype) : oitd(member<<16|otype) {}
        bool operator < (const Oitd32 & o) const { return oitd < o.oitd; }
        uint32_t get() const { return oitd; }
      };
    class Oitd32Index : public Oitd32
      {
        int instanz;
      public:
        Oitd32Index(int member, int otype, int index):Oitd32(member,otype),instanz(index) {}
        bool operator < (const Oitd32Index & o) const { if(oitd < o.oitd) return true; if(oitd==o.oitd) return instanz<o.instanz; return false; }
      };
    class SubscriptionInfo
      {
      public:
        DEVICE_VALUE_TYPE_TAC * metaInfo;
        int instanz;
        int oldValue;
        SubscriptionInfo():metaInfo(0),instanz(0),oldValue(-1) {}
        SubscriptionInfo(DEVICE_VALUE_TYPE_TAC &mi, int inst):metaInfo(&mi), instanz(inst), oldValue(-1) {}
      };
    public:
    // utility providing hashcode calculation
    class Fnv
      {
        static const uint32_t offset_basis;
        static const uint32_t prime;
        uint32_t hash;
      public:
        Fnv();
        void update(size_t n, const void *pdata);
        uint32_t getHash() const { return hash; }
      };
    private:
    const std::string name;
    std::string version;
    typedef std::map<Oitd32, DEVICE_VALUE_TYPE_TAC>      oitd2dvt_t;
    oitd2dvt_t oitd2dvt;
    typedef std::map<Oitd32Index, SubscriptionInfo>        oitd2subscription_t;
    oitd2subscription_t subscriptions;

    std::string calcVersion() const;
    DeviceValueIndexType toCctrl(TaDeviceValueIndexType dttapi) const;

    public:
    CltDeviceValueProviders(const std::string & pname):name(pname),version(""),oitd2dvt(),subscriptions() {}

    // assume that pValueTypes points to static memory
    // store fragment into oitd2dvt map
    void add(unsigned nvalueTypes, const DEVICE_VALUE_TYPE_TAC *pValueTypes);
    const std::string & getName() { return name; }
    const std::string & getVersion() ;
    void registerAtCctrl(const DataReceiver & dr);
    bool getMetadata(DeviceValuesMetaDataCctrl *pm);
    void subscribe(OitdCctrlA & oitds);
    void unsubscribe(OitdCctrlA & oitds);
    CLIENT * sendValues(CLIENT *clnt, int64_t forTick) ;
  };

}
#endif /* CCTRL_CLTDEVICEVALUES_H_ */
