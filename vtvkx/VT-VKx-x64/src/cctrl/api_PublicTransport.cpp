/**
 * @file api_PublicTransport.cpp
 * Public transport handling module (OEV-Speicher)
 */

#include <map>
#include <sstream>
#include <queue>
#include <ctime>

#include "start/awpWrapper.h"
#include "instregistry.h"
#include "CControl.h"
#include "util/repdef.h"
#include "cctrl/wrappedCctrlDtos.h"

#include "PublicTransport.h"

#include "util/TacEnums.h"

#include <util/CControlEnums.h>

#include "start/tac_time.h"

#include "util/what.h"
DEFINE_WHATINFO;

using namespace std;
using namespace tackernel;
using namespace sitraffic;

static RideEventSourceCctrl tacRideEventSourceToCctrl(TacRideEventSource eventSource)
{
    switch (eventSource) {
        case TAC_RES_AFD:
            return RESC_AFD;
        case TAC_RES_Det:
            return RESC_Det;
        case TAC_RES_ITCS:
            return RESC_ITCS;
        case TAC_RES_RideManager:
            return RESC_RideManager;
        case TAC_RES_SimGui:
            return RESC_SimGui;
        case TAC_RES_Ta:
            return RESC_Ta;
        case TAC_RES_unknown:
            return RESC_unknown;
        default:
            Report(PT_ERROR, "unknown TacRideEventSource used %d", eventSource);
            return RESC_unknown;
    }
}

static AmliEventTypeCctrl tacAmliEventTypeToCctrl(TacAmliEventType eventType)
{
    switch (eventType) {
        case TAC_AET_CheckIn:
            return AET_CheckIn;
        case TAC_AET_CheckOutRaw:
            return AET_CheckOutRaw;
        case TAC_AET_CheckOutAmli:
            return AET_CheckOutAmli;
        case TAC_AET_EmergencyCheckIn:
            return AET_EmergencyCheckIn;
        case TAC_AET_ForcedCheckOut:
            return AET_ForcedCheckOut;
        case TAC_AET_Blocked:
            return AET_Blocked;
        case TAC_AET_unknown:
            return AET_unknown;
        default:
            Report(PT_ERROR, "unknown TacAmliEventType used %d", eventType);
            return AET_unknown;
    }
}

static AmliCheckOutTypeCctrl tacAmliCheckOutTypeToCctrl(TacAmliCheckOutType coType)
{
    switch (coType) {
        case TAC_ACOR_CheckIn:
            return ACOR_CheckIn;
        case TAC_ACOR_OK:
            return ACOR_OK;
        case TAC_ACOR_TooLate:
            return ACOR_TooLate;
        case TAC_ACOR_Off:
            return ACOR_Off;
        case TAC_ACOR_SwitchOnOk:
            return ACOR_SwitchOnOk;
        case TAC_ACOR_SwitchOnNA:
            return ACOR_SwitchOnNA;
        case TAC_ACOR_NotAffected:
            return ACOR_NotAffected;
        case TAC_ACOR_StageExtended:
            return ACOR_StageExtended;
        case TAC_ACOR_CheckOutByNextNA:
            return ACOR_CheckOutByNextNA;
        case TAC_ACOR_StageUnmodified:
            return ACOR_StageUnmodified;
        case TAC_ACOR_CheckOutByNext:
            return ACOR_CheckOutByNext;
        case TAC_ACOR_unknown:
            return ACOR_unknown;
        default:
			Report(PT_ERROR, "unknown TacAmliCheckOutType used %d", coType);
			return ACOR_unknown;
    }
};

static int64_t getCurrentOmcTick()
{
    SignalIndicationsWCctrl & si = InstRegistry::Instance().getSi();
    return si.baseTicktime;
}


void PublicTransport::addAmliTelegram(const TacAmliDto *amli)
{
    if (amliBuffer.size() == MAX_TELEGRAMS)
    {
        TelegramCont<TacAmliDto> t = amliBuffer.front();
        t.state = TelegramState::OVERFLOWED;
        archiveAmliTelegram(t);
        amliBuffer.pop_front();
        stats.amlisOflowed++;
    }
    // create our own copy since amli pointer is owned by TA
    amliBuffer.emplace_back(poolNewTacAmliDto(amli), TelegramState::PENDING, getCurrentOmcTick());
    stats.amlisRecvd++;
    if(isReportEnabledFor(PT_DEBUG))
      Report(PT_EVENT, "%s", tacAmliDtoString(amli, true).c_str());
    else
      Report(PT_EVENT&~R_FILI, "%s", tacAmliDtoString(amli, false).c_str());
}

const TacAmliDto &PublicTransport::firstAmli()
{
    Report(PT_EVENT, "%s", "called");
    return *amliBuffer.front().telegram;
}

