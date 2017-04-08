/*
 * evhttpadapter.cpp
 *
 *  Created on: 13.08.2015
 *      Author: zelgerpe
 */

#include "evhttpadapter.h"
#include <event2/event.h>
#include <event2/event_struct.h>
#include <event2/http.h>
#include <event2/http_struct.h>
#include <event2/buffer.h>

#include <memory>
//#include <cstdint>
#include <iostream>
#include <sstream>
#include <signal.h>

#include "util/what.h"
DEFINE_WHATINFO;

using namespace sitraffic;

EvHttpRequest::EvHttpRequest(evhttp_request *rq) :
    req(rq), uri(evhttp_request_get_evhttp_uri(rq)), query(0), postData(NULL), pmap(), _format(HttpReplyFormat::HTML)
{
}

EvHttpRequest::~EvHttpRequest() {
  if (this->postData)
    free(this->postData);
}

const char * EvHttpRequest::getHost() const
{
  // use the request, since evhttp_uri_get_host(uri) seems to return always null
  return evhttp_request_get_host(req);
}

const char *
EvHttpRequest::getPath() const
{
  return evhttp_uri_get_path(uri);
}
const char *
EvHttpRequest::getQuery()
{
  if (query)
    return query;
  else
    return query = evhttp_uri_get_query(uri);
}

void
EvHttpRequest::fillQParams(std::map<std::string, const char *> & kvm)
{
  kvm.clear();
  evkeyvalq keyvals =
    { 0, 0 };
  evhttp_parse_query_str(getQuery(), &keyvals);
  for (evkeyval* kv = keyvals.tqh_first; kv; kv = kv->next.tqe_next)
    {
      //evbuffer_add_printf(OutBuf, "key=%s value=%s<br/>\n", kv->key, kv->value);
      kvm[kv->key] = kv->value;
      if (kv->next.tqe_next == *keyvals.tqh_last)
        break;
    }
}
const std::map<std::string, const char *> &
EvHttpRequest::getParams()
{
  if(pmap.empty())
  fillQParams(pmap);
  return pmap;
}

const char *
EvHttpRequest::getParam(std::string param)
{
    const char *value = nullptr;
    auto & par_map(this->getParams());
    auto valueIt = par_map.find(param);
    if(valueIt != par_map.end())
    {
        value = valueIt->second;
    }
    return value;
}

/**
 * returns true if http request is POST
 */
bool
EvHttpRequest::isPost() const {
  return evhttp_request_get_command(this->req) == EVHTTP_REQ_POST;
}

/**
 * returns pointer to post data of request
 *
 * if requeste in question is not POST the value is undefined
 * use EvHttpRequest::isPost() to check that
 */
const char*
EvHttpRequest::getPostData() {
  if (this->postData == NULL) {
    size_t postLength = evbuffer_get_length(evhttp_request_get_input_buffer(this->req));
    this->postData = (char*)malloc(sizeof(char)*(postLength + 1));
    this->postData[postLength] = '\0';
    evbuffer_copyout(evhttp_request_get_input_buffer(this->req), this->postData, postLength);
  }
  return this->postData;
}

// set format of HTTP resulting reply
void EvHttpRequest::setFormat(HttpReplyFormat format)
{
	_format = format;
}

// get format of HTTP resulting reply
HttpReplyFormat EvHttpRequest::getFormat()
{
	return _format;
}

std::string
EvHttpRequest::toString()
{
  std::ostringstream ss;
  ss << "host=" << (getHost()==0?((const char *)"null"):getHost()) << "<br/>";
  ss << "path=" << getPath() << "<br/>";
  ss << "query=" << getQuery() << "<br/>";
  const std::map<std::string, const char *> & m = getParams();
  for (auto it = m.begin();
      it != m.end(); it++)
    {
      ss << "key=" << it->first << " value=" << it->second << "<br/>";
    }
  return ss.str();
}

//------------------- EvRequestHandler ----------------------

// set format of HTTP resulting reply depending on request
void EvRequestHandler::doRequestFormatSetup(EvHttpRequest & /*ehr*/)
{
	// dummy
}

//------------------- EvHttpServer ----------------------
static void
signal_cb(evutil_socket_t /*fd*/, short /*event*/, void *arg)
{
        //cout << "got signal:" << std::endl;
        event_base_loopbreak((event_base*)arg);
}


EvHttpServer::EvHttpServer(event_base *peventBase, const char * SrvAddress, uint16_t SrvPort) :
        Server(0), evSigInt(0), evSigTerm(0), eventBase(peventBase), rhs()
{
  if (peventBase == 0)
    this->eventBase = event_base_new();//event_init();
  Server = evhttp_new(this->eventBase);//evhttp_start(SrvAddress, SrvPort);
  evhttp_bind_socket(Server, SrvAddress, SrvPort);
  evhttp_set_gencb(Server, onRequestDefault, this);
  /* Initalize one event */
  evSigInt = evsignal_new(this->eventBase, SIGINT, signal_cb, this->eventBase);
  evSigTerm = evsignal_new(this->eventBase, SIGTERM, signal_cb, this->eventBase);

  event_add(evSigInt, NULL);
  event_add(evSigTerm, NULL);
}

