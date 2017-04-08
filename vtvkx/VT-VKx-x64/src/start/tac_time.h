/*
 * tac_time.h
 *
 *  Created on: 16.11.2016
 *      Author: zelgerpe
 */

#ifndef START_TAC_TIME_H_
#define START_TAC_TIME_H_

#include <stdint.h>
#include "util/what.h"
DEFINE_WHATINFO_HEADER;


struct TacDateTime;
namespace sitraffic {
/** convert local date time into omc-ticks, based on last received signal indications
 * @param tdt address of result date time
 * @return SUCCESS, NO_SUCCESS
 */
int64_t tacDateTime2OmcTicks(const ::TacDateTime *tdt);
}

#endif /* START_TAC_TIME_H_ */
