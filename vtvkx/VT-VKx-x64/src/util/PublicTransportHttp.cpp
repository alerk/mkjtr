#include <sstream>

#include <string>
#include <iomanip>
#include "util/what.h"
#include "instregistry.h"
#include "cctrl/htmlHandler.h"
#include "cctrl/PublicTransport.h"
#include "util/TacEnums.h"

#include "PublicTransportHttp.h"
DEFINE_WHATINFO;

using namespace sitraffic;
using namespace tackernel;
using namespace std;

static string telegramStateString(TelegramState state)
{
    string str;
    switch (state) {
        case TelegramState::PENDING:
            str = "RECVD";
            break;
        case TelegramState::PROCESSED:
            str = "SENT";
            break;
        case TelegramState::OVERFLOWED:
            str = "OFLOW";
            break;
        default:
            str = "UNKNOWN";
    }
    return str;
}

static void addAmliTelegramRow(TableHelper &t, TelegramCont<TacAmliDto> &cont)
{
    const TacAmliDto *amli = cont.telegram;
    const TacRideEvent *telegram = &amli->telegram;
    t.startRow();
    t.addItem(cont.tickReceived);
    t.addItem(cont.tickSent);
    t.addItem(telegramStateString(cont.state));

    t.addItem(telegram->reportingPointNo);
    t.addItem(telegram->lineNo);
    t.addItem(telegram->destinationNo);
    t.addItem(telegram->runNo);
    t.addItem(telegram->priority);

    t.addItem(telegram->vehicleLength);
    t.addItem(telegram->manualDirection);
    t.addItem(telegram->imbalance);
    t.addItem(telegram->ptDirectionNo);
    t.addItem(TacEnums::getTacRideEventSource(telegram->source));

    ostringstream ss;
    ss << amli->dateTime.year+1900 << "-" << amli->dateTime.month+1 << "-" <<  amli->dateTime.day
        << " " <<
        setfill('0') << setw(2)
        << amli->dateTime.hour << ":" << amli->dateTime.minute << ":" << amli->dateTime.second;
    t.addItem(ss.str());

    t.addItem(amli->greenTime);
    t.addItem((int)amli->ptModActive);
   // t.addItem((double)amli->tx);
    char txs[10];
    snprintf(txs, sizeof(txs), "%1.1f", ((double)amli->tx / 1000.0));
    t.addItem(txs);
    //sstx << setprecision(4) << ((double)amli->tx / 1000.0);
    t.addItem(amli->signalProgramNo);
    t.addItem(amli->actualStage);
    t.addItem(amli->nextStage);
    t.addItem(amli->travelTime);
    t.addItem(amli->txGNS);
    t.addItem(amli->txGNE);
    t.addItem(amli->amliTimeout);

    t.addItem(TacEnums::getTacAmliEventType(amli->type));
    t.addItem(TacEnums::getTacAmliCheckOutType(amli->checkOutSubType));

    t.addItem(amli->signalGroupNo);

    t.endRow();
}

static void addRideEventRow(TableHelper &t, TelegramCont<TacRideEvent> &cont)
{
    const TacRideEvent *telegram = cont.telegram;
    t.startRow();
    t.addItem(cont.tickReceived);
    t.addItem(cont.tickSent);
    t.addItem(telegramStateString(cont.state));
    t.addItem(telegram->reportingPointNo);
    t.addItem(telegram->lineNo);
    t.addItem(telegram->destinationNo);
    t.addItem(telegram->runNo);
    t.addItem(telegram->priority);

    t.addItem(telegram->vehicleLength);
    t.addItem(telegram->manualDirection);
    t.addItem(telegram->imbalance);
    t.addItem(telegram->ptDirectionNo);
    t.addItem(TacEnums::getTacRideEventSource(telegram->source));
    t.endRow();
}

