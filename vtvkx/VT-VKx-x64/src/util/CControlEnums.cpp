/**
 * @file      CControlEnums.cpp
 * Provider human-readable strings of enums used in C-Control
 *
 * To developers:
 * As C++11 does not support reflections, this very responsible job
 * has to be done manually with care and love
 */


#include <string>
#include <map>

#include "util/CControlEnums.h"

#include "util/what.h"
DEFINE_WHATINFO;

using namespace tackernel;
using namespace std;

#define TO_MAP_ELEMENT(str) {str, #str}

static const map<RideEventSourceCctrl,string> RIDE_EVENT_SOURCE_CCTRL_STRINGS =
{
        TO_MAP_ELEMENT(RESC_AFD),
        TO_MAP_ELEMENT(RESC_Det),
        TO_MAP_ELEMENT(RESC_ITCS),
        TO_MAP_ELEMENT(RESC_RideManager),
        TO_MAP_ELEMENT(RESC_SimGui),
        TO_MAP_ELEMENT(RESC_unknown),
};

static const map<AmliEventTypeCctrl,string> AMLI_EVENT_TYPE_CCTRL_STRINGS =
{
        TO_MAP_ELEMENT(AET_CheckIn),
        TO_MAP_ELEMENT(AET_CheckOutRaw),
        TO_MAP_ELEMENT(AET_CheckOutAmli),
        TO_MAP_ELEMENT(AET_EmergencyCheckIn),
        TO_MAP_ELEMENT(AET_ForcedCheckOut),
        TO_MAP_ELEMENT(AET_Blocked),
        TO_MAP_ELEMENT(AET_unknown),
};

static const map<AmliCheckOutTypeCctrl,string> AMLI_CHECK_OUT_TYPE_CCTRL_STRINGS =
{
        TO_MAP_ELEMENT(ACOR_CheckIn),
        TO_MAP_ELEMENT(ACOR_OK),
        TO_MAP_ELEMENT(ACOR_TooLate),
        TO_MAP_ELEMENT(ACOR_Off),
        TO_MAP_ELEMENT(ACOR_SwitchOnOk),
        TO_MAP_ELEMENT(ACOR_SwitchOnNA),
        TO_MAP_ELEMENT(ACOR_NotAffected),
        TO_MAP_ELEMENT(ACOR_StageExtended),
        TO_MAP_ELEMENT(ACOR_CheckOutByNextNA),
        TO_MAP_ELEMENT(ACOR_StageUnmodified),
        TO_MAP_ELEMENT(ACOR_CheckOutByNext),
        TO_MAP_ELEMENT(ACOR_unknown),
};

static const map<R09AttrSelCctrl,string> R09_ATTR_SEL_CCTRL_STRINGS =
{
        TO_MAP_ELEMENT(R09_Imbalance),
        TO_MAP_ELEMENT(R09_Priority),
        TO_MAP_ELEMENT(R09_ManDir),
        TO_MAP_ELEMENT(R09_VehicleLen),
        TO_MAP_ELEMENT(R09_VehicleNo),
        TO_MAP_ELEMENT(R09_CompanyNo),
        TO_MAP_ELEMENT(R09_PtDetNo),
        TO_MAP_ELEMENT(R09_SignalGroup),
        TO_MAP_ELEMENT(R09_Unknown)
};

template<typename T>
  static string lookupEnum(map<T, string> t2string, T enumToLookup)
    {
      try
        {
          return t2string.at(enumToLookup);
        }
      catch (std::out_of_range & oor)
        {
          return to_string(static_cast<int>(enumToLookup));
        }
    }

const string CControlEnums::getRideEventSourceCctrl(RideEventSourceCctrl ccEnum)
{
    return lookupEnum(RIDE_EVENT_SOURCE_CCTRL_STRINGS, ccEnum);
}

const string CControlEnums::getAmliEventTypeCctrl(AmliEventTypeCctrl ccEnum)
{
    return lookupEnum(AMLI_EVENT_TYPE_CCTRL_STRINGS, ccEnum);
}

const string CControlEnums::getAmliCheckOutTypeCctrl(AmliCheckOutTypeCctrl ccEnum)
{
    return lookupEnum(AMLI_CHECK_OUT_TYPE_CCTRL_STRINGS, ccEnum);
}

const string CControlEnums::getR09AttrSelCctrl(R09AttrSelCctrl attr)
{
    return lookupEnum(R09_ATTR_SEL_CCTRL_STRINGS, attr);
}

