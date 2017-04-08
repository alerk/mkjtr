/**
 * @file      TestAndDiagnostics.cpp
 * Provider of JSON compatible data via WEB interface
 *
 */

#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>
#include <util/json.hpp>
#include "instregistry.h"
#include "CControl.h"
#include "cctrl/cltDeviceValues.h"
#include "repdef.h"
#include <util/TacEnums.h>
#include <util/TestAndDiagnostics.h>

#include "util/what.h"
DEFINE_WHATINFO;

using namespace sitraffic;
using namespace std;
using namespace tackernel;
using json = nlohmann::json;

typedef string (*StructToJson)(EvHttpRequest&);

static string TacOperatingStateString(EvHttpRequest&);
static string TacVersionString(EvHttpRequest&);
static string TacInjectReportEntry(EvHttpRequest&);
static string RequestTaRunmodeString(EvHttpRequest&);
static string ModifiedRequestTaRunmodeString(EvHttpRequest&);
static string TacSetLockSwitchOff(EvHttpRequest&);
static string TacReleaseControl(EvHttpRequest&);
static string TacSetTX(EvHttpRequest&);
static string TacInsertAmliEventToArchive(EvHttpRequest&);
static string TacGetRideEvent(EvHttpRequest&);

/** Mapper from structure to JSON serializing */
static map<string,StructToJson> STRUCT_TO_JSON_MAP = {
        {"TacOperatingState", TacOperatingStateString},
		{"TacGetVersions", TacVersionString},
		{"TacInjectReportEntry", TacInjectReportEntry},
        {"RequestTaRunmode", RequestTaRunmodeString},
        {"ModifiedRequestTaRunmode", ModifiedRequestTaRunmodeString},
        {"TacSetLockSwitchOff", TacSetLockSwitchOff},
        {"TacReleaseControl", TacReleaseControl},
        {"TacSetTX", TacSetTX},
        {"TacInsertAmliEventToArchive", TacInsertAmliEventToArchive},
        {"TacGetRideEvent", TacGetRideEvent }
};

/**
 * It this request on default page
 *
 * @param[in] ehr HTTP request
 * @return true in default
 */
static bool isDefaultPage(EvHttpRequest & ehr)
{
    bool state = true;
    const char *name = ehr.getParam("name");
    if(name != nullptr)
    {
        state = false;
    }
    return state;
}


/*
 * Default page (without parameters)
 *
 * @return resulting HTML page
 */
static string defaultPage(void)
{
    ostringstream ss;
    ss <<  R"*eNd*(
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN"
        "http://www.w3.org/TR/html4/strict.dtd">

<html>
<head>
<title>TestAndDiagnostics</title>
</head>
<body>
<script type="text/javascript">
    document.write('<script type="text/javascript" src="http://' + window.location.hostname + '/js/tac/TestAndDiagnostics.js"></' + 'script>' );
</script>
</body>
</html>
)*eNd*";
    return ss.str();
}

/*
 * Create JSON list of available structures
 *
 * @return resulting JSON string
 */
static string pageJsonList(void)
{
    ostringstream ss;
    json j = {};
    {
        vector<string> stj;
        {
            for(const auto& kvIt : STRUCT_TO_JSON_MAP)
            {
                stj.push_back(kvIt.first);
            }
        }
        j["list"] = stj;
    }

    ss << j;

    return ss.str();
}

/* favourite request format */
void TestAndDiagnosticsHttpRequestHandler::doRequestFormatSetup(EvHttpRequest & ehr)
{
    if(isDefaultPage(ehr) == true)
    {
        ehr.setFormat(HttpReplyFormat::PLAIN);
    }
    else
    {
        ehr.setFormat(HttpReplyFormat::PLAIN);
    }
}

/*  Get JSON fomatted data */
string TestAndDiagnosticsHttpRequestHandler::doRequest(EvHttpRequest & ehr)
{
    string result = "N/A";
    setCurrentThreadName("http");

    if(isDefaultPage(ehr) == false)
    {
        const char *name = ehr.getParam("name");

        if(name != nullptr)
        {
            auto valueIt = STRUCT_TO_JSON_MAP.find(name);
            if(valueIt != STRUCT_TO_JSON_MAP.end())
            {
                result = (*valueIt->second)(ehr);
            }
            else
            {
                result = "name is unknown";
            }
        }
    }
    else
    {
        const char *list = ehr.getParam("list");
        if(list != nullptr)
        {
            result = pageJsonList();
        }
        else
        {
            result = defaultPage();
        }
    }
    return result;
}

