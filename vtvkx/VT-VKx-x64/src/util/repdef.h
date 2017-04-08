#ifndef _repdef_h
#define _repdef_h
#include "report.h"

#include "util/what.h"
DEFINE_WHATINFO_HEADER;


/********************************************************************************/
/*                                                                              */
/*   headerfile for definition of report constants                              */
/*                                                                              */
/*     History:                                                                 */
/*              20.06.2016 copied from \migra\dvp\common\inc\repdef.h           */
/*                                                                              */
/*                                                                              */
/*   Each component should define its report constants only here,               */
/*   derive from TA_... constants and add only the category                      */
/*   ==> see examples below                                                     */
/*                                                                              */
/********************************************************************************/

// description report constants
// 0|1|2|3|4        : priority 0 = only outputs if system aborts or does not start, non-maskable
//                             1 = no outputs at startup, errors, infos with status character
//                             2 = limited outputs at startup, warnings, infos with event character, enabled per default
//                             3 = normal outputs, infos, disabled per default
//                             4 = many outputs, debug, disabled per default
// R_CAT('X')       : Category X

// TA_TRACE:  extremely intensive debug outputs,
#define TA_TRACE(cat) REPORT_LOCATION, (             APP_FILE1 |            R_FILI |          R_DATI | R_CYAN   | cat | 5)
// TA_DEBUG:  intensive debug outputs,
#define TA_DEBUG(cat) REPORT_LOCATION, (             APP_FILE1 |            R_FILI | R_FUNC | R_DATI | R_GRAY   | cat | 4)
// TA_INFO:  universal program infos,
#define TA_INFO(cat)  REPORT_LOCATION, (             APP_FILE1 |            R_FILI | R_FUNC | R_DATI | R_BLACK  | cat | 3)
// TA_EVENT: important event: e.g. command, order etc. (same prio as warnings)
#define TA_EVENT(cat) REPORT_LOCATION, (APP_SCREEN | APP_FILE1 | R_THREAD | R_FILI | R_FUNC | R_DATI | R_GREEN  | cat | 2)
// TA_WARNING: Warnings about unexpected situations etc.
#define TA_WARN(cat)  REPORT_LOCATION, (APP_SCREEN | APP_FILE1 | R_THREAD | R_FILI | R_FUNC | R_DATI | R_ORANGE | cat | 2)
// TA_STATE: important status changes: (same prio as errors)
#define TA_STATE(cat) REPORT_LOCATION, (APP_SCREEN | APP_FILE1 | R_THREAD | R_FILI | R_FUNC | R_DATI | R_BLUE   | cat | 1)
// TA_ERROR  Errors
#define TA_ERROR(cat) REPORT_LOCATION, (APP_SCREEN | APP_FILE1 | R_THREAD | R_FILI | R_FUNC | R_DATI | R_RED    | cat | 1)
// TA_FATAL: critical errors which causes abort or prevents startup of program/component
#define TA_FATAL(cat) REPORT_LOCATION, (APP_SCREEN | APP_FILE1 | R_THREAD | R_FILI | R_FUNC | R_DATI | R_MAGENTA| cat | 0)

// VA_DEBUG: output in second "va_debug" file
#define VA_DEBUG_FILE(cat) REPORT_LOCATION, (        APP_FILE2 |                              R_DATI | R_TRIM |   cat | 0)


// all categories in alphabetical order:

// -------------------- A API -----------------------------
#define API_TRACE          TA_TRACE(R_CAT('A'))
#define API_DEBUG          TA_DEBUG(R_CAT('A'))
#define API_INFO           TA_INFO(R_CAT('A'))
#define API_EVENT          TA_EVENT(R_CAT('A'))
#define API_WARNING        TA_WARN(R_CAT('A'))
#define API_STATE          TA_STATE(R_CAT('A'))
#define API_ERROR          TA_ERROR(R_CAT('A'))
#define API_FATAL          TA_FATAL(R_CAT('A'))

