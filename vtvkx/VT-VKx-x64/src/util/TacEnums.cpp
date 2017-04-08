/**
 * @file      TacEnums.cpp
 * Provider human-readable strings of enums used in TacKernel
 *
 * To developers:
 * As C++11 does not support reflections, this very responsible job
 * has to be done manually with care and love
 */

#include <string>
#include <map>
#include <exception>

#include "util/TacEnums.h"

#include "util/what.h"
DEFINE_WHATINFO;

using namespace tackernel;
using namespace std;

#define TO_MAP_ELEMENT(str) {str, #str}

static const map<TaRunmode,string> TA_RUNMODE_STRINGS =
{
        TO_MAP_ELEMENT(TARM_OFF),
        TO_MAP_ELEMENT(TARM_REDUCED),
        TO_MAP_ELEMENT(TARM_IT),
        TO_MAP_ELEMENT(TARM_PT),
        TO_MAP_ELEMENT(TARM_FULLY),
};

static const map<TacOperator,string> TAC_OPERATOR_STRINGS =
{
        TO_MAP_ELEMENT(TAC_OP_TA),
        TO_MAP_ELEMENT(TAC_OP_GUI),
        TO_MAP_ELEMENT(TAC_OP_RPC),
        TO_MAP_ELEMENT(TAC_OP_CENTER),
        TO_MAP_ELEMENT(TAC_OP_JAUT),
        TO_MAP_ELEMENT(TAC_OP_BAZ),
        TO_MAP_ELEMENT(TAC_OP_MAX_OP),
};

static const map<TacMainState,string> TAC_MAIN_STATE_STRINGS =
{
        TO_MAP_ELEMENT(TAC_MS_OFF),
        TO_MAP_ELEMENT(TAC_MS_ERR_OFF),
        TO_MAP_ELEMENT(TAC_MS_LOCAL_SELECTED_SP),
        TO_MAP_ELEMENT(TAC_MS_CENTER_SELECTED_SP),
        TO_MAP_ELEMENT(TAC_MS_MANUAL_CONTROL_CYCLIC),
        TO_MAP_ELEMENT(TAC_MS_MANUAL_CONTROL_ACYCLIC),
        TO_MAP_ELEMENT(TAC_MS_MA_CO_AC_TRANSITION),
        TO_MAP_ELEMENT(TAC_MS_SP_CHANGE_GSP),
        TO_MAP_ELEMENT(TAC_MS_SYNC_GSP),
        TO_MAP_ELEMENT(TAC_MS_PI_ON_OFF_WHILE_FIXED_TIME),
        TO_MAP_ELEMENT(TAC_MS_PI_ON_OFF_WHILE_MC_ACYCLIC),
        TO_MAP_ELEMENT(TAC_MS_CHANGE_TO_FIXED_TIME_CONTROL),
        TO_MAP_ELEMENT(TAC_MS_CHANGE_TO_MANUAL_CONTROL_ACYCLIC),
        TO_MAP_ELEMENT(TAC_MS_SI_FIRE),
        TO_MAP_ELEMENT(TAC_MS_SP_CHANGE_STRETCH),
        TO_MAP_ELEMENT(TAC_MS_SYNC_STRETCH),
        TO_MAP_ELEMENT(TAC_MS_DELAY_TIME),
        TO_MAP_ELEMENT(TAC_MS_RECOVER_FROM_EMERGENCY_MODE),
        TO_MAP_ELEMENT(TAC_MS_CHANGE_TO_ALL_RED),
        TO_MAP_ELEMENT(TAC_MS_ALL_RED),
        TO_MAP_ELEMENT(TAC_MS_SP_CHANGE_INTERGREEN),
};