/*  (JMX) get path to html page */
string TestAndDiagnosticsHttpRequestHandler::getPath()
{
    ostringstream ss;

    ss << "/" << TestAndDiagnosticsHttpRequestHandler::NAME;

    return ss.str();
}

/**
 * Returns a JSON error string for invalid request
 * to function with parameters.
 * @param request object which caused this error
 */
static string jsonNotAPostRequest(EvHttpRequest &ehr) {
    string name = ehr.getParam("name");
    return "{ \"status\" : \"ERROR_PARAM\", \"message\" : \"you need to make POST request to pass paramaters\
 to " + name + " function\" }";
}

/**
 * Return a JSON error message if JSON in POST is not
 * in valid input argument format
 * @param exception which was rised when error occured
 */
static string jsonSyntaxError(std::exception &e) {
    string msg = e.what();
    return "{ \"status\" : \"ERROR_PARAM\", \"message\" : \"" + msg + "\" }";
}

/*
 * helper functions for JSON parsing
 */
static void assertJSONDefined(const json &j, const string &name) {
    if (j == nullptr)
        throw std::invalid_argument(name + " is mandatory argument");
}

static void assertJSONStruct(const json &j, const string &name) {
    assertJSONDefined(j, name);
    if (!j.is_object())
        throw std::invalid_argument(name + " should be a structure");
}

static void assertJSONNumber(const json &j, const string &name) {
    assertJSONDefined(j, name);
    if (!j.is_number())
        throw std::invalid_argument(name + " should be an integer");
}

static void assertJSONString(const json &j, const string &name) {
    assertJSONDefined(j, name);
    if (!j.is_string())
        throw std::invalid_argument(name + " should be a string/enum");
}

static void assertJSONArray(const json &j, const string &name, int size = -1) {
    assertJSONDefined(j, name);
    if (!j.is_array())
        throw std::invalid_argument(name + " is an array argument");
    if (size >= 0 && ((j.size() != (unsigned)size)))
        throw std::invalid_argument(name + " array has " + to_string(size) + " members");
}

/**
 * Create JSON string from actual TacOperatingState
 *
 * @return resulting string
 */
static string TacOperatingStateString(EvHttpRequest&)
{
    ostringstream ss;

    TacOperatingState sTacOpState;
    tacGetOperatingState (&sTacOpState);

    json j = {
            {"wTX",17},
            {"eOperator","TAC_OP_JAUT"},
            {"eMainState","TAC_MS_SYNC_GSP"},
            {"ePIState",{"TAC_PI_ACTIVE","TAC_PI_NOT_ACTIVE"}},
            {"wSpNrActual",2},
            {"wSpNrRequested",3},
            {"eSpecialIntervention","TAC_SI_FIRE_3"},
            {"wGsp",90},
            {"eRequestedSync","TAC_RS_GSP"},
            {"wStageNr",5},
            {"wStageNrRequested",6},
            {"ePIStateRequested",{"TAC_PIRQ_OFF_DEFAULT","TAC_PIRQ_NOT_USED"}}
    };

    j["wTX"] = sTacOpState.wTX;
    j["eOperator"] = TacEnums::getTacOperator(sTacOpState.eOperator);
    j["eMainState"] = TacEnums::getTacMainState(sTacOpState.eMainState);
    {
        vector<string> pis;
        for(unsigned i=0; i < MAX_PARTIAL_INTERSECTIONS;i++)
        {
            pis.push_back(TacEnums::getTacPIState(sTacOpState.ePIState[i]));
        }
        j["ePIState"] = pis;
    }
    j["wSpNrActual"] = sTacOpState.wSpNrActual;
    j["wSpNrRequested"] = sTacOpState.wSpNrRequested;
    j["eSpecialIntervention"] = TacEnums::getTacSpecialIntervention(sTacOpState.eSpecialIntervention);
    j["wGsp"] = sTacOpState.wGsp;
    j["eRequestedSync"] = TacEnums::getTacRequestedSync(sTacOpState.eRequestedSync);
    j["wStageNr"] = sTacOpState.wStageNr;
    j["wStageNrRequested"] = sTacOpState.wStageNrRequested;
    {
        vector<string> pis;
        for(unsigned i=0; i < MAX_PARTIAL_INTERSECTIONS;i++)
        {
            pis.push_back(TacEnums::getTacPIRqState(sTacOpState.ePIStateRequested[i]));
        }
        j["ePIStateRequested"] = pis;
    }

    ss << j;

    return ss.str();
}