// -------------------- a for autoregression tests -------------------
#define AUTOREGR_TRACE          TA_TRACE(R_CAT('a'))
#define AUTOREGR_DEBUG          TA_DEBUG(R_CAT('a'))
#define AUTOREGR_INFO           TA_INFO(R_CAT('a'))
#define AUTOREGR_EVENT          TA_EVENT(R_CAT('a'))
#define AUTOREGR_WARNING        TA_WARN(R_CAT('a'))
#define AUTOREGR_ERROR          TA_ERROR(R_CAT('a'))
#define AUTOREGR_FATAL          TA_FATAL(R_CAT('a'))

// -------------------- B free ----------------------------
#define B_FREE_TRACE          TA_TRACE(R_CAT('B'))
#define B_FREE_DEBUG          TA_DEBUG(R_CAT('B'))
#define B_FREE_INFO           TA_INFO(R_CAT('B'))
#define B_FREE_EVENT          TA_EVENT(R_CAT('B'))
#define B_FREE_WARNING        TA_WARN(R_CAT('B'))
#define B_FREE_ERROR          TA_ERROR(R_CAT('B'))
#define B_FREE_FATAL          TA_FATAL(R_CAT('B'))

// -------------------- b free  ---------------------------------------
#define b_FREE_INFO           TA_INFO(R_CAT('b'))
#define b_FREE_EVENT          TA_EVENT(R_CAT('b'))
#define b_FREE_WARNING        TA_WARN(R_CAT('b'))
#define b_FREE_ERROR          TA_ERROR(R_CAT('b'))
#define b_FREE_FATAL          TA_FATAL(R_CAT('b'))

// -------------------- C Config ----------------------------------------------
#define CFG_DEBUG        TA_DEBUG(R_CAT('C'))
#define CFG_INFO         TA_INFO(R_CAT('C'))
#define CFG_EVENT        TA_EVENT(R_CAT('C'))
#define CFG_WARNING      TA_WARN(R_CAT('C'))
#define CFG_STATE        TA_STATE(R_CAT('C'))
#define CFG_ERROR        TA_ERROR(R_CAT('C'))
#define CFG_FATAL        TA_FATAL(R_CAT('C'))

// -------------------- c free  ---------------------------------------
#define c_FREE_INFO           TA_INFO(R_CAT('c'))
#define c_FREE_EVENT          TA_EVENT(R_CAT('c'))
#define c_FREE_WARNING        TA_WARN(R_CAT('c'))
#define c_FREE_ERROR          TA_ERROR(R_CAT('c'))
#define c_FREE_FATAL          TA_FATAL(R_CAT('c'))

// -------------------- D for Detector -----------------------------------------
#define DET_DEBUG        TA_DEBUG(R_CAT('D'))
#define DET_INFO         TA_INFO(R_CAT('D'))
#define DET_EVENT        TA_EVENT(R_CAT('D'))
#define DET_WARNING      TA_WARN(R_CAT('D'))
#define DET_ERROR        TA_ERROR(R_CAT('D'))
#define DET_FATAL        TA_FATAL(R_CAT('D'))

// -------------------- d free ------------------------------
#define d_FREE_INFO           TA_INFO(R_CAT('d'))
#define d_FREE_EVENT          TA_EVENT(R_CAT('d'))
#define d_FREE_WARNING        TA_WARN(R_CAT('d'))
#define d_FREE_ERROR          TA_ERROR(R_CAT('d'))
#define d_FREE_FATAL          TA_FATAL(R_CAT('d'))

// -------------------- E free ----------------------------------------------
#define E_FREE_INFO           TA_INFO(R_CAT('E'))
#define E_FREE_EVENT          TA_EVENT(R_CAT('E'))
#define E_FREE_WARNING        TA_WARN(R_CAT('E'))
#define E_FREE_ERROR          TA_ERROR(R_CAT('E'))
#define E_FREE_FATAL          TA_FATAL(R_CAT('E'))

// -------------------- e free  ----------------------------
#define e_FREE_INFO           TA_INFO(R_CAT('e'))
#define e_FREE_EVENT          TA_EVENT(R_CAT('e'))
#define e_FREE_WARNING        TA_WARN(R_CAT('e'))
#define e_FREE_ERROR          TA_ERROR(R_CAT('e'))
#define e_FREE_FATAL          TA_FATAL(R_CAT('e'))

