// *****************************************************************************
//
//     Projekt:
//                  Siemens AG ZN-Muenchen ANL TD MWE 51
//
//     COPYRIGHT (C) SIEMENS AG 1994 ALL RIGHTS RESERVED CONFIDENTIAL
//
// *****************************************************************************
// Weitergabe sowie Vervielfaeltigung dieser Daten, Verwertung und Mitteilung
// ihres Inhalts nicht gestattet, soweit nicht ausdruecklich zugestanden.
// Zuwiderhandlung verpflichtet zu Schadenersatz. Alle Rechte vorbehalten, ins-
// besondere fuer den Fall der Patenterteilung oder GM-Eintragung der technischen 
// Inhaltsteile.
// *****************************************************************************
//
//     Modulname:   Report.cpp
//     Verfasser:   F. Philipp 
//
//     Datum:       27.07.94
//     Version:     1.00
//
//     Aufgabe:     
//
/*     History:                                                                 */
/*                20.06.2016 copied from \migra\dvp\common\report.cpp           */
//
/*
Comment:

*/
// *****************************************************************************

#include "util/what.h"
DEFINE_WHATINFO;


 #define Dprintf(...)
//#define Dprintf  fflush(stdout);printf
#define DStatement(...)
// #define DStatement(x) x
#define DFLUSH
// #define DFLUSH fflush(stdout);

// va_xxx
//#include <stdio.h>
//#include <stdarg.h>
//#include <limits.h>
//#include <ctype.h>
//#include <errno.h>
//#include <thread.h>
#include <climits>
#include <cerrno>
#include<cstdio>
#include<cstdlib>
#include<cstdarg>
#include<cstring>
#include<cctype>
#include<ctime>
#include<thread>
#include <sys/time.h>
#include<atomic>
#include <map>
#include "props.h"


// #include "snprintf.h"
#include "report.h"
#include "TestAndDiagnostics.h"

// #include "CControl.h"
// #include "api_va.h"
// #include "StateMessage.h"


#ifdef WIN32
 #define localtime_r(x, y) localtime_s((y), (x))
 // #define LocalTimeNow(tmPtr, tmpTime) localtime_s(tmPtr, tmpTime);
#else
 // #define LocalTimeNow(tmPtr, tmpTime) localtime_r(tmpTime, tmPtr);
#endif


// Gross- + Kleinbuchstaben
#define MAX_CAT         64
#define MAX_ENTRY_COUNT	2048


static const char sm_pathTrenner = '/';

// 32 bit: tick:28[8], thread:36[4], file:40[4], line:44[4], func:48[4], attr:52[4],
// 64 bit: tick:32[8], thread:40[8], file:48[8], line:56[4], func:64[8], attr:72[8],
class RepLine
{
  public:							// 32bit   64bit
   char         time[32];			// 28      32
   int64_t      tick;				//  8		8
   std::thread::id threadId;		//  4		8
   const char * file;				//  4		8
   int          line;				//  4		4
   const char * func;				//  4		8
   long         attr;				//  4		8
   char         text[432];			//200	  432
   	   	  	  	  	  	  	// sum:   256	  512

   RepLine(): tick(0), threadId(0), file(NULL), line(0), func(NULL), attr(0) {time[0]='\0';text[0]='\0';}
   void commit(const char * f)  {file = f;}
   bool isReady()               {return file != NULL;}
   void processed()             {file = NULL;}
};

static const long MAX_BUF_LINES = 300;
static RepLine lineBuf[MAX_BUF_LINES]; // Buffer for Report messages
static std::atomic_long writeCount(-1);                  // count for incoming messages
static long throwAwayCount = 0;
static std::atomic_long readCount(-1);                   // count for processed messages


static Properties sm_props("cfg/report.properties");
static std::string KeyLevels = "Levels";

// size is MAX_CAT+1 because sm_prio[0] is global level
static	int	sm_prio[MAX_CAT+1] = {2};
static	long	sm_attrOnGlobal = 0L;
static	long	sm_attrOffGlobal = ~0L;

//typedef std::map<int, SignalIndicationWCctrl> sgPatterns_t;

static std::map<std::thread::id, std::string> sm_threadNames;

static  long         lenMax;

static   int64_t            sm_baseTicktime; // OMC tick from SignalIndications
static   t_fscreenprint     screenFkt;
static   t_fHistoryAppender historyAppenderFkt = NULL;
static   char               file1NameNeu[PATH_MAX];
static   char               file1NameAlt[PATH_MAX];
static   char               file2NameNeu[PATH_MAX];
static   char               file2NameAlt[PATH_MAX];
static   bool               flushImmediately = false;

