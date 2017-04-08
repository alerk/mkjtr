/*
 * statistics.h
 *
 *  Created on: 20.01.2016
 *      Author: zelgerpe
 */

#ifndef START_STATISTICS_H_
#define START_STATISTICS_H_

#include <algorithm>
#include <sstream>
#include <map>
#include <cmath>

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
  {

    /*
     * simple Histogram helper class
     */
    template<typename Value_t = int, typename Count_t = int>
      class Histogramm
        {
          const Value_t binsize;
          const Value_t skipBinValueFrom, skipBinValueTo;
          typedef std::map<Value_t, Count_t> maptype;
          // key=roundtrip time in ms, value=number of occurrences
          maptype histMap;

          bool isSkip(Value_t binValue) const
            {
              return skipBinValueFrom <= binValue && binValue <= skipBinValueTo;
            }

        public:
          Histogramm(Value_t binsize_ = 10, Value_t skipFrom = 0, Value_t skipTo = 0) :
              binsize(binsize_), skipBinValueFrom(skipFrom), skipBinValueTo(skipTo), histMap()
            {
            }

          Value_t getBinsize() const { return binsize; }

          void clear() { histMap.clear(); }

          void addValue(Value_t value)
            {
              const Value_t binValue = toBinValue(value);
              Count_t newCount;
              typename maptype::iterator it = histMap.find(binValue);
              if (it == histMap.end())
                newCount = 1;
              else
                newCount = it->second + 1;
              histMap[binValue] = newCount;
            }

          std::string toString() const
            {
              std::ostringstream ss;

              ss << "Histogramm [";
              for (typename maptype::const_iterator e = histMap.cbegin(); e != histMap.cend(); ++e)
                {
                  // print skipped values bold
                  if(isSkip(e->first))
                    ss << "<b>" << (e->first * binsize) << ':'<< e->second << "</b> ";
                  else
                    ss << (e->first * binsize) << ':' << e->second << ' ';
                }
              ss << "]" << " skipped " << skipBinValueFrom << ".." << skipBinValueTo;
              return ss.str();
            }

          void printBinsCsv(std::ostream & os) const
            {
              bool isfirst=true;
              for (typename maptype::const_iterator e = histMap.cbegin(); e != histMap.cend(); ++e)
                {
                  if(isSkip(e->first))
                    continue;
                  if (isfirst)
                    isfirst=false;
                  else
                    os << ',';
                  os << '\"' << (e->first * binsize) << '\"';
                }
            }
          void printCountsCsv(std::ostream & os) const
            {
              bool isfirst=true;
              for (typename maptype::const_iterator e = histMap.cbegin(); e != histMap.cend(); ++e)
                {
                  if(isSkip(e->first))
                    continue;
                  if (isfirst)
                    isfirst=false;
                  else
                    os << ',';
                  os << '\"' << e->second << '\"';
                }
            }
        protected:
          Value_t toBinValue(Value_t value) const
            {
              Value_t binValue = value / binsize;
              return binValue;
            }

        };

    class Stats
      {
        uint32_t count;

        int32_t max = INT32_MIN; //Integer.MIN_VALUE;

        int32_t min = INT32_MAX; //Integer.MAX_VALUE;

        double sum;

        double sum2;

      public:
        void add(int32_t v)
          {
            min = std::min(min, v); //Math.min(min, v);
            max = std::max(max, v); //Math.max(max, v);
            sum += v;
            sum2 += ((double) v) * v;
            ++count;
          }

        double average()
          {
            if (count == 0)
              return 0;
            return sum / count;
          }

        void reset()
          {
            count = 0;
            sum = 0;
            sum2 = 0;
          }

        double standardDeviation()
          {
            if (count <= 2)
              return 0;
            double ret = (sum2 - (sum * sum) / count) / (count - 1);
            if (ret < 0)
              return 0;
            ret = std::sqrt(ret);
            return ret;
          }

        std::string toString()
          {
            std::ostringstream ss;
            ss << "Stats [count=" << this->count << ", average=" << this->average() << ", standardDeviation="
                << this->standardDeviation() << "]";
            return ss.str();
          }
      };
  }

#endif /* START_STATISTICS_H_ */