void PublicTransport::popAmliTelegram()
{
    TelegramCont<TacAmliDto> amli = amliBuffer.front();
    amli.state = TelegramState::PROCESSED;
    amli.tickSent = getCurrentOmcTick();
    archiveAmliTelegram(amli);
    amliBuffer.pop_front();
    stats.amlisSent++;
}

void PublicTransport::archiveAmliTelegram(TelegramCont<TacAmliDto> telegram)
{
    if (amliArchive.size() == MAX_ARCHIVED_TELEGRAMS) {
        amliMemPool.push_back(amliArchive.front().telegram);
        amliArchive.pop_front();
    }

    amliArchive.push_back(telegram);
    //Report(PT_EVENT, "%s", tacAmliDtoString(amli).c_str());
}

void PublicTransport::addPtTelegram(const TacRideEvent *pt)
{
    if(isReportEnabledFor(PT_DEBUG))
      Report(PT_EVENT, "%s", tacPtTelegramString(pt, true).c_str());
    else
      Report(PT_EVENT&~R_FILI, "%s", tacPtTelegramString(pt, false).c_str());

    if (ptTelegramBuffer.size() == MAX_TELEGRAMS) {
        TelegramCont<TacRideEvent> re = ptTelegramBuffer.front();
        re.state = TelegramState::OVERFLOWED;
        archivePtTelegram(re);
        ptTelegramBuffer.pop_front();
        stats.ridesOflowed++;
    }
    ptTelegramBuffer.emplace_back(poolNewTacRideEvent(pt), TelegramState::PENDING, getCurrentOmcTick());
    stats.ridesRecvd++;
}

const TacRideEvent &PublicTransport::firstPtTelegram()
{
    return *ptTelegramBuffer.front().telegram;
}

void PublicTransport::popPtTelegram()
{
    TelegramCont<TacRideEvent> re = ptTelegramBuffer.front();
    re.state = TelegramState::PROCESSED;
    re.tickSent = getCurrentOmcTick();
    archivePtTelegram(re);
    ptTelegramBuffer.pop_front();
    stats.ridesSent++;
}

void PublicTransport::archivePtTelegram(TelegramCont<TacRideEvent> telegram)
{
    if (ptTelegramArchive.size() == MAX_ARCHIVED_TELEGRAMS)
    {
        rideEventMemPool.push_back(ptTelegramArchive.front().telegram);
        ptTelegramArchive.pop_front();
    }

    ptTelegramArchive.push_back(telegram);
    //Report(PT_EVENT, "%s", tacAmliDtoString(amli).c_str());
}

long PublicTransport::amliTelegramCount()
{
    return static_cast<long>(amliBuffer.size());
}

long PublicTransport::ptTelegramCount()
{
    return static_cast<long>(ptTelegramBuffer.size());
}

const TacAmliDto *PublicTransport::poolNewTacAmliDto(const TacAmliDto *original)
{
    TacAmliDto *newAmli;
    if (amliMemPool.size() > 0)
    {
        newAmli = const_cast<TacAmliDto *>(amliMemPool.front());
        *newAmli = *original;
        amliMemPool.pop_front();
    }
    else
    {
        newAmli = new TacAmliDto(*original);
    }
    return newAmli;
}

const TacRideEvent *PublicTransport::poolNewTacRideEvent(const TacRideEvent *original)
{
    TacRideEvent *newRe;
    if (rideEventMemPool.size() > 0)
    {
        newRe = const_cast<TacRideEvent *>(rideEventMemPool.front());
        *newRe = *original;
        rideEventMemPool.pop_front();
    }
    else
    {
        newRe = new TacRideEvent(*original);
    }
    return newRe;
}

bool_t booleanToBool(BOOLEAN value)
{
    switch(value)
    {
    case TRUE:
        return TRUE;
    case FALSE:
        return FALSE;
    default:
        return FALSE;
    }
}

/* Insert Extended AMLI event to Firmware archive */
SLONG tacInsertAmliEventToArchive(TacAmliDto *amli)
{
    PublicTransport &pt = InstRegistry::Instance().getPublicTransport();
    pt.addAmliTelegram(amli);
    return SUCCESS;
}

SLONG tacGetRideEvent(TacRideEvent *telegram)
{
    PublicTransport &pt = InstRegistry::Instance().getPublicTransport();
    if (pt.ptTelegramCount() == 0)
        return NO_SUCCESS;

    *telegram = pt.firstPtTelegram();
    pt.popPtTelegram();

    return SUCCESS;
}

