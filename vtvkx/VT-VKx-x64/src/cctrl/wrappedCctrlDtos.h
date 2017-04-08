/*
 * define wrapper for c-control data transfer objects as needed to provide additional methods.
 */
#ifndef WRAPPEDCCTRLDTOS_H_
#define WRAPPEDCCTRLDTOS_H_

#include <map>
#include "CControl.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
  {
    class RequestedStatusWCctrl : public RequestedStatusCctrl
      {
        typedef RequestedStatusCctrl basetype;
      public:
        basetype & base()
          {
            return *(reinterpret_cast<basetype*>(this));
          }

        /*
         * @return true if
         *        main intersection is requested ON,
         *     && a requested spl is provided and
         *     && at least one partial intersection is ON requested.
         */
        bool isAtleastOnePiOn() const;
        /*
         * @return address of first modification with modifications.modType==mod or NULL if none found
         */
        const TcTTVModOnOffStateCctrl * getMod(RequestedStatusModNoCctrl mod) const;
        /*
         * @return bitfield, 2^0 ModTD on, 2^1 mod public transport, 2^2 mod individual traffic
         */
        unsigned getModBits() const;

      };

    class ActualStatusWCctrl : public ActualStatusCctrl
      {
        typedef ActualStatusCctrl basetype;
      public:
        basetype & base()
          {
            return *(reinterpret_cast<basetype*>(this));
          }

        /* @return first modifikation with mod found in as.modifications */
        const TTVModOnOffStateCctrl * getMod(TypeOfModificationCctrl mod) const;

        /* @return bitfield, 2^0 ModTD on, 2^1 mod public transport, 2^2 mod individual traffic */
        unsigned getModBits() const;

        /** @return bitfield, 2^1 is set if partial intersection 1 is ON,
         *                    2^2 is set if partial intersection 2 is ON... */
        unsigned getOnPiMask() const;
        /** check if partial intersection with number subintersectionNumber is marked on inside onPiMask.
         * @onPiMask bitfield as received from getModBits()
         * @subintersectionNumber number of partial intersection 1..4
         * @return true if partial intersection with number subintersectionNumber is marked ON inside onPiMask
         */
        static bool isPiOn(unsigned onPiMask, int subintersectionNumber);
      };

    class SignalIndicationWCctrl : protected SignalIndicationCctrl
      {
      public:
        SignalIndicationWCctrl & operator = (const SignalIndicationCctrl & o)
          {
            *((SignalIndicationCctrl *)this) = o;
            return *this;
          }
        int getSgNo() const { return sgNo; }
        int getOcitColor() const { return ocitColor &0x1ff; }
        bool hasState() const { return ocitColor  & 0xf000; }
        // o oooo oooo
        unsigned char getState()  const { return static_cast<unsigned char>((ocitColor >> 12)-1); }
        const char * getStateTxt() const;
        int getColorStartTime() const {return colorStartTime; }

      };
    class SignalIndicationsWCctrl
      {
      public:
        typedef std::map<int, SignalIndicationWCctrl> sgPatterns_t;
        int64_t baseTicktime;
        int64_t baseUtcMinusTicktime;
        int64_t tx;

        const SignalIndicationWCctrl * lookup(int sigNr) const;

        SignalIndicationsWCctrl() : baseTicktime(0), baseUtcMinusTicktime(0), tx(0), sgPatterns() {}
        SignalIndicationsWCctrl & operator = (const SignalIndicationsCctrl & o);
        const sgPatterns_t & getSgPatterns() const { return sgPatterns; }
        std::string getDecodedSgPattern() const;
        int64_t getBaseUtcMinusTicktime() const;

      protected:
        sgPatterns_t sgPatterns;
      };

    //by Valentin
    class LampStatusWCctrl : protected LampStatusCctrl
      {
      public:
        LampStatusWCctrl & operator = (const LampStatusCctrl & o)
          {
            *((LampStatusWCctrl *)this) = o;
            return *this;
          }
        int getSgNo() const { return sgNo; }
        bool isGreenHeadFault()const;
        bool isRedHeadFault()const;
        bool isAmberHeadFault()const;

      protected:
        int countLampStateFault(LampFaultStateCctrl* heads, int len)const;
      };
  }

#endif