string PublicTransportHttpRequestHandler::doRequest(EvHttpRequest &)
{
    PublicTransport &pt = InstRegistry::Instance().getPublicTransport();

    ostringstream ss;
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
)*eNd*";

    TableHelper t;
    t.appendHeader("Ride events", "rideevents");
    t.startTitle();
    t.addTitleColumn("rcvd");
    t.addTitleColumn("sent");
    t.addTitleColumn("state");
    t.addTitleColumn("repPointNum").addTitleColumn("line");
    t.addTitleColumn("dest").addTitleColumn("run");
    t.addTitleColumn("priority").addTitleColumn("vehLen");
    t.addTitleColumn("manDir").addTitleColumn("imbal");
    t.addTitleColumn("ptDir").addTitleColumn("source");

    for (std::list<TelegramCont<TacRideEvent>>::reverse_iterator it = pt.getRideEventIteratorBegin();
            it != pt.getRideEventIteratorEnd(); ++it) {
        TelegramCont<TacRideEvent> reCont = *it;
        addRideEventRow(t, reCont);
    }

    for (std::list<TelegramCont<TacRideEvent>>::reverse_iterator it = pt.getRideEventArchivedIteratorBegin();
            it != pt.getRideEventArchivedIteratorEnd(); ++it) {
        TelegramCont<TacRideEvent> reArch = *it;
        addRideEventRow(t, reArch);
    }

    t.endTable();
    ss << t.toString();

    TableHelper tabAmli;
    tabAmli.appendHeader("Amli telegrams", "amlis");
    tabAmli.startTitle();
    tabAmli.addTitleColumn("rcvd");
    tabAmli.addTitleColumn("sent");
    tabAmli.addTitleColumn("state");

    tabAmli.addTitleColumn("RP").addTitleColumn("line");
    tabAmli.addTitleColumn("dest").addTitleColumn("run");
    tabAmli.addTitleColumn("prio").addTitleColumn("vehLen");
    tabAmli.addTitleColumn("manDir").addTitleColumn("imbal");
    tabAmli.addTitleColumn("ptDir").addTitleColumn("source");

    tabAmli.addTitleColumn("date and time");
    tabAmli.addTitleColumn("greenTime");
    tabAmli.addTitleColumn("ptMode");
    tabAmli.addTitleColumn("TX [s]");
    tabAmli.addTitleColumn("SP"); // signal program
    tabAmli.addTitleColumn("actStg");
    tabAmli.addTitleColumn("nxtStg");
    tabAmli.addTitleColumn("TT"); // TravelTime
    tabAmli.addTitleColumn("GNS");
    tabAmli.addTitleColumn("GNE");
    tabAmli.addTitleColumn("Timeout");
    tabAmli.addTitleColumn("eventType");
    tabAmli.addTitleColumn("coType");
    tabAmli.addTitleColumn("SG"); // signal group

    for (std::list<TelegramCont<TacAmliDto>>::reverse_iterator it = pt.getAmliIteratorBegin();
            it != pt.getAmliIteratorEnd(); ++it) {
        TelegramCont<TacAmliDto> amliCont = *it;
        addAmliTelegramRow(tabAmli, amliCont);
    }

    for (std::list<TelegramCont<TacAmliDto>>::reverse_iterator it = pt.getAmliArchivedIteratorBegin();
            it != pt.getAmliArchivedIteratorEnd(); ++it) {
        TelegramCont<TacAmliDto> amliArch = *it;
        addAmliTelegramRow(tabAmli, amliArch);
    }

    tabAmli.endTable();
    ss << tabAmli.toString();

    TableHelper tabStats;
    tabStats.appendHeader("Telegram statistic", "stats");
    tabStats.startTitle();
    tabStats    .addTitleColumn("type")           .addTitleColumn("sent")     .addTitleColumn("received")  .addTitleColumn("overflowed");
    tabStats.startRow().addItem("Ride events")   .addItem(pt.stats.ridesSent).addItem(pt.stats.ridesRecvd).addItem(pt.stats.ridesOflowed).endRow();
    tabStats.startRow().addItem("Amli telegrams").addItem(pt.stats.amlisSent).addItem(pt.stats.amlisRecvd).addItem(pt.stats.amlisOflowed).endRow();

    tabStats.endTable();
    ss << tabStats.toString();

    ss <<  R"*eNd*(
</table>
</body>
</html>
)*eNd*";

    return ss.str();
}

string PublicTransportHttpRequestHandler::getPath()
{
    ostringstream ss;
    ss << "/" << PublicTransportHttpRequestHandler::NAME;
    return ss.str();
}

void PublicTransportHttpRequestHandler::doRequestFormatSetup(EvHttpRequest & ehr)
{
    ehr.setFormat(HttpReplyFormat::PLAIN);
}