// -------------------- F free -------------------------------
#define F_FREE_INFO           TA_INFO(R_CAT('F'))
#define F_FREE_EVENT          TA_EVENT(R_CAT('F'))
#define F_FREE_WARNING        TA_WARN(R_CAT('F'))
#define F_FREE_ERROR          TA_ERROR(R_CAT('F'))
#define F_FREE_FATAL          TA_FATAL(R_CAT('F'))

// -------------------- f free  ----------------------------
#define f_FREE_INFO           TA_INFO(R_CAT('f'))
#define f_FREE_EVENT          TA_EVENT(R_CAT('f'))
#define f_FREE_WARNING        TA_WARN(R_CAT('f'))
#define f_FREE_ERROR          TA_ERROR(R_CAT('f'))
#define f_FREE_FATAL          TA_FATAL(R_CAT('f'))

// -------------------- G free ----------------------------------------------
#define G_FREE_INFO           TA_INFO(R_CAT('G'))
#define G_FREE_EVENT          TA_EVENT(R_CAT('G'))
#define G_FREE_WARNING        TA_WARN(R_CAT('G'))
#define G_FREE_ERROR          TA_ERROR(R_CAT('G'))
#define G_FREE_FATAL          TA_FATAL(R_CAT('G'))

// -------------------- g Dialog  ----------------------------
#define DIALOG_INFO           TA_INFO(R_CAT('g'))
#define DIALOG_EVENT          TA_EVENT(R_CAT('g'))
#define DIALOG_WARNING        TA_WARN(R_CAT('g'))
#define DIALOG_ERROR          TA_ERROR(R_CAT('g'))
#define DIALOG_FATAL          TA_FATAL(R_CAT('g'))

// -------------------- H free ----------------------------------------------
#define H_FREE_INFO           TA_INFO(R_CAT('H'))
#define H_FREE_EVENT          TA_EVENT(R_CAT('H'))
#define H_FREE_WARNING        TA_WARN(R_CAT('H'))
#define H_FREE_ERROR          TA_ERROR(R_CAT('H'))
#define H_FREE_FATAL          TA_FATAL(R_CAT('H'))

// -------------------- h free  ----------------------------
#define h_FREE_INFO           TA_INFO(R_CAT('h'))
#define h_FREE_EVENT          TA_EVENT(R_CAT('h'))
#define h_FREE_WARNING        TA_WARN(R_CAT('h'))
#define h_FREE_ERROR          TA_ERROR(R_CAT('h'))
#define h_FREE_FATAL          TA_FATAL(R_CAT('h'))

// -------------------- I for Initializing -------------------
#define INIT_INFO        TA_INFO(R_CAT('I'))
#define INIT_EVENT       TA_EVENT(R_CAT('I'))
#define INIT_WARNING     TA_WARN(R_CAT('I'))
#define INIT_STATE       TA_STATE(R_CAT('I'))
#define INIT_ERROR       TA_ERROR(R_CAT('I'))
#define INIT_FATAL       TA_FATAL(R_CAT('I'))

// -------------------- i free  ----------------------------
#define i_FREE_INFO           TA_INFO(R_CAT('i'))
#define i_FREE_EVENT          TA_EVENT(R_CAT('i'))
#define i_FREE_WARNING        TA_WARN(R_CAT('i'))
#define i_FREE_ERROR          TA_ERROR(R_CAT('i'))
#define i_FREE_FATAL          TA_FATAL(R_CAT('i'))

// -------------------- J reserved for Jaut ------------------
#define JAUT_TRACE       TA_TRACE(R_CAT('J'))
#define JAUT_INFO        TA_INFO(R_CAT('J'))
#define JAUT_EVENT       TA_EVENT(R_CAT('J'))
#define JAUT_WARNING     TA_WARN(R_CAT('J'))
#define JAUT_ERROR       TA_ERROR(R_CAT('J'))
#define JAUT_FATAL       TA_FATAL(R_CAT('J'))

// -------------------- j free  ----------------------------
#define j_FREE_INFO           TA_INFO(R_CAT('j'))
#define j_FREE_EVENT          TA_EVENT(R_CAT('j'))
#define j_FREE_WARNING        TA_WARN(R_CAT('j'))
#define j_FREE_ERROR          TA_ERROR(R_CAT('j'))
#define j_FREE_FATAL          TA_FATAL(R_CAT('j'))

