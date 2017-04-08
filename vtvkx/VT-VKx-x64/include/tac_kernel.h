#ifndef _TAC_KERNEL_H
#define _TAC_KERNEL_H

/*! \mainpage TAC interface
 *
 * TAC interface is a C interface to the controller firmware. It can be used do implement custom
 * traffic actuation solutions.
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

//TODO: for now we include def_ger.h because of PACKING macros
#include "def_ger.h"

/* data types and max values of the data types */
typedef unsigned char  UTINY;   // 8 Bit (unsigned)
typedef unsigned char  BYTE;
typedef unsigned char  UBYTE;   // used in ocit
typedef signed   char  STINY;   // 8 Bit (signed)

typedef unsigned char  BOOLEAN; // TRUE/FALSE

typedef unsigned short USHORT;  // 16 Bit Integer (unsigned)
typedef unsigned short WORD;
typedef signed   short SSHORT;  // 16 Bit Integer (signed)

#ifdef WIN32
typedef unsigned long  ULONG;   // 32 Bit Integer (unsigned)
typedef unsigned long  DWORD;
typedef signed   long  SLONG;   // 32 Bit Integer (signed)
#else
typedef uint32_t  ULONG;   // 32 Bit Integer (unsigned)
typedef uint32_t  DWORD;
typedef int32_t   SLONG;   // 32 Bit Integer (signed)
#endif

#define MAX_UTINY    (UTINY)0xFF
#define MAX_UBYTE    (UBYTE)0xFF
#define MAX_STINY    (STINY)0x7F

#define MAX_USHORT  (USHORT)0xFFFF

#define MAX_SSHORT  (SSHORT)0x7FFF
#define MAX_SLONG    (SLONG)0x7FFFFFFF
#define MAX_ULONG    (ULONG)0xFFFFFFFF

/*
 * enum for TaRunmode
 */
typedef enum
{
    TARM_OFF,      //!< TA should shut down -> replaces LastCall
    TARM_REDUCED,  //!< IT-modification should be off and PT-modification should be off
    TARM_IT,       //!< IT-modification should be on and the PT-modification should be off
    TARM_PT,       //!< IT-modification should be off and the PT-modification should be on
    TARM_FULLY     //!< IT-modification should be on and the PT-modification should be on
} TaRunmode;

/** enum for operator */
typedef enum
{
    TAC_OP_TA,      //!< traffic actuation
    TAC_OP_GUI,     //!< GUI (pc)
    TAC_OP_RPC,     //!< remote pc
    TAC_OP_CENTER,  //!< control center
    TAC_OP_JAUT,    //!< JAUT
    TAC_OP_BAZ,     //!< BAZ
    TAC_OP_MAX_OP   //!< max. possible operator (always last entry)
} TacOperator;

/** enum for main state */
typedef enum
{
    TAC_MS_OFF,                         //!< state = off (e.g. yellow flashing) ->  from end off-switching sequence until start of first on-switching sequence
    TAC_MS_ERR_OFF,                     //!< switched off due to fatal program error
    TAC_MS_LOCAL_SELECTED_SP,           //!< state = fixed time/TA, signalplan from local control/Jaut is running
    TAC_MS_CENTER_SELECTED_SP,          //!< state = fixed time/TA signalplan from control center is running
    TAC_MS_MANUAL_CONTROL_CYCLIC,       //!< state = manual control cyclic
    TAC_MS_MANUAL_CONTROL_ACYCLIC,      //!< state = manual control acyclic is running -> manual stage is signalized, no stage transition is running, !!state is currently not suported by sX-controller 
    TAC_MS_MA_CO_AC_TRANSITION,         //!< state = stage transition by acyclic manual control is running, !!state is currently not suported by sX-controller
    TAC_MS_SP_CHANGE_GSP,               //!< state = signal program change by GSP method, from the request of signalplan changing until GSP. Also by changing to cyclic manual control, if a signalplan changing is necessary
    TAC_MS_SYNC_GSP,                    //!< state = synchronization by GSP method, -> when controller should run synchonized and is currently not synchronized. The state could be set for several cycletimes.(dependent to the parameter 'number of cycletimes until controller is synchronized)
    TAC_MS_PI_ON_OFF_WHILE_FIXED_TIME,  //!< state = fixed time control and the (de-)activation routine is for at least 1 part intersection running, also if the complete intersection is switching on or off
    TAC_MS_PI_ON_OFF_WHILE_MC_ACYCLIC,  //!< state = manual control acyclic and the (de-)activation routine is for at least 1 part intersection running -> state is currently not suported by sX-controller.
    TAC_MS_CHANGE_TO_FIXED_TIME_CONTROL,        //!< state = Changing to fixed time mode / TA from modes like all-red, acycl. Manual control -> state is currently not suported by sX-controller.
    TAC_MS_CHANGE_TO_MANUAL_CONTROL_ACYCLIC,    //!< state = change to manual control acyclic -> state is currently not suported by sX-controller.
    TAC_MS_SI_FIRE,                             //!< state = special intervention fire brigade (number see: eSpecialIntervention)
    TAC_MS_SP_CHANGE_STRETCH,                   //!< state = signal program change by STRETCH method, From request of signalplan changing until it is finished at the STRETCH-area. -> state is currently not suported by sX-controller
    TAC_MS_SYNC_STRETCH,                        //!< state = synchronization by STRETCH method. From request of signalplan changing until it is finished at the STRETCH-area. -> state is currently not suported by sX-controller
    TAC_MS_DELAY_TIME,                          //!< state = delay time is running (after power restored)
    TAC_MS_RECOVER_FROM_EMERGENCY_MODE,         //!< state = firmware takes over control from emergency mode (CBC controls)
    TAC_MS_CHANGE_TO_ALL_RED,                   //!< state = change to all red running, not (yet) possible -> state is currently not suported by sX-controller
    TAC_MS_ALL_RED,                             //!< state = all red is running, not (yet) possible -> state is currently not suported by sX-controller
    TAC_MS_SP_CHANGE_INTERGREEN                 //!< state = signal program change by intergreen time method, -> state is currently not suported by sX-controller
} TacMainState;

/** enum for state of partial intersections */
typedef enum
{
    TAC_PI_NOT_USED,                              //!< pi not configured
    TAC_PI_NOT_ACTIVE,                            //!< pi is switched off (i.e. yellow flashing, dark, ...) by operator
    TAC_PI_ERROR_OFF,                             //!< pi is off because an error occurred
    TAC_PI_WAITING_FOR_START_ACTIVATION_ROUTINE,  //!< pi is in off-mode, should switch on synchronized and is waiting till activation routine starts
    TAC_PI_ACTIVATION_ROUTINE,                    //!< activation routine for the pi is running
    TAC_PI_ACTIVE,                                //!< pi is running
    TAC_PI_WAITING_FOR_DEACTIVATION_TIME,         //!< pi is in on-mode, should switch off and is waiting till deactivation time is reached
    TAC_PI_DEACTIVATION_ROUTINE,                  //!< deactivation routine for the pi is running
} TacPIState;

/** enum for requested partial intersection state */
typedef enum
{
    /** partial intersections not used. */
    TAC_PIRQ_NOT_USED=0,
    /**
     * sub-intersection is to be switched on to signal program defined by object TargetSignalProgram and for all active sub
     * intersections applies respectively: signal program defined by ActualSignalProgram is executed
     */
    TAC_PIRQ_ON=1,
    /**
     * sub-intersection is to be switched off default and for all active sub intersections applies respectively: signal
     * groups chosen by configuration are flashing normally vehicle signal groups of secondary direction, the remaining
     * signal groups are dark
     */
    TAC_PIRQ_OFF_DEFAULT=2,
    /**
     * sub-intersection is to be switched off flashing secondary direction and for all active sub
     * intersections applies respectively: vehicle signal goups of secondary direction are flashing, the remaining signal
     * groups are dark "Aus-Blinken".
     */
    TAC_PIRQ_OFF_FLASH_SECONDARY_DIRECTION=3,
    /**
     * sub-intersection is to be switched off dark and all signal groups of active sub intersections are dark
     * respectively "Aus-Dunkel".
     */
    TAC_PIRQ_OFF_DARK=4,
    /**
     * sub-intersection is to be switched off flashing all and all signal groups of
     * active sub intersections are flashing respectively " AUS-Zustand Alles Gelb-Blinken ".
     */
    TAC_PIRQ_OFF_FLASH_ALL=5,
    /**
     * partial intersection must be shut down due to a pending signal monitoring error.
     * set by firmware only !
     */
    TAC_PIRQ_FAULT_SHUTDOWN=6,
    /**
     * requested partial intersection state does not differ (semantically) from actual partial intersection state
     * set by firmware only !
     */
    TAC_PIRQ_MATCH_ACTUAL_STATE=7
} TacPIRqState;

