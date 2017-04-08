/*
 * cltDeviceValues.cpp
 *
 *  Created on: 08.12.2015
 *      Author: zelgerpe
 */

#include <sstream>
#include "DataReceiver.h"
#include "util/repdef.h"
#include "cctrl/cltDeviceValues.h"
#include "instregistry.h"
#include "util/TestAndDiagnostics.h"
#include "cctrl/Config.h"

#include "util/what.h"
DEFINE_WHATINFO;


using namespace sitraffic;


//    static
const uint32_t CltDeviceValueProviders::Fnv::offset_basis = 2166136261;
//    static
const uint32_t CltDeviceValueProviders::Fnv::prime = (1<<24) + (1<<8) + 0x93;

CltDeviceValueProviders::Fnv::Fnv() : hash(offset_basis) {}

void CltDeviceValueProviders::Fnv::update(size_t n, const void *pdata)
  {
    const unsigned char * p= static_cast<const unsigned char *>(pdata);
    while(n)
      {
        hash = (hash ^ *p) * prime;
        ++p; --n;
      }
  }


// assume that pValueTypes points to static memory
// store fragment into oitd2dvt map
void CltDeviceValueProviders::add(unsigned nvalueTypes, const DEVICE_VALUE_TYPE_TAC *pValueTypes)
  {
    for (unsigned i = 0; i < nvalueTypes; i++)
      {
        const DEVICE_VALUE_TYPE_TAC & dvt(pValueTypes[i]);
        oitd2dvt[Oitd32(dvt.member, dvt.otype)] = dvt;
      }
  }

const std::string & CltDeviceValueProviders::getVersion()
  {
    if (version.empty())
      version = calcVersion();
    return version;
  }

std::string CltDeviceValueProviders::calcVersion() const
  {
    // calculate hash crc32
    Fnv fnv;
    for (auto it = oitd2dvt.cbegin(); it != oitd2dvt.cend(); ++it)
      {
        fnv.update(strlen(it->second.name), it->second.name);
        fnv.update(strlen(it->second.longName), it->second.longName);
        fnv.update(strlen(it->second.description), it->second.description);
        fnv.update(6 * sizeof(int), &it->second.member);
      }
    std::ostringstream ss;
    ss.flags(std::ios::hex | std::ios::right);
    ss.width(8);
    ss.fill('0');
    ss << fnv.getHash();
    return ss.str();
  }

void CltDeviceValueProviders::registerAtCctrl(const DataReceiver & dr)
  {
    setVersion("DevVar", getVersion().c_str());
    Report(DEVVAR_STATE, "name: %s, versions: %s", name.c_str(),
    		sitraffic::TestAndDiagnosticsHttpRequestHandler::getTacVersionsJSON().c_str());

    CLIENT * clnt = dr.getCCtrlClient();
    if (clnt)
      {
        RetCodeCctrl retCode;
        if (registerdevicevariableprovider_8(const_cast<char *>(dr.getMyHostname()), dr.getSvcPort(), dr.getSvcTcpPort(),
            const_cast<char *>(name.c_str()), const_cast<char *>(getVersion().c_str()), &retCode, clnt) != RPC_SUCCESS
            || retCode != RC_OK)
          {
            Report(DEVVAR_ERROR, "registerdevicevariableprovider_8(%s, %d, %d, %s, %s, %d) failed %s", const_cast<char *>(dr.getMyHostname()),
                dr.getSvcPort(), dr.getSvcTcpPort(), const_cast<char *>(name.c_str()), const_cast<char *>(getVersion().c_str()),
                retCode, clnt_sperror(clnt, ""));
          }
        clnt_destroy(clnt);
      }
  }

DeviceValueIndexType CltDeviceValueProviders::toCctrl(TaDeviceValueIndexType dttapi) const
  {
    switch(dttapi)
      {
      case TADVI_Fd:
        return DVI_Fd;
      /** a device value per detector. */
      case TADVI_Detector:
        return DVI_Detector;
      /** a device value per signal group. */
      case TADVI_SG: // = 3,
        return DVI_SG;
      /** a device value per signal program. */
      case TADVI_SignalProgram:// = 4,
        return DVI_SignalProgram;
      /** a device value per stage (Phase). */
      case TADVI_Stage:// = 5,
        return DVI_Stage;
      /** a device value per partial intersection (Teilknoten). */
      case TADVI_PartialIntersection:// = 6,
        return DVI_PartialIntersection;
      /** a device value per non signal group output. */
      case TADVI_Output:// = 7,
        return DVI_Output;
      /** public transport control point (Meldpunkt) */
      case TADVI_PublicTransportCP:// = 10,
        return DVI_PublicTransportCP;
      /** public transport group */
      case TADVI_PublicTransportGroup:// = 11,
        return DVI_PublicTransportGroup;
      /** public transport signal group */
      case TADVI_PublicTransportSgGroup:// = 12,
        return DVI_PublicTransportSgGroup;
      /** unknown other index type */
      case TADVI_other:// = 99
        break;
      }
    return DVI_other;
  }