// -------------------- K free -----------------------------
#define K_FREE_INFO           TA_INFO(R_CAT('K'))
#define K_FREE_EVENT          TA_EVENT(R_CAT('K'))
#define K_FREE_WARNING        TA_WARN(R_CAT('K'))
#define K_FREE_ERROR          TA_ERROR(R_CAT('K'))
#define K_FREE_FATAL          TA_FATAL(R_CAT('K'))

// -------------------- k free  ----------------------------
#define k_FREE_INFO           TA_INFO(R_CAT('k'))
#define k_FREE_EVENT          TA_EVENT(R_CAT('k'))
#define k_FREE_WARNING        TA_WARN(R_CAT('k'))
#define k_FREE_ERROR          TA_ERROR(R_CAT('k'))
#define k_FREE_FATAL          TA_FATAL(R_CAT('k'))

// -------------------- L Lock (e.g. SwitchOff)  -------------------
#define LOCK_DEBUG          TA_DEBUG(R_CAT('L'))
#define LOCK_INFO           TA_INFO(R_CAT('L'))
#define LOCK_EVENT          TA_EVENT(R_CAT('L'))
#define LOCK_WARNING        TA_WARN(R_CAT('L'))
#define LOCK_ERROR          TA_ERROR(R_CAT('L'))
#define LOCK_FATAL          TA_FATAL(R_CAT('L'))

// -------------------- l free  ----------------------------
#define l_FREE_INFO           TA_INFO(R_CAT('l'))
#define l_FREE_EVENT          TA_EVENT(R_CAT('l'))
#define l_FREE_WARNING        TA_WARN(R_CAT('l'))
#define l_FREE_ERROR          TA_ERROR(R_CAT('l'))
#define l_FREE_FATAL          TA_FATAL(R_CAT('l'))

// -------------------- M reserved for MOTION ----------------
#define MOTION_INFO      TA_INFO(R_CAT('M'))
#define MOTION_EVENT     TA_EVENT(R_CAT('M'))
#define MOTION_WARNING   TA_WARN(R_CAT('M'))
#define MOTION_ERROR     TA_ERROR(R_CAT('M'))
#define MOTION_FATAL     TA_FATAL(R_CAT('M'))

// -------------------- m for state Messages -------------
#define MSG_TRACE        TA_TRACE(R_CAT('m'))
#define MSG_DEBUG        TA_DEBUG(R_CAT('m'))
#define MSG_INFO         TA_INFO(R_CAT('m'))
#define MSG_EVENT        TA_EVENT(R_CAT('m'))
#define MSG_WARNING      TA_WARN(R_CAT('m'))
#define MSG_STATE        TA_STATE(R_CAT('m'))
#define MSG_ERROR        TA_ERROR(R_CAT('m'))
#define MSG_FATAL        TA_FATAL(R_CAT('m'))

// -------------------- N free --------------------
#define NLS_DEBUG          TA_DEBUG(R_CAT('N'))
#define NLS_INFO           TA_INFO(R_CAT('N'))
#define NLS_EVENT          TA_EVENT(R_CAT('N'))
#define NLS_WARNING        TA_WARN(R_CAT('N'))
#define NLS_ERROR          TA_ERROR(R_CAT('N'))
#define NLS_FATAL          TA_FATAL(R_CAT('N'))

// -------------------- n free ---------------------
#define n_FREE_INFO           TA_INFO(R_CAT('n'))
#define n_FREE_EVENT          TA_EVENT(R_CAT('n'))
#define n_FREE_WARNING        TA_WARN(R_CAT('n'))
#define n_FREE_ERROR          TA_ERROR(R_CAT('n'))
#define n_FREE_FATAL          TA_FATAL(R_CAT('n'))

// -------------------- O reserved for OCIT ----------
#define OCIT_INFO        TA_INFO(R_CAT('O'))
#define OCIT_EVENT       TA_EVENT(R_CAT('O'))
#define OCIT_WARNING     TA_WARN(R_CAT('O'))
#define OCIT_ERROR       TA_ERROR(R_CAT('O'))
#define OCIT_FATAL       TA_FATAL(R_CAT('O'))