/** enum for special intervention */
typedef enum
{
    TAC_SI_NOT_ACTIVE,    //!< no special intervention is running
    TAC_SI_FIRE_1,        //!< special intervention fire brigade 1 is running
    TAC_SI_FIRE_2,        //!< special intervention fire brigade 2 is running
    TAC_SI_FIRE_3,        //!< special intervention fire brigade 3 is running
    TAC_SI_RAILWAY_FULLY, //!< railway intervention "fully intervention" is running
    TAC_SI_RAILWAY_PART   //!< railway intervention "part intervention" is running
} TacSpecialIntervention;

/** enum for requested synchronisation */
typedef enum
{
    TAC_RS_NO,         //!< no synchronisation requested
    TAC_RS_GSP,        //!< synchronisation at GSP requested
    TAC_RS_STRETCH = 3 //!< synchronisation by STRETCH requested, state is currently not suported by sX-controller
} TacRequestedSync;

/** enum for lock state (value) */
typedef enum
{
    TAC_LS_DO_NOT_CARE,    //!< don't care, used when lock should keept current value
    TAC_LS_LOCK,           //!< locked
    TAC_LS_UNLOCK          //!< unlocked
} TacLockState;

/** Public Transport telegram source */
typedef enum
{
    TAC_RES_AFD = 0, //< Local PT telegram receiver board AFD
    TAC_RES_Det = 1, //< Local detector for emergency checkin
    TAC_RES_ITCS = 2, //< Intermodal transport control system (formerly RBL) or AVLS (Automatic Vehicle Location System); request received via control center.
    TAC_RES_RideManager = 3, //!< Forced check-out event by ride manager in case of a missing check-out event.
    TAC_RES_SimGui = 4, //!< Event create by GUI for simulation.
    TAC_RES_Ta=5,       //!< Event created by traffic actuated control.
    TAC_RES_unknown = 255, //!< None of the above defined values.
} TacRideEventSource;

/** Specifies kind of AMLI check-out event */
typedef enum
{
    /** Event is a check in event.<br>
     * red-green-transition=0; green-red-transition=0
     */
    TAC_ACOR_CheckIn = 0,
    /** Public transport signal group at red state before first check-in event and green-red-transition after check-out event.<br>
     * red-green-transition=[1..253]; green-red-transition=[1..253]
     */
    TAC_ACOR_OK = 1,
    /** Check out event received after green-red-transition and configured red time of public transport signal groups.<br>
     * red-green-transition=0; green-red-transition=0
     */
    TAC_ACOR_TooLate = 2,
    /** Check out event while intersection was switched off.<br>
     * red-green-transition=0; green-red-transition=255
     */
    TAC_ACOR_Off = 3,
    /** Check out event received during activation routine with public transport signal group at green-state and before
     * green-red-transition.<br>
     * red-green-transition=255; green-red-transition=[1..253]
     */
    TAC_ACOR_SwitchOnOk = 4,
    /** Check out event received during activation routine does not affect change of public transport signal group, i.e no
     * green-red-transition 15s after this event.<br>
     * red-green-transition=255; green-red-transition=254
     */
    TAC_ACOR_SwitchOnNA = 5,
    /** Public transport signal group at green state before first check-in event and at least 15s after check-out event.<br>
     * red-green-transition=254; green-red-transition=254
     */
    TAC_ACOR_NotAffected = 6,
    /** Public transport signal group at green state before first check-in event and green-red-transition after check-out event.<br>
     * red-green-transition=254; green-red-transition=[1..253]
     */
    TAC_ACOR_StageExtended = 7,
    /** Public transport signal group at green state before first check-in event and after check-out event of next ride received.<br>
     * red-green-transition=254; green-red-transition=0
     */
    TAC_ACOR_CheckOutByNextNA = 8,
    /** Public transport signal group at red state before first check-in event and at green state least 15s after check-out event.<br>
     * red-green-transition=[1..253]; green-red-transition=254
     */
    TAC_ACOR_StageUnmodified = 9,
    /** Public transport signal group at red state before first check-in event and check-out event of next ride received.<br>
     * red-green-transition=[1..253]; green-red-transition=0
     */
    TAC_ACOR_CheckOutByNext = 10,
    /** None of the above defined values. */
    TAC_ACOR_unknown = 255
} TacAmliCheckOutType;

/** Type of AMLI event */
typedef enum
{
    TAC_AET_CheckIn = 0, //!< Normal check-in event
    TAC_AET_CheckOutRaw = 1, //!< Check out event without AMLI green-red-state state transition.
    TAC_AET_CheckOutAmli = 2, //!< Check out event including AMLI green-red-state state transition.
    TAC_AET_EmergencyCheckIn = 3, //!< Emergency check-in event.
    TAC_AET_ForcedCheckOut = 4, //!< Forced check-out event.
    /** Public transport request was received by traffic actuation but blocked by a high-priority request. Time stamp of
     * event is the time the TA received the high-priority request.
     */
    TAC_AET_Blocked = 5,
    TAC_AET_unknown = 15, //!< None of the above defined values.
} TacAmliEventType;


/**
 * fuer neue Funktion 'DetectorRead' (ersetzt bisherige Funktion 'DetektorLesen')
 */
typedef struct
{
    USHORT wDetIdx;         //!< Logische Detektornummer
    USHORT wError;          //!< Detektorstoerung (Defines siehe oben DetStoeGeraet)
    USHORT wQuality;        //!< Qualitaet
    USHORT wAU;             //!< aktueller Detektorzustand
    USHORT wZS;             //!< Zaehlwert
    USHORT wBU;             //!< Summe der Belegtzeiten
    USHORT wBA;             //!< geglaettete Belegtzeitensumme
    USHORT wBS;             //!< sekuendlicher Belegtwert
    USHORT wSS;             //!< Stichproben Summenwert
    USHORT wLS;             //!< laufende Luecke
    USHORT wLU;             //!< letzte abgeschlossene Luecke
    USHORT wLA;             //!< geglaettete Luecke
    USHORT wA;              //!< Puffer Original Zaehlwert (MEXWA-Intervall)
    USHORT wB;              //!< Mittlere Verkehrsstaerke (MEXWA-Intervall)
    USHORT wC;              //!< Puffer Original Belegungswert (MEXWA-Intervall)
    USHORT wD;              //!< Mittlere Belegungszeit (MEXWA-Intervall)
    USHORT wHoldTime;       //!< Belegungsdauer durch das Fahrzeug in 10ms
    USHORT wLastGap;        //!< Letzte Luecke der Schleife vor dem Fahrzeug in 10ms
    USHORT wDriveDuration;  //!< Fahrdauer von Schleife A->B in ms
    USHORT wSpeed;          //!< Geschwindigkeit des Fahrzeugs in km/h
    USHORT wVehLength;      //!< Laenge des Fahrzeugs in 0,1 m
    USHORT wVehType;        //!< Typ des Fahrzeugs (nach TLS)
    USHORT wStatus;         //!< Statusbyte 0=aktuell, >0 Alter des Fzg. In Sekunden
} DET_MEASURED_DATA; //TODO: rename to Tac naming convention?

/** time representation */
typedef struct TacDateTime {
    int second;   //!< seconds after the Minute 0-61
    int minute;   //!< minutes after the hour 0-59
    int hour;     //!< hours since midnight 0-23
    int day;      //!< day of the month 1-31
    int month;    //!< months since January 0-11
    int year;     //!< years since 1900
    int wday;     //!< the number of days since Sunday 0..6
    int reserved[2];
} TacDateTime;

#define MAX_TKR                     4   // Maximale Anzahl an Teilkreuzungen
#define MAX_PARTIAL_INTERSECTIONS MAX_TKR    //!< maximum partial inteserctions

//by Valentin
typedef enum
{   TAC_IS_NONE = 0,
    TAC_IS_ON = 1,
    TAC_IS_OFF_DEFAULT = 2,
    TAC_IS_OFF_FLASH_SECONDARY_DIRECTION = 3,
    TAC_IS_OFF_DARK = 4,
    TAC_IS_OFF_FLASH_ALL = 5,
    TAC_IS_MANUAL_CYCLIC = 6,
    TAC_IS_MANUAL_ACYCLIC = 7,
    TAC_IS_FATAL_ERROR = 8,
    TAC_IS_RECOVER_FROM_EMERGENCY_MODE = 9,
} TacIState;