/**
 * Create JSON string from requested TaRunmode
 *
 * @return resulting string
 */
static string RequestTaRunmodeString(EvHttpRequest&)
{
    ostringstream ss;

    TaRunmode eTaRunmode;
    eTaRunmode = tacGetRequestTaRunmode();

    json j = {
            {"TaRunmode",TARM_OFF},
    };

    j["TaRunmode"] = TacEnums::getTaRunmode(eTaRunmode);

    ss << j;

    return ss.str();
}


/**
 * Create JSON string from modified requested TaRunmode
 *
 * @return resulting string
 */
static string ModifiedRequestTaRunmodeString(EvHttpRequest&)
{
    ostringstream ss;

    TaRunmode eTaRunmode;
    eTaRunmode = tacGetModifiedRequestTaRunmode();

    json j = {
            {"TaRunmode",TARM_OFF},
    };

    j["TaRunmode"] = TacEnums::getTaRunmode(eTaRunmode);

    ss << j;

    return ss.str();
}

/**
 * Create JSON string describing form for lock setting
 */
static string getFormTacSetLockSwitchOff(void) {
    json form;
    json enums = json::array();
    for (auto &it : TacEnums::getTacLockStateStrings()) {
        enums.push_back(it.second);
    }
    form["state"] = json::object();
    form["state"]["lockPI"] = json::array();
    int i;
    for (i = 0; i < MAX_PARTIAL_INTERSECTIONS; i++) {
        json value;
        value["type"] = "__t_enum";
        value["enums"] = enums;
        form["state"]["lockPI"].push_back(value);
    }
    return form.dump();
}

/**
 * Create JSON string describing form for setting tx
 */
static string getFormTacSetTX(void) {
    json form;
    json value;
    value["type"] = "__t_number";
    form["tx"] = value;
    return form.dump();
}

/**
 * Create JSON string describing form for inserting amli
 */
static string getFormTacInsertAmliEventToArchive(void) {
    json form;
    json value;
    json number;
    json dateTime;
    number["type"] = "__t_number";
    dateTime = json::object();
    dateTime["second"] = number;
    dateTime["minute"] = number;
    dateTime["hour"] = number;
    dateTime["day"] = number;
    dateTime["month"] = number;
    dateTime["year"] = number;
    dateTime["reserved"] = json::array();
    dateTime["reserved"][0] = number;
    dateTime["reserved"][1] = number;
    dateTime["reserved"][2] = number;
    form["dateTime"] = dateTime;

    json telegram = json::object();
    telegram["type"] = number;
    telegram["subType"] = number;
    telegram["reportingPointNo"] = number;
    telegram["lineNo"] = number;
    telegram["runNo"] = number;
    telegram["destinationNo"] = number;
    telegram["priority"] = number;
    telegram["vehicleLength"] = number;
    telegram["manualDirection"] = number;
    telegram["imbalance"] = number;
    telegram["reserved1"] = number;
    telegram["reserved2"] = number;
    telegram["reserved3"] = number;

    json source;
    json ride_event_source_enums = json::array();
    for (auto &it : TacEnums::getTacRideEventSourceStrings()) {
        ride_event_source_enums.push_back(it.second);
    }

    source["type"] = "__t_enum";
    source["enums"] = ride_event_source_enums;
    telegram["source"] = source;

    form["greenTime"] = number;
    form["ptModActive"] = number;
    form["tx"] = number;
    form["signalProgramNo"] = number;
    form["actualStage"] = number;
    form["nextStage"] = number;
    form["travelTime"] = number;
    form["txGNS"] = number;
    form["txGNE"] = number;
    form["amliTimeout"] = number;


    json type;
    json amli_event_type_enums = json::array();
    for (auto &it : TacEnums::getTacAmliEventTypeStrings()) {
        amli_event_type_enums.push_back(it.second);
    }

    type["type"] = "__t_enum";
    type["enums"] = amli_event_type_enums;
    form["type"] = type;

    json checkOutSubType;
    json amli_check_out_type_enums = json::array();
    for (auto &it : TacEnums::getTacAmliCheckOutTypeStrings()) {
        amli_check_out_type_enums.push_back(it.second);
    }

    checkOutSubType["type"] = "__t_enum";
    checkOutSubType["enums"] = amli_check_out_type_enums;
    form["checkOutSubType"] = checkOutSubType;
    form["signalGroupNumber"] = number;

    form["telegram"] = telegram;

    return form.dump();
}