static  FILE * file1 = NULL;
static  FILE * file2 = NULL;
static FILE * OpenFile(FILE * fileP, const char * const fileName)
{
    if(fileP != NULL) // already open
       return fileP;

    Dprintf("OpenFile: %s\n", fileName);

    return fopen(fileName, "a");
}

static void CloseFile(FILE * file)
{
    if(file == NULL)
      return;

    Dprintf("CloseFile: %s\n", file==file1?"file1":file==file2?"file2":"?");

    fclose(file);
}



static inline int GetPrio(long attr)
{
	return attr & 0x07;
}

static inline int GetCatNr(long attr)
{
	return ((attr) >> 3) & 0x3F;
}

static inline char GetCatChar(int nr)
{
	return (char)(nr <= 26 ? nr + 'A' - 1 : nr - 26 + 'a' - 1);
}

static inline char CharToPrio(char c)
{
	return (char)((c - '0') & 0x07);
}

static inline char PrioToChar(int p)
{
	return (char)(GetPrio(p) + '0');
}

static inline int CharToCatNr(char g)
{
	return ((g >= 'A' && g <= 'Z')
				? (g - 'A' + 1)
				:  ((g >= 'a' && g <= 'z')
					? (g - 'a' + 27)
					: (0)
					)
			);
}

static inline long GetFlags(long attr)
{
	return attr & 0xFFFF0000;
}

static long ModifyAttrWithGlobal(long attr)
{
	/********* Filterung durch globale Attribute *********/
	attr |= sm_attrOnGlobal;
	attr &= sm_attrOffGlobal;

	return attr;
}


//***********************************************************************
//**   27.06.16, 10:08:07 REPORT V3.00 from Mon Jun 27 10:06:27 2016   **
//*********************************************************


static void PrintReportHeader(FILE * pFile, bool append)
{
  static const char *pStars = "*****************************************************************************************\n";
  char timeBuf[128];
  struct tm tmpTm;
  time_t now;
  time(&now);
  localtime_r(&now, &tmpTm);
  strftime(timeBuf, sizeof(timeBuf), "%d.%m.%y, %H:%M:%S", &tmpTm);
  if(append)
    fputs("\n", pFile);
  fputs(pStars, pFile);
  fprintf(pFile, "**            %s REPORT V3.00 from %s            **\n", timeBuf, __TIMESTAMP__);
  fputs(pStars, pFile);

  ///////////////////// Versions ///////////////////////
  fputs(sitraffic::TestAndDiagnosticsHttpRequestHandler::getTacVersionsJSON().c_str(), pFile);
  fputs("\n", pFile);
  fputs(pStars, pFile);

  ///////////////////// WhatInfo ///////////////////////
  CWhatInfo * file = CWhatInfo::GetHead();
  fprintf(pFile, "CompileTime \t\t\tFile TimeStamp \t\tSource \n");
  while (file != 0)
  {
      Dprintf(       "%s ...\t%s \t%s \n", file->GetCompileTime(), file->GetTimeStamp(), file->GetFile() );
      if(file->GetBaseFile() == 0) // skip header files
        fprintf(pFile, "%s ...\t%s \t%s \n", file->GetCompileTime(), file->GetTimeStamp(), file->GetFile() );
      file = file->GetNext();
  }
  fputs(pStars, pFile);

  // tmp /////////////////////////////////////////////////////////////////////////////////////////////////
  char buf[128];
 RepLine r;
#define OFFS(x) (int)((char *)&(r.x) - (char *)&(r.time))
#define SIZE(x) (int)sizeof(r.x)
  sprintf(buf, "tick:%d[%d], thread:%d[%d], file:%d[%d], line:%d[%d], func:%d[%d], attr:%d[%d], ",
		  OFFS(tick), SIZE(tick), OFFS(threadId), SIZE(threadId), OFFS(file), SIZE(file), OFFS(line), SIZE(line),
		  OFFS(func), SIZE(func), OFFS(attr), SIZE(attr));
  fputs(buf, pFile);
  fputs("\n", pFile);
  fputs(pStars, pFile);
}

/*>***************************************************************************
**                                                                          **
**   createFileNames                                                        **
**   ~~~~~~~~~~~~~~~                                                        **
**                                                                          **
**   Aufruf:  int BestimmeDateinamen(const char *pure_name, char *newFile,  **
**                char *alt_file, int buf_len )                             **
**                                                                          **
**   Param.:  (IN)  pure_name = file of report file without extensions      **
**            (OUT) newFile = Name of Report file                           **
**            (OUT) oldFile = Name of backup Report file                    **
**            (IN)  bufLen = available buffer length for both file names    **
**   Return:  0                                                             **
**           -1 = Error                                                     **
**                                                                          **
**   Descr.:                                                                **
***************************************************************************<*/
static int createFileNames(const char *pure_name,
                          char *newFile, char *oldFile, int bufLen)
{
	Dprintf("pure_name=%s bufLen=%d\n", pure_name, bufLen);

	snprintf(oldFile, bufLen, "%s.log.1", pure_name);
	snprintf(newFile, bufLen, "%s.log", pure_name);

	return 0;
}


