#ifdef WIN32
#include <winsock2.h>
#endif
#include "DataReceiver.h"
#include "Config.h"
#include "mv/Detectors.h"

#include <rpc/pmap_clnt.h>
#include <stdlib.h>
#include <unistd.h>
#include "util/repdef.h"
#include "instregistry.h"
#include "LocalResolv.h"
#include "ConfigNotificationImpl.h"

#include "util/what.h"
DEFINE_WHATINFO;


using namespace sitraffic;

extern "C" void ccontroldatareceiver_8(struct svc_req *rqstp, register SVCXPRT *transp);

void DataReceiver::registerCControlDataReceiverService()
{
  SVCXPRT *transp;
  // don't register data receiver at rpc-portmapper since
  // there SHALL be multiple data receiver processes possible at same host
  // and each data receiver registers at its ccontrol service
  //pmap_unset (CcontrolDataReceiver, Sprint8R);
  transp = svcudp_create(RPC_ANYSOCK);
  if (transp == NULL) {
      Report(RPC_ERROR, "%s", "cannot create udp service.");
          exit(1);
  }
  svcPort=transp->xp_port;
  if (!svc_register(transp, CcontrolDataReceiver, Sprint8R, ccontroldatareceiver_8, /* IPPROTO_UDP avoid register at portmapper */ 0)) {
      Report(RPC_ERROR, "%s", "unable to register (ccontroldatareceiver_8, udp).");
          exit(1);
  }

  transp = svctcp_create(RPC_ANYSOCK, 8192, 8192);
  if (transp == NULL) {
      Report(RPC_ERROR, "%s", "cannot create tcp service.");
          exit(1);
  }
  svcTcpPort=transp->xp_port;
  if (!svc_register(transp, CcontrolDataReceiver, Sprint8R, ccontroldatareceiver_8, /* IPPROTO_UDP avoid register at portmapper */ 0)) {
      Report(RPC_ERROR, "%s", "unable to register (ccontroldatareceiver_8, tcp).");
          exit(1);
  }

}

extern "C" enum clnt_stat subscribe4tdt_8(char *, int , TdtSubscriptionInfoCctrl , enum RetCodeCctrl *, CLIENT *);


CLIENT * DataReceiver::subscribe()
{
    RetCodeCctrl retCode;
    CLIENT *clnt = getCCtrlClient("udp", 5);
    if (clnt) clnt = subscribing(clnt);
    if (clnt)
    {
        MkjtricDataTypeCctrl tdts2subscribe[] = { TDT_ACTUALSTATUS,
                TDT_REQUESTEDSTATUS, TDT_DETRAW, TDT_ACTUALSIGNALINDICATIONS,
                TDT_RIDEEVENT, TDT_ARCHIVEEVENTS };
        for (MkjtricDataTypeCctrl tdt : tdts2subscribe)
        {
            TdtSubscriptionInfoCctrl data2subscribe =
                    { tdt, { AS_NoArchive, 0 } };
            if (tdt == TDT_ACTUALSIGNALINDICATIONS)
                data2subscribe.TdtSubscriptionInfoCctrl_u.withRedGreenStates =
                        true; //activate sending of redGreenStates
            if (tdt == TDT_ARCHIVEEVENTS)
            {
                data2subscribe.TdtSubscriptionInfoCctrl_u.archiveInfo.selector =
                        AS_LampStates;
                data2subscribe.TdtSubscriptionInfoCctrl_u.archiveInfo.count = 1;
            }
            if (subscribe4tdt_8((char *) myHostname, svcPort, data2subscribe,
                                &retCode, clnt) != RPC_SUCCESS
                    || retCode != RC_OK)
            {
                clnt_destroy(clnt);
                clnt = 0;
                Report(RPC_ERROR,
                       "subscribe4tdt_8(%s, %d, %d) failed retCode=%d",
                       myHostname, svcPort, data2subscribe.tdt, retCode);
                break;
            }
        }
    }

    return clnt;
}