//by Valentin
typedef enum
{
    TAC_IOM_SPECIAL = 0,
    TAC_IOM_TRAFFIC_ACUATED = 1,
    TAC_IOM_MANUAL_STOP = 2,
    TAC_IOM_LOCAL_FIX = 3,
    TAC_IOM_LOCAL_TIME_SCHEDULE = 4,
    TAC_IOM_CONTROL_CENTER = 5,
} TacIOpMode;

//by Valentin
typedef enum
{
    TAC_RIOP_RMNS_ON = 1,
    TAC_RIOP_OFF_DEFAULT = 2,
    TAC_RIOP_OFF_FLASH_SECONDARY_DIRECTION = 3,
    TAC_RIOP_OFF_DARK = 4,
    TAC_RIOP_OFF_FLASH_ALL = 5,
    TAC_RIOP_MANUAL_CYCLIC = 6,
    TAC_RIOP_MANUAL_ACYCLIC = 7,
} TacReqIOpMode;

/** This structure contains all basic information about intersection state.
 *
 * It can by retrieved by calling tacGetOperatingState(). */
typedef struct
{
    SSHORT       wTX;                 //!< unit = 100msec, 0 to (cycle time - 10), value = (-1) if controller is switched off
    TacOperator  eOperator;           //!< actual operator  ToDo: still necessary ?
    TacMainState eMainState;          //!< resulting state for complete controller (can be different to the state of the part intersections)
    TacIState    eIState;             //Intersection state
    TacIOpMode   eIOpMode;            //Intersection operational mode
    TacReqIOpMode   eReqIOpMode;      //Requested intersection operational mode
    TacPIState   ePIState[MAX_PARTIAL_INTERSECTIONS]; //!< actual state for each part intersection
    SSHORT       wSpNrActual;         //!< number of the actual running signal program, During the off-state the number from the last signalplan is inserted. When the switch-on starts(waiting for sync. switch-on or first second of switch-on sequence) the number of the signalplan which should run is insert. range from 1 to MAX_SIGNAL_PROGRAMS
    SSHORT       wSpNrRequested;      //!< number of the current requested signal program, from recognising the new signalplan request until the signalplan is running (normally the GSP from the new signalplan), range from 1 to MAX_SIGNAL_PROGRAMS, -1 if no signal program is requested
    TacSpecialIntervention eSpecialIntervention;  //!< state is currently not suported by sX-controller, 0 = special intervention is not running, 1 = special intervention firebrigade plan 1 is running, 2 = special intervention firebrigade plan 2 is running, 3 = special intervention firebrigade plan 3 is running, 4 = special intervention railway complett influenceing is running, 5 = special intervention railway partial influenceing is running
    SSHORT       wGsp;                //!< 1 if tx is at GSP (true is remaining while controller is waiting for sync. at GSP), 0 if tx is not at GSP 
    TacRequestedSync  eRequestedSync; //!< displays if a synchronisation is requested (controller is/should run in synchronization mode and TX is unequal to refli) and which kind of synchronisation is requested
    SSHORT       wStageNr;            //!< number of current manual stage if bMainState = AL_AI_MST_PH or AL_AI_MST_CHANGE_TO_PH, range: 0 to x, 0 if manual stage control is not active
    SSHORT       wStageNrRequested;   //!< number of requested manual stage if bMainState = AL_AI_MST_PH_WECHSEL, range: 0 to x, 0 if no stage transition is running or manual stage control is not active
    TacPIRqState ePIStateRequested[MAX_PARTIAL_INTERSECTIONS]; //!< requested state for each part intersection
} TacOperatingState;

/** structure representing TA lock request */
typedef struct {
    TacLockState lockPI[MAX_PARTIAL_INTERSECTIONS];     //!< state of switch-off lock for each partial intersection
} TacLockSwitchOff;

/** archive key values of a single archive DTO */
typedef struct {
    long long timeStamp; //!< time difference measured in milliseconds, elapsed since midnight, January 1, 1970 UTC.
    long long tickTimeOfLastChange; //!< CBC Device tick time in ms, since last start of CBC when one of these common states has changed.
    long long primaryKey; //!< Archive primary key value
} TacArchiveKeys;

/** Public transport telegram */
typedef struct {
    USHORT  reportingPointNo; //!< reporting point number
    USHORT  lineNo; //!< line number (e.g 9)
    USHORT  destinationNo; //!< destination number (e.g. = encoding of Ceska)
    USHORT  runNo;  //!< run number (e.g. corresponding to 1815)
    USHORT  priority; //!< Priority (e.g. 0=none, 1=normal, 2=high, 3=highest)
    USHORT  vehicleLength; //!< Vehicle Length  ==0 if None provided
    USHORT  manualDirection;    //!< Requested direction (e.g. 0=default)
    SSHORT  imbalance; //!< Imbalance in seconds >0 too late; <0 too early (or the other way round, dependent on the customer)
    USHORT  ptDirectionNo; //!< Number of configured public transport direction
    TacRideEventSource  source; //!< not yet needed/provided by OevSpeicher
    SSHORT  reserved1; //!< Reserved
    SSHORT  reserved2; //!< Reserved
    SSHORT  reserved3; //!< Reserved
} TacRideEvent;

/** A data transfer object returned by archive requests. It includes (the archive DTO keys and) the AMLI public transport events. */
typedef struct {
    TacDateTime dateTime; //!< local time of processing/creation of AMLi event
    TacRideEvent telegram; //!< PT telegram
    int greenTime; //!< At check-in: Time difference in milliseconds between required green state and check-in event.
    BOOLEAN ptModActive; //!< Flag indicating the public transport modification of the traffic actaution is runnig, or not.
    int tx; //!< Cycle time counter value when the ride event was received. (unit ms, starting with 0)
    int signalProgramNo; //< Number of running signal program
    int actualStage; //!< actual stage
    int nextStage; //!< requested stage
    /** At check-in event: estimated traveltime in milliseconds from check-in to check-out reporting point number.
     * At check-out event: time difference between check-out event and last check-in events in milliseconds.
     */
    int travelTime;
    int txGNS; //!< Cycle time counter value at begin of green state. (unit ms, starting with 0)
    int txGNE; //!< Cycle time counter value at end of green state. (unit ms, starting with 0)
    int amliTimeout; //!< At AMLI check-out: Time difference in milliseconds between end of green state and check-out event.
    TacAmliEventType type; //!< Type of AMLI event
    TacAmliCheckOutType checkOutSubType; //!< Sub type of AMLI check-out event default=TAC_ACOR_unknown
    int signalGroupNo;  //!< number of influenced Signal Group
} TacAmliDto;

// retrieve inventory ta parameter blob.
// @param pData address where pointer to param blob is stored
//              memory responsibility is passed to the caller,
//              if caller does not need memory anymore, it has to be released with GibSpeicherFrei
// @param nData address where function stores number of parameter bytes read.
// @return == 0 OK,
//         != 0 Error, no data read, *pData, *nData untouched.
int tacGetComponentParams(void ** pData, unsigned * nData);
/**
 *  retrieve draft ta parameter blob. Only successful while a config transaction is in progress.
 *  Use this function to access the draft parameter blob within the check() implementation (see ta_checkConfig_t).
 *  @param pData address where pointer to param blob is stored
 *              memory responsibility is passed to the caller,
 *              if caller does not need memory anymore, it has to be released with GibSpeicherFrei
 *  @param nData address where function stores number of parameter bytes read.
 *  @return == 0 OK,
 *          != 0 Error, no data read, *pData, *nData untouched.
 */
int tacGetDraftComponentParams(void ** pData, unsigned * nData);

/**
 * copy date time based on last received signal indication into *tdt
 * @param tdt address of result date time
 * @return SUCCESS, NO_SUCCESS
 */
SLONG tacGetDateTime(TacDateTime *tdt);

/**
 * Get requested (by firmware) TA running mode
 *
 * @return    TA running mode
 */
TaRunmode tacGetRequestTaRunmode(void);

/**
 * Get requested (by firmware) TA running mode
 *
 * In general tacGetModifiedRequestTaRunmode() is equal to tacGetRequestedTaRunmode() with one exception:
 * If TA is running in ::TARM_FULLY/::TARM_IT/::TARM_PT mode and requested status is ::TARM_OFF, the function getModifiedRequestTaRunmode() returns ::TARM_REDUCED as (modified) requested mode as long as TA is also returning ::TARM_REDUCED as current state.
 * When TA finally returns ::TARM_REDUCED mode, function forwards original requested mode ::TARM_OFF to TA when called.
 * (TA can not be switched from ::TARM_FULLY to ::TARM_OFF directly, but ::TARM_FULLY via ::TARM_REDUCED to ::TARM_OFF).
 *
 * @return    TA (modified) running mode
 */
TaRunmode tacGetModifiedRequestTaRunmode(void);