/*>***************************************************************************
**                                                                          **
**   Initialisiere die Reportfunktion                                       **
**   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~                                       **
**                                                                          **
**   Aufruf:  int IniReport(char *taskname,  int level,                     **
**              char *logname ,  long log_anz,                              **
**              char *tracename, long trace_anz);                           **
**                                                                          **
**   Param.:  Name der Task,                                                **
**            Name der Logdatei, Laenge der Logdatei,                       **
**            Name der Tracedatei, Laenge der Tracedatei                    **
**                                                                          **
**   Rueckg.: levels                                                        **
**                                                                          **
**   Beschr.: Aus dem Environment wird der Name fuer die Datei, auf die     **
**            Reports der Task geschrieben werden, geholt.                  **
**            Zusaetzlich wird aus diesem Namen durch anhaengen von ".alt"  **
**            der Name der Backupdatei gebildet, falls bei der Dateilaenge  **
**            nicht "| R_APPEND" angegeben ist.                             **
***************************************************************************<*/
const char *  IniReport(const char *taskName, int level, const char *logname, long log_anz, const char *tracename, long trace_anz)
{
	Dprintf("IniReport\n");

	sm_props.Read();

	setCurrentThreadName(taskName);
	sm_attrOnGlobal  = 0L;    /* R_SCREEN wieder ausschalten */
	sm_attrOffGlobal = ~0L;

	lenMax = log_anz & ~R_APPEND;

	if (logname == NULL)
	 {
		logname = "default1TA";
	 }
	
	createFileNames(logname, file1NameNeu, file1NameAlt, PATH_MAX);
	file1NameNeu[PATH_MAX-1] = '\0';

	bool append = true;
	if( ! (log_anz & R_APPEND) )
	{
		if (rename(file1NameNeu, file1NameAlt) == -1)
				remove(file1NameNeu);
		append = false;
	}
  
	Dprintf("IniReport: logname=%s fileNameNeu=%s fileNameAlt=%s lenMax=%ld\n", logname, file1NameNeu, file1NameAlt, lenMax);

	file1 = OpenFile(file1, file1NameNeu);
	if ( file1 )
	{
	  PrintReportHeader(file1, append);
	  fclose(file1);
	  file1 = NULL;
	}
	else
	  fprintf(stderr, "%s: log file %s does not exist\n", taskName, file1NameNeu);


	if (tracename == NULL)
    {
	    tracename = "default2TA";
    }
	createFileNames(tracename, file2NameNeu, file2NameAlt, PATH_MAX);
	file2NameNeu[PATH_MAX-1] = '\0';

	append = true;
	if( ! (trace_anz & R_APPEND) )
	{
		if (rename(file2NameNeu, file2NameAlt) == -1)
				remove(file2NameNeu);
		append = false;
	}

	Dprintf("IniReport: logname=%s file2NameNeu=%s file2NameAlt=%s lenMax=%ld\n", logname, file2NameNeu, file2NameAlt, lenMax);

	file2 = OpenFile(file2, file2NameNeu);
	if ( file2 )
	{
	  PrintReportHeader(file2, append);
	  fclose(file2);
	  file2 = NULL;
	}
	else
	  fprintf(stderr, "%s: log file 2 %s does not exist\n", taskName, file2NameNeu);


	/******************* Level der FehlerAusgaben *****************/
	/* Level wurde mit uebergeben */
	for(int i = 0; i < MAX_CAT; i++)
		sm_prio[i] = level;

	const char * persistedLevels = sm_props.GetValue(KeyLevels);
	const char * currentLevels = LevelReport(persistedLevels);

	return currentLevels;

} /* IniReport() */ 


/*>***************************************************************************
**                                                                          **
**   LevelReport                                                            **
**   ~~~~~~~~~~~                                                            **
**                                                                          **
**   Aufruf:  const char *LevelReport(const char *gruppen_level);           **
**                                                                          **
**   Param.:  (E) gl = String mit Levels in den jeweiligen Gruppen          **
**                gl == NULL: Levels nur zurueckmelden                      **
**                                                                          **
**   Rueckg.:  String mit eingestellten Levels                              **
**                                                                          **
**   Beschr.: Levels gruppenspezifisch setzen und zurueckmelden             **
**                                                                          **
***************************************************************************<*/

