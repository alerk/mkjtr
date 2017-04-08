#include <util/CControlEnums.h>
#include <list>
#include "tac_kernel.h"


#include "util/what.h"
DEFINE_WHATINFO_HEADER;

/* max number telegrams in pending queue (these are telegrams which will be
 * picked up by TA or sent to firmware) */
#define MAX_TELEGRAMS   20

/* length of archive list for debugging */
#define MAX_ARCHIVED_TELEGRAMS 20

namespace sitraffic {

enum TelegramState {
    PENDING,
    PROCESSED,
    OVERFLOWED
};

struct TelegramStats {
    long amlisRecvd;
    long amlisSent;
    long amlisOflowed;
    long ridesRecvd;
    long ridesSent;
    long ridesOflowed;
   TelegramStats() : amlisRecvd(0), amlisSent(0), amlisOflowed(0), ridesRecvd(0), ridesSent(0), ridesOflowed(0) {};
};

template <typename PT>
struct TelegramCont {
    const PT *telegram;
    enum TelegramState state;
    int64_t tickReceived;
    int64_t tickSent;
    TelegramCont(const PT *t, TelegramState s, long r) : telegram(t), state(s), tickReceived(r), tickSent(0) {};
    TelegramCont(const TelegramCont<PT>&) = default;
    TelegramCont &operator=(const TelegramCont<PT>&) = default;
    ~TelegramCont() {}
};

class PublicTransport {
    std::list<TelegramCont<TacAmliDto>> amliBuffer;
    std::list<TelegramCont<TacRideEvent>> ptTelegramBuffer;

    std::list<TelegramCont<TacAmliDto>> amliArchive;
    std::list<TelegramCont<TacRideEvent>> ptTelegramArchive;

    /* To prevent httpHandler which runs in separate thread from crashing
     * tacKernel we use memory pools of datagrams. Otherwise it could happen
     * that WebGUI thread has private copy of pointer to already freed data.
     * Instead of dealocating datagram is returned to the pool to be used later. */
    std::list<const TacAmliDto*> amliMemPool;
    std::list<const TacRideEvent*> rideEventMemPool;

    const TacAmliDto *poolNewTacAmliDto(const TacAmliDto *original);
    const TacRideEvent *poolNewTacRideEvent(const TacRideEvent *original);

public:
    TelegramStats stats;
    PublicTransport() : amliBuffer{}, ptTelegramBuffer{}, amliArchive{},
        ptTelegramArchive{}, amliMemPool{}, rideEventMemPool{}, stats{} {}
    void addAmliTelegram(const TacAmliDto *amli);
    const TacAmliDto &firstAmli();
    void popAmliTelegram();
    long amliTelegramCount();
    void archiveAmliTelegram(TelegramCont<TacAmliDto>);
    std::list<TelegramCont<TacAmliDto>>::reverse_iterator getAmliIteratorBegin() {
        return amliBuffer.rbegin();
    }
    std::list<TelegramCont<TacAmliDto>>::reverse_iterator getAmliArchivedIteratorBegin() {
        return amliArchive.rbegin();
    }
    std::list<TelegramCont<TacAmliDto>>::reverse_iterator getAmliIteratorEnd() {
        return amliBuffer.rend();
    }
    std::list<TelegramCont<TacAmliDto>>::reverse_iterator getAmliArchivedIteratorEnd() {
        return amliArchive.rend();
    }

    void addPtTelegram(const TacRideEvent *pt);
    const TacRideEvent &firstPtTelegram();
    void popPtTelegram();
    long ptTelegramCount();
    void archivePtTelegram(TelegramCont<TacRideEvent>);
    std::list<TelegramCont<TacRideEvent>>::reverse_iterator getRideEventIteratorBegin() {
        return ptTelegramBuffer.rbegin();
    }
    std::list<TelegramCont<TacRideEvent>>::reverse_iterator getRideEventArchivedIteratorBegin() {
        return ptTelegramArchive.rbegin();
    }
    std::list<TelegramCont<TacRideEvent>>::reverse_iterator getRideEventIteratorEnd() {
        return ptTelegramBuffer.rend();
    }
    std::list<TelegramCont<TacRideEvent>>::reverse_iterator getRideEventArchivedIteratorEnd() {
        return ptTelegramArchive.rend();
    }

    SLONG sendAmlis();

    static std::string tacPtTelegramString(const TacRideEvent *telegram, bool verbose=true);
    static std::string amliDtoString(const AmliDtoCctrl *amliDto, bool verbose=true);
    static std::string tacAmliDtoString(const TacAmliDto *amli, bool verbose=true);
};

}