void tacSetResponseTaRunmode(TaRunmode rm);

/** this function is used to request different types of lock in firmware
 *
 * @param state requested state of switch off lock
 * @return SUCCESS, ERROR_PARAM, NO_SUCCESS
 */
SLONG tacSetLockSwitchOff(TacLockSwitchOff *state);

/** Insert Extended AMLI event to Firmware archive
 *
 * @param amliEx Extendewm AMLI DTO to send to firmware archive
 * @return SUCCESS, ERROR_PARAM, NO_SUCCESS
 */
SLONG tacInsertAmliEventToArchive(TacAmliDto *amli);

/** Gets public transport telegram from firmware
 *
 * @param telegram pointer caller allocated structure for telegram
 * @return SUCCESS, NO_SUCCESS in case there is no telegram
 */
SLONG tacGetRideEvent(TacRideEvent *telegram);

/** release control of TA (after reaching GSP)
 *
 * @param void
 * @return SUCCESS, NO_SUCCESS
 */
SLONG tacReleaseControl();

/** set several version informations (optional)
 * for use in GUI, report-file and autoregression log
 *
 * @param const char *key e.g. "TL", "PDM", "TacKernel"
 * @param const char *value e.g. compile time
 * @return SUCCESS, NO_SUCCESS
 */
SLONG setVersion (const char * key, const char * value);
#define setCompileTime(k) setVersion(k"CompileTime", __DATE__ " " __TIME__);

SLONG tacGetOperatingState (TacOperatingState *psTacOpState); // replaces "BetriebszustandLesen"

//---- start of config notification interface (impl at api_Config.cpp) ----

/** forward deklaration, class that collects and converts messages. */
struct TacMsgList;

enum TacMsgSeverity
{
  TAC_MS_OK,    //<! to be used with informations if used with status marked with green symbol
  TAC_MS_HINT,  //<! to be used with hints if used with status marked with blue symbol
  TAC_MS_WARN,  //<! to be used with warnings if used with status marked with yellow symbol
  TAC_MS_ERROR  //<! to be used with error if used with status marked with orange symbol
};

enum TacCheckConfigResult
{
  TAC_CR_NOT_AFFECTED,  //<! config consumer was not affected by the configuration to check.
  TAC_CR_ON_THE_FLY,    //<! config consumer is able to activate the configuration to check on the fly, that is without restart or reset.
  TAC_CR_TA_RESTART,    //<! TA needs restart to operate with new config
  /** config consumer has detected a problem inside the configuration to check that makes it impossible for this config consumer to operate with it.
    * returning this value prevents transition from draft to iunventory of the configuration to check. */
  TAC_CR_REFUSE
};

/** address type of configuration. To be used as bit field */
enum TacConfigType
{
        TAC_CT_BASE = 1,        //<! address the traffic related configuration (hlc_base).
        TAC_CT_SYSTEM = 2,      //<! address the system related configuration (hlc_system).
        TAC_CT_TACODE = 4,      //<! address the traffic actuation control code.
        TAC_CT_TAPARAM = 8,     //<! address the traffic actuation control parameter.
};

// add a message to list pMsgList
// @param pMsgList handle of list, passed to ta_checkConfig_t
// @param severity of the message
// @param format printf style, this format string is used as key for nls translation at firmware.
// @param further parameter according to format string
void tacMsgListAddMsg(struct TacMsgList* pMsgList, enum TacMsgSeverity severity, const char * format, ...)
#if defined (__GNUG__) || defined (__GNUC__)
__attribute__ ((format (printf, 3, 4)))
#endif
;

// check new draft configuration, to be implemented by TA.
// to pass an associated message, call e.g. invoke:
//  const char * missingParameterName;
//  int lineNo;
//  ...
//  tacMsgListAddMsg(pMsgList, TACMS_ERROR, "missing Parameter %s at line %d", missingParameterName, lineNo);
//
// @param configTypeSet bitwise ored values of enum TacConfigType
// @param pMsgList handle for use by tacMsgListAddMsg to add some messages
// @return result of check by TA
typedef enum TacCheckConfigResult (*ta_checkConfig_t)(unsigned configTypeSet, struct TacMsgList *pMsgList);

// activates (new) configuration for a registered consumer
// activation is only done if new configuration has been accepted by consumer before.
typedef void (*ta_activateConfig_t)();

// clean up a previous checkConfig call if transaction has been canceled (by user)
// or it was activated
typedef void (*ta_cleanupConfig_t)();

// register config notification callback functions.
// the TA may do this typically at AWP_init
// @param configTypeSet bitwise ored values of enum TacConfigType, that tac shall register for.
// @param fcheck function to be called by tac kernel if a new configuration will be loaded
// @param factivate notification that new configuration is being activated
// @param fcleanup notification that current configuration transaction is finished
void tacRegisterConfigNotifications(unsigned configTypeSet, ta_checkConfig_t fcheck, ta_activateConfig_t factivate, ta_cleanupConfig_t fcleanup);

//---- end of config notification interface ----
/* >>>>>>>>>>>>>>> alternative for Geraetestammdaten */
/** currently known intersection controller types */
enum TacCfgIntersectionControllerType {
  TAC_ICT_UNKNOWN=0,    //!< controller type not known, abnormal situation
  TAC_ICT_C_10_L_61=61, //!< low voltage (40V) controller with a maximum of 32 signal groups
  TAC_ICT_C_10_H_62=62, //!< high voltage (230V) controller with a maximum of 32 signal groups
  TAC_ICT_C_10_LX_63=63,//!< low voltage (40V) controller with more than 32 signal groups
  TAC_ICT_C_10_HX_64=64,//!< high voltage (230V) controller with more than 32 signal groups
  TAC_ICT_C_10_V_65=65, //!< very low voltage (24V) controller with a maximum of 32 signal groups
  TAC_ICT_C_10_VX_66=66 //!< very low voltage (24V) controller with more than 32 signal groups
};

/** DTO to provide general configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfg_8}. */
typedef struct TacCfgGeneralParam
{
    enum TacCfgIntersectionControllerType controllerType; //!< controller type we are running on
    char      name[32]; //!< base configuration: ShortName
    int       tccNo;  //!< trafic control center number.
    int       guiNo;  //!< intersection controler number, unique for this tcc.
    int       fnr;    //!< field device number (in case of OCIT, CANTO) -1 if not defined
    int       systemPulse_ms; //!< interval between successive TA invocations in ms valid range: 100..1000
    char      centerType[16];   //!< control center connection type, empty string in case there is no control center connection.
}TacCfgGeneralParam;

/**
 * @param isDraft if !=0 the draft configuration is accessed, only valid inside config transaction,
 *                   ==0 the inventory configuration is accessed (should work always)
 * @param pGeneralParam pointer to caller allocated TacCfgGeneralParam, the function copies the values into the structure.
 * @return SUCCESS general parameters are filled in
 *         else    parameters not filled completely
 */
SLONG tacCfgGetGeneralParam(int isDraft, TacCfgGeneralParam *pGeneralParam);
/* <<<<<<<<<<<<<<< alternative for Geraetestammdaten */

// FIXME: begin of old API stuff
//
#define MAX_SIGNALPLAENE           32   // Maximale Anzahl an Signalplaenen
#define MAX_SONDERPLAENE            5   // Maximale Anzahl an Sondersignalplaenen
#define MAX_GEBER                   4   // max possible number of devices for each signal group
#define MAX_ZZ_MATRIZEN             3   // Maximale Anzahl an Zwischenzeitmatrizen
#define MAX_HRP                     8   // Maximale Anzahl an Hauptrastpunkten
#define MAX_FO                      8   // Maximale Anzahl an Fortschaltpunkten
#define MAX_ZEITEN_PRO_UMLAUF       4   // Maximale Anzahl an Signalwechselzeiten pro Umlauf (SIGRUDEFS)
#define MAX_STRETCHBEREICHE         8   // Maximale Anzahl an Koordinierungsbereichen fuer das Stretchverfahren
#define GEBER_TEXT_LEN              2   // max. Laenge fuer eine Geberbezeichnung (Anwenderbezeichnung)

#define MAX_SIGNALGRUPPEN          96
#define MAX_VALID_FBKOM            16   // max count of possible different colors and color combinations for each signal group
#define STD_TEXT                   34   // max. Laenge  eines Standard Textes fuer OTEC kompatible Bezeichnungen
#define KURZ_TEXT                  10   // max. Laenge  eines kurzen Textes z.B. Anwenderbezeichnung

/**
 * Mindestdauern (VD_MINDESTFREIGABELISTE, VD_MINDESTSPERRLISTE)
 * Definition von Mindestzeiten
 */