const char *LevelReport(const char *gl)
{

#define SET_LEVEL_GLOBAL 100

	static char levels[MAX_CAT * 2];


	int level = 3;  /* default */
	char *cptr = levels;
	int i, cat;
	int init = 1;


	if (gl != NULL)
	{
		/* SetLevels(gl); */
		if (*gl == '+')  /*** APPEND ****/
		{
			char c = *(++gl);
			if (isdigit(c))
			{
				c = CharToPrio(c);
				for (i = 0; i < MAX_CAT; ++i)
					if (!(sm_prio[i] & Ri_INDIVIDUELL))
						sm_prio[i] = c;
			}
		}
		else              /**** SET ****/
		{
			char c = *gl;
			if (isdigit(c))
			{
				c = CharToPrio(c);
				for(i = 0; i < MAX_CAT; ++i)
					sm_prio[i] = c;
			}
		}

		for (i = (int)strlen(gl) - 1; i >= 0; --i)
		{
			char c = gl[i];
			if ( c == '-')
			{
				level = SET_LEVEL_GLOBAL;
			}
			else if ( isdigit(c) )
			{
				level = CharToPrio(c);
				/* printf("Level %d erkannt\n", level); */
			}
			else if( ( isalpha(c) ) || ( c == '_' ) ) // damit Blanks im String nicht stoeren
			{
				cat = CharToCatNr(c);
				if (level == SET_LEVEL_GLOBAL)
					sm_prio[cat] = sm_prio[0];
				else
					sm_prio[cat] = level | Ri_INDIVIDUELL;
				/* printf("Level %d in Cat %d, %c eingestellt\n",
					level, gruppe, Ri_GRUPPE(cat)); */
			}
		}


	}

	/*** GetLevels(levels); ***/
	*cptr++ = (char)(sm_prio[0] + '0');
	level = sm_prio[0];
	for (i = 1; i < 53; ++i)
	{
		int prio = sm_prio[i];
		if (prio & Ri_INDIVIDUELL)
		{
			prio &= 0x07;
			if (init)
			{
				init = 0;
				level = prio;
			}

			if (prio != level)       /* naechster Level neu */
				*cptr++ = PrioToChar(level);
			*cptr++ = GetCatChar(i);

			/* printf("level[%c]=%d, level[%c]=%d\n",
				Ri_GRUPPE(i), level, Ri_GRUPPE(i+1), levelM[i+1]&0x03); */
			level = prio;
		}
	}
	if (!init)       /* mindestens eine individuelle Gruppe */
		*cptr++ = PrioToChar(level);

	*cptr = '\0';


	return levels;
}

//--------------------------------------------------------
const char * getPersistedLevels()
{
  const char * persistedLevels = sm_props.GetValue(KeyLevels);
  return persistedLevels;
}

//--------------------------------------------------------
bool persistLevelReport()
{
     const char * currentLevels = LevelReport(NULL);
     sm_props.SetValue(KeyLevels, currentLevels);
     bool ret = sm_props.Write();
     return ret;
}

/*>***************************************************************************
**                                                                          **
**   SetScreenPrint                                                         **
**   ~~~~~~~~~~~~~~~~                                                       **
**                                                                          **
**   Aufruf :  void SetScreenPrint(void (*fptr)(const char*));              **
**                                                                          **
**   Param. :  (E)   fptr   = Zeiger auf Funktion, die die                  **
**                            R_SCREEN -Reports erledigen soll              **
**   Rueckg.:  void                                                         **
**                                                                          **
***************************************************************************<*/
void SetScreenPrint (t_fscreenprint fsp)
{
    Dprintf("Report.SetScreenPrint\n");
    screenFkt = fsp;
}


void SetHistoryAppender(t_fHistoryAppender fha)
{
    Dprintf("Report.SetHistoryAppender\n");
    historyAppenderFkt = fha;
}

/*>***************************************************************************
**                                                                          **
**   AttrOnReport                                                           **
**   ~~~~~~~~~~~~                                                           **
**                                                                          **
**   Aufruf:  int AttrOnReport(long attr);                                  **
**                                                                          **
**   Param.:  (E) attr = Bitleiste mit Attributen, die global gesetzt       **
**                       werden                                             **
**                                                                          **
**   Rueckg.: eingeschaltete Attribute                                      **
**                                                                          **
**   Beschr.: Setzen globaler Attribute der Reports, diese ueberschreiben   **
**            die beim Aufruf uebergebenen Attribute.                       **
**            Zusaetzlich kann mit dieser Funktion die Farbe und der Level  **
**            neu eingestellt werden (wie bei IniReport() ! )               **
***************************************************************************<*/
long AttrOnReport(long attr)
{
	long onFlags;

	sm_attrOnGlobal |=  GetFlags(attr);
	sm_attrOffGlobal |=  GetFlags(attr);

	onFlags = sm_attrOnGlobal;

	return onFlags;
}