EvHttpServer::~EvHttpServer()
{
  evhttp_free(Server);
  event_free(evSigInt);
  event_free(evSigTerm);
  event_base_free(eventBase);
  for (std::map<std::string, EvRequestHandler *>::iterator it = rhs.begin(); it != rhs.end(); it++)
    {
      delete it->second;
    }
}
int
EvHttpServer::dispatch()
{
  int ret = event_base_dispatch(eventBase);
  if (ret == -1)
    {
      std::cerr << "Failed to run message loop." << std::endl;
    }
  return ret;
}
void
EvHttpServer::add(std::string path, EvRequestHandler * rh)
{
  rhs[path] = rh;
  evhttp_set_cb(Server, path.c_str(), onRequestPath, this);
}

/**
 * Generates a dynamic content of HTML page. Mainly headers etc.
 *
 * @param req Request for page generating
 * @param ptr (Page specific) HTTP handler for generating
 */
void
EvHttpServer::onRequestPath(evhttp_request *req, void *ptr)
{
  EvHttpServer & s(*((EvHttpServer*) ptr));
  evbuffer *OutBuf = evhttp_request_get_output_buffer(req);
  if (!OutBuf)
    return;
  EvHttpRequest rq(req);

  HttpReplyFormat format = rq.getFormat();
  {
  // format setup
	  s.rhs.find(rq.getPath())->second->doRequestFormatSetup(rq);
	  format = rq.getFormat();
  }
  if(format == HttpReplyFormat::HTML)
  {
	  evbuffer_add_printf(OutBuf, "<html>\n");
	  evbuffer_add_printf(OutBuf, "<title>TACKernel%s</title>\n", rq.getPath());
	  evbuffer_add_printf(OutBuf, "<body>\n");
  }
  {
  // body
	  std::map<std::string, EvRequestHandler *>::iterator it = s.rhs.find(
			  rq.getPath());
	  while (it != s.rhs.end() && it->first == std::string(rq.getPath()))
	  {
		  evbuffer_add_printf(OutBuf, "%s", it->second->doRequest(rq).c_str());
		  it++;
	  }
  }
  if(format == HttpReplyFormat::HTML)
  {
	  evbuffer_add_printf(OutBuf, "</body>\n</html>\n");
  }
  evhttp_send_reply(req, HTTP_OK, "", OutBuf);
}

//static
void
EvHttpServer::onRequestDefault(evhttp_request *req, void * /*ptr*/)
{
  evbuffer *OutBuf = evhttp_request_get_output_buffer(req);
  if (!OutBuf)
    return;
  EvHttpRequest ehr(req);
  evbuffer_add_printf(OutBuf,
      "<html><body><center><h1>Hello World!</h1><p>uri: %s</p>%s</center></body></html>",
      req->uri, ehr.toString().c_str());
  evhttp_send_reply(req, HTTP_OK, "", OutBuf);
}

class IndexRequestHandler : public EvRequestHandler
{
  EvHttpServer * srv;
  IndexRequestHandler(const IndexRequestHandler &o ) = delete;
  IndexRequestHandler & operator =(const IndexRequestHandler &o ) = delete;
public:
  IndexRequestHandler(EvHttpServer & s) :
      srv(&s)
  {
  }
  IndexRequestHandler(IndexRequestHandler& o) :
      EvRequestHandler(o), srv(o.srv)
  {
  }
  virtual std::string
  doRequest(EvHttpRequest & /*ehr*/)
  {
    std::ostringstream ss;
    ss << "<center>" << "<h1>Index</h1>" << "<ul>" << std::endl;

    for (std::map<std::string, EvRequestHandler *>::const_iterator it =
        srv->getRhs().begin(); it != srv->getRhs().end(); it++)
      {
        ss << "<li><a href=\"" << it->first << "\">" << it->first << "</a></li>"
            << std::endl;
      }
    ss << "</ul></center>";
    return ss.str();
  }
};

EvRequestHandler *
EvHttpServer::createIndexHandler()
{
  return new IndexRequestHandler(*this);
}

#ifdef MIT_MAIN
class TestRh : public EvRequestHandler
{
  const char * msg;
public:
  TestRh(const char *m):msg(m){}
  virtual std::string doRequest(EvHttpRequest & ehr)
  {
    std::ostringstream ss;
    ss << "<p>the message is:</p><p>" << msg << "</p>" << std::endl;
    return ss.str();
  }
};


using namespace sitraffic;

int main()
{
#ifdef WIN32
        WSADATA WSAData;
        WSAStartup(0x101, &WSAData);
#endif
        EvHttpServer Server;
        Server.add("/msg1", new TestRh("bla"));
        Server.add("/msg2", new TestRh("bla bla"));
        Server.dispatch();
        std::cout << "leaving" << std::endl;
      return 0;
 }

#endif