/**
 * Create JSON string describing form for release control
 */
static string getFormTacReleaseControl(void) {
    return "{}";
}

static std::string encodeStatus(SLONG ret)
{
   switch(ret)
   {
   case SUCCESS:
	   return "{ \"status\" : \"SUCCESS\" }";
	   break;
   case NO_SUCCESS:
	   return "{ \"status\" : \"NO_SUCCESS\" }";
	   break;
   default:
	   return std::string("{ \"status\" : ") + to_string(ret) + " }";
   }
}

/**
 * Handler for setting switch off lock state
 *
 * @param JSON string in POST data representing the function argument
 */
static string TacSetLockSwitchOff(EvHttpRequest &ehr)
{
    TacLockSwitchOff lockState;

    if (ehr.getParam("form") != nullptr)
        return getFormTacSetLockSwitchOff();

    if (!ehr.isPost())
        return jsonNotAPostRequest(ehr);

    string postData = ehr.getPostData();

    json params;
    try {
        params = json::parse(postData);

        json state = params["state"];
        assertJSONStruct(state, "state");

        json lockPI = state["lockPI"];
        assertJSONArray(lockPI, "lockPI", MAX_PARTIAL_INTERSECTIONS);

        int i = 0;
        for (auto &it : lockPI) {
            assertJSONString(it, "lockPI member(s)");
            lockState.lockPI[i++] = TacEnums::getTacLockStateValue(it);
        }
    }
    catch(std::exception &e)
    {
        return jsonSyntaxError(e);
    }

    SLONG ret = tacSetLockSwitchOff(&lockState);

    return encodeStatus(ret); //"{ \"status\" : \"SUCCESS\" }";
}


/** this function is used to set the TX of the firmware player TODO: move to api_va.h
 *
 * @param tx as int in seconds
 * @return SUCCESS, NO_SUCCESS
 */
extern SLONG tacSetTX(int tx);


/**
 * Handler for setting tx
 *
 * @param JSON string in POST data representing the argument
 */
static string TacSetTX(EvHttpRequest &ehr) {

    if (ehr.getParam("form") != nullptr)
        return getFormTacSetTX();

    if (!ehr.isPost())
        return jsonNotAPostRequest(ehr);

    int tx;
    string postData = ehr.getPostData();

    json params;
    try {
        params = json::parse(postData);
        json txj = params["tx"];
        assertJSONNumber(txj, "tx");
        auto a = txj;
        tx = a;
    }
    catch(std::exception &e)
    {
        return jsonSyntaxError(e);
    }

    SLONG ret = tacSetTX(tx);

    return encodeStatus(ret); //"{ \"status\" : \"SUCCESS\" }";
}

/**
 * Handler for releasing control
 */
static string TacReleaseControl(EvHttpRequest &ehr)
{
    if (ehr.getParam("form") != nullptr)
        return getFormTacReleaseControl();

    if (!ehr.isPost())
        return jsonNotAPostRequest(ehr);

    SLONG ret = tacReleaseControl();

    return encodeStatus(ret); //"{ \"status\" : \"SUCCESS\" }";
}

/**
 * handler for inserting amli
 *
 * @param json string in post data representing the argument
 */