/*>***************************************************************************
**                                                                          **
**   AttrOffReport                                                          **
**   ~~~~~~~~~~~~~                                                          **
**                                                                          **
**   Aufruf:  int AttrOffReport(long attr);                                 **
**                                                                          **
**   Param.:  (E) attr = Bitleiste mit Attributen, die global geloescht     **
**                       werden                                             **
**                                                                          **
**   Rueckg.:  ausgeschaltete Attribute                                     **
**                                                                          **
**   Beschr.: Loeschen globaler Attribute der Reports, diese ueberschreiben **
**            die beim Aufruf uebergebenen Attribute.                       **
**            Zusaetzlich kann mit dieser Funktion die Farbe und der Level  **
**            neu eingestellt werden (wie bei IniReport() ! )               **
***************************************************************************<*/
long AttrOffReport(long attr)
{
	long offFlags;

	sm_attrOnGlobal  &= ~GetFlags(attr);
	sm_attrOffGlobal &= ~GetFlags(attr);

	offFlags = ~sm_attrOffGlobal;

	return offFlags;
}

/*>***************************************************************************
**                                                                          **
**   AttrRestoreReport                                                      **
**   ~~~~~~~~~~~~~~~~~                                                      **
**                                                                          **
**   Aufruf:  int AttrResetReport(long attr);                               **
**                                                                          **
**   Param.:  (E) attr = Bitleiste mit Attributen, die global rueckgesetzt  **
**                       werden                                             **
**                                                                          **
**   Rueckg.:  restaurierte Attribute                                       **
**                                                                          **
**   Beschr.: Ruecksetzen globaler Attribute der Reports.                   **
**            Die beim Aufruf uebergebenen Attribute gelten wieder.         **
***************************************************************************<*/
long AttrRestoreReport(long attr)
{
	if (attr == R_RESTORE_ALL)
	{
		sm_attrOnGlobal  = 0;       /* alle glob. Attribute ruecksetzen */
		sm_attrOffGlobal = ~0;
	}
	else
	{
		sm_attrOnGlobal  &= ~GetFlags(attr); /* nur uebergeb. Flags ruecksetz. */
		sm_attrOffGlobal |= GetFlags(attr);
	}

	attr = ~sm_attrOnGlobal & sm_attrOffGlobal;

	return attr;
}

const char * ColorString(int color)
{
    switch(color)
    {
      case  R_BLACK: return "BLACK";
      case  R_RED: return "RED";
      case  R_GREEN: return "GREEN";
      case  R_BLUE: return "BLUE";
      case  R_MAGENTA: return "MAGENTA";
      case  R_ORANGE : return "ORANGE";
      case  R_CYAN : return "CYAN";
      case  R_GRAY: return "GRAY";
      default: return "?";
    }
}

const char * PrioString(int prio, int color)
{
   static char unknown[20];
   switch(prio)
   {
     case 0:      return "FATAL";
     case 1:      return color == R_BLUE ? "STATE" : "ERROR";
     case 2:      return color == R_GREEN ? "EVENT" : "WARN";
     case 3:      return "INFO";
     case 4:      return "DEBUG";
     case 5:      return "TRACE";
     default:     snprintf(unknown, sizeof(unknown), "<%d>", prio); return unknown;
   }
}


/*>***************************************************************************
**                                                                          **
**   AttributString                                                         **
**   ~~~~~~~~~~~~~~                                                         **
**                                                                          **
**   Aufruf:  char *AttributeToString(long attr)                               **
**                                                                          **
**                                                                          **
**   Param.:  (E) long attr = Attribut des Reports                          **
**                                                                          **
**   Rueckg.: Attribute als String                                          **
**                                                                          **
**   Beschr.:                                                               **
**                                                                          **
***************************************************************************<*/
const char *AttributeToString(long attr)
{
	static char string[500];

	string[0] = '\0';

	if (attr & APP_FILE1)
	  strcat (string, "APP_FILE1+");
	if (attr & APP_FILE2)
	  strcat (string, "APP_FILE2+");
	if (attr & R_THREAD)
	  strcat (string, "THREAD+");
	if (attr & R_FILI)
	  strcat (string, "FILI+");
	if (attr & R_FUNC)
	  strcat (string, "FUNC+");
	if (attr & R_PRIO)
	  strcat (string, "PRIO+");
	if (attr & APP_SCREEN)
	  strcat (string, "SCREEN+");
	if (attr & APP_HISTORY)
	  strcat (string, "HISTORY+");
	if (attr & APP_PRINTER)
	  strcat (string, "PRINTER+");
	if (attr & R_ERRNO)
	  strcat (string, "ERRNO+");
	if (attr & R_DATE)
	  strcat (string, "DATE+");
	if (attr & R_TIME)
	  strcat (string, "TIME+");
	if (attr & R_PATH)
	  strcat (string, "PATH+");

	sprintf(string, "%s%s+%c%d", string, ColorString(R_EXTRACT_COL(attr)), GetCatChar(Ri_GET_CAT(attr)), Ri_GET_PRIO(attr));

	return string;
}