typedef PACKED_STRUCT
{
    SSHORT wDauerStandard;  //!< Dauer der Mindestsperr- bzw. Freigabezeit im Normalfall [ZEIT: 1, 0.1][DEF: 0][MIN:0][MAX:MAX_MINZEIT_DAUER]
    SSHORT wDauerSpeziell;  //!< Dauer der Mindestsperr- bzw. Freigabezeit in Verbindung mit einer spez. Signalisierung [DEF: 0][ZEIT: 1, 0.1][MIN:0][MAX:MAX_MINZEIT_DAUER]
    UTINY  bFarbeSpeziell;  //!< spezielle Farbkombination [DATAOBJ: VD_FARBKOMBINATIONEN][DEF: MAX_UTINY][MIN: 0] [MAX: MAX_VALID_FBKOM - 1]
    UTINY  bZwangsfolge;    //!< zwingende Frei- bzw. Sperrfolge [DEF: NEIN][REF: CB0026]
} MINDAUER;

/**
 * List of minimum red/green times
 */
typedef PACKED_STRUCT
{
        char        aName[STD_TEXT];   //!< Bezeichnung der Mindestdauerliste [ASCII][NULLT:][MAX:32][INSTANZNAME:]
        MINDAUER    sDauer[MAX_SIGNALGRUPPEN];  //!< index is signal group number -1
} MINDAUERLISTE;

// mindauer related defines
#define MAX_MINZEIT_DAUER        1200   // Maximal zulaessige Mindestsperrzeit

//TODO: convert to enums? whatabout ERROR_* defines?
// ToDo: hier muss ein Abgleich erfolgen, Werte aus BBX-Firmware
#define SUCCESS         (ULONG)0x00
#define NO_SUCCESS      (ULONG)0x0000ffff
#define ERROR_PARAM     ((SLONG)-13)                // bei falschen Parametern
#define ERROR_PARAMETER     2                       // Fehlerh. Parameter uebergeben (z.B. ungueltige TaskID)

//TODO: convert to enums?
// Returnwert -> DetStoeGeraet
#define DET_OK             0   // Detektor ist OK
#define DET_ERR_BELEGUNG  13   // Detektor ist fehlerhaft aufgrund von Belegungsueberwachung
#define DET_ERR_LUECKE    14   // Detektor ist fehlerhaft aufgrund von Lueckenueberwachung
#define DET_ERR_HW        15   // Detektor ist fehlerhaft aufgrund von Hardwareueberwachung
#define DET_NOT_USED     255   // Detektor ist nicht versorgt

//TODO: convert to enums?
// bArt -> DetektorSetzen
#define DET_VALUE_ZS 1
#define DET_VALUE_SS 8
#define DET_VALUE_LS 10
#define DET_VALUE_LU 12

//TODO: convert to enums? which functions should use this?
// error values
#define ERROR_ZAEHLERART                 ((SLONG)-1)     // bei ungueltiger Zaehlerart
#define ERROR_IN_PSOS                    ((SLONG)-4)
#define ERROR_POINTER_BEREICH            ((SLONG)-7)     // bei ungueltigem Speicherbereich
#define ERROR_BEREICHSPARAMETER          ((SLONG)-11)    // bei Fehler in wVonDetNr bzw. wBisDetNr
#define ERROR_DETEKTORART                ((SLONG)-12)    // bei ungueltiger bArt
#define ERROR_PARAM                      ((SLONG)-13)    // bei falschen Parametern
#define ERROR_SIG_UNBENUTZT              ((SLONG)-15)    // bei ungueltiger Signalgruppe
#define ERROR_TKR_FALSCHER_ZUSTAND       ((SLONG)-17)
#define ERROR_NO_MEMORY                  ((SLONG)-19)    // wenn kein Speicher mehr frei
#define ERROR_ZERO_SIZE                  ((SLONG)-20)    // wenn in lGroesse NULL angegebn wurde
#define ERROR_IN_API                     ((SLONG)-24)    // wenn Fehler direkt in API - Funktion

// bArt -> DetektorLesen -> wird bei neuer Funktion DetektorRead nicht mehr benoetigt
//TODO: these are not used in tac. should be?
#define DET_VALUES_ALL      0xff
#define DET_VALUE_SER_ALL   0xfb

//TODO: convert to enums?
#define DET_VALUE_AU      0
#define DET_VALUE_ZS      1
#define DET_VALUE_BU      2
#define DET_VALUE_BA      4
#define DET_VALUE_BS      6
#define DET_VALUE_SS      8
#define DET_VALUE_LS     10
#define DET_VALUE_LU     12
#define DET_VALUE_LA     14
#define DET_VALUE_A      16
#define DET_VALUE_B      18
#define DET_VALUE_C      20
#define DET_VALUE_D      22

//TODO: rename to Tac convention?
extern SLONG DetectorRead (USHORT wFromDetNo, USHORT wToDetNo, DET_MEASURED_DATA* pResultArray); // -> replaces DetektorLesen
extern SLONG DetectorValueSet (USHORT wDetNo, UTINY bTyp, USHORT wNewValue);                     // -> replaces DetektorwertSetzen

/**
 * Eckdaten (VD_ECKDATEN)
 */
typedef struct
  {
     UTINY bCfgValid;          //!< 0: Konfiguration wurde geaendert 255: Versorgungseingriff aktiv  -> nicht bei Lizenzierung
     UTINY bAnzSG;             //!< count of signal group instances
     UTINY bMaxSgNo;           //!< greatest signal group number
     UTINY bAnzTK;             //!< count of hardware part intersections                             -> nicht bei Lizenzierung
     UTINY bMaxTkNo;           //!< greatest of hardware part intersection number                    -> nicht bei Lizenzierung
     UTINY bAnzSipl;           //!< count of signal plan instances                                   -> nicht bei Lizenzierung
     UTINY bMaxSplNo;          //!< greatest signal plan number                                      -> nicht bei Lizenzierung
     UTINY bInputCnt;          //!< count of detector instances (0-250)
     UTINY bMaxInputNo;        //!< greatest detector number    (0-250)
     UTINY bOutputCnt;         //!< count of output instances   (0-148)
     UTINY bMaxOutputNo;       //!< greatest output number      (0-148)
     UTINY bAnzPhasen;         //!< count of stage instances                                         -> nicht bei Lizenzierung
     UTINY bMaxStageNo;        //!< greatest stage number                                            -> nicht bei Lizenzierung
     UTINY bAnzPhasenUeb;      //!< count of stage transition instances                              -> nicht bei Lizenzierung
     UTINY bMaxStageTrans;     //!< greatest stage transition number                                 -> nicht bei Lizenzierung
     UTINY bAnzZZMatrizen;     //!< count of intergreen time matrices                                -> nicht bei Lizenzierung
     UTINY bMaxZZMatrizenNo;   //!< greatest number of intergreen time matrices                      -> nicht bei Lizenzierung
     UTINY bAnzVZBBMatrizen;   //!< count of offset time matrices begin begin                        -> nicht bei Lizenzierung
     UTINY bMaxVZBBMatrizenNo; //!< greatest number of offset time matrices begin begin              -> nicht bei Lizenzierung
     UTINY bAnzVZEEMatrizen;   //!< count of offset time matrices end end                            -> nicht bei Lizenzierung
     UTINY bMaxVZEEMatrizenNo; //!< greatest number of offset time matrices end end                  -> nicht bei Lizenzierung
     UTINY bAnzEFolgen;        //!< count of switch on sequences                                     -> nicht bei Lizenzierung
     UTINY bMaxSwitchOnSequNo; //!< greatest switch on sequence number                               -> nicht bei Lizenzierung
     UTINY bAnzAFolgen;        //!< count of switch off sequences                                    -> nicht bei Lizenzierung
     UTINY bMaxSwitchOffSequNo;//!< greatest switch off sequence number                              -> nicht bei Lizenzierung
     UTINY bAnzUeseqSF;        //!< count of transition sequences (stop to free) instances           -> nicht bei Lizenzierung
     UTINY bMaxUeseqSFNo;      //!< greatest number of transition sequences (stop to free)           -> nicht bei Lizenzierung
     UTINY bAnzUeseqFS;        //!< count of transition sequences (free to stop) instances           -> nicht bei Lizenzierung
     UTINY bMaxUeseqFS;        //!< greatest number of transition sequences (free to stop)           -> nicht bei Lizenzierung
     UTINY bAnzFarben;         //!< maximale Anzahl Farbkombinationen                                -> nicht bei Lizenzierung
     SSHORT bSiplUmlaufzeit[MAX_SIGNALPLAENE];  //!< Umlaufzeiten pro Signalplan                     -> nicht bei Lizenzierung
     SSHORT bFeuerwehrplaene[MAX_SONDERPLAENE]; //!< Umlaufzeiten pro Sonderplan                     -> nicht bei Lizenzierung
  } CONFIG_BUF;

