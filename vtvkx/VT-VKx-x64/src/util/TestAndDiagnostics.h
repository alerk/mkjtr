/**
* @file      TestAndDiagnostics.h
* Interface of provider of JSON compatible data via WEB interface
*
*/

#ifndef UTIL_TESTANDDIAGNOSTICS_H_
#define UTIL_TESTANDDIAGNOSTICS_H_

#include "evhttpadapter.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
{
    /**
    * Web JSON http request handler
    */
    class TestAndDiagnosticsHttpRequestHandler :public EvRequestHandler
    {
      public:
    	static constexpr const char* NAME = "TestAndDiagnostics";
    	TestAndDiagnosticsHttpRequestHandler() {}
        /**
         *  Get JSON fomatted data
         *
         * @param[in] ehr HTTP request
         * @return    Generated string
         */
        virtual std::string doRequest(EvHttpRequest & ehr);
        virtual void doRequestFormatSetup(EvHttpRequest & ehr);
        /**
         * (JMX) get path to html page
         *
         * @return    Path to HTML (JMX) page
         */
        static std::string getPath();
        static std::string getTacVersionsHTML();
        static std::string getTacVersionsJSON();
        static bool isVersionInvalidated();
        static void setVersionInvalidated(bool invalidated);

        static const char* getDemoKey();
        static bool isDemoVersion();
    };

}

#endif /* UTIL_TestAndDiagnostics_H_ */