static const map<TacPIState,string> TAC_PI_STATE_STRINGS =
{
        TO_MAP_ELEMENT(TAC_PI_NOT_USED),
        TO_MAP_ELEMENT(TAC_PI_NOT_ACTIVE),
        TO_MAP_ELEMENT(TAC_PI_ERROR_OFF),
        TO_MAP_ELEMENT(TAC_PI_WAITING_FOR_START_ACTIVATION_ROUTINE),
        TO_MAP_ELEMENT(TAC_PI_ACTIVATION_ROUTINE),
        TO_MAP_ELEMENT(TAC_PI_ACTIVE),
        TO_MAP_ELEMENT(TAC_PI_WAITING_FOR_DEACTIVATION_TIME),
        TO_MAP_ELEMENT(TAC_PI_DEACTIVATION_ROUTINE),
};
static const map<TacPIRqState,string> TAC_PIRQ_STATE_STRINGS =
{
        TO_MAP_ELEMENT(TAC_PIRQ_NOT_USED),
        TO_MAP_ELEMENT(TAC_PIRQ_ON),
        TO_MAP_ELEMENT(TAC_PIRQ_OFF_DEFAULT),
        TO_MAP_ELEMENT(TAC_PIRQ_OFF_FLASH_SECONDARY_DIRECTION),
        TO_MAP_ELEMENT(TAC_PIRQ_OFF_DARK),
        TO_MAP_ELEMENT(TAC_PIRQ_OFF_FLASH_ALL),
        TO_MAP_ELEMENT(TAC_PIRQ_FAULT_SHUTDOWN),
        TO_MAP_ELEMENT(TAC_PIRQ_MATCH_ACTUAL_STATE),
};

static const map<TacSpecialIntervention,string> TAC_SPECIAL_INTERVENTIONS_STRINGS =
{
        TO_MAP_ELEMENT(TAC_SI_NOT_ACTIVE),
        TO_MAP_ELEMENT(TAC_SI_FIRE_1),
        TO_MAP_ELEMENT(TAC_SI_FIRE_2),
        TO_MAP_ELEMENT(TAC_SI_FIRE_3),
        TO_MAP_ELEMENT(TAC_SI_RAILWAY_FULLY),
        TO_MAP_ELEMENT(TAC_SI_RAILWAY_PART),
};

static const map<TacRequestedSync,string> TAC_REQUESTED_SYNC_STRINGS =
{
        TO_MAP_ELEMENT(TAC_RS_NO),
        TO_MAP_ELEMENT(TAC_RS_GSP),
        TO_MAP_ELEMENT(TAC_RS_STRETCH),
};

static const map<TacLockState, string> TAC_LOCK_STATE_STRINGS = {
        TO_MAP_ELEMENT(TAC_LS_DO_NOT_CARE),
        TO_MAP_ELEMENT(TAC_LS_LOCK),
        TO_MAP_ELEMENT(TAC_LS_UNLOCK)
};

static const map<TacRideEventSource, string> TAC_RIDE_EVENT_SOURCE_STRINGS = {
        TO_MAP_ELEMENT(TAC_RES_AFD),
        TO_MAP_ELEMENT(TAC_RES_Det),
        TO_MAP_ELEMENT(TAC_RES_ITCS),
        TO_MAP_ELEMENT(TAC_RES_RideManager),
        TO_MAP_ELEMENT(TAC_RES_SimGui),
        TO_MAP_ELEMENT(TAC_RES_Ta),
        TO_MAP_ELEMENT(TAC_RES_unknown)
};

static const map<TacAmliCheckOutType, string> TAC_AMLI_CHECKOUT_TYPE_STRINGS = {
        TO_MAP_ELEMENT(TAC_ACOR_CheckIn),
        TO_MAP_ELEMENT(TAC_ACOR_OK),
        TO_MAP_ELEMENT(TAC_ACOR_TooLate),
        TO_MAP_ELEMENT(TAC_ACOR_Off),
        TO_MAP_ELEMENT(TAC_ACOR_SwitchOnOk),
        TO_MAP_ELEMENT(TAC_ACOR_SwitchOnNA),
        TO_MAP_ELEMENT(TAC_ACOR_NotAffected),
        TO_MAP_ELEMENT(TAC_ACOR_StageExtended),
        TO_MAP_ELEMENT(TAC_ACOR_CheckOutByNextNA),
        TO_MAP_ELEMENT(TAC_ACOR_StageUnmodified),
        TO_MAP_ELEMENT(TAC_ACOR_CheckOutByNext),
        TO_MAP_ELEMENT(TAC_ACOR_unknown)
};

