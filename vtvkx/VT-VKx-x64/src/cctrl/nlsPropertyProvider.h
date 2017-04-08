/*
 * nlsPropertyProvider.h
 * handles reading of local nls property files and converting them into CCtrl structure.
 * entry is void fill(NlsMappingsCctrl & nms); called by CControl_server.cpp
 *
 *  Created on: 17.12.2015
 *      Author: zelgerpe
 */

#ifndef CCTRL_NLSPROPERTYPROVIDER_H_
#define CCTRL_NLSPROPERTYPROVIDER_H_

#include <string>
#include <map>
#include <vector>
#include "CControl.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

namespace sitraffic
  {
  class NlsPropertyProvider
    {

      static void readDir(const std::string & dirname, std::vector<std::string> & files);
      static std::string langFromFilename(const std::string & fname);

      public:
       class Properties
        {

        public:

          Properties() : m_map()  {  }
          Properties(const std::string& strFile);

          void fill(NlsMappingCctrl & nmc);

          bool Read(const std::string& strFile);
        protected:
          std::string trim(const std::string& str);
          std::map<std::string, std::string> m_map;
        };

       /**
         *  read nls  files in directory cfg/nls
         *  the files are expected to be named: xxxx_LL.properties
         *  with xxxx is an arbitrary name, LL are 2 characters language id e.g. de
         *  @param nms ccontrol nls mapping, to be freed after call by xdr_free
         */
      static void fill(NlsMappingsCctrl & nms);

    };
  }



#endif /* CCTRL_NLSPROPERTYPROVIDER_H_ */