static string TacInsertAmliEventToArchive(EvHttpRequest &ehr) {

    if (ehr.getParam("form") != nullptr)
        return getFormTacInsertAmliEventToArchive();

    if (!ehr.isPost())
        return jsonNotAPostRequest(ehr);

    string postData = ehr.getPostData();

    TacAmliDto amli;

    try {
        json params = json::parse(postData);

        json type = params["type"];
        assertJSONString(type, "type");
        amli.type = TacEnums::getTacAmliEventTypeValue(type);

        json checkOutSubType = params["checkOutSubType"];
        assertJSONString(checkOutSubType, "checkOutSubType");
        amli.checkOutSubType = TacEnums::getTacAmliCheckOutTypeValue(checkOutSubType);

        params = json::parse(postData);
        json dateTime = params["dateTime"];
        assertJSONStruct(dateTime, "dateTime");
        vector<string> dateTimeMembers = { "second", "minute", "hour", "day", "month", "year" };
        for (auto & element : dateTimeMembers)
            assertJSONNumber(dateTime[element], element);

        amli.dateTime.second = dateTime["second"];
        amli.dateTime.minute = dateTime["minute"];
        amli.dateTime.hour = dateTime["hour"];
        amli.dateTime.day = dateTime["day"];
        amli.dateTime.month = dateTime["month"];
        amli.dateTime.year = dateTime["year"];

        json reserved = dateTime["reserved"];
        assertJSONArray(reserved, "reserved", 3);

        amli.dateTime.reserved[0] = reserved[0];

        int i = 0;
        for (auto &it : reserved) {
            assertJSONNumber(it, "reserved members");
            amli.dateTime.reserved[i] = it;
        }

        json telegram = params["telegram"];
        vector<string> telegramMembers = { "reportingPointNo", "lineNo", "runNo",
                    "destinationNo", "priority", "vehicleLength", "manualDirection", "imbalance",
                    "reserved1", "reserved2", "reserved3" };
        for (auto & element : telegramMembers)
            assertJSONNumber(telegram[element], element);

        amli.telegram.reportingPointNo = telegram["reportingPointNo"];
        amli.telegram.lineNo = telegram["lineNo"];
        amli.telegram.runNo = telegram["runNo"];
        amli.telegram.destinationNo = telegram["destinationNo"];
        amli.telegram.priority = telegram["priority"];
        amli.telegram.vehicleLength = telegram["vehicleLength"];
        amli.telegram.manualDirection = telegram["manualDirection"];
        amli.telegram.imbalance = telegram["imbalance"];
        amli.telegram.ptDirectionNo = telegram["ptDirectionNo"];
        assertJSONString(telegram["source"], "source");
        amli.telegram.source = TacEnums::getTacRideEventSourceValue(telegram["source"]);
        amli.telegram.reserved1 = telegram["reserved1"];
        amli.telegram.reserved2 = telegram["reserved2"];
        amli.telegram.reserved3 = telegram["reserved3"];

        vector<string> numMembers = { "greenTime", "ptModActive", "tx", "signalProgramNo",
                    "actualStage", "nextStage", "travelTime", "txGNS", "txGNE", "amliTimeout",
                    /*"ptDirectionNo"*/ };
        for (auto & element : numMembers)
            assertJSONNumber(params[element], element);


        amli.greenTime = params["greenTime"];
        amli.ptModActive = params["ptModActive"];
        amli.tx = params["tx"];
        amli.signalProgramNo = params["signalProgramNo"];
        amli.actualStage = params["actualStage"];
        amli.nextStage = params["nextStage"];
        amli.travelTime = params["travelTime"];
        amli.txGNS = params["txGNS"];
        amli.txGNE = params["txGNE"];
        amli.amliTimeout = params["amliTimeout"];
//        amli.ptDirectionNo = params["ptDirectionNo"];
    }
    catch(std::exception &e)
    {
        return jsonSyntaxError(e);
    }

    SLONG ret  = tacInsertAmliEventToArchive(&amli);

    return encodeStatus(ret); //"{ \"status\" : \"SUCCESS\" }";
}

/**
 * handler for inserting amli
 *
 * @param json string in post data representing the argument
 */