//---------------------------------------------------------------------------------------------------------------------------------------
void setCurrentThreadName(const char * threadname)
{
   sm_threadNames[std::this_thread::get_id()] = threadname;
}

//---------------------------------------------------------------------------------------------------------------------------------------
const char * getThreadName(const std::thread::id& id)
{
   std::map<std::thread::id, std::string>::const_iterator i = sm_threadNames.find(id);
   if (i != sm_threadNames.end())
        return i->second.c_str();

   static char unknown[20];
   auto threadId = std::this_thread::get_id();
   snprintf(unknown, sizeof(unknown), "%p", (void *)&threadId);
   return unknown;
}


//---------------------------------------------------------------------------------------------------------------------------------------
void setCurrentReportTicks(int64_t baseTicktime)
{
   sm_baseTicktime = baseTicktime;
}

int64_t getCurrentReportTicks()
{
  return sm_baseTicktime;
}


//---------------------------------------------------------------------------------------------------------------------------------------
RepLine * getWriteRepLine(long currWriteCount)
{
   long writeIdx = currWriteCount % MAX_BUF_LINES;
   RepLine * lineToWrite = &lineBuf[writeIdx];
   if(lineToWrite->isReady())
   {
     Dprintf("WARN: lineToWrite isReady: currWriteCount=%ld writeIdx=%ld currReadCount=%ld", currWriteCount, writeIdx, readCount.load());
   }

   return lineToWrite;
}

//---------------------------------------------------------------------------------------------------------------------------------------
int VReport (const char *file, int line, const char *function, long attr, const char *format, va_list parptr) __attribute__((format(printf, 5, 0)));

int VReport (const char *file, int line, const char *function, long attr, const char *format, va_list parptr)
{
        Dprintf("VReport: file:%s, line:%d, function:%s, attr:0x%lx format:'%s' {%ld %ld %ld}\n",
            file, line, function, attr, format, writeCount.load(), readCount.load(), throwAwayCount);DFLUSH;

        if(writeCount >= (readCount + MAX_BUF_LINES)) // "overflow"
        {
          throwAwayCount++;
          Dprintf("VReport: throwAway {%ld %ld %ld}\n", writeCount.load(), readCount.load(), throwAwayCount);DFLUSH;
          return -1;
        }

        long currWriteCount = writeCount.fetch_add(1) + 1;

        Dprintf("VReport: currWriteCount: %ld {%ld %ld %ld}\n", currWriteCount, writeCount.load(), readCount.load(), throwAwayCount);DFLUSH;

        RepLine * msg = getWriteRepLine(currWriteCount);
        if(msg == NULL)
        {
           throwAwayCount++;
           Dprintf("VReport: msg==NULL -- throwAway {%ld %ld %ld}\n", writeCount.load(), readCount.load(), throwAwayCount);DFLUSH;
           return -1;
        }

        msg->tick = sm_baseTicktime;
        timeval tv;
        gettimeofday(&tv, NULL);

	int	ret = 1;
	

	Dprintf("VReport: Attr = %s col=%d=%s\n", AttributeToString(attr), R_EXTRACT_COL(attr), ColorString(R_EXTRACT_COL(attr)) );
	attr = ModifyAttrWithGlobal(attr);
	Dprintf("VReport: mod. Attr = %s\n", AttributeToString(attr) );


	/**************** mit Uhrzeit ? ***************/
	if (attr & (R_DATE|R_TIME))
	{
          const char * timeFormat;

          //using namespace std::chrono;
          //milliseconds ms = duration_cast< milliseconds >(system_clock::now().time_since_epoch());


          struct tm tmpTm;
          time_t now = tv.tv_sec;
          localtime_r(&now, &tmpTm);
          if (attr & R_DATE)
          {
              if (attr & R_TIME)
                timeFormat = "%Y-%m-%d %H:%M:%S";
              else
                timeFormat = "%Y-%m-%d";
          }
          else
          {
              timeFormat = "%H:%M:%S";
          }

          size_t tlen = strftime(msg->time, sizeof(msg->time), timeFormat, &tmpTm);
          if (attr & R_PRIO)
            snprintf(msg->time+tlen, sizeof(msg->time)-tlen, ",%06ld {%ld %ld %ld}", tv.tv_usec, writeCount.load(), readCount.load(), throwAwayCount);
          else
            snprintf(msg->time+tlen, sizeof(msg->time)-tlen, ",%03ld", tv.tv_usec/1000);

	}


       size_t len = vsnprintf(msg->text, sizeof(msg->text), format, parptr);
       if(len >= sizeof(msg->text)-1)
       {
         sprintf(msg->text + sizeof(msg->text) - 4, "...");
       }
       else
       {
         /********* do errno  **************/
         char errnoText[256] = "";
         if (attr & R_ERRNO)
         {
            snprintf(errnoText, sizeof(errnoText), "\terrno:%d=%s", errno, strerror(errno));
         }
         strncat(msg->text, errnoText, sizeof(msg->text) - len);
       }


       msg->threadId = std::this_thread::get_id();
       msg->func = function;
       msg->line = line;
       msg->attr = attr;
       msg->commit(file);


       if( /*tv.tv_sec - lastCall > 2 ||*/ flushImmediately) // for calls after EndReport()
         flushReport();

       //lastCall = tv.tv_sec;

       return ret;
}

