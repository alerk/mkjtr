/*
 * evDispatchThread.cpp
 * create EvHttpServer server, start dispath in own thread.
 *
 *  Created on: 01.09.2015
 *      Author: zelgerpe
 */

#include "evhttpadapter.h"
#include "instregistry.h"
#include <pthread.h>
#include "report.h"

#include "util/what.h"
DEFINE_WHATINFO;

using namespace sitraffic;

//static
void * EvHttpServer::dispatchEvents(void *srv)
{
  EvHttpServer & server=*static_cast<EvHttpServer*>(srv);
  setCurrentThreadName("http"); // for Report outputs
  server.dispatch();
  EvHttpServer::stopped=true;
  return 0;
}

static pthread_t evHttpDispatchThread;

bool EvHttpServer::stopped=false;

// @TODO shall only called once
void EvHttpServer::createRegisterAndDispatch(uint16_t SrvPort)
{
  static EvHttpServer server(0, "0.0.0.0", SrvPort);
  server.add("/", server.createIndexHandler());
  InstRegistry::Instance().set(server);
  pthread_create(&evHttpDispatchThread, 0, dispatchEvents, &server);
}
