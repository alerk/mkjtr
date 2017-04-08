/*
 * nlsPropertyProvider.cpp
 *
 *  Created on: 17.12.2015
 *      Author: zelgerpe
 */

#include "nlsPropertyProvider.h"
#include "util/repdef.h"
#include"CControl.h"
#include <fstream>
#include <dirent.h>

#include "util/what.h"
DEFINE_WHATINFO;


using namespace sitraffic;

NlsPropertyProvider::Properties::Properties(const std::string& strFile) :
    m_map()
  {
    Read(strFile);
  }

bool NlsPropertyProvider::Properties::Read(const std::string& strFile)
  {

    std::ifstream is(strFile.c_str());

    if (!is.is_open())
      {
        Report(NLS_ERROR, "can't open nls property file:%s", strFile.c_str());
        return false;
      }

    while (!is.eof())
      {

        std::string strLine;

        getline(is, strLine);

        std::string::size_type nPos = strLine.find('=');

        if (std::string::npos == nPos)
          continue; // no '=', invalid line;

        std::string strKey = trim(strLine.substr(0, nPos));

        std::string strVal = trim(strLine.substr(nPos + 1));

        m_map.insert(std::map<std::string, std::string>::value_type(strKey, strVal));
      }
    return true;
  }

void NlsPropertyProvider::Properties::fill(NlsMappingCctrl & nmc)
  {
    nmc.map.map_val = (NlsElementCctrl *) malloc(m_map.size() * sizeof(nmc.map.map_val[0]));
    nmc.map.map_len = 0;
    for (auto i = m_map.begin(); i != m_map.end(); ++i)
      {
        nmc.map.map_val[nmc.map.map_len].key = strdup(i->first.c_str());
        nmc.map.map_val[nmc.map.map_len].value = strdup(i->second.c_str());
        ++nmc.map.map_len;
      }
  }

std::string NlsPropertyProvider::Properties::trim(const std::string& str)
  {
    std::string s = str;
    std::string::size_type p;
    while ((p = s.length()) > 0 && (unsigned char) s[p - 1] <= ' ')
      s.resize(p - 1);
    while ((p = s.length()) > 0 && (unsigned char) s[0] <= ' ')
      s.erase(0, 1);
    return s;
  }


// read and send nls.properties
void NlsPropertyProvider::readDir(const std::string & dirname, std::vector<std::string> & files)
  {
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(dirname.c_str())) != NULL)
      {
        /* print all the files and directories within directory */
        while ((ent = readdir(dir)) != NULL)
          {
            //printf ("%s\n", ent->d_name);
            std::string fname(ent->d_name);
            if (fname.find(".properties") != std::string::npos)
              files.push_back(fname);
          }
        closedir(dir);
      }
    else
      {
        /* could not open directory */
        Report(NLS_ERROR, "can't open dir:%s", dirname.c_str());
      }
  }
  /**
   * @param fname filename without prefix
   * @return lang shortcut, that is 2 chars (XY) between _XY.properties
   */
  std::string NlsPropertyProvider::langFromFilename(const std::string & fname)
  {
    std::string::size_type posLast_ = fname.rfind('_');
    std::string::size_type posLastPoint = fname.rfind('.');
    int diff = static_cast<int>(posLastPoint - posLast_);
    if (posLast_ ==std::string::npos || posLastPoint ==std::string::npos || posLastPoint < (unsigned)2 || diff != 3)
      return "";
    std::string lang = fname.substr(posLast_ + 1, diff-1);
    return lang;
  }

  /**
   *  read nls  files in directory cfg/nls
   *  the files are expected to be named: xxxx_LL.properties
   *  with xxxx is an arbitrary name, LL are 2 characters language id e.g. de
   *  @param nms ccontrol nls mapping, to be freed after call by xdr_free
   */
  void NlsPropertyProvider::fill(NlsMappingsCctrl & nms)
  {
      std::vector<std::string> files;
      const std::string dir("cfg/nls");
      readDir(dir, files);
      nms.NlsMappingsCctrl_val = (NlsMappingCctrl*)malloc(files.size()*sizeof(nms.NlsMappingsCctrl_val[0]));
      nms.NlsMappingsCctrl_len = 0;
      for(std::string file:files)
        {
          Report(NLS_DEBUG, "processing nls file:%s", file.c_str());
          std::string lang=langFromFilename(file);
          nms.NlsMappingsCctrl_val[nms.NlsMappingsCctrl_len].lang = strdup(lang.c_str());
          Properties p;
          std::string pfile(dir+"/"+file);
          if(!p.Read(pfile))
            continue;
          p.fill(nms.NlsMappingsCctrl_val[nms.NlsMappingsCctrl_len]);
          ++nms.NlsMappingsCctrl_len;
        }
      Report(NLS_EVENT, "NlsMappingsCctrl len:%d ", nms.NlsMappingsCctrl_len);
   }
