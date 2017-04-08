//==MODULE=====================================================================
//=============================================================================
//
// Copyright (c) 1996 Siemens AG
// All rights reserved
// copied from /vsr/dvp/common/inc/what.h
//
// Projekt: TAC
//
// Modulname: what.h
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

#ifndef _WHAT_H_
#define _WHAT_H_

// global #include section


// global #define section
#ifndef __TIMESTAMP__
#define __TIMESTAMP__ "????.??.??" // only for eclipse
#endif

#ifndef __BASE_FILE__
#define __BASE_FILE__ "??????" // only for eclipse
#endif

#define CONCAT_2(A,B) A##B
#define CONCAT_1(A,B) CONCAT_2(A,B)

#define DEFINE_WHATINFO        static const CWhatInfo cwhatInfo(__FILE__, __TIMESTAMP__, __DATE__ " " __TIME__); inline void unused_declaration_of_variable_cwhatInfo(){(void)cwhatInfo;}
#define DEFINE_WHATINFO_HEADER static const CWhatInfo CONCAT_1(cwhatInfo,__COUNTER__)(__FILE__, __BASE_FILE__, __TIMESTAMP__, __DATE__ " " __TIME__);


// global type definitions

// global const definitions

// exported global declarations

// exported global functions declarations

// forward declarations

// exported global class declarations

class CWhatInfo
{
	// static member variable (sm_)
 	static CWhatInfo * sm_head;

	// member variable (m_)
   	const char * m_file;
 	const char * m_basefile;
   	const char * m_timestamp;
   	const char * m_compileTime;

	CWhatInfo * m_next;

	// static member functions

	// member functions
private:
	CWhatInfo(const CWhatInfo&); // no copy ctor
	CWhatInfo& operator=(const CWhatInfo&);

	//void Print();

public:
	CWhatInfo(const char *file, const char * timestamp, const char * compileTime);
	CWhatInfo(const char *file, const char *basefile, const char * timestamp, const char * compileTime);
	~CWhatInfo();
	static CWhatInfo *  GetHead();
        inline const char * GetFile(){return m_file;};
        inline const char * GetBaseFile(){return m_basefile;};
        inline const char * GetTimeStamp(){return m_timestamp;};
        inline const char * GetCompileTime(){return m_compileTime;};
	inline CWhatInfo *  GetNext(){return m_next;};
};

#endif


