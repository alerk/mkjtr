/*
 * StateMessage.h
 *
 *  Created on: 17.12.2015
 *      Author: philippf
 */

#ifndef STATEMESSAGE_H_
#define STATEMESSAGE_H_

#include "util/what.h"
DEFINE_WHATINFO_HEADER;


namespace sitraffic
  {

    /**
     * please call after initializing tacKernel + PDM init in first awp function call (maybe PDM sent already some errors)
     */
    void initStateMessages();

    // tac internal VA_MESSAGE message numbers
    const SSHORT VA_MESSAGE_switchingEvents_not_realized_as_expected = 256; // Parameter 1 TX in 100ms units
    const char * const VA_MESSAGE_switchingEvents_not_realized_as_expected_format = "message %d. No.: %d TX=%d: switchingEvents not realized as expected, sg:%d expected:%d gotOcitColor:%d !";

    const SSHORT VA_MESSAGE_Tick_Slip_detected = 257; // Parameter 1 TX in 100ms units, Parameter 2 number of slipped 100ms ticks
    const char * const VA_MESSAGE_Tick_Slip_detected_format = "message %d. No.: %d TX=%d: %d 100ms Ticks slipped !";

    const SSHORT VA_MESSAGE_TaExecutionDurationWarn = 258;
    const char * const VA_MESSAGE_TaExecutionDurationWarn_format = "message %d. No.: %d TX=%d: TA execution duration=%d ms";


    const SSHORT IdReportAsDetailState = 299;

    /*
     * sends Message (format!=0) or removes state Message (format==0)
     *  status == false removes Message immediately after sending via rpc
     */
    SLONG UpdateCltMessage(SSHORT wErrorID, const SSHORT* pTabuPar, USHORT anzPar, const SeverityCctrl& severity,
        const char * format, bool status = true);

    /*
       * update and sends status Message (format!=0) or removes state Message (format==0)
       *
    */
    inline SLONG UpdateCltStatus(SSHORT wErrorID, const SSHORT* pTabuPar, USHORT anzPar, const SeverityCctrl& severity,
          const char * format)
      {return UpdateCltMessage(wErrorID, pTabuPar, anzPar, severity, format, true);}

    /*
       * sends Message (format!=0) or removes state Message (format==0)
       * removes Message immediately after sending via rpc
     */
    inline SLONG SendCltMessage(SSHORT wErrorID, const SSHORT* pTabuPar, USHORT anzPar, const SeverityCctrl& severity,
            const char * format)
      {return UpdateCltMessage(wErrorID, pTabuPar, anzPar, severity, format, false);}


    // avoid sending the state messages during runtime of TA
    SLONG flushStateMessages();

  }

#endif /* STATEMESSAGE_H_ */
