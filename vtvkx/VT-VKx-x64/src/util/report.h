// *****************************************************************************
//
//     Projekt:     TA
//                  Siemens AG ZN-Muenchen
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
//     Modulname:   report.h
//
//     Date:        04.05.94
//     Version:     1.00
//
//     Logging Feature
//
//     History:
//              20.06.2016 copied from \migra\dvp\common\inc\report.h
//
/*
*/
// *****************************************************************************

#ifndef Report_H                             /* multiple include protection */
#define Report_H


#include "util/what.h"
DEFINE_WHATINFO_HEADER;


#ifdef __cplusplus
extern "C" {
#endif


#ifdef  __cplusplus
 #define __STDC_FORMAT_MACROS
 #include <inttypes.h>
 #include <stddef.h>   // int64_t

// #define PRId64 "lld"
// avoid yellow warning marker in eclipse:
#ifdef _WIN64
  #undef PRId64
  #define PRId64 "I64d"
#endif

#else
 typedef long long  int64_t;
 typedef int bool;
#endif
/*****************************************************************************
**                          Environment variables                           **
*****************************************************************************/
# define REPORT_EXPORT extern
//__declspec( dllexport )


/*****************************************************************************
**                       common defines                                     **
*****************************************************************************/
#define REPORT_LOCATION __FILE__,__LINE__,__FUNCTION__  /* location information   */

#define R_LEVEL_ENV       -1     /* Level defined from environment          */
#define R_RESTORE_ALL     -1     /* reset all attributes                    */
#define R_LOG_ENV         NULL   /* Name of log file from environment       */
#define R_TRACE_ENV       NULL   /* Name of trace file from environment     */
#define R_APPEND   0x80000000L   /* output is appended to <log/trace>.log at start  */

/*****************************************************************************
**                           Colors                                         **
*****************************************************************************/
#ifndef _COLORS_
#define Ri_BLACK          0
#define Ri_RED            1
#define Ri_GREEN          2
#define Ri_BLUE           3
#define Ri_MAGENTA        4
#define Ri_ORANGE         5
#define Ri_CYAN           6
#define Ri_GRAY           7
#endif
                           
/*****************************************************************************
**                   Structures of Report functions                         **
*****************************************************************************/

typedef void (*t_fscreenprint) (long attr, const char * time, int64_t ticks, const char * func, const char * text);
REPORT_EXPORT void SetScreenPrint(t_fscreenprint fsp);

typedef void (*t_fHistoryAppender) (long attr, const char * file, const char * func, const char* text);
REPORT_EXPORT void SetHistoryAppender(t_fHistoryAppender fha);


/*****************************************************************************
**                   Prototypes of the Report functions                     **
*****************************************************************************/

REPORT_EXPORT const char * IniReport (const char *taskname,
                              int         prio,  
                              const char *logname,   
                              long        log_anzahl,
                              const char *tracename,
                              long        trace_anzahl);

/* "Erste" Funktion eines Programms/einer Task                              */
/*  taskname: Name der Task oder des Programms                              */
/*  prio:     Einzustellende Prioritaet fuer diese Task                     */
/*  logname:  Name der Logdatei ohne Extension oder NULL ->                 */
/*            Name der Datei steht im Environment unter dem Namen LOGBUCH   */
/*  log_anzahl: Maximale Laenge der Logdatei in Bytes ( | R_APPEND )        */
/*  tracename:  Name der Tracedatei ohne Extension oder NULL ->             */
/*              Name der Datei steht im Environment unter dem Namen TRACE   */
/*  trace_anzahl: Maximale Laenge der Tracedatei ( | R_APPEND)              */
const char *GetReportLabel();

REPORT_EXPORT long AttrOnReport      (long attr); /* global enable of the attribute   */
REPORT_EXPORT long AttrOffReport     (long attr); /* global disable of the attribute  */
REPORT_EXPORT long AttrRestoreReport (long attr); /* restore of the local attributes  */
                                    /* also: R_RESTORE_ALL for all attributes */

REPORT_EXPORT const char * AttributeToString(long attr);
REPORT_EXPORT const char * ColorString(int color);
REPORT_EXPORT long         AttributValue  (const char *attrs);
 
REPORT_EXPORT const char * LevelReport    (const char *cat_level);
REPORT_EXPORT int          LevelReportFromCat(char cat);  /* ret = 0, 1, 2, 3 */
REPORT_EXPORT const char * getPersistedLevels();
REPORT_EXPORT bool         persistLevelReport();

REPORT_EXPORT void setCurrentThreadName(const char * threadname);
REPORT_EXPORT void setCurrentReportTicks(int64_t baseTicktime);
REPORT_EXPORT int64_t getCurrentReportTicks();

REPORT_EXPORT int         Report (const char *file, int line, const char *function, long attr, const char *format, ...)
#if defined (__GNUG__) || defined (__GNUC__)
__attribute__ ((format (printf, 5, 6)))
#endif
;
int  flushReport(void);
int  EndReport(void);


/*****************************************************************************
**         Avoid calculation of parameters if not needed                    **
*****************************************************************************/
// ReportConditional

#define  ReportC(_attr)  if(Report(_attr, 0))Report(_attr

// Avoid calculation of parameters if prio of cat doesn't match level
// recommended for frequently used Reports with prio 3
// example:
// before:   Report (ABC_INFO , "Info=%s", BigCalc() );   BigCalc() is executed,
//                                                        even if there is no Report output
//
// after:    ReportC(ABC_INFO), "Info=%s", BigCalc() );   BigCalc() is not executed,
//                                                        if there is no Report output
//
// memory aid: parenthesize attribute with pair of "brackets" 'C' and ')' .


#define  isReportEnabledFor(_attr)  (Report(_attr, 0)>0)

/*****************************************************************************
**                 suppress report showers                                  **
*****************************************************************************/
#define BEGIN_SUPPRESS_COUNT_SHOWER(x) {\
      long _t = time(0);\
      static long _tt = 0;\
      static long _SuppressedCount = 1;\
      if(_t-_tt < x)\
        _SuppressedCount++; \
      else\
      { _tt = _t;

#define END_SUPPRESS_COUNT_SHOWER  _SuppressedCount = 1; }  }

#define BEGIN_SUPPRESS_SHOWER(x) {\
      long _t = time(0);\
      static long _tt = 0;\
      if(_t-_tt >= x)\
      { _tt = _t;

#define END_SUPPRESS_SHOWER  }  }

// example:
//
//    BEGIN_SUPPRESS_SHOWER(60)   // Report max once per minute (60sec)
//    {
//      Report(ERROR, "error occurred");
//    }
//    END_SUPPRESS_SHOWER  
//
// with counting of suppressed messages:
//   
//    BEGIN_SUPPRESS_COUNT_SHOWER(60)   // Report max once per minute (60sec)
//    {
//      Report(ERROR, "error occurred %d-times", _SuppressedCount);
//    }
//    END_SUPPRESS_COUNT_SHOWER     


/*****************************************************************************
**                               Attributes                                 **
*****************************************************************************/

/* Level/Priority:       Bits  0 -  2  -->  Priorities from 0 - 7          */
#define Ri_GET_PRIO(x)  ((int)((x)&0x00000007))
#define Ri_GET_CAT(x)   (((int)((x)&0x000001F8))>>3)
#define Ri_INDIVIDUELL  0x0010 /* Bit 5 : internal flag for category prio  */
/*                      Bits  3 - 8     category of prio                   */
#define Ri_CAT(g)     ((g>='A'&&g<='Z'?(g-'A'+1)<<3:(g>='a'&&g<='z'?(g-'a'+27)<<3:0)) )
// HP Compiler cannot Characterize Operator !!!
//#define R_SET_PRIO(g,p) (Ri_CAT(#@g) | ( p & 0x03))
//#define R_CAT(g)      (Ri_CAT(#@g))
//#define R_SET_PRIOxxx(g,p) (Ri_CAT(g) | ( p & 0x07))
#define R_CAT(g)        (Ri_CAT(g))

/* colors:              Bits  9 - 11                                       */
/* set colors:          Bit  12                                            */
#define Ri_SET_COL      0x00002000L                     /* only intern !!! */
#define Ri_SHIFT_COL    9                               /* only intern !!! */
//#define Ri_COL(x)       (((long)(x)<<Ri_SHIFT_COL)|Ri_SET_COL)  /* "       */
#define Ri_COL(x)        (((long)((x)&0x07)<<Ri_SHIFT_COL))       /* "       */
#define R_EXTRACT_COL(x) ((int)((x)&(0x07<<Ri_SHIFT_COL)))        /* "       */

#define R_BLACK          Ri_COL(Ri_BLACK)
#define R_RED            Ri_COL(Ri_RED)
#define R_GREEN          Ri_COL(Ri_GREEN)
#define R_BLUE           Ri_COL(Ri_BLUE)
#define R_MAGENTA        Ri_COL(Ri_MAGENTA)
#define R_ORANGE         Ri_COL(Ri_ORANGE)
#define R_CYAN           Ri_COL(Ri_CYAN)
#define R_GRAY           Ri_COL(Ri_GRAY)
/* reserved:            Bits 13 - 15                                                */

#define Ri_GET_FLAGS(x) ((x)&0xFFFF0000)     /* intern !                            */

// appenders
#define APP_SCREEN       0x00010000          /* Write to screen (http gui)          */
#define APP_PRINTER      0x00020000          /* Write to printer  ?                 */
#define APP_MBOX         0x00040000          /* Write to GUI message box  ?         */
#define APP_WTRACE       0x00080000          /* Write to Windows-TRACE   ?          */
#define APP_SYSLOG       0x00080000          /* Write to syslog          ?          */

#define APP_FILE1        0x00100000          /* Write to file 1                     */
#define APP_FILE2        0x00200000          /* Write to file 2                     */

#define APP_BEMA         0x00400000          /* Write to Scala/BEMA                 */
#define APP_HISTORY      0x00400000          /* Write to DetailState-History        */

// attributes
#define R_THREAD         0x00800000          /* with thread id                      */

#define R_DATE           0x01000000          /* with Date                           */
#define R_TIME           0x02000000          /* with Time                           */
#define R_DATI           (R_DATE|R_TIME)     /* with Date + Time                    */
#define R_TRIM           0x04000000          /* reduced output                      */
#define R_FILI           0x08000000          /* with source file and line number    */

#define R_FUNC           0x10000000          /* with name of calling function       */
#define R_ERRNO          0x20000000          /* with 'errno'  ?                     */
#define R_PRIO           0x40000000          /* with own category and prio          */
#define R_PATH           0x80000000          /* with full path at R_FILE            */
  
#ifdef __cplusplus
}
#endif

#endif
// ********************************END******************************************

