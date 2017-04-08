/*! \file
 * main implementation of tacKernelDemo, TA_main.cpp.
 *
 *  Created on: 26.11.2015
 *      Author: zelgerpe
 */

#include "instregistry.h"
#include "mv/Detectors.h"
#include "cctrl/cltDeviceValues.h"
#include "util/repdef.h"
#include "util/TestAndDiagnostics.h"
#include "util/TacDemoHttp.h"
#include "evhttpadapter.h"

#include "tac_kernel.h"
#include <ctime>

#include "util/what.h"
DEFINE_WHATINFO;

sitraffic::TacDemoOptions options;

/** provide AU device value for test/demo purpose.
 * @param oitd32 ignored
 * @param instanz instance number of the desired device variable, = detector number
 * @param pvalue address of result device value, set to -1 if detector is unknown/not configured.
 * @return 0
 */
static int getAUValue(ULONG /*oitd32*/, int instanz, int *pvalue)
  {
    const sitraffic::Detector *pdet=sitraffic::InstRegistry::Instance().getDetectors().getDet(instanz);
        if(pdet)
          *pvalue = (int)pdet->getAU();
        else
          *pvalue = -1;
    return 0;
  }

/** meta definition of demo device value <b>AU</b> */
static DEVICE_VALUE_TYPE_TAC sDevVar[] = {
    {getAUValue, "AU", "actual occupied", "bla bla", 60, 880, TADVI_Detector, 0, 255, 0, 8, 0 }
};

/**
 * AWP_Init function used for tacKernelDemo.
 * outputs a log, registers a device variable AU that is to be polled by tac kernel via (@ref getAUValue)
 */
int dummyInit()
  {
    sitraffic::EvHttpServer& evHttpServer = sitraffic::InstRegistry::Instance().getEvHttpServer();
    sitraffic::TacDemoHttpRequestHandler *demoHttp = new sitraffic::TacDemoHttpRequestHandler(options);
    evHttpServer.add(demoHttp->getPath(), demoHttp);

    Report(TAC_INFO, "dummyAwpInit called");
    sitraffic::InstRegistry::Instance().getCltDeviceValueProviders().add(1, &sDevVar[0]);
    return AWP_RS_VA_LAEUFT;
  }

/**
 * AWP_Function used for tacKernelDemo.
 * put a log, set response ta runmode equal to requested tac runmode after 5 invocations/seconds.
 * @return AWP_RS_VA_LAEUFT
 */
int dummyFunction()
  {
    const unsigned taRunmodeSetDelay=5;
    static unsigned ncalls=0;
    static TaRunmode lastSetRunmode=TARM_OFF;
    Report(TAC_INFO, "dummyAwpFunction called %u time", ++ncalls);

    // set ta runmode needed for deploy test
    const TaRunmode rqrm=tacGetRequestTaRunmode();
    if(ncalls >taRunmodeSetDelay && rqrm!=lastSetRunmode)
      {
        tacSetResponseTaRunmode(rqrm);
        lastSetRunmode = rqrm;
    }

    if (options.loopbackOn) {
        // PT telegram loopback
        TacRideEvent pt;
        SLONG ret = tacGetRideEvent(&pt);
        if (ret == SUCCESS) {
            TacAmliDto amli;

            tacGetDateTime(&amli.dateTime);

            amli.telegram = pt;
            amli.greenTime = 0;
            amli.ptModActive = FALSE;
            amli.tx = 0;
            amli.signalProgramNo = 0;
            amli.actualStage = 0;
            amli.nextStage = 0;
            amli.travelTime = 0;
            amli.txGNS = 0;
            amli.txGNE = 0;
            amli.amliTimeout = 0;
            //amli.ptDirectionNo = pt.ptDirectionNo;
            amli.type = TAC_AET_unknown;
            amli.checkOutSubType = TAC_ACOR_unknown;
            amli.signalGroupNo = 0;

            tacInsertAmliEventToArchive(&amli);
        }
    }

    return AWP_RS_VA_LAEUFT;
  }

/**
 * main of tacKernelDemo
 * @param argc number of parameters
 * @param argv commandline parameters, see (@ref tac_main)
 * @return see (@ref tac_main)
 */
int main(int argc, char *argv[])
{
	setVersion(sitraffic::TestAndDiagnosticsHttpRequestHandler::getDemoKey(), __DATE__ " " __TIME__); // indicate TacKernel DEMO for autoregression tests
    return tac_main(argc, argv, dummyInit, dummyFunction);
}