bool CltDeviceValueProviders::getMetadata(DeviceValuesMetaDataCctrl *pm)
  {
    Report(DEVVAR_DEBUG, "getMetadata dvpName=%s dvpVersion=%s", name.c_str(), getVersion().c_str());
    // dynamically allocate mem, to be freed by xdr_free that uses mem_free that is free()
    pm->providerType = UID_TRAFFIC_ACTUACTED_CCONTROL;
    pm->name = strdup(name.c_str());
    pm->vers = strdup(getVersion().c_str());
    pm->dvt.dvt_len = static_cast<uint32_t>(oitd2dvt.size());
    pm->dvt.dvt_val = reinterpret_cast<DeviceValueTypeCctrl *>(malloc(pm->dvt.dvt_len * sizeof(pm->dvt.dvt_val[0])));
    DeviceValueTypeCctrl *pcctrl = pm->dvt.dvt_val;
    for (auto it = oitd2dvt.cbegin(); it != oitd2dvt.cend(); ++it)
      {
        const DEVICE_VALUE_TYPE_TAC & dvt(it->second);
        pcctrl->name = strdup(dvt.name);
        pcctrl->longName = strdup(dvt.longName);
        pcctrl->description = strdup(dvt.description);
        pcctrl->member = dvt.member;
        pcctrl->otype = dvt.otype;
        pcctrl->dvit = toCctrl(dvt.dvit);
        pcctrl->minIndex = dvt.minIndex;
        pcctrl->maxIndex = dvt.maxIndex;
        pcctrl->minValue = dvt.minValue;
        pcctrl->maxValue = dvt.maxValue;
        ++pcctrl;
      }
    return true;
  }

// store subscription info
void CltDeviceValueProviders::subscribe(OitdCctrlA & oitds)
  {
    for(unsigned i=0; i<oitds.OitdCctrlA_len; i++)
      {
        // lookup Meta info
        Oitd32 oitd(oitds.OitdCctrlA_val[i].member, oitds.OitdCctrlA_val[i].otype);
        auto it=this->oitd2dvt.find(oitd);
        if(it!=oitd2dvt.cend())
          {
            SubscriptionInfo si(it->second, oitds.OitdCctrlA_val[i].index);
            Oitd32Index oitdi(oitds.OitdCctrlA_val[i].member, oitds.OitdCctrlA_val[i].otype, oitds.OitdCctrlA_val[i].index);
            subscriptions[oitdi]=si;
            Report(DEVVAR_INFO, "subscribing %d:%d %d", oitds.OitdCctrlA_val[i].member, oitds.OitdCctrlA_val[i].otype, oitds.OitdCctrlA_val[i].index);
          }
        else
          Report(DEVVAR_WARNING, "ignoring subscription %d:%d %d",oitds.OitdCctrlA_val[i].member, oitds.OitdCctrlA_val[i].otype, oitds.OitdCctrlA_val[i].index);
      }
  }
void CltDeviceValueProviders::unsubscribe(OitdCctrlA & oitds)
  {
    for(unsigned i=0; i<oitds.OitdCctrlA_len; i++)
      {
        Oitd32Index oitdi(oitds.OitdCctrlA_val[i].member, oitds.OitdCctrlA_val[i].otype, oitds.OitdCctrlA_val[i].index);
        if(subscriptions.erase(oitdi))
          Report(DEVVAR_INFO, "unsubscribed %d:%d %d", oitds.OitdCctrlA_val[i].member, oitds.OitdCctrlA_val[i].otype, oitds.OitdCctrlA_val[i].index);
      }
  }


/*
 * collect the values and send them to cctrl service.
 * @param client        rpc handle where to send the deviceValues via setdevicevalues_8, may be 0 in that case a new one is fetched from DataReceiver
 * @param forTick       tick to stamp changed values, may be the one the signalisation is calculated for.
 * @return clnt         used to send values or 0 on failure
 */
CLIENT* CltDeviceValueProviders::sendValues(CLIENT *clnt,  int64_t forTick)
  {
    DeviceValuesCctrl dvc;
    dvc.timestamp = forTick;//InstRegistry::Instance().getSi().baseTicktime;
    dvc.dv.dv_len = 0;
    dvc.dv.dv_val = (DeviceValueCctrl*)malloc(subscriptions.size() * sizeof(dvc.dv.dv_val[0]));
    for(auto it=subscriptions.begin(); it!=subscriptions.end(); it++)
      {
        if(it->second.metaInfo)
          {
            DEVICE_VALUE_TYPE_TAC & dvtt(*it->second.metaInfo);
            DeviceValueCctrl & dv(dvc.dv.dv_val[dvc.dv.dv_len]);
            (*dvtt.getValue)(it->first.get(), it->second.instanz, &dv.value);
            if(dv.value==it->second.oldValue)
              continue;
            it->second.oldValue=dv.value;
            dv.oitd.member=dvtt.member;
            dv.oitd.otype=dvtt.otype;
            dv.oitd.index=it->second.instanz;
            dv.tickTime = forTick+dvtt.offset_sysPulses*InstRegistry::Instance().getConfig().getSystemPulseMs(); //dvc.timestamp;
            dvc.dv.dv_len++;
            Report(DEVVAR_DEBUG, "sendValues %d:%d %d value=%d tick=%" PRId64, dv.oitd.member, dv.oitd.otype, dv.oitd.index, dv.value, (int64_t)dvc.timestamp);
          }
      }
    // send to fw
    if (dvc.dv.dv_len)
      {
        if (!clnt)
          clnt = InstRegistry::Instance().getDataReceiver().getCCtrlClient();

        if(clnt && setdevicevalues_8(dvc, 0, clnt)!=RPC_SUCCESS)
          {
            Report(DEVVAR_ERROR, "setdevicevalues_8() failed %s", clnt_sperror(clnt, ""));
            clnt_destroy(clnt);
            clnt=0;
          }
      }
    free(dvc.dv.dv_val);
    return clnt;
  }

// to be called from AWP_Init from each component
//extern
int registerDeviceValueProvider(unsigned nValueTypes, const DEVICE_VALUE_TYPE_TAC *pValueTypes)
  {
    InstRegistry::Instance().getCltDeviceValueProviders().add(nValueTypes, pValueTypes);
    return SUCCESS;
  }