// @return c-control client to our firmware host, has to be destroyed by clnt_destroy() if not used anymore
CLIENT *  DataReceiver::getCCtrlClient(const char * prot, int timeoutSec) const
{
  CLIENT * clnt = clnt_create (host2subscribe, CcontrolService, Sprint8R, prot);
  if (clnt == NULL) {
          char buf[256];
          snprintf(buf, sizeof buf, "clnt_create(%s, %d, %d, %s) failed", host2subscribe, CcontrolService, Sprint8R, prot);
          clnt_pcreateerror (buf);
          Report(RPC_ERROR, "%s", clnt_spcreateerror (buf));
          return 0;
  }
  struct timeval timeout={timeoutSec,0};
  clnt_control(clnt, CLSET_TIMEOUT, (char *)&timeout);
  return clnt;
}

#ifdef WIN32
const char* inet_ntop(short af, const void* src, char* dst, int cnt){

    struct sockaddr_in srcaddr;

    memset(&srcaddr, 0, sizeof(struct sockaddr_in));
    memcpy(&(srcaddr.sin_addr), src, sizeof(srcaddr.sin_addr));

    srcaddr.sin_family = af;
    if (WSAAddressToString((struct sockaddr*) &srcaddr, sizeof(struct sockaddr_in), 0, dst, (LPDWORD) &cnt) != 0) {
        DWORD rv = WSAGetLastError();
        printf("WSAAddressToString() : %ld\n",rv);
        return NULL;
    }
    return dst;
}
#else
#include <arpa/inet.h>
#include <netdb.h>
#endif

void DataReceiver::setMyHostname()
  {
    if (!GetLocalEndpointIp(host2subscribe, myHostname, sizeof(myHostname))) {
      printf("myHostname resolve failed\n");
      exit(1);
    }
    printf("myHostname=%s\n", myHostname);
  }

extern "C" enum clnt_stat renew_8(char *inetadr, int port, enum RetCodeCctrl *clnt_res,  CLIENT *clnt);

#ifdef WIN32
void sleep(int secs)
{
        Sleep(secs*1000);
}
#endif


//static
void * DataReceiver::WorkerThreadProc(DataReceiver * pinst)
{
  return pinst->run();
}

void * DataReceiver::run()
  {
    CLIENT *clnt = 0;
    RetCodeCctrl retCode;
    setCurrentThreadName("renew"); // data receiver (re)subscribe and renew thread.
    for (;;)
      {
        if (clnt == 0)
          {
            clnt = subscribe();
            if (clnt == 0)
              {
                Report(RPC_INFO, "subscribe(%s, %s) failed, retrying", host2subscribe, myHostname);
                sleep(5);
              }
          }
        else
          {
            sleep(90);
            if (renew_8(&myHostname[0], this->svcPort, &retCode, clnt) != RPC_SUCCESS || retCode != RC_OK)
              { /* resubscribe */
                Report(RPC_ERROR, "renew_8(%s, %d, %d) failed %s", &myHostname[0], this->svcPort, retCode, clnt_sperror(clnt, ""));
                clnt_destroy(clnt);
                clnt = 0;
              }
          }
      }
    if (clnt)
      clnt_destroy(clnt);
    return NULL;
  }

typedef void *(* pthreadfunc_t)(void *);

// start the data receiver,
// start thread that subscribes at firmware
void DataReceiver::start()
{
  setMyHostname();
  registerCControlDataReceiverService();
  pthread_create(&leaseRenewalThread, 0, (pthreadfunc_t)WorkerThreadProc, this);
}

/**
 * called prior (re)subscribe, fills configuration data that is necessary with first signal indications ticks.
 * creates and registers the Config instance at InstRegistry
 * @param client rpc client handle
 * @return clnt or null in case of rpc error
 */
CLIENT * DataReceiver::subscribing(CLIENT * clnt)
{
  static Config config;
  static ConfigNotificationImpl configNotification;

  // fill Detectors values
  Detectors & dets=InstRegistry::Instance().getDetectors();
  clnt = dets.fill(clnt);

  clnt = config.fill(clnt);

  if(clnt) InstRegistry::Instance().set(config);

  configNotification.registerAtCctrl(clnt, *this);
  InstRegistry::Instance().set(configNotification);

  return clnt;
}
