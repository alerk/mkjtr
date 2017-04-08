/**
* @file      TacDemoHttp.h
* Interface of provider of tacKernelDemo web interface
*
*/

#ifndef UTIL_TACDEMOHTTP_H_
#define UTIL_TACDEMOHTTP_H_

#include "evhttpadapter.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
{
    struct TacDemoOptions {
        int loopbackOn = 1;
    };

    class TacDemoHttpRequestHandler :public EvRequestHandler
    {
        TacDemoOptions &opts;
      public:
    	static constexpr const char* NAME = "TacKernelDemo";
    	TacDemoHttpRequestHandler(TacDemoOptions &o) : opts(o) {}

        virtual std::string doRequest(EvHttpRequest & ehr);
        virtual void doRequestFormatSetup(EvHttpRequest & ehr);

        static std::string getPath();
    };
}

#endif /* UTIL_TACDEMOHTTP_H_ */
