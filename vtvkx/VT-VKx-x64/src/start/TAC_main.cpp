/*
 ============================================================================
 Name        : TA_main.c
 Author      : zelgerpe
 Version     :
 Copyright   : Siemens ITS
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <rpc/rpc.h>
#include "DataReceiver.h"
#include "evhttpadapter.h"
#include "instregistry.h"
#include "mv/Detectors.h"
#include "cctrl/htmlHandler.h"
#include "start/awpWrapper.h"
#include "cctrl/cltDeviceValues.h"
#include "cctrl/PublicTransport.h"
#include "cctrl/Archive.h"
#include "util/repdef.h"
#include "tac_kernel.h"

#include "start/api_TaRunmode.h"
#include "util/TestAndDiagnostics.h"
#include "util/PublicTransportHttp.h"

#include "util/what.h"
DEFINE_WHATINFO;

#ifndef WIN32
#include <mcheck.h>
#endif

static int initwinsock()
{
  int wsaret=0;
  /* initialize the winsock2.dll, only needed if running on WIN32 */
  #ifdef WIN32
          WSADATA wsaData;
          WORD  wVersionRequested = MAKEWORD(2, 2);
          /* initialize winsock dll */
          if ((wsaret=WSAStartup(wVersionRequested, &wsaData))!=0)
                  {
                  fprintf (stderr, "WSAStartup failed %d", wsaret);
                  }
          else
          if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
              /* Tell the user that we could not find a usable */
              /* WinSock DLL.                                  */
              printf("Could not find a usable version of Winsock.dll\n");
              WSACleanup();
              return 1;
          }
          else
              printf("The Winsock 2.2 dll was found okay\n");

  #endif
  return wsaret;
}


using namespace sitraffic;


//-------------------------------------------------------------------------------




static const char * firmwareHost_default = "localhost";
#ifdef WIN32
  static const char fsPathSeperator='\\';
#else
  static const char fsPathSeperator= '/';
#endif


#define _VERSION_STRING_        "0.0.0.1"

int tac_main(int argc, char *argv[], int(*awpinit)(), int(*awpfunction)())
{
#ifndef WIN32
      printf("starting mtrace()\n");
      mtrace();
#endif

    setCompileTime("Tac");

	const char * levels = IniReport("main", 2, "log/logTA", 2000000, "/tmp/va_debug", 500000);

	Report(INIT_STATE, "TacKernel: %s start with PID=%d and Report-Level: %s",
			                _VERSION_STRING_, (int)getpid(), levels);

	if(initwinsock())
    {
	  Report(INIT_ERROR, "initwinsock failed");
      return -1;
    }

	const char *firmwareHost=firmwareHost_default;
	std::string exeName(argv[0]);
	std::string::size_type pos=exeName.find_last_of(fsPathSeperator);
	if(pos!=std::string::npos)
	  exeName=exeName.substr(pos+1);
	std::string dvpName=exeName;
      bool withMitigation=true;
      int c;
      uint16_t jmxPort=8012; // JMX adapter http adapter debug port forwarded by lighttpd on sX
      while((c=getopt(argc,argv,"r:h:n:j:N"))!=-1)
        switch(c)
        {
         case 'r':
           break;
        case 'h':
          firmwareHost=optarg;
          break;
        case 'n':
          dvpName = optarg;
          break;
        case 'j':
          jmxPort=(uint16_t)strtoul(optarg, 0, 0);
          break;
        case 'N':
          withMitigation=false;
          break;
        default:
          printf("[-h <firmwareHost>][-n <devName>][-j <jmxHttpAdapterPort>][-N]\n");
          Report(INIT_ERROR, "unknown opt %c", c);
          return -2;
        }
      ModOnOffHelperStateHandler sh {};
      InstRegistry::Instance().set(sh);
      AwpWrapper aw(awpinit, awpfunction, withMitigation);
      InstRegistry::Instance().set(aw);
      CltDeviceValueProviders cltDeviceValueProviders(dvpName);
      InstRegistry::Instance().set(cltDeviceValueProviders);

      EvHttpServer::createRegisterAndDispatch(jmxPort);
      EvHttpServer& evHttpServer = InstRegistry::Instance().getEvHttpServer();
      evHttpServer.add(PublicTransportHttpRequestHandler::getPath(),
        new PublicTransportHttpRequestHandler());
      evHttpServer.add("/signalIndications", new SiRequestHandler());
      evHttpServer.add("/operatingMode", new OperatingModeHtmlRequestHandler());
      evHttpServer.add("/StateMessage", new StateMessageHtmlRequestHandler());
      evHttpServer.add(TaRunmodeHtmlRequestHandler::getPath(), new TaRunmodeHtmlRequestHandler());
      evHttpServer.add("/Report", new ReportHtmlRequestHandler());
      evHttpServer.add("/WhatInfo", new WhatInfoHtmlRequestHandler());
      evHttpServer.add(TestAndDiagnosticsHttpRequestHandler::getPath(), new TestAndDiagnosticsHttpRequestHandler());

      sitraffic::DataReceiver dr(firmwareHost);
      InstRegistry::Instance().set(dr);

      Detectors detectors;
      InstRegistry::Instance().set(detectors);

      //by Valentin
      Archive archive;
      InstRegistry::Instance().set(archive);

      dr.start();

      PublicTransport pt;
      InstRegistry::Instance().set(pt);

      Report(INIT_STATE, "pdmx: started with firmwareHost = %s, withMitigation = %d, dvpName = %s", firmwareHost, withMitigation, dvpName.c_str());

      /* start rpc handler loop, may be left with svc_exit() see AwpWrapper */
      svc_run ();
      Report(INIT_STATE, "svc_run returned");
      EndReport();
      // don't call exit here, to give the destructors a chance to run
      return 0;
}