// -------------------- o for OeV-Speicher (PT Public Transport) -----------
#define OeV_INFO           TA_INFO(R_CAT('o'))
#define OeV_EVENT          TA_EVENT(R_CAT('o'))
#define OeV_WARNING        TA_WARN(R_CAT('o'))
#define OeV_ERROR          TA_ERROR(R_CAT('o'))
#define OeV_FATAL          TA_FATAL(R_CAT('o'))

#define PT_DEBUG          TA_DEBUG(R_CAT('o'))
#define PT_INFO           TA_INFO(R_CAT('o'))
#define PT_EVENT          TA_EVENT(R_CAT('o'))
#define PT_WARNING        TA_WARN(R_CAT('o'))
#define PT_ERROR          TA_ERROR(R_CAT('o'))
#define PT_FATAL          TA_FATAL(R_CAT('o'))

// -------------------- P for PDM --------------------
#define PDM_DEBUG          VA_DEBUG_FILE(R_CAT('P')|4)
#define PDM_INFO           VA_DEBUG_FILE(R_CAT('P')|3)
#define PDM_EVENT          VA_DEBUG_FILE(R_CAT('P')|R_GREEN|2)
#define PDM_WARNING        VA_DEBUG_FILE(R_CAT('P')|2)
#define PDM_ERROR          VA_DEBUG_FILE(R_CAT('P')|1)
#define PDM_FATAL          VA_DEBUG_FILE(R_CAT('P')|0)

// -------------------- p free ---------------------
#define p_FREE_INFO           TA_INFO(R_CAT('p'))
#define p_FREE_EVENT          TA_EVENT(R_CAT('p'))
#define p_FREE_WARNING        TA_WARN(R_CAT('p'))
#define p_FREE_ERROR          TA_ERROR(R_CAT('p'))
#define p_FREE_FATAL          TA_FATAL(R_CAT('p'))

// -------------------- Q free ---------------------
#define Q_FREE_INFO           TA_INFO(R_CAT('Q'))
#define Q_FREE_EVENT          TA_EVENT(R_CAT('Q'))
#define Q_FREE_WARNING        TA_WARN(R_CAT('Q'))
#define Q_FREE_ERROR          TA_ERROR(R_CAT('Q'))
#define Q_FREE_FATAL          TA_FATAL(R_CAT('Q'))

// -------------------- q free  ---------------------
#define q_FREE_INFO           TA_INFO(R_CAT('q'))
#define q_FREE_EVENT          TA_EVENT(R_CAT('q'))
#define q_FREE_WARNING        TA_WARN(R_CAT('q'))
#define q_FREE_ERROR          TA_ERROR(R_CAT('q'))
#define q_FREE_FATAL          TA_FATAL(R_CAT('q'))

// -------------------- R for Runmodes ---------------------
#define RUNMODE_DEBUG    TA_DEBUG(R_CAT('R'))
#define RUNMODE_INFO     TA_INFO(R_CAT('R'))
#define RUNMODE_EVENT    TA_EVENT(R_CAT('R'))
#define RUNMODE_WARNING  TA_WARN(R_CAT('R'))
#define RUNMODE_STATE    TA_STATE(R_CAT('R'))
#define RUNMODE_ERROR    TA_ERROR(R_CAT('R'))
#define RUNMODE_FATAL    TA_FATAL(R_CAT('R'))

// -------------------- r for RPC ------------------------
#define RPC_DEBUG          TA_DEBUG(R_CAT('r'))
#define RPC_INFO           TA_INFO(R_CAT('r'))
#define RPC_EVENT          TA_EVENT(R_CAT('r'))
#define RPC_WARNING        TA_WARN(R_CAT('r'))
#define RPC_ERROR          TA_ERROR(R_CAT('r'))
#define RPC_FATAL          TA_FATAL(R_CAT('r'))

// -------------------- S for signal groups ----------------
#define SIGNALGROUP_DEBUG     TA_DEBUG(R_CAT('S'))
#define SIGNALGROUP_INFO      TA_INFO(R_CAT('S'))
#define SIGNALGROUP_EVENT     TA_EVENT(R_CAT('S'))
#define SIGNALGROUP_WARNING   TA_WARN(R_CAT('S'))
#define SIGNALGROUP_ERROR     TA_ERROR(R_CAT('S'))
#define SIGNALGROUP_FATAL     TA_FATAL(R_CAT('S'))

