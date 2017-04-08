
#define _POSIX_C_SOURCE 2
#include <time.h>
#include "instregistry.h"
#include "cctrl/wrappedCctrlDtos.h"
#include "util/repdef.h"
#include "start/tac_time.h"

#include "tac_kernel.h"

#include "util/what.h"
DEFINE_WHATINFO;

/** fetch utc from last received signal indications
 * convert it into localtime
 * @param pStm  struct tm to be filled by this function
 * @param pUtc  if !=NULL utc is filled in
 * @return pStm or NULL if localtime returns error.
 */
static struct tm * getStmFromLastSi(struct tm * pStm, time_t *pUtc)
  {
    // fetch utc time
   const sitraffic::SignalIndicationsWCctrl & si=sitraffic::InstRegistry::Instance().getSi();
   const int64_t utcMillies=si.baseUtcMinusTicktime + si.baseTicktime;
   time_t utc= static_cast<time_t>((utcMillies+500)/1000);
   if(!localtime_r(&utc, pStm))
     return NULL;
   if(pUtc)
     *pUtc=utc;
   return pStm;
  }

SLONG tacGetDateTime(TacDateTime *tdt)
  {
    struct tm stm;
    if(getStmFromLastSi(&stm, NULL))
      {
        tdt->second=stm.tm_sec; tdt->minute=stm.tm_min; tdt->hour=stm.tm_hour;
        tdt->day=stm.tm_mday; tdt->month=stm.tm_mon; tdt->year=stm.tm_year; tdt->wday=stm.tm_wday;
        tdt->reserved[0]=stm.tm_yday; tdt->reserved[1]=stm.tm_isdst;
        return SUCCESS;
      }
      else return NO_SUCCESS;
  }

int64_t sitraffic::tacDateTime2OmcTicks(const TacDateTime *tdt)
  {
    const sitraffic::SignalIndicationsWCctrl & si = sitraffic::InstRegistry::Instance().getSi();
    struct tm stm;
    time_t epoch;
    int64_t ret;
    stm.tm_sec=tdt->second; stm.tm_min=tdt->minute; stm.tm_hour=tdt->hour;
    stm.tm_mday=tdt->day; stm.tm_mon=tdt->month; stm.tm_year=tdt->year; stm.tm_isdst=-1;
    epoch = mktime(&stm);
    if (epoch < 0)
      {
        ret = si.baseTicktime;
        Report(PT_ERROR, "mktime could not convert timeinfo to epoch");
      }
    else
      {
        ret = ((int64_t) epoch * 1000) - si.baseUtcMinusTicktime;
      }
    return ret;
  }