typedef PACKED_STRUCT
  {
    UTINY color;                  //!< OCIT colorcode
    UTINY stateInfo;              //!< Zustandsinfo ueber die versorgten Farbindexe auf die Signalgruppe bezogen [REF: CB0309][DEF: LZ_UNZULAESSIG]
  } SG_ColorInfo;

/**
 * Signalgruppenparameter (VD_SIG_BESCHREIBUNG)
 */
typedef PACKED_UNION
  {
    char aName[MAX_GEBER][GEBER_TEXT_LEN]; //!< 1,5,21 und 25 vom System belegt, bei Plustechnik frei 1 und 21

    PACKED_STRUCT
    {
      char bGeber1[GEBER_TEXT_LEN]; //!< Geberbezeichnung Geber 1[ASCII:]
      char bGeber2[GEBER_TEXT_LEN]; //!< Geberbezeichnung Geber 2[ASCII:]
      char bGeber3[GEBER_TEXT_LEN]; //!< Geberbezeichnung Geber 3[ASCII:]
      char bGeber4[GEBER_TEXT_LEN]; //!< Geberbezeichnung Geber 4[ASCII:]
    }sGeberBezName;
  } GEBER_BEZ;

typedef PACKED_STRUCT
  {
    GEBER_BEZ	aGeber;
    char bGeber_Rot_3[KURZ_TEXT];   //!< Kanal ohne Rotstromueberwachung (nicht bei Plustechnik)[ASCII:]
  } ALL_GEBER_BEZ;

typedef PACKED_UNION
  {
    UTINY    bSigOutput;  //!< Struktur als Byte
    PACKED_STRUCT
      {
        UTINY       bUnused : 5;  //!< Bitfeld (Bit (3-7) derzeit nicht benutzt
        UTINY       bRed    : 1;  //!< 1: Roter Signalausgang ist benutzt [DEF: 1]
        UTINY       bAmber  : 1;  //!< 1: Gelber Signalausgang ist benutzt [DEF: 1]
        UTINY       bGreen  : 1;  //!< 1: Gruener Signalausgang ist genutzt [DEF: 1]
      } sSigOutput;
  } SIGOUT_PATTERN;

/**
 * Aenderungen zur Anpassung an die erfassungstechnische Sicht
 */
typedef PACKED_STRUCT
  {
    char  aBezeichnung[STD_TEXT];      //!< Anwenderbezeichnung der Signalgruppe[ASCII][NULLT:][MAX:32]
    UTINY bTkrZuordnung;               //!< Zuordnung der Signalgruppe zur Teilkreuzung[REF: CB0043][DEF: NICHTVORHANDEN]
    UTINY bTyp;                        //!< Typ der Signalgruppe [DEF: OHNE_TYP] [REF: CB0045]
    UTINY bRichtung;                   //!< Richtung der Signalgruppe [DEF: HAUPTRICHTUNG ][REF: CB0303]
    UTINY bOptUebergang;               //!< Signalgruppe soll am optimalen Uebergang teilnehmen [DEF: JA] [REF: CB0026]
    UTINY bAllesRotBild;               //!< Alles Rot: Zustand der SG im Alles Rot Fall [DEF: FK_ROT ] [MIN: 0] [MAX: MAX_VALID_FBKOM - 1]
    UTINY bAllesGelbBild;              //!< Alles Gelb: Zustand der SG im Alles Gelb Fall [DEF: FK_GELB ] [MIN: 0] [MAX: MAX_VALID_FBKOM - 1]
    UTINY bAusGelbBlnkBild;            //!< Aus Blinken: Zustand der SG im Zustand Aus Gelb blinken[DEF: FK_DUNKEL][MIN: 0] [MAX: MAX_VALID_FBKOM - 1]
    UTINY bAusDunkelBild;              //!< Aus Dunkel: Zustand der SG im Zustand Aus Dunkel [DEF: FK_DUNKEL] [MIN: 0] [MAX: MAX_VALID_FBKOM - 1]
    UTINY bAusAllesGelbBild;           //!< Sonder Aus Blinken: Zustand der SG im Zustand Aus Alles Gelb[DEF: FK_DUNKEL] [MIN: 0] [MAX: MAX_VALID_FBKOM - 1]
    SIGOUT_PATTERN bGenutzteAusg;      //!< Ausgaenge (Rot-Gelb-Gruen) die wirklich genutzt werden [DEF: 7]
    ALL_GEBER_BEZ  sKopf;              //!< Geberbezeichnungen der Signalgruppe
    UTINY nColorInfo;                  //!< number of valid following entries
    SG_ColorInfo   sVTZustand[MAX_VALID_FBKOM]; //!< Zustandsinfo fuer die versorgten Farben, bezogen auf die Signalgruppe
  } SIGGRUPPE;

typedef PACKED_STRUCT
 {
  SSHORT wZeit[MAX_ZEITEN_PRO_UMLAUF];      /*!< Zeitpunkt, zu dem die Signalgruppe die Signalisierung, die durch Farbe definiert ist,
    beendet in 100ms, 0..TU-1 [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF] */
  UTINY  bFarbe[MAX_ZEITEN_PRO_UMLAUF];     //!< zugehoerige Signalisierung [DEF: MAX_UTINY][MIN: 0][MAX: MAX_VALID_FBKOM - 1]
 } SIGRUDEFS;      // enthaelt alle Signalwechselzeitpunkte fuer den Signalplan

/**
 * (Sonder-)Signalplaene (VD_SIGNALPLAN, VD_FEUERWEHRPLAN, VD_BUESTRAPLAN)
 */
typedef PACKED_STRUCT
 {
  SSHORT wStauchpunkt;  //!< Sprungpunkt zum Ueberspringen eines Bereichs im Signalplan [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF]
  SSHORT wDehnpunkt;    //!< Bezugspunkt fuer das Stauchen und Dehnen des Signalplans [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF]
  USHORT wFaktor;       //!< Faktor fuer Verteilung der Dehnung [DEF: 0][MIN: 1][MAX: 5]
 } STRETCHBEREICHE;

