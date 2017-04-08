/**
 * @file      TacEnums.h
 * Interface of provider readable strings of enums used in TacKernel
 *
 */

#ifndef UTIL_TACENUMS_H_
#define UTIL_TACENUMS_H_

#include <string>

#include "tac_kernel.h"
#include <map>

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace tackernel
{
/**
 * Converter from enum value to string
 */
class TacEnums
{
public:
    /**
     * Convert TaRunmode to string
     *
     * @param[in] tacEnum TaRunmode enum
     * @return Resulting string
     */
    static const std::string getTaRunmode(TaRunmode tacEnum);
    /**
     * Convert TacOperator to string
     *
     * @param[in] tacEnum TacOperator enum
     * @return Resulting string
     */
    static const std::string getTacOperator(TacOperator tacEnum);
    /**
     * Convert TacMainState to string
     *
     * @param[in] tacEnum TacMainState enum
     * @return Resulting string
     */
    static const std::string getTacMainState(TacMainState tacEnum);
    /**
     * Convert TacPIState to string
     *
     * @param[in] tacEnum TacPIState enum
     * @return Resulting string
     */
    static const std::string getTacPIState(TacPIState tacEnum);
    /**
     * Convert TacPIState to string
     *
     * @param[in] tacEnum TacPIState enum
     * @return Resulting string
     */
    static const std::string getTacPIRqState(TacPIRqState tacEnum);
    /**
     * Convert TacSpecialIntervention to string
     *
     * @param[in] tacEnum TacSpecialIntervention enum
     * @return Resulting string
     */
    static const std::string getTacSpecialIntervention(TacSpecialIntervention tacEnum);
    /**
     * Convert TacRequestedSync to string
     *
     * @param[in] tacEnum TacRequestedSync enum
     * @return Resulting string
     */
    static const std::string getTacRequestedSync(TacRequestedSync tacEnum);

    /**
     * Convert string to TacLockState enum
     *
     * @param[in] string with name of lock state enum
     */
    static TacLockState getTacLockStateValue(std::string enumString);

    /* Returns map of TacLockState names
     */
    static const std::map<TacLockState, std::string> getTacLockStateStrings();

     /**
     * Convert string to TacRideEventSource enum
     *
     * @param[in] string with name of ride event source
     */
    static TacRideEventSource getTacRideEventSourceValue(std::string enumString);

    /**
     * Convert TacRideEventSource to string
     *
     * @param[in] tacEnum TacRideEventSource enum
     * @return Resulting string
     */
    static const std::string getTacRideEventSource(TacRideEventSource tacEnum);
    /* Returns map of TacRideEventSource names
     */
    static const std::map<TacRideEventSource, std::string> getTacRideEventSourceStrings();

     /**
     * Convert string to TacAmliEventType enum
     *
     * @param[in] string with name of amli event type
     */
    static TacAmliEventType getTacAmliEventTypeValue(std::string enumString);

    /**
     * Convert TacAmliEventType to string
     *
     * @param[in] tacEnum TacAmliEventType enum
     * @return Resulting string
     */
    static const std::string getTacAmliEventType(TacAmliEventType tacEnum);
    /* Returns map of TacAmliEventType names
     */
    static const std::map<TacAmliEventType, std::string> getTacAmliEventTypeStrings();

     /**
     * Convert string to TacAmliCheckOutType enum
     *
     * @param[in] string with name of amli checkout type
     */
    static TacAmliCheckOutType getTacAmliCheckOutTypeValue(std::string enumString);

    /**
     * Convert TacAmliCheckOutType to string
     *
     * @param[in] tacEnum TacAmliCheckOutType enum
     * @return Resulting string
     */
    static const std::string getTacAmliCheckOutType(TacAmliCheckOutType tacEnum);
    /* Returns map of TacAmliCheckOutType names
     */
    static const std::map<TacAmliCheckOutType, std::string> getTacAmliCheckOutTypeStrings();
};
}

#endif /* UTIL_TACENUMS_H_ */