static string TacGetRideEvent(EvHttpRequest &)
{
    TacRideEvent ev;
    SLONG ret = tacGetRideEvent(&ev);

    string retStr;

    if (ret == SUCCESS) {
        json telegram = json::object();

        telegram["reportingPointNo"] = ev.reportingPointNo;
        telegram["lineNo"] = ev.lineNo;
        telegram["runNo"] = ev.runNo;
        telegram["destinationNo"] = ev.destinationNo;
        telegram["priority"] = ev.priority;
        telegram["vehicleLength"] = ev.vehicleLength;
        telegram["manualDirection"] = ev.manualDirection;
        telegram["imbalance"] = ev.imbalance;
        telegram["source"] = TacEnums::getTacRideEventSource(ev.source);
        telegram["reserved1"] = ev.reserved1;
        telegram["reserved2"] = ev.reserved2;
        telegram["reserved3"] = ev.reserved3;

        retStr = telegram.dump();
    } else {
        retStr = encodeStatus(ret);
    }

    return retStr;
}

static std::map<string, string> versionMap;
static bool invalidated = false;

SLONG setVersion (const char * key, const char * value)
{
	versionMap[key] =  value;
	invalidated = true;
	return SUCCESS;
}

bool TestAndDiagnosticsHttpRequestHandler::isVersionInvalidated()
{
  return invalidated;
}

void TestAndDiagnosticsHttpRequestHandler::setVersionInvalidated(bool inval)
{
  invalidated = inval;
}

/**
 * Create JSON string from versions
 *
 * @return resulting string
 */
static string TacVersionString(EvHttpRequest & )
{
	ostringstream ss;
	json j;
	for (std::map<string, string>::iterator it = versionMap.begin(); it != versionMap.end(); it++)
	{
		j[it->first] = it->second;
	}
	ss << j;
    return ss.str();
}

 string TestAndDiagnosticsHttpRequestHandler::getTacVersionsJSON()
{
  EvHttpRequest * dummy = 0;
  return TacVersionString(*dummy);
}


 string TestAndDiagnosticsHttpRequestHandler::getTacVersionsHTML()
{
	ostringstream ss;
	for (std::map<string, string>::iterator it = versionMap.begin(); it != versionMap.end(); it++)
	{
	  ss << "<BR>";

	  // if( it->first ...) TODO filter not wished keys
	  ss << it->first;
	  ss << ": ";
	  if(it->second.find("version ") == 0)
		  ss << (it->second).substr(8);
	  else
		  ss << it->second;

	}
	return ss.str();
}

/**
 * Create JSON string describing form for setting tx
 */
static string getFormTacInjectReportEntry(void) {
    json form;
    json value;
    value["type"] = "__t_string";
    form["text"] = value;
    return form.dump();
}


/**
 * Handler for injecting Report entries e.g. from autoregression test
 *
 * @param JSON string in POST data representing the argument
 */
static string TacInjectReportEntry(EvHttpRequest &ehr) {

    if (ehr.getParam("form") != nullptr)
        return getFormTacInjectReportEntry();

    if (!ehr.isPost())
        return jsonNotAPostRequest(ehr);

    string text;
    string postData = ehr.getPostData();

    json params;
    try {
        params = json::parse(postData);

        json textj = params["text"];
        assertJSONString(textj, "text");

        auto a = textj;
        text = a;
    }
    catch(std::exception &e)
    {
    	string syntaxError = jsonSyntaxError(e);
    	Report(AUTOREGR_ERROR, "%s\n", (syntaxError + " in: " + postData).c_str());
        return syntaxError;
    }

    Report(AUTOREGR_EVENT, "%s\n", text.c_str());

    return TestAndDiagnosticsHttpRequestHandler::getTacVersionsJSON();
}

//-----------------------------------------------------------------------------------
static string DEMO_KEY = "DEMO";
const char* TestAndDiagnosticsHttpRequestHandler::getDemoKey()
{
  return DEMO_KEY.c_str();
}

bool TestAndDiagnosticsHttpRequestHandler::isDemoVersion()
{
   std::map<string, string>::const_iterator i = versionMap.find(DEMO_KEY);
   if (i != versionMap.end())
        return true;

   ReportC(TAC_DEBUG), "isDemoVersion: Key %s not found in %s", DEMO_KEY.c_str(), getTacVersionsJSON().c_str());

   return false;
}