static const map<TacAmliEventType, string> TAC_AMLI_EVENT_TYPE_STRINGS = {
        TO_MAP_ELEMENT(TAC_AET_CheckIn),
        TO_MAP_ELEMENT(TAC_AET_CheckOutRaw),
        TO_MAP_ELEMENT(TAC_AET_CheckOutAmli),
        TO_MAP_ELEMENT(TAC_AET_EmergencyCheckIn),
        TO_MAP_ELEMENT(TAC_AET_ForcedCheckOut),
        TO_MAP_ELEMENT(TAC_AET_Blocked),
        TO_MAP_ELEMENT(TAC_AET_unknown)
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

const string TacEnums::getTaRunmode(TaRunmode tacEnum)
{
    return lookupEnum(TA_RUNMODE_STRINGS, tacEnum);
}

const string TacEnums::getTacOperator(TacOperator tacEnum)
{
    return lookupEnum(TAC_OPERATOR_STRINGS, tacEnum);
}

const string TacEnums::getTacMainState(TacMainState tacEnum)
{
    return lookupEnum(TAC_MAIN_STATE_STRINGS, tacEnum);
}

const string TacEnums::getTacPIState(TacPIState tacEnum)
{
    return lookupEnum(TAC_PI_STATE_STRINGS, tacEnum);
}
const string TacEnums::getTacPIRqState(TacPIRqState tacEnum)
{
    return lookupEnum(TAC_PIRQ_STATE_STRINGS, tacEnum);
}

const string TacEnums::getTacSpecialIntervention(TacSpecialIntervention tacEnum)
{
    return lookupEnum(TAC_SPECIAL_INTERVENTIONS_STRINGS, tacEnum);
}

const string TacEnums::getTacRequestedSync(TacRequestedSync tacEnum)
{
    return lookupEnum(TAC_REQUESTED_SYNC_STRINGS, tacEnum);
}

TacLockState TacEnums::getTacLockStateValue(string enumString) {
    for (auto &iEnum : TAC_LOCK_STATE_STRINGS) {
        if (iEnum.second.compare(enumString) == 0) {
            return iEnum.first;
        }
    }

    throw std::invalid_argument(enumString + " is not valid enum");
}

const map<TacLockState, string> TacEnums::getTacLockStateStrings() {
    return TAC_LOCK_STATE_STRINGS;
}

const string TacEnums::getTacRideEventSource(TacRideEventSource tacEnum)
{
    return lookupEnum(TAC_RIDE_EVENT_SOURCE_STRINGS, tacEnum);
}

const map<TacRideEventSource, string> TacEnums::getTacRideEventSourceStrings() {
    return TAC_RIDE_EVENT_SOURCE_STRINGS;
}

TacRideEventSource TacEnums::getTacRideEventSourceValue(string enumString) {
    for (auto &iEnum : TAC_RIDE_EVENT_SOURCE_STRINGS) {
        if (iEnum.second.compare(enumString) == 0) {
            return iEnum.first;
        }
    }

    throw std::invalid_argument(enumString + " is not valid enum");
}

const string TacEnums::getTacAmliEventType(TacAmliEventType tacEnum)
{
    return lookupEnum(TAC_AMLI_EVENT_TYPE_STRINGS, tacEnum);
}

const map<TacAmliEventType, string> TacEnums::getTacAmliEventTypeStrings() {
    return TAC_AMLI_EVENT_TYPE_STRINGS;
}

TacAmliEventType TacEnums::getTacAmliEventTypeValue(string enumString) {
    for (auto &iEnum : TAC_AMLI_EVENT_TYPE_STRINGS) {
        if (iEnum.second.compare(enumString) == 0) {
            return iEnum.first;
        }
    }

    throw std::invalid_argument(enumString + " is not valid enum");
}

const string TacEnums::getTacAmliCheckOutType(TacAmliCheckOutType tacEnum)
{
    return lookupEnum(TAC_AMLI_CHECKOUT_TYPE_STRINGS, tacEnum);
}

const map<TacAmliCheckOutType, string> TacEnums::getTacAmliCheckOutTypeStrings() {
    return TAC_AMLI_CHECKOUT_TYPE_STRINGS;
}

TacAmliCheckOutType TacEnums::getTacAmliCheckOutTypeValue(string enumString) {
    for (auto &iEnum : TAC_AMLI_CHECKOUT_TYPE_STRINGS) {
        if (iEnum.second.compare(enumString) == 0) {
            return iEnum.first;
        }
    }

    throw std::invalid_argument(enumString + " is not valid enum");
}
