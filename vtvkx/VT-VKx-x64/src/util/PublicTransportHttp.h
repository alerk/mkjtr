/**
* @file      PublicTransportHttp.h
* Interface of provider of PublicTransport web interface
*
*/

#ifndef UTIL_PUBLICTRANSPORTHTTP_H_
#define UTIL_PUBLICTRANSPORTHTTP_H_

#include "evhttpadapter.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
{
    class PublicTransportHttpRequestHandler : public EvRequestHandler
    {
      public:
    	static constexpr const char* NAME = "PublicTransport";

        virtual std::string doRequest(EvHttpRequest & ehr);
        virtual void doRequestFormatSetup(EvHttpRequest & ehr);

        static std::string getPath();
    };
}

#endif /* UTIL_PUBLICTRANSPORT_H_ */