//---------------------------------------------------------------------------------------------------------------------------------------
int Report(const char *file, int line, const char *function, long attr, const char *format, ...)
{
    va_list	parPtr;
    va_start(parPtr, format);

    int ret = 0;

    int prio = GetPrio(attr);
    int cat = GetCatNr(attr);
    int level = Ri_GET_PRIO(sm_prio[cat]);


   //  Dprintf("Report: file:%s, line:%d, func:%s, attr:0x%lX, prio=%d catNr=%d(%c) levelForCat:%d format:'%s'\n", file, line, function, attr, prio, cat, GetCatChar(cat), level, format);

    if (prio <= level)
    {
        if(format == NULL) // for ReportC macro
          ret = 1;
        else
          ret = VReport(file, line, function, attr, format, parPtr);
    }

    va_end(parPtr);
    return ret;
}

//---------------------------------------------------------------------------------------------------------------------------------------
RepLine * getRepLine(long currReadCount)
{
   long readIdx = currReadCount % MAX_BUF_LINES;
   RepLine * lineToRead = &lineBuf[readIdx];
   if(lineToRead->isReady())
     return lineToRead;
   else
     return NULL; // new report message in preparation, but not yet finished --> try next time
}

//---------------------------------------------------------------------------------------------------------------------------------------
FILE * writeToFile(FILE * fileP, const char * fileNameNew, const char * fileNameOld, const char * header, const char * func, const char * text)
{
    char pBuf[400];
    int len = snprintf(pBuf, sizeof(pBuf), "%s%s: %s\n", header, func, text);

     if(len >= (int)sizeof(pBuf))
       strcpy(pBuf + sizeof(pBuf) - 4, "...\0");
     else
       pBuf[sizeof(pBuf) - 1] = '\0';

     FILE * fp = OpenFile(fileP, fileNameNew);

    long sizeFtell;
    if (fp && (  (sizeFtell = ftell(fp)) > lenMax))
    {
      /*** move file to backup file ***/
      Dprintf("wrap files ftell = %ld > lenMax = %ld", sizeFtell, lenMax);
      fputs( ">>>>>>>>>>>>>>>>> bye, bye >>>>>>>> wrap to new file >>>>>>>>>>>>>>>>>>>", fp);

      fclose(fp);

      DStatement( int ret = ) rename(fileNameNew, fileNameOld);
      Dprintf("wrap files: renamed fileNameNew=%s to fileNameOld=%s with return %d", fileNameNew, fileNameOld, ret);

      fp = OpenFile(NULL, fileNameNew);
      if (fp)
          PrintReportHeader(fp, false);
       else
          fprintf(stderr, "log %s not available\n", fileNameNew);
    }

    if (fp)
    {
      fputs(pBuf, fp);
      // fclose(fp); --> later
    }
    return fp;
}

