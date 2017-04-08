/*
 * tiny http server for debug purpose. Planned to use analog to JMX http adapter with java.
 *
 */
#ifndef EVHTTPADAPTER_H
#define EVHTTPADAPTER_H


// forwards to avoid #include <evhttp.h>
struct evhttp_uri;
struct evhttp_request;
struct event_base;
struct evhttp;
struct event;

//#include <cstdint>
#include <stdint.h>
#include <string>
#include <map>

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
{
	enum class HttpReplyFormat: int
	{
		HTML,
		PLAIN
	};

  class EvHttpRequest
  {
    evhttp_request *req;
    const evhttp_uri *uri;
    const char *query;
    char *postData;
    std::map<std::string, const char *> pmap;
    EvHttpRequest(const EvHttpRequest & o) = delete;
    EvHttpRequest & operator=(const EvHttpRequest & o) = delete;
	HttpReplyFormat _format;
  public:
    EvHttpRequest(evhttp_request *rq);
    ~EvHttpRequest();
    /** @return hostname of the request uri. see evhttp_uri_get_host() */
    const char * getHost() const;
    const char * getPath() const;
    const char * getQuery();
    const char * getPostData();
    bool isPost() const;
    const std::map<std::string, const char *> & getParams();
    const char * getParam(std::string param);
    void fillQParams(std::map<std::string, const char *> & kvm);
    std::string toString();
    /**
     * set format of HTTP resulting reply
     *
     * @param[in] format Format of HTTP reply
     */
    void setFormat(HttpReplyFormat format);
    /**
     * get format of HTTP resulting reply
     *
     * @return Format of HTTP reply
     */
    HttpReplyFormat getFormat();
  };

  class EvRequestHandler
  {
  public:
    EvRequestHandler()
    {
    }
    EvRequestHandler(EvRequestHandler & /*o*/)
    {
    }
    virtual std::string
    doRequest(EvHttpRequest & /*ehr*/)
    {
      return "";
    }
    /**
     * set format of HTTP resulting reply depending on request
     *
     * @param[in,out] ehr HTTP request
     */
    virtual void doRequestFormatSetup(EvHttpRequest & ehr);
    virtual ~EvRequestHandler()
    {
    }
  };

// wrapper for libevent's httpd
  class EvHttpServer
  {
    evhttp *Server;
    event *evSigInt;
    event *evSigTerm;
    event_base *eventBase;
    // key=path, value=heap allocated request handler
    std::map<std::string, EvRequestHandler *> rhs;
    static bool stopped;        // true if dispatch thread has returned, that is process is to be stopped
  public:
    EvHttpServer(event_base *eventBase = 0, const char * SrvAddress = "0.0.0.0",
        uint16_t SrvPort = 5555);
    ~EvHttpServer();
    int
    dispatch();

    void
    add(std::string path, EvRequestHandler * rh);

    const std::map<std::string, EvRequestHandler *> &
    getRhs()
    {
      return rhs;
    }

    static void createRegisterAndDispatch(uint16_t SrvPort = 5555);
    static void * dispatchEvents(void *srv);
    static bool isStopped() { return stopped; }

  protected:
    static void
    onRequestPath(evhttp_request *req, void *ptr);

    static void
    onRequestDefault(evhttp_request *req, void *ptr);

    EvRequestHandler *
    createIndexHandler();

  private:
    const EvHttpServer& operator = (const EvHttpServer& other);
    EvHttpServer(const EvHttpServer& other);

  };
}

#endif
