#include <sstream>

#include <string>
#include "util/what.h"

#include "TacDemoHttp.h"
DEFINE_WHATINFO;

using namespace sitraffic;
using namespace std;

string TacDemoHttpRequestHandler::doRequest(EvHttpRequest & ehr)
{
    string result = "N/A";
    ostringstream ss;
    const char *loop = ehr.getParam("loopbackOn");

    try {
        if (loop != nullptr)
            opts.loopbackOn = std::stoi(loop) != 0;
    } catch (const std::exception &e) {
        return string("invalid argument");
    }

    ss <<  R"*eNd*(
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
        "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<title>TacKernelDemo</title>
</head>
<body>
<a href="/">Back</a>
<hr/>
<table border="1">
)*eNd*";

    ss << "<tr><td><a href=?loopbackOn="
        << !opts.loopbackOn << ">loopbackOn</a></td><td>"
        << opts.loopbackOn << "</td></tr>" << endl;

    ss <<  R"*eNd*(
</table>
</body>
</html>
)*eNd*";

    return ss.str();
}

string TacDemoHttpRequestHandler::getPath()
{
    ostringstream ss;
    ss << "/" << TacDemoHttpRequestHandler::NAME;
    return ss.str();
}

void TacDemoHttpRequestHandler::doRequestFormatSetup(EvHttpRequest & ehr)
{
    ehr.setFormat(HttpReplyFormat::PLAIN);
}