typedef PACKED_STRUCT
  {
    char   aSignalplanName[STD_TEXT]; //!< VSR - Bezeichnung des Signalplans [ASCII][NULLT:][MAX:32]
    SSHORT wEzp;                      //!< Einschaltzeitpunkt fuer komplette Kreuzung [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF ]
    SSHORT wAzp;                      //!< Ausschaltzeitpunkt fuer komplette Kreuzung [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF ]
    SSHORT wGsp;                      //!< Guenstigster Schaltpunkt [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF ]
    SSHORT wTu;                       //!< Umlaufzeit [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF + 10 ]
    SSHORT wVersatz;                  //!< Versatz des Signalplans gegeueber der Referenzzeit [MIN: 0][MAX: MAX_UMLAUF + 10 ]
    SSHORT wVersion;                  //!< Version des Signalplans [DEF: 1][MIN:0][MAX:999]
    SSHORT wGsyv;                     //!< Gruppensynchronisier-Vorrast [DEF: MAX_SSHORT][MIN:0][MAX: MAX_UMLAUF ]
    SSHORT wGsyh;                     //!< Gruppensynchronisier-Hauptrast [DEF: MAX_SSHORT][MIN:0][MAX: MAX_UMLAUF ]
    SSHORT wZsyv;                     //!< Zentralensynchronisier-Vorrast [DEF: MAX_SSHORT][MIN:0][MAX: MAX_UMLAUF ]
    SSHORT wZsyh;                     //!< Zentralensynchronisier-Hauptrast [DEF: MAX_SSHORT][MIN:0][MAX: MAX_UMLAUF]
    UTINY bSiplNr;                    //!< Dem Plan Zugeordnete Zentralenplannummer [DEF: 0][MIN: 0][MAX: MAX_UTINY ]
    UTINY bReserve;                   //!< Fuellbyte [DEF: 0][MIN: 0][MAX: MAX_UTINY ]
    UTINY bNrMinFreigabeListe;        //!< Nummer der Mindestfreigabezeitliste [DEF: 0][MIN: 0] [MAX: MAX_MINDESTFREIGABELISTEN - 1]
    UTINY bNrMinSperrListe;           //!< Nummer der Mindestsperrliste [DEF: 0][MIN: 0] [MAX: MAX_MINDESTSPERRLISTEN - 1]
    UTINY bESBFolge;                  //!< Nummer der EAbilderfolge fuer globales Einschalten [DEF: 0][MIN: 0] [MAX: MAX_EAPLAENE - 1]
    UTINY bASBFolge;                  //!< Nummer der EAbilderfolge fuer globales Ausschalten [DEF: 0][MIN: 0] [MAX: MAX_EAPLAENE - 1]
    UTINY bFS_Nr;                     //!< Nummer der aktuellen Uebergangssequenzen Freigeben --> Sperren [DEF: 0][MIN: 0] [MAX: MAX_UESEQ_KOMBINATIONEN - 1]
    UTINY bSF_Nr;                     //!< Nummer der aktuellen Uebergangssequenzen Sperren --> Freigeben [DEF: 0][MIN: 0] [MAX: MAX_UESEQ_KOMBINATIONEN - 1]
    UTINY bZZ_Nr;                     //!< Nummer der fuer den Signalplan gueltigen ZZ-Matrix [DEF: 0][MIN: 0] [MAX: MAX_ZZ_MATRIZEN-1]
    UTINY bVZ_NrBB;                   //!< Nummer der fuer den Signalplan gueltigen VZ-Matrix [DEF: MAX_UTINY][MIN: 0] [MAX: MAX_VZ_MATRIZEN-1]
    UTINY bVZ_NrEE;                   //!< Nummer der fuer den Signalplan gueltigen VZ-Matrix [DEF: MAX_UTINY][MIN: 0] [MAX: MAX_VZ_MATRIZEN-1]
    UTINY bZplNr;                     //!< Zentralenplannummer wird bei SV  - Plaenen im System generiert [DEF: MAX_UTINY][MIN: 0][MAX: 99]
    UTINY bStretchBerAnzahl;          //!< Definition der Bereiche fuer das Stretch-Verfahren 0 = nicht versorgt [DEF: 0] [MIN: 1] [MAX: MAX_STRETCHBEREICHE]
    SSHORT wHrpv[MAX_HRP];            //!< Hand-Vorrastpunkte [DEF: MAX_SSHORT] [MIN: 0] [MAX: MAX_UMLAUF ]
    SSHORT wHrph[MAX_HRP];            //!< Hand-Hauptrastpunkte [DEF: MAX_SSHORT] [MIN: 0] [MAX: MAX_UMLAUF]
    SSHORT wZfov[MAX_FO];             //!< FO-Vorrastpunkte [DEF: MAX_SSHORT] [MIN: 0] [MAX: MAX_UMLAUF ]
    SSHORT wZfoh[MAX_FO];             //!< FO-Hauptrastpunkte [DEF: MAX_SSHORT] [MIN: 0] [MAX: MAX_UMLAUF ]
    SIGRUDEFS sSiGruDefs[MAX_SIGNALGRUPPEN];                //!<Festlegungen fuer die einzelnen Signalgruppen
    STRETCHBEREICHE sStretchBereiche[MAX_STRETCHBEREICHE];  //!< Definition der Phasen-Bereiche
  } SIGNALPLAN;

typedef struct {
    SSHORT wTX;        // 0 bis Umlaufzeit in 1/10 Sek. minus 10, wird bei laufenden Spl um 10 aufaddiert, hat im Auszustand den Wert (-1)
    UTINY  bZustand;   // Signalisierungszustand der Signalgruppe, im Auszustand auf SPERREN setzen
    UTINY  bFarbe;     // 0 ... 15, aus Farbkombinationsliste
    SSHORT wDauer;     // beginnt mit 10 wenn ein Uebergangssignal oder ein Endzustand beginnt, wird sek. um 10 erhoeht, MaxWert = MAXINT
    SSHORT wRestDauer; // nur bei Uebergangssignalen gesetzt, gibt die verbleibende Dauer des Uebergangssignales wiede
    UTINY  bMindestzeitErfuellt; // TRUE|FALSE, Mindestzeit ist erfuellt, wenn die Dauer (Mindestsperr- oder Mindestfreigabezeit)
                                 // gem. Datenversorgung erfuellt ist. Die aktuelle Sekunde wird mitgezaehlt, da ein Signalisierungswechsel
                                 // erst am Ende der Sekunde wirksam werden kann.
                                  // Dies gilt auch fuer wDauer und wRestDauer.
} SIGSZ_STRUCT;

// SigZustandLesenTkr
typedef struct { UTINY  bSigNr;               // 0 ... MAXSG - 1
                 UTINY  bZustand;             // Signalisierungszustand der Signalgruppe
                 UTINY  bFarbe;               // 0 ... 15, aus Farbkombinationsliste
                 SSHORT wDauer;               // beginnt mit 10 wenn ein Uebergangssignal oder ein Endzustand beginnt, wird sek. um 10 erhoeht, MaxWert = MAXINT
                 SSHORT wRestDauer;           // nur bei Uebergangssignalen gesetzt, gibt die verbleibende Dauer des Uebergangssignales wieder
                 UTINY  bMindestzeitErfuellt; // TRUE|FALSE, Mindestzeit ist erfuellt, wenn die Dauer (Mindestsperr- oder Mindestfreigabezeit)
                                              // gem. Datenversorgung erfuellt ist. Die aktuelle Sekunde wird mitgezaehlt, da ein Signalisierungswechsel
                                              // erst am Ende der Sekunde wirksam werden kann.
                                              // Dies gilt auch fuer wDauer und wRestDauer.
} SIGSZ;

typedef struct
{
    SSHORT wTX;     // laeuft von 0 bis Umlaufzeit - 10 (in 1/10 Sek.). Wird bei laufendem Signalzeitenplan sekuendlich um 10 aufaddiert.
                                 // hat im Auszustand den Wert (-1)
    UTINY  bAnzSig; // Anzahl der Signalgruppen fuer diese Tkr
    SIGSZ  sSigZustand[MAX_SIGNALGRUPPEN];
} TKRSZ_STRUCT;




#define LAENGE_KN_BEZ              64   // max. Laenge der Knotenbezeichnung
/**
 * Geraetestammdaten (VD_STAMMDATEN)
 */
typedef PACKED_STRUCT
  {
    // nicht bei Lizenzierung
    UTINY  bTyp;                          //!< Geraetetyp
    // nicht bei Lizenzierung
    char   aGeraetename[LAENGE_KN_BEZ];   //!< Standort des Geraetes, ASCII, NULLT
    // nicht bei Lizenzierung
    USHORT wAmtsNr;                       //!< Amtsnummer an dem der Knoten angeschlosen ist [MIN: 0][MAX: MAX_USHORT][DEF: 0]
     // nicht bei Lizenzierung
   USHORT  wKnotenNr;                     //!< Knotennummer der Kreuzung [MIN: 1][MAX: MAX_USHORT][DEF: 1]
    // nicht bei Lizenzierung
    UTINY  bSystemTakt;                   //!< Systemtakt des Geraets [DEF: TAKT1000MS] [REF: CB0028]
    // nicht bei Lizenzierung
    UTINY  bZentralenTyp[16];             //!< Zentralenanschlusstyp des Geraet [DEF: KEINE_ZENTRALE]
    USHORT wLaendercode;                  //!< Laenderkennungen der Textdateien fuer die Ausgaben des Geraets [REF: CB0025][DEF: DEUTSCH]
  } STAMMDATEN;

#define VD_STAMMDATEN           1   // VD_Obj AL_sStammdaten
#define VD_MINDESTFREIGABELISTE 7   // VD_Obj AL_sMindestfreigabeliste
#define VD_MINDESTSPERRLISTE    8   // VD_Obj AL_sMindestsperrliste
#define VD_SIGNALPLAN           15  // VD_Obj AL_sSignalplan
#define VD_SIG_BESCHREIBUNG     24  // VD_Obj AL_sSigBeschreibung
#define VD_ECKDATEN            100  //

#define AI_VSG_DATEN_LESEN_ERG0          ((SLONG) 0)     // Versorgungsdaten erfolgreich uebergeben
#define AI_VSG_DATEN_LESEN_ERG1          ((SLONG)-1)     // Versorgungsdatenobjekt nicht bekannt
// TODO: needed?
#define AI_VSG_DATEN_LESEN_ERG2          ((SLONG)-2)     // Versorgungsdatenstruktur nicht bekannt

#define AI_SYS_DATEN_LESEN_ERG0          ((SLONG) 0)     // Systemdaten erfolgreich uebergeben
#define AI_SYS_DATEN_LESEN_ERG1          ((SLONG)-1)     // Systemdatenobjekt nicht bekannt
#define AI_SYS_DATEN_LESEN_ERG2          ((SLONG)-2)     // Systemdatenstruktur nicht bekannt

#define HAUPTRICHTUNG 1
#define NEBENRICHTUNG 0

