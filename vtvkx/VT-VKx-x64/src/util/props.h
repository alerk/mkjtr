#include <map>
 #include <string>
 #include <fstream>
 #include <iostream>

#include "util/what.h"
DEFINE_WHATINFO_HEADER;


 using namespace std;

 class Properties
 {
 public:

     Properties (const string& strFile) : m_strFile(strFile), m_map() {}

     bool Read ()
     {
       ifstream is(m_strFile.c_str());
       if (!is.is_open())
         return false;

       while (!is.eof())
       {
           string strLine;

           getline(is,strLine);

           size_t nPos = strLine.find('=');

           if (string::npos == nPos) continue; // no '=', invalid line;

           string strKey = strLine.substr(0,nPos);

           string strVal = strLine.substr(nPos + 1, strLine.length() - nPos + 1);

           m_map.insert(map<string,string>::value_type(strKey,strVal));
       }

       return true;
     }

     bool Write ()
     {
          ofstream os(m_strFile.c_str());

          if (!os.is_open())
            return false;

          map<string,string>::const_iterator i;

          for(i = m_map.begin(); i != m_map.end(); i++)
          {
             os << i->first << "=" << i->second << "\n";
          }
          os.close();
          return true;
      }

     bool GetValue(const string& strKey, string& strValue) const
     {
         map<string,string>::const_iterator i;

         i = m_map.find(strKey);

         if (i != m_map.end())
         {
             strValue = i->second;
             return true;
         }
         return false;
     }

     const char * GetValue(const string& strKey) const
     {
        map<string,string>::const_iterator i;
        i = m_map.find(strKey);

        if (i != m_map.end())
        {
            return i->second.c_str();
        }

        return NULL;
      }

     void SetValue(const string& strKey, string strValue)
     {
         m_map[strKey] = strValue;
     }

 protected:
     string m_strFile;
     map<string,string> m_map;
 };