SLONG PublicTransport::sendAmlis()
{
    PublicTransport &pt = *this;

    int amliCount = (int)pt.amliTelegramCount();
    if(amliCount <= 0)
    	 return SUCCESS;

    CLIENT *clnt=sitraffic::InstRegistry::Instance().getAwpWrapper().getFwUdpClient();
     if(!clnt)
         return NO_SUCCESS;

    enum RetCodeCctrl clnt_res = RetCodeCctrl();


    AmliDtosCctrl amlis;

    amlis.AmliDtosCctrl_len = amliCount;
    amlis.AmliDtosCctrl_val = new AmliDtoCctrl[amliCount];

    list<TelegramCont<TacAmliDto>>::iterator it = amliBuffer.begin();
    for (int i = 0; it != amliBuffer.end(); it++, i++)
    {
        const TacAmliDto &amli = *it->telegram;

        AmliDtoCctrl *amliCc = &amlis.AmliDtosCctrl_val[i];
        {
            ArchiveKeysCctrl * const keysCc = &amliCc->keys;
            keysCc->timeStamp = 0;
            keysCc->tickTimeOfLastChange = 0;
            keysCc->primaryKey = tacDateTime2OmcTicks(&amli.dateTime);
        }
        {
            RideEventCctrl * event = &amliCc->event;
            const TacRideEvent * telegram  = &amli.telegram;
            event->source = tacRideEventSourceToCctrl(telegram->source);

            event->rpNo = telegram->reportingPointNo;
            event->lineNo = telegram->lineNo;
            event->destinationNo = telegram->destinationNo;
            event->runNo = telegram->runNo;

            event->attrs.attrs_val = new R09AttributeCctrl[5];
            event->attrs.attrs_len = 4;
            event->attrs.attrs_val[0].selector = R09_Imbalance;
            event->attrs.attrs_val[0].value = telegram->imbalance;
            event->attrs.attrs_val[1].selector = R09_Priority;
            event->attrs.attrs_val[1].value = telegram->priority;
            event->attrs.attrs_val[2].selector = R09_ManDir;
            event->attrs.attrs_val[2].value = telegram->manualDirection;
            event->attrs.attrs_val[3].selector = R09_VehicleLen;
            event->attrs.attrs_val[3].value = telegram->vehicleLength;
            if(amli.signalGroupNo > 0)
			  {
				event->attrs.attrs_val[event->attrs.attrs_len].selector = R09_SignalGroup;
				event->attrs.attrs_val[event->attrs.attrs_len].value = amli.signalGroupNo;
				event->attrs.attrs_len++;
			  }

        }

        amliCc->greenTime = amli.greenTime;
        amliCc->ptModActive = booleanToBool(amli.ptModActive);
        amliCc->tx = amli.tx;
        amliCc->spNo = amli.signalProgramNo;
        amliCc->actStage = amli.actualStage;
        amliCc->nextStage = amli.nextStage;
        amliCc->travelTime = amli.travelTime;
        amliCc->txGNS = amli.txGNS;
        amliCc->txGNE = amli.txGNE;
        amliCc->amliTimeout = amli.amliTimeout;
        amliCc->ptDirNo = amli.telegram.ptDirectionNo;//amli.ptDirectionNo;
        amliCc->type = tacAmliEventTypeToCctrl(amli.type);
        amliCc->coType = tacAmliCheckOutTypeToCctrl(amli.checkOutSubType);

        if(amliBuffer.size() == 1)
           Report(PT_EVENT&~R_FILI, "%s", amliDtoString(amliCc, isReportEnabledFor(PT_DEBUG)).c_str());  // normal case: shorten line
        else
           Report(PT_EVENT, "amli[%d]: %s", i, amliDtoString(amliCc, isReportEnabledFor(PT_DEBUG)).c_str());

    }

    SLONG ret = NO_SUCCESS;
    clnt_stat stat = processedamli_8(amlis, &clnt_res, clnt);
    if (stat != RPC_SUCCESS || clnt_res != RC_OK)
    {
        Report(PT_ERROR, "processedamli_8(%p)=%d=%s, %d", &amlis, stat, clnt_sperror(clnt, ""), clnt_res);
    }
    else
    {
        ret = SUCCESS;
        // if the call was successful we can clear the amli lists
        for (int i = 0; i < amliCount; i++)
            pt.popAmliTelegram();
    }

    for (int i = 0; i < amliCount; i++)
        delete [] amlis.AmliDtosCctrl_val[i].event.attrs.attrs_val;

    delete [] amlis.AmliDtosCctrl_val;

    return ret;
}


string PublicTransport::tacPtTelegramString(const TacRideEvent *telegram, bool verbose)
{
    ostringstream ss;

    ss << "{";
    ss << "rp:" << telegram->reportingPointNo;
    ss << ", line:" << telegram->lineNo;
    ss << ", dest:" << telegram->destinationNo;
    ss << ", run:" << telegram->runNo;
    ss << ", ptDir:" << telegram->ptDirectionNo;
    if(verbose)
    {
		ss << ", prio:" << telegram->priority;
		ss << ", vehLen:" << telegram->vehicleLength;
		ss << ", manDir:" << telegram->manualDirection;
		ss << ", imbal:" << telegram->imbalance;
    }
    ss << "}";

    return ss.str();
}