// [BOCB: CB0309]
#define LZ_UNZULAESSIG      (UTINY)0xFF //  Farbzustand ist fuer diese Signalgruppe nicht zulaessig
#define LZ_STD_SPERR        (UTINY)0x00 //  Farbzustand ist Standardsperr Farbe
#define LZ_STD_FREI         (UTINY)0x01 //  Farbzustand ist Standardfreigabe Farbe
#define LZ_UEB_SF_VT_SPERR  (UTINY)0x02 //  Farbzustand ist Uebergangssignal Sperren -> Frei    mit VT_Zustand Sperren
#define LZ_UEB_SF_VT_FREI   (UTINY)0x03 //  Farbzustand ist UEbergangssignal Sperren -> Frei    mit VT_Zustand Freigeben
#define LZ_UEB_FS_VT_SPERR  (UTINY)0x04 //  Farbzustand ist Uebergangssignal Frei    -> Sperren mit VT_Zustand Sperren
#define LZ_UEB_FS_VT_FREI   (UTINY)0x05 //  Farbzustand ist Uebergangssignal Frei    -> Sperren mit VT_Zustand Freigeben
#define LZ_SPERR            (UTINY)0x06 //  Farbzustand ist alternative Sperrfarbe
#define LZ_FREI             (UTINY)0x07 //  Farbzustand ist alternative Freifarbe

// bZustand -> SigZustandLesenSig, SigZustandLesenTkr
#define AL_AI_SPERREN                   (UTINY)0
#define AL_AI_FREIGEBEN                 (UTINY)1
#define AL_AI_SF                        (UTINY)2
#define AL_AI_FS                        (UTINY)3

// bZustand -> SigWunschLesenSig
#define SPERREN                         (UTINY)0
#define FREIGEBEN                       (UTINY)1


#define TABU_ANZ_ERR_PAR            8 // number of parameter bytes for each error message
#define VA_MELDUNG                 88 // TA state message
#define VA_FEHLER                  85 // TA error message, standard
#define VA_PRIO_FEHLER             89 // TA error message, fatal
#define TL_MELDUNG                 90 // TA-user  report message             -> from user logic
#define TL_PRIO_FEHLER             91 // TA-user  error  message, standard   -> from user logic
                                      // TA-user  error  message, fatal      -> not possible in user logic
enum TaDeviceValueIndexType
  {
    /** a device value per field device, that is only one instance per field device, index=0 */
    TADVI_Fd = 1,
    /** a device value per detector. */
    TADVI_Detector = 2,
    /** a device value per signal group. */
    TADVI_SG = 3,
    /** a device value per signal program. */
    TADVI_SignalProgram = 4,
    /** a device value per stage (Phase). */
    TADVI_Stage = 5,
    /** a device value per partial intersection (Teilknoten). */
    TADVI_PartialIntersection = 6,
    /** a device value per non signal group output. */
    TADVI_Output = 7,
    /** public transport control point (Meldpunkt) */
    TADVI_PublicTransportCP = 10,
    /** public transport group */
    TADVI_PublicTransportGroup = 11,
    /** public transport signal group */
    TADVI_PublicTransportSgGroup = 12,
    /** unknown other index type */
    TADVI_other = 99
  };

// bZustand -> SigWunschSetzenSig
#define AL_AI_SGS_SPERREN               (UTINY)0
#define AL_AI_SGS_FREIGEBEN             (UTINY)1
#define AL_AI_SGS_SIG_UNVERAENDERT      (UTINY)2

// TA return values, values between 0 and 99 are valid values and spezify the state of the TA
#define AWP_RS_VA_LAEUFT                      0 // VA steuert
#define AWP_RS_VA_LAEUFT_OHNE_TL_PARA         1 // VA steuert ohne TL-Parameter (SL-Logik)
#define AWP_RS_VA_FESTZT_ENDE                 2 // Festzeitsteuerung beendet, warten auf Uebernahme durch VA
#define AWP_RS_VA_FESTZT_TL                   3 // Festzeitsteuerung aufgrund der Anwenderlogik
#define AWP_RS_VA_FESTZT_PARA                 4 // Festzeitsteuerung aufgrund der Parametrierung
#define AWP_RS_VA_FESTZT_BEDIENER             5 // Festzeitsteuerung durch Bediener (reduzierter Betrieb)
#define AWP_RS_VA_FESTZT_PARA_FEHLEN          6 // Festzeitsteuerung aufgrund fehlender Parameter
#define AWP_RS_VA_UMSCHALTPLAN                7 // VA Umschaltplan laeuft
#define AWP_RS_VA_AUSSCHALTPLAN               8 // Festzeitsteuerung, Ausschaltung laeuft
#define AWP_RS_VA_FESTZT_BETR                 9 // Festzeitsteuerung aufgrund der Betriebsart (z.B. Handbetrieb)
#define AWP_RS_VA_FESTZT_TL_PARA_FEHLEN      10 // Festzeitsteuerung aufgrund fehlender TL-Parameter
#define AWP_RS_VA_FESTZT_FEUERWEHR           11 // Festzeitsteuerung, Feuerwehrplan laeuft
#define AWP_RS_VA_FESTZT_BAHN                12 // Festzeitsteuerung, Bahnplan laeuft
#define AWP_RS_VA_ENDE                       98 // VA wird abgeschalten, Steuerung abgeben und Aufraeumarbeiten durchfuehren
#define AWP_RS_VA_NETZAUSFALLZEIT            99 // Netzausfallzeit laeuft, VA wartet auf Einschaltung
#define AWP_RS_INIT_OK                      100 // Funktion AWP_Init, Initialisierung wurde fehlerfrei durchlaufen
#define AWP_RS_VA_FESTZT_SIMULATION         200 // Festzeitsteuerung in der Simulation, nur in der Simulation moeglich
#define AWP_RS_BUSY                         200 // traegt BZ ein (zur Ueberwachung der Laufzeit), nur im Steuergeraet moeglich
#define AWP_RS_TIMEOUT                      201 // traegt VA-Task ein (Laufzeitueberschreitung in Funktion AWP_Init)
#define AWP_RS_DONGLE_INVALID               254 // Funktion AWP_Init oder AWP_Funktion, falscher Dongle fuer VA
#define AWP_RS_INIT_ERROR                   255 // Funktion AWP_Init, Fehler bei der Initialisierung (kein dyn. Speicher

typedef struct
{
    int  (*getValue)(ULONG oitd, int instanz, int *pvalue);
    const char *name;
    const char *longName;
    const char *description;
    int   member;
    int   otype;
    enum  TaDeviceValueIndexType dvit;
    int   minIndex;
    int   maxIndex;
    int   minValue;
    int   maxValue;
    int   offset_sysPulses;  // number of system pulses to add to tick time of next take effect time of each value
} DEVICE_VALUE_TYPE_TAC;

// to be called from AWP_Init from each component
int registerDeviceValueProvider(unsigned nValueTypes, const DEVICE_VALUE_TYPE_TAC *pValueTypes);

SLONG SigZustandLesenSig (UTINY bSigNr, SIGSZ_STRUCT *pSZ);
SLONG SigZustandLesenTkr (UTINY bTkrNr, TKRSZ_STRUCT *pSZ);
SLONG SigWunschSetzenSig (UTINY bSigNr, UTINY bZustand, UTINY bFarbe);
SLONG SigWunschSetzenSigAtTick (UTINY bSigNr, UTINY bZustand, UTINY bFarbe, int64_t takeEffectTick);//by Valentin

//by Valentin
SLONG setIntersectionMainState(TacIState tis, int durationMs);
SLONG setSignalProgramNumber(UTINY bSigPrgNumber);


SLONG UmlaufzeitRuecksetzen (void);

UTINY VA_SgZustandBerechnen (UTINY bSgInd, UTINY bFarbeInd);

SLONG ZZWertLesen (UTINY bNr, UTINY ESig, UTINY RSig, SSHORT* pZZWert);

SLONG VsgDatenLesen (UTINY bObjID, UTINY bIndex, void* pObject);

// eine kpl. Matrix || eine kpl. Zeile || eine kpl. Spalte moeglich, abhaengig von ESig und RSig
SLONG ZZObjekteLesen (UTINY bNr, UTINY ESig, UTINY RSig, void *pZZObject);

/**
 * check if signal group with number sgNo is configured.
 * @param sgNo  signal group number to check
 * @return !=0 if signal group sgNo is configured
 *         ==0 if there is no signal group with sgNo configured
 */
int tacConfigIsSgConfigured(int sgNo);

SLONG TabuMeldungAbsetzen (SSHORT id, SSHORT* pTabuPar, USHORT anzShorts);

void  resetStatus (); // reset all TA (detail) states



#ifdef __cplusplus
}
#endif

#endif // _TAC_KERNEL_H
