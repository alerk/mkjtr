/**
 * @file      CControlEnums.h
 * Interface of provider readable strings of enums used in C-Control
 *
 */

#ifndef UTIL_CCONTROLENUMS_H_
#define UTIL_CCONTROLENUMS_H_

#include "CControl.h"
#include "util/what.h"
#include <string>
DEFINE_WHATINFO_HEADER;

namespace tackernel
{
/**
 * Converter from enum value to string
 */
class CControlEnums
{
public:

    /**
     * Convert RideEventSourceCctrl to string
     *
     * @param[in] ccEnum RideEventSourceCctrl enum
     * @return Resulting string
     */
    static const std::string getRideEventSourceCctrl(RideEventSourceCctrl ccEnum);

    /**
     * Convert AmliEventTypeCctrl to string
     *
     * @param[in] ccEnum AmliEventTypeCctrl enum
     * @return Resulting string
     */
    static const std::string getAmliEventTypeCctrl(AmliEventTypeCctrl ccEnum);

    /**
     * Convert AmliCheckOutTypeCctrl to string
     *
     * @param[in] ccEnum AmliCheckOutTypeCctrl enum
     * @return Resulting string
     */
    static const std::string getAmliCheckOutTypeCctrl(AmliCheckOutTypeCctrl ccEnum);

    /**
     * Convert R09AttributeCctrl to string
     *
     * @param[in] attr R09AttributeCctrl enum
     * @return Resulting string
     */
    static const std::string getR09AttrSelCctrl(R09AttrSelCctrl attr);
};
}

#endif

