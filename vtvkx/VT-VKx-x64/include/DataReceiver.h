/*
 * DataReceiver.h
 *
 *  Created on: 19.08.2015
 *      Author: zelgerpe
 */

#ifndef DATARECEIVER_H_
#define DATARECEIVER_H_

#include "CControl.h"
#include <stdio.h>
#include <pthread.h>

#include "util/what.h"
DEFINE_WHATINFO_HEADER;


namespace sitraffic
{
  class DataReceiver
  {
    const char *host2subscribe;
    int svcPort;
    int svcTcpPort;
    char myHostname[192];
    pthread_t     leaseRenewalThread;
    void registerCControlDataReceiverService();
    CLIENT * subscribe();
    // called prior (re)subscribintg
    CLIENT* subscribing(CLIENT * clnt);
    void setMyHostname();
    static void * WorkerThreadProc(DataReceiver * pinst);
    void *run();
  public:
    DataReceiver(const char *host2subscribe_) : host2subscribe(host2subscribe_), svcPort(-1), svcTcpPort(-1), leaseRenewalThread(0) {}
    void start();
    CLIENT * getCCtrlClient(const char * prot="udp", int timeoutSec=5) const;

	const char* getHost2subscribe() const {		return host2subscribe;	}
	const char* getMyHostname()     const {		return myHostname;	}
	int         getSvcPort()        const {		return svcPort;	}
	int         getSvcTcpPort()     const {         return svcTcpPort; }
};
}

#endif /* DATARECEIVER_H_ */