// -------------------- s free -------------------------
#define s_FREE_INFO           TA_INFO(R_CAT('s'))
#define s_FREE_EVENT          TA_EVENT(R_CAT('s'))
#define s_FREE_WARNING        TA_WARN(R_CAT('s'))
#define s_FREE_ERROR          TA_ERROR(R_CAT('s'))
#define s_FREE_FATAL          TA_FATAL(R_CAT('s'))

// -------------------- T for general TAC  ------------------------
#define TAC_DEBUG        TA_DEBUG(R_CAT('T'))
#define TAC_INFO         TA_INFO(R_CAT('T'))
#define TAC_EVENT        TA_EVENT(R_CAT('T'))
#define TAC_WARNING      TA_WARN(R_CAT('T'))
#define TAC_STATE        TA_STATE(R_CAT('T'))
#define TAC_ERROR        TA_ERROR(R_CAT('T'))
#define TAC_FATAL        TA_FATAL(R_CAT('T'))

// -------------------- t for timeline, time  ------------------------
#define TIME_DEBUG          TA_DEBUG(R_CAT('t'))
#define TIME_INFO           TA_INFO(R_CAT('t'))
#define TIME_EVENT          TA_EVENT(R_CAT('t'))
#define TIME_WARNING        TA_WARN(R_CAT('t'))
#define TIME_ERROR          TA_ERROR(R_CAT('t'))
#define TIME_FATAL          TA_FATAL(R_CAT('t'))

// -------------------- U reserved for Ueberwacher -------
#define UEBW_INFO        TA_INFO(R_CAT('U'))
#define UEBW_EVENT       TA_EVENT(R_CAT('U'))
#define UEBW_WARNING     TA_WARN(R_CAT('U'))
#define UEBW_ERROR       TA_ERROR(R_CAT('U'))
#define UEBW_FATAL       TA_FATAL(R_CAT('U'))

// -------------------- u free  -------------------------
#define u_FREE_INFO           TA_INFO(R_CAT('u'))
#define u_FREE_EVENT          TA_EVENT(R_CAT('u'))
#define u_FREE_WARNING        TA_WARN(R_CAT('u'))
#define u_FREE_ERROR          TA_ERROR(R_CAT('u'))
#define u_FREE_FATAL          TA_FATAL(R_CAT('u'))

// -------------------- V for Device Variables ---------------------------
#define DEVVAR_DEBUG          TA_DEBUG(R_CAT('V'))
#define DEVVAR_INFO           TA_INFO(R_CAT('V'))
#define DEVVAR_EVENT          TA_EVENT(R_CAT('V'))
#define DEVVAR_WARNING        TA_WARN(R_CAT('V'))
#define DEVVAR_STATE          TA_STATE(R_CAT('V'))
#define DEVVAR_ERROR          TA_ERROR(R_CAT('V'))
#define DEVVAR_FATAL          TA_FATAL(R_CAT('V'))

// -------------------- v free ---------------------------
#define v_FREE_INFO           TA_INFO(R_CAT('v'))
#define v_FREE_EVENT          TA_EVENT(R_CAT('v'))
#define v_FREE_WARNING        TA_WARN(R_CAT('v'))
#define v_FREE_ERROR          TA_ERROR(R_CAT('v'))
#define v_FREE_FATAL          TA_FATAL(R_CAT('v'))

// -------------------- W  for AWP (Wrapper) -----------------------------
#define AWP_DEBUG          TA_DEBUG(R_CAT('W'))
#define AWP_INFO           TA_INFO(R_CAT('W'))
#define AWP_EVENT          TA_EVENT(R_CAT('W'))
#define AWP_WARNING        TA_WARN(R_CAT('W'))
#define AWP_ERROR          TA_ERROR(R_CAT('W'))
#define AWP_FATAL          TA_FATAL(R_CAT('W'))

