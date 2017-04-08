/**
* @file      api_TaRunmode.h
* Header file for TA Runmode handling module
*/

#ifndef START_API_TARUNMODE_H_
#define START_API_TARUNMODE_H_

#include "evhttpadapter.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
{
    /**
    * TaRunmode HTML request handler
    */
    class TaRunmodeHtmlRequestHandler :public EvRequestHandler
    {
      public:
    	static constexpr const char* NAME = "TaRunmode";
        /** Constructor TaRunmode HTML request handler */
        TaRunmodeHtmlRequestHandler() {}
        /* Generate a request result */
        virtual std::string doRequest(EvHttpRequest & ehr);
        /* Get  path to (JMX) HTML */
        static std::string getPath();
    };

}

#endif /* START_API_TARUNMODE_H_ */
