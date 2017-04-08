//==MODULE=====================================================================
//=============================================================================
//
// Copyright (c) 1996 Siemens AG
// All rights reserved
// copied from /vsr/dvp/common/what.cpp
//
// Projekt: TAC
//
// Modulname: what.cpp
// Version:
//
// Aufgabe:
//
// Ausgabe der WhatInfo
//
// Bekannte Fehler:
//
// Uebersetzungsanweisungen:
//
// Autoren		Initialen	Abteilung
// Gerhard Kogler	GK		Siemens Graz
//
// Historie:
// AE-Art AE-Nr Datum    Autor	Version Kurzbeschreibung
// --     0000  11.09.96 GK	V0.1    Ausgabe der WhatInfo
//=============================================================================
/*
 
Klasse zum Ausgeben der WhatInfo 
 * ********
*/
//==END========================================================================
// local used #include section
#include <evhttpadapter.h>
#include <util/what.h>
#include <cstdio>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

DEFINE_WHATINFO

//#include <string>
#include "cctrl/htmlHandler.h"

//DEFINE_WHATINFO


// local used #define section

// local type definitions

// local const definitions

// static global definitions
CWhatInfo * CWhatInfo::sm_head = 0;

// static global functions definitions

// global functions definitions

// global definitions

// member functions definitions

//==FUNCTION===================================================================
//
// Funktionsname:	CWhatInfo
// Parameter:		keine
// Rueckgabewert:	keine
// Globale Variablen:	keine
//					Konstruktor
//
//==END========================================================================
CWhatInfo::CWhatInfo(const char *file, const char * timestamp, const char * compileTime)
		:m_file(file),m_basefile(0),m_timestamp(timestamp),m_compileTime(compileTime), m_next(sm_head)
{
	//Print();
	sm_head = this;
}

//==FUNCTION===================================================================
//
// Funktionsname:	CWhatInfo for headerfiles
// Parameter:		basefile = name of surrounding c(pp) file
// Rueckgabewert:	keine
// Globale Variablen:	keine
//					Konstruktor
//
//==END========================================================================

CWhatInfo::CWhatInfo(const char *file, const char *basefile, const char * timestamp, const char * compileTime)
		:m_file(file),m_basefile(basefile),m_timestamp(timestamp),m_compileTime(compileTime), m_next(sm_head)
{
	//Print();
	sm_head = this;
}

//==FUNCTION===================================================================
//
// Funktionsname:	CWhatInfo
// Parameter:		keine
// Rueckgabewert:	keine
// Globale Variablen:	keine
//					Destruktor
//
//==END========================================================================
CWhatInfo::~CWhatInfo()
{
  if (this == sm_head)
  {
    sm_head = m_next;
    return;
  }//endif

  CWhatInfo *prev = sm_head;

  for (CWhatInfo *pwi=sm_head->m_next; pwi; prev = pwi, pwi = pwi->m_next)
  {
    if (pwi == this)
    {
      prev->m_next = m_next;
      break;
    }//endif
  }//endfor
}	


//==FUNCTION===================================================================
//
// Funktionsname:	PrintAll
// Parameter:		keine
// Rueckgabewert:	keine
// Globale Variablen:	keine
//		: Gibt die verkettete Liste aller WhatInfo's aus !
//
//==END========================================================================
// static
CWhatInfo * CWhatInfo::GetHead()
{
//	for (CWhatInfo *pwi=sm_head; pwi; pwi = pwi->m_next)
//		pwi->Print();
    return sm_head;
}

namespace sitraffic
{

std::string trimBaseFile(const char * file)
{
	std::string filename(file);

	size_t idx = filename.find_last_of('/');
	if (idx != std::string::npos)
	{
		filename = filename.substr(idx+1); // e.g. ../src/start/TAC_main.cpp --> TAC_main.cpp
	}

	size_t ext = filename.find(".cpp");
	if (ext != std::string::npos)
	{
		filename = filename.substr(0, ext); // e.g. TAC_main.cpp --> TAC_main
	}

	return filename;
}

std::string prepareFileName(const char * file)
{
	std::string filename(file);

	size_t ext = filename.find("TA_main.cpp"); // expose TA_main.cpp to indicate TacKernelDemo is running
	if (ext != std::string::npos)
	{
		filename = "<B>" + filename + "</B>";
	}

	return filename;
}


std::string WhatInfoHtmlRequestHandler::doRequest(EvHttpRequest & /*ehr*/)
{
    std::ostringstream ss;
    ss << "<center>";

    TableHelper t;
    char buf[30];
    sprintf(buf, "WhatInfo");
    t.appendHeader(buf, "id4711");
    t.startTitle();
    t.addTitleColumn("Source File").addTitleColumn("File TimeStamp (in clearcase)").addTitleColumn("Compiled at").endTitle();

    CWhatInfo * file = CWhatInfo::GetHead();
    while (file != 0)
    {
       if(!file->GetBaseFile())
          t.startRow().addItem(prepareFileName(file->GetFile())).addItem(file->GetTimeStamp()).addItem(file->GetCompileTime()).endRow();
        file = file->GetNext();
    }
    t.endTable();
    ss << t.toString();

    ss << "<BR>";

     TableHelper th;
     sprintf(buf, "WhatInfo for Headerfiles");
     th.appendHeader(buf, "id4712");
     th.startTitle();
     th.addTitleColumn("Header File").addTitleColumn("File TimeStamp (in clearcase)").addTitleColumn("Base files (default: *.cpp)").endTitle();

     std::map<std::string, std::map<std::string, std::vector<CWhatInfo *> > > map;

     file = CWhatInfo::GetHead();
     while (file != 0)
     {
    	 if(file->GetBaseFile())
    	 {
    		 map[file->GetFile()][file->GetTimeStamp()].push_back(file);
    	 }
         file = file->GetNext();
     }

     for (auto it = map.cbegin(); it != map.cend(); ++it)
     {
    	 std::string headerFile = (*it).first;
    	 std::map<std::string, std::vector<CWhatInfo *> > instancesH = (*it).second;

         for (auto itH = instancesH.cbegin(); itH != instancesH.cend(); ++itH)
         {
           std::string timestamp = (*itH).first;
		   std::vector<CWhatInfo *>  surrFiles = (*itH).second;

		   std::string surroundingFiles;
		   for (auto surr = surrFiles.cbegin(); surr != surrFiles.cend(); ++surr)
		   {
			   surroundingFiles += trimBaseFile((*surr)->GetBaseFile());
			   surroundingFiles += "; ";
		   }

		   if(instancesH.size() > 1) // header file in use with different versions !
			   th.startRow().addItem("<B>" + headerFile + "</B>").addItem("<B>" + timestamp + "</B>").addItem(surroundingFiles).endRow();
		   else
		     th.startRow().addItem(headerFile).addItem(timestamp).addItem(surroundingFiles).endRow();
         }

     }


     th.endTable();
     ss << th.toString();


     ss << "</center>" << std::endl;
    return ss.str();
}



}