// -------------------- w  free  ---------------------------
#define w_FREE_INFO           TA_INFO(R_CAT('w'))
#define w_FREE_EVENT          TA_EVENT(R_CAT('w'))
#define w_FREE_WARNING        TA_WARN(R_CAT('w'))
#define w_FREE_ERROR          TA_ERROR(R_CAT('w'))
#define w_FREE_FATAL          TA_FATAL(R_CAT('w'))

// -------------------- X free  ----------------------------
#define X_FREE_INFO           TA_INFO(R_CAT('X'))
#define X_FREE_EVENT          TA_EVENT(R_CAT('X'))
#define X_FREE_WARNING        TA_WARN(R_CAT('X'))
#define X_FREE_ERROR          TA_ERROR(R_CAT('X'))
#define X_FREE_FATAL          TA_FATAL(R_CAT('X'))

// -------------------- x free ----------------------------
#define x_FREE_INFO           TA_INFO(R_CAT('x'))
#define x_FREE_EVENT          TA_EVENT(R_CAT('x'))
#define x_FREE_WARNING        TA_WARN(R_CAT('x'))
#define x_FREE_ERROR          TA_ERROR(R_CAT('x'))
#define x_FREE_FATAL          TA_FATAL(R_CAT('x'))

// -------------------- Y Dummy ----------------------------
#define DUMMY_DEBUG          TA_DEBUG(R_CAT('Y'))
#define DUMMY_INFO           TA_INFO(R_CAT('Y'))
#define DUMMY_EVENT          TA_EVENT(R_CAT('Y'))
#define DUMMY_WARNING        TA_WARN(R_CAT('Y'))
#define DUMMY_STATE          TA_STATE(R_CAT('Y'))
#define DUMMY_ERROR          TA_ERROR(R_CAT('Y'))
#define DUMMY_FATAL          TA_FATAL(R_CAT('Y'))

// -------------------- y free  ----------------------------
#define y_FREE_INFO           TA_INFO(R_CAT('y'))
#define y_FREE_EVENT          TA_EVENT(R_CAT('y'))
#define y_FREE_WARNING        TA_WARN(R_CAT('y'))
#define y_FREE_ERROR          TA_ERROR(R_CAT('y'))
#define y_FREE_FATAL          TA_FATAL(R_CAT('y'))

// -------------------- Z free  -----------------------------
#define Z_FREE_INFO           TA_INFO(R_CAT('Z'))
#define Z_FREE_STATE          TA_STATE(R_CAT('Z'))
#define Z_FREE_EVENT          TA_EVENT(R_CAT('Z'))
#define Z_FREE_WARNING        TA_WARN(R_CAT('Z'))
#define Z_FREE_ERROR          TA_ERROR(R_CAT('Z'))
#define Z_FREE_FATAL          TA_FATAL(R_CAT('Z'))

// -------------------- z free  ----------------------------
#define z_FREE_INFO           TA_INFO(R_CAT('z'))
#define z_FREE_EVENT          TA_EVENT(R_CAT('z'))
#define z_FREE_WARNING        TA_WARN(R_CAT('z'))
#define z_FREE_ERROR          TA_ERROR(R_CAT('z'))
#define z_FREE_FATAL          TA_FATAL(R_CAT('z'))

// ----------------------------------------------------------
// called from http gui (JMX) for displaying available report categories
//
#ifdef __cplusplus
inline const char * getReportGroupDescription(char cat)
{
  switch(cat)
  {
    case 'A': return "API";
    case 'a': return "autoregression";
    case 'C': return "Config";
    case 'D': return "Detector";
    case 'g': return "Dialog";
    case 'I': return "Initializing";
    case 'L': return "Lock";
    case 'm': return "state messages";
    case 'N': return "NLS";
    case 'P': return "PDM";
    case 'o': return "oev-Speicher (PT)";
    case 'R': return "Run modes";
    case 'r': return "rpc";
    case 'S': return "Signal groups";
    case 'T': return "TAC";
    case 't': return "timeline / time";
    case 'V': return "Device Variables";
    case 'W': return "AWP (Wrapper)";
    case 'Y': return "DUMMY";

    case 'J': case 'M': case 'O': case 'U':
      return "reserved";
    default: return NULL; // for free categories
  }
}
#endif // __cplusplus

#endif