string PublicTransport::tacAmliDtoString(const TacAmliDto *amli, bool verbose)
{
    ostringstream ss;

    ss << "{";
    ss << "" << amli->dateTime.year+1900 << "-" << amli->dateTime.month+1 << "-" <<  amli->dateTime.day;
    ss << " " << amli->dateTime.hour << ":" << amli->dateTime.minute << ":" << amli->dateTime.second;

    ss << ": " << tacPtTelegramString(&amli->telegram, false);

//    ss << ", dateTime: { seconds: " << amli->dateTime.second;
//    ss << ", minute: " << amli->dateTime.minute;
//    ss << ", hour: " << amli->dateTime.hour;
//    ss << ", day: " << amli->dateTime.day;
//    ss << ", month: " << amli->dateTime.month;
//    ss << ", year: " << amli->dateTime.year;

//    ss << ", PtDir:" << amli->ptDirectionNo;
    ss << ", TT:" << amli->travelTime;
    ss << ", GNS:" << amli->txGNS;
    ss << ", GNE:" << amli->txGNE;
    ss << ", tx:" << amli->tx;
    if(verbose)
     {
		ss << ", sp:" << amli->signalProgramNo;
		ss << ", stage:" << amli->actualStage;
		ss << ", nextStg:" << amli->nextStage;
		ss << ", greenTime:" << amli->greenTime;
		ss << ", ptMod:" << (int)amli->ptModActive;
		ss << ", amliTimeout:" << amli->amliTimeout;
     }
    ss << ", sg:" << amli->signalGroupNo;
    ss << ", type:" << TacEnums::getTacAmliEventType(amli->type);
    ss << ", coType:" << TacEnums::getTacAmliCheckOutType(amli->checkOutSubType);
    ss << ", source:" << TacEnums::getTacRideEventSource(amli->telegram.source);
    ss << ", res:[" << amli->dateTime.reserved[0] <<  "," << amli->dateTime.reserved[1] << "," << amli->dateTime.reserved[2] << "] },";
    ss << "}";

    return ss.str();
}




string PublicTransport::amliDtoString(const AmliDtoCctrl *amliDto, bool verbose)
{
    const AmliDtoCctrl *amliCc = amliDto;
    ostringstream ss;

    ss << "{";
    {
        const ArchiveKeysCctrl * keysCc = &amliCc->keys;
        //ss << "{";
        // ss << " time:" << keysCc->timeStamp;
        // ss << ", tick:" << keysCc->tickTimeOfLastChange;
        ss << "key:" << keysCc->primaryKey;
    }
    {
        const RideEventCctrl * ride = &amliCc->event;
        ss << ", ride:{";
        std::string src = CControlEnums::getRideEventSourceCctrl(ride->source);
        if(src.length() >= 5)
          ss << "src:" << src.substr(4);
        else
          ss << "src:" << src;
        ss << ", rp:" << ride->rpNo;
        ss << ", line:" << ride->lineNo;
        ss << ", dest:" << ride->destinationNo;
        ss << ", run:" << ride->runNo;
        ss << "}";

       // ss << ", attrs:[";
        //for(unsigned i = 0; i<ride->attrs.attrs_len; i++)
        //{
//            ss << ", attr[" << i << "]: {";
//            ss << CControlEnums::getR09AttrSelCctrl(ride->attrs.attrs_val[i].selector);
//            ss << ":";
//            ss << ride->attrs.attrs_val[i].value;
//            ss << ", ";
//        }
//        ss << "]";
    }
    ss << ", PtDir:" << amliCc->ptDirNo;
    ss << ", TT:" << amliCc->travelTime;
    ss << ", GNS:" << amliCc->txGNS;
    ss << ", GNE:" << amliCc->txGNE;
    ss << ", tx:" << amliCc->tx;
    if(verbose)
     {
		ss << ", actStage:" << amliCc->actStage;
		ss << ", nextStage:" << amliCc->nextStage;
		ss << ", timeout:" << amliCc->amliTimeout;
		ss << ", sp:" << amliCc->spNo;
		ss << ", greenTime:" << amliCc->greenTime ;
		ss << ", ptMod:" << amliCc->ptModActive;
     }
    ss << ", type:" << CControlEnums::getAmliEventTypeCctrl(amliCc->type);
    ss << ", coTyp:" << CControlEnums::getAmliCheckOutTypeCctrl(amliCc->coType);
    ss << "}";

    return ss.str();
}