//---------------------------------------------------------------------------------------------------------------------------------------
int writeReport(RepLine * msg)
{
        Dprintf("writeReport: time:%s tick:%lld file:%s, line:%d, function:%s, attr:0x%lx format:'%s'\n", msg->time, msg->tick, msg->file,  msg->line,  msg->func,  msg->attr,  msg->text);

        long attr =  msg->attr;
        int          ret = 1;
        char         threadBuf[20] = "";
        char         prioBuf[30] = "";
        char         filiBuf[50] = "";
        const char * fileName = "";
        const char * func = "";
        char         header[256];


        Dprintf("writeReport: Attr = %s col=%d=%s\n", AttributeToString(attr), R_EXTRACT_COL(attr), ColorString(R_EXTRACT_COL(attr)) );
        attr = ModifyAttrWithGlobal(attr);
        Dprintf("writeReport: mod. Attr = %s\n", AttributeToString(attr) );

        int prio = GetPrio(attr);
        int color = R_EXTRACT_COL(attr);
        const char * severityString;


        /********* mit Source-Namen und mit Zeilennummer? **********/
        if (attr & R_FILI)
        {
            fileName = strrchr(msg->file, sm_pathTrenner);
            if (fileName)
                    ++fileName;                     /* fuehrenden \ entfernen */
            else
                    fileName = msg->file;           /* Pfad war nicht angegeben */
            if (!(attr & R_PATH) && fileName)       /***** nur Name ? *****/
                   msg->file = fileName;

            snprintf(filiBuf, sizeof(filiBuf), " (%s:%d) ", fileName,  msg->line);
        }


        if (attr & R_TRIM)
        {
          size_t endIdx = strlen(msg->text) - 1; // trim pdm va_debug.log
          if(msg->text[endIdx] == '\n')
            msg->text[endIdx] = '\0';

          severityString = "";
        }
        else
        {
          severityString = PrioString(prio, color);
        }


        /*************** mit eigener Prioritaet ? ************/
         if(attr & R_PRIO)
         {
            snprintf(prioBuf, sizeof(prioBuf), " %c%d ", GetCatChar(GetCatNr(attr)), prio);
         }

        /**************** with tread ? ***************/
        if (attr & R_THREAD)
        {
            snprintf(threadBuf, sizeof(threadBuf), "[%s] ", getThreadName(msg->threadId));
        }

        /********* with function/method name ? *****************/
        if (attr & R_FUNC)
        {
            func =  msg->func;
        }

        long nr = readCount + 1;
        snprintf(header, sizeof(header), "%s %ld: %s%s%s %s", msg->time, nr, prioBuf, threadBuf, severityString, filiBuf);



        /************** Append to file1 ? ************/
        if (attr & APP_FILE1)
        {
          file1 = writeToFile(file1, file1NameNeu, file1NameAlt, header, func, msg->text);
        }

        /************** Append to file2 ? ************/
        if (attr & APP_FILE2)
        {
          file2 = writeToFile(file2, file2NameNeu, file2NameAlt, header, func, msg->text);
        }

        /************** Write to DetailState History ************/
        if (attr & APP_HISTORY)
        {
            if(historyAppenderFkt != NULL)
            {
              Dprintf("APP_HISTORY call historyAppenderFkt for %s %s\n", AttributeToString(attr), msg->text);
              (*historyAppenderFkt)(attr, msg->file, msg->func, msg->text);
            }
            else
            {
               Dprintf("APP_HISTORY set but no historyAppenderFkt available\n");
            }
        }
        else
        {
          Dprintf("APP_HISTORY not selected for %s %s\n", AttributeToString(attr), msg->text);
        }

        /**************  Write to screen / JMX / http etc. ************/
        if ( attr & APP_SCREEN )
        {
            if (screenFkt)
            {
                (*screenFkt)(attr, header, sm_baseTicktime, func, msg->text);
            }
        }

        return ret;
}

//---------------------------------------------------------------------------------------------------------------------------------------
int flushReport (void)
{
    if(readCount == writeCount)
      return 0; // nothing new

    if(readCount > writeCount)
    {
      Dprintf("flushReport: something went wrong: readCount=%ld writeCount=%ld\n", readCount.load(), writeCount.load());DFLUSH;
      return -1; // something went wrong
    }

    Dprintf("flushReport: readCount=%ld writeCount=%ld throwAwayCount=%ld\n", readCount.load(), writeCount.load(), throwAwayCount);DFLUSH;

    while(readCount < writeCount)
    {
       RepLine * lineToRead = getRepLine(readCount.load() + 1); // look ahead
       if(lineToRead == NULL)
         break; // finished

       writeReport(lineToRead);

       lineToRead->processed();
       readCount++;
    }

    if(throwAwayCount > 0)
    {
      long n = throwAwayCount;
      throwAwayCount = 0;
      static char buf[50];
      snprintf(buf, sizeof(buf), "###### throw away %ld report entries ######", n);
      file1 = writeToFile(file1, file1NameNeu, file1NameAlt, "", "", buf);
    }

    CloseFile(file1);
    file1 = NULL;
    CloseFile(file2);
    file2 = NULL;

    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------------
int EndReport (void)
{
    int ret = flushReport();
    flushImmediately = true;

    file1 = OpenFile(file1, file1NameNeu);
    if ( file1 != NULL)
    {
      fputs("-------------------------------------- END --------------------------------------\n", file1);
      fclose(file1);
      file1 = NULL;
    }

    file2 = OpenFile(file2, file2NameNeu);
     if ( file2 != NULL)
     {
       fputs("-------------------------------------- END --------------------------------------\n", file2);
       fclose(file2);
       file2 = NULL;
     }

    return ret;
}


// *********************************END*****************************************

