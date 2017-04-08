/***************************************************************************
* Datei        API_VA.H                                                    *
****************************************************************************
* Verfasser:    Fischer                                                    *
* Erstellt :    08.15                                   Version 1.0        *
*                                                                          *
* Funktion :    API_VA.H enthaelt die Deklarationen der API-Funktionen     *
*               die der VA-Steuerungskern den VA-Steuerverfahren zur       *
*               zur Verfuegung stellt                                      *
*                                                                          *
* Aenderung:              ( Name )              ( Datum )                  *
*                                                                          *
*                      ( Beschreibung / Begruendung )                      *
*                                                                          *
* Version 1.0     MO MM ITS TS BTR Fischer         08.15                   *
*                                                                          *
*    Neuerstellung                                                         *
*                                                                          *
***************************************************************************/

/*
 * Aenderungen
 */
// Aenderungen in der Version 1.0
// 
// xx.xx.xx  xxx
//    xxx
// ToDo:
//  - delete function BetriebszustandLesen (get operational state) after it is replaced by the function "tacGetOperatingState" in complete project  

#ifndef _API_VA_H              // verhindert wiederholtes Laden
#define _API_VA_H

#if defined __cplusplus
extern "C" {
#endif

// needed includes should be registered here
#include "def_ger.h"  // compiler dependent defines

#include "tac_kernel.h"

// basic max values

//#define MAX_AUSGAENGE             250   // max possible number of logical outputs
//#define MAX_DETEKTOREN            250   // max possible number of logical detectors

//#define LAENGE_KN_NR                6   // max. Laenge der Knotennummer
//#define EXTENDED_TEXT              30   // max. Laenge eines erweiterten Textes z.B. Anwenderbezeichnung

// Konstanten mit globalen Maximalwerten
//#define MAX_VZ_MATRIZEN             3   // Maximale Anzahl an Versatzmatrizen
//#define MAX_MINDESTFREIGABELISTEN   3   // Maximale Anzahl an Mindestfreigabelisten
//#define MAX_MINDESTSPERRLISTEN      3   // Maximale Anzahl an Mindestsperrlisten
//#define MAX_EAPLAENE                3   // Maximale Anzahl an Ein- bzw. Ausschaltplaenen
// #define MAX_ZENTRALENPLAENE         2   // Maximale Anzahl an Zentralenplaenen
#define MAX_FEUERWEHRPLAENE         3   // Maximale Anzahl an Feuerwehrplaenen
#define MAX_BUESTRAPLAENE           2   // Maximale Anzahl an Buestraplaenen
// #define ZENTRALENPLAN_1             (MAX_SIGNALPLAENE+1)    // SignalplanNr. Zentr.-Plan1
// #define ZENTRALENPLAN_2             (MAX_SIGNALPLAENE+2)    // SignalplanNr. Zentr.-Plan2
// #define MAX_EABILDER                32  // Maximale Anzahl an Ein- / Ausschaltbildern
// #define MAX_EAFOLGEN                8   // Maximale Anzahl an Ein- / Ausschaltfolgen
// #define MAX_EAB_PRO_FOLGE          24   // Maximale Anzahl an Ein- / Ausschaltbildern pro Folge
//#define MAX_PHASEN                 16   // Maximale Anzahl an Phasen
//#define MAX_STRETCHUMLAEFE          3   // Maximale Anzahl an Umlaeufen fuer das Stretchverfahren
//#define MAX_UEB_PRO_SEQUENZ         4   // Maximale Anzahl an Uebergangsbildern pro Uebergangssequenz
//#define MAX_UESEQ                  16   // Anzahl Uebergangssequenzen SF und FS
//#define MAX_UESEQ_KOMBINATIONEN     3   // Anzahl der Uebergangssequenzkombinationen
// #define MAX_AUS_MODI                3   // Maximale Anzahl an verschiedenen Auszustaenden
//#define MAX_PHASENUEB              32   // Maximale Anzahl an versorgbaren Phasenuebergaengen
//#define MAX_TAGESPLAENE            32   // Maximale Anzahl an Tagesplaenen
//#define MAX_WOCHENPLAENE            4   // Maximale Anzahl an Wochenplaenen
// #define MAX_JAHRESKALENDER          3   // Maximale Anzahl an Jahreskalendern
// #define MAX_PAR                     8   // Maximale Anzahl an Parametern fuer einen Tagesplaneintrag
//#define MAX_TPE_PAR                22   // Maximale Anzahl an Parametern fuer einen Tagesplaneintrag
//#define MAX_TP_EINTRAEGE           16   // Maximale Anzahl an Eintraegen pro Tagesplan
//#define MAX_TABU_PAR                8   // TABU_ANZ_ERR_PAR, Maximale Anzahl an Tagebuchparametern
// #define MAX_ASCII_PAR              16   // Maximale Anzahl an ASCII - Parametern fuer eine SISI Tagebuchmeldung
// #define MAX_SISI_PAR                2   // Maximale Anzahl an normalen Parametern fuer eine SISI Tagebuchmeldung
// #define MAX_MENUETEXT_LAENGE     4096   // Maximale Laenge fuer einen Menuetext
// #define MAX_TABUTEXT_LAENGE        50   // Maximale Laenge fuer einen Tagebuchtext
// #define MAX_SISITEXT_LAENGE        50   // Maximale Laenge fuer einen Sisi-Tagebuchtext
// #define MAX_MENUETEXTE            128   // Maximale Anzahl an verschiedenen Menuetexten
//#define MAX_TABUTEXTE             128   // Maximale Anzahl an verschiedenen Tagebuchtexten
// #define MAX_EXTTEXTE               64   // Maximale Anzahl an verschiedenen erweiterten Tagebuchtexten
//#define MAX_SISITEXTE             255   // Maximale Anzahl an verschiedenen Sisimeldungstexten
// #define MAX_QUIT_ANFOS             16   // Maximale Anzahl an zu quittierenden Anforderungen
// #define MAX_LEITFADEN_LAENGE     4096   // Maximale Laenge des Leitfadens
//#define MAX_TAE_MP                128   // Maximale Anzahl an versorgbaren Meldepunkten
//#define MAX_DETUEBW_BEREICHE        4   // Maximale Anzahl an Plausibilitaetsbereichen fuer die Detektorueberwachung
//#define MAX_TP_TAGE                 8   // Maximale Anzahl an Tagen fuer die Standardtagesplanzuordnung
//#define MAX_PRIO_FELDER             6   // Maximale Anzahl an Feldern zur Zeitpriorisierung in AL_sParameter
// #define MAX_ROTFAHRER              64   // Maximale Anzahl an Versorgungseintraegen fuer Rotfahrer
//#define MAX_UMLAUF                  2990   // Maximale Umlaufzeit fuer Signalplaene bzw. Schaltpunkte (1-300 s)
//#define MAX_ZWISCHENZEIT         1200   // Maximal zulaessige Zwischenzeit
// #define MAX_VERSATZ              1200   // Maximaler Absolutwert eines Versatzes (Beginn/Ende)
//#define MAX_UEBSEQUENZ            600   // Maximal zulaessige Dauer der Uebergangssequenz einer Signalgruppe
//#define MAX_UEB_DAUER            2400   // Maximal zulaessige Dauer eines Uebergangs
//#define MAX_PLATTER                 8   // Maximale Anzahl an Platter-Adressen (0-7)
//#define MAX_RM_KANAELE              4   // Maximale Anzahl an Rueckmeldekanaelen
// #define MAX_VERON_SIGNALE           5   // Maximale Anzahl an Veronsignalen
// #define MAX_PORT_BYTES             42   // Maximale Anzahl an Bytes fuer Detektoreingaenge (pro Platter Bit - orientiert)
// #define MAX_DET_SLOTS             100   // Maximale Anzahl an Slots fuer das Einlesen der Detektor-Rohwerte
// #define MAX_BEFA_BEF_BYTES          6   // Maximale Anzahl an Befehlsbytes der BEFA
// #define MAX_DISPLAY_BUF             7   // Maximale Anzahl an verschiedenen Displaypuffern fuer das BAZ
//#define MAX_BAZ_INPUTS            256   // Maximale Anzahl an Zusatzeingaengen der BAZ
//#define MAX_BAZ_OUTPUTS           256   // Maximale Anzahl an Zusatzausgaengen der BAZ
//#define MAX_PANEL_TASTEN           32   // Maximale Anzahl an Tastenzuordnungen zu Funktionen der BAZ/HP-Tastatur
// #define MAX_BAZ_DSP_ZEILEN          4   // Maximale Anzahl an Zeilen des BAZ-Displays
// #define MAX_BAZ_DSP_SPALTEN        20   // Maximale Anzahl an Zeilen des BAZ-Displays
//#define MAX_SONDERTAGE             32   // Maximale Anzahl an einmaligen Sondertagen
//#define MAX_FEIERTAGE              32   // Maximale Anzahl an jaehlichen festen bzw. beweglichen Feiertagen
//#define MAX_OSTERSONNTAGE         100   // Maximale Anzahl an Ostersonntagseintraegen
//#define MAX_FERIENEINTRAEGE        20   // Maximale Anzahl an Ferienbereichen
//#define MAX_SYSEREIGNISSE          32   // Maximale Anzahl an Systemereignissen
//#define MAX_ZEITEN_PHAUEB           2   // Maximale Anzahl an Wechselzeiten pro Phasenuebergang

//#define MAX_EA_SP                   8   // Anzahl an Schaltpunkten (pro SG) innerhalb eines EA-Plans
#define MAX_BEDIENER                6   // Maximale Anzahl an erlaubten Geraetebedienern
// #define MAX_BAZ_SONDERTASTEN        8   // Feste BAZ-Tasten fuer freie Nutzung
//#define MAX_BAZ_LEDS               32   // Maximale Anzahl an LED's am BAZ
// #define MAX_VA_VAR                512   // Maximale Anzahl an VA-Variablen
// #define MAX_VA_DYN_PAR             50   // Maximale Anzahl an VA-Variablen
// #define MAX_VA_INFO_LEN            64   // Maximale Anzahl an VA-Informationsbytes
// #define MAX_ZP_INFO_LEN            60   // Maximale Anzahl an Zentralenprozess-Informationsbytes
// #define MAX_IMP_VAR               256   // Maximale Anzahl an VA Import-Variablen
// #define MAX_EXP_VAR               256   // Maximale Anzahl an VA Export-Variablen
// #define MAX_VA_ZUSATZ_INFO          2   // Maximale Anzahl der VA-Parameter-Strukturen
// #define MAX_BEF_BYTES               6   // Maximale Anzahl an Befa-Befehlsbytes
// #define MAX_MELDEBYTES              2   // Maximale Anzahl an Befa-Meldebytes
// #define MAX_VA_KOMPONENTEN         10   // Maximale Anzahl an moeglichen VA-Komponenten
// #define MAX_MODIFIKATIONEN         13   // Maximale Anzahl an moeglichen freien projektspezifischen Modifikationen
// #define MAX_LED_MODULE            100   // Maximale Anzahl an LED-Modulen
// #define MAX_OV_ANF                 32   // Maximale Anzahl an OEV-Richtungen
// #define MAX_LSLS_CARDS              8
//#define MAX_SONMELD_MAP             4
#define MAX_MODMAP                  4   // Maximale Anzahl der Mappings von Modifikationen auf Ausgaenge
// #define VERSION_LEN                 4   // Anzahl der Bytes fuer eine Versionsnummer (z.B. 3.1.0.0)

#define MAX_ERR_REL_MX         (UTINY)2 // maximale relevate Matrix Eintraege, BildeUebergangsDaten, BildeUebergang
#define MAX_ERR_REL_SG         (UTINY)2 // maximale relevate Signalgruppen,    BildeUebergangsDaten, BildeUebergang

// english-like redefinitions
#define MAX_SIGNAL_PROGRAMS MAX_SIGNALPLAENE //!< maximum signal programs

/*
 * C900-Typdefintionen und Literale fuer die Anwenderfunktionen
 */

/*
 * Allgemein
 */

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

// return values for fct. SysDatenLesen

// ToDo: check if it is possible to replace this in pdmx functions -> error codes from sw_BildeUebergang
#define SUMI_KEIN_ERR          (UTINY)0  // no SUMI error

// ToDo: check if still neccessary -> source identifier
#define HERKUNFT_VA             (UTINY)0
#define HERKUNFT_PC             (UTINY)1
#define HERKUNFT_ABF            (UTINY)2
#define HERKUNFT_VSR            (UTINY)3
#define HERKUNFT_SU             (UTINY)4
#define HERKUNFT_SISI           (UTINY)5
#define HERKUNFT_ZZ             (UTINY)6
#define HERKUNFT_FE             (UTINY)7
#define HERKUNFT_MIF            (UTINY)8
#define HERKUNFT_MIS            (UTINY)9
#define HERKUNFT_TU             (UTINY)10

// ToDo: check if still neccessary -> Werte fuer Anwenderprogrammzustaende
#define AWP_NOT_LOADED      0   // Initailiserungszustand
#define AWP_CANCELED        1   // BZ-Task hat AWP wegen Fehler beendet
#define AWP_STOPPED         2   // Zustand nach tStart von LD, wird von LD und ABF gesetzt.
#define AWP_STARTED         3   // AWP-START bzw. ABF -> BZ fuehrt AWP aus.
#define AWP_R_STARTED       4   // AWP soll in Festzeitbetrieb gehen
#define AWP_LAST_CALL       5   // AWP steht kurz vor absoluter Beendigung, dient zur Bereingung
#define AWP_INITIALIZE      6   // AWP ist in Initilisierungsphase

// ToDo: check if still neccessary -> Werte fuer Parameterdatenbereiche
#define PARAM_NOT_LOADED    0
#define PARAM_LOADING       1
#define PARAM_LOADED        2

// ToDo: check constants for "STAMMDATEN.wSprachen"
#define DEUTSCH           49     // Konstante fuer Deutsch
#define ENGLISCH           1     // Konstante fuer Englisch
#define FLAEMISCH         32     // Konstante fuer Flaemisch
#define FRANZOESISCH      33     // Konstante fuer Franzoesisch
#define SPANISCH          34     // Konstante fuer Spanisch
#define ITALIENISCH       39     // Konstante fuer Italienisch
#define TSCHECHISCH       42     // Konstante fuer Tschechisch
#define DAENISCH          45     // Konstante fuer Daenisch
#define POLNISCH          48     // Konstante fuer Polnisch
#define RUSSISCH           7     // Konstante fuer Russisch
#define INTERNATIONAL     99     // Konstante fuer beliebige Sprache
#define DEUTSCH_ROM      254     // Konstante fuer Deutsch festversorgt im ROM
#define LITAUISCH        370     // Konstante fuer Litauisch

// ToDo: check if still neccessary -> Konstanten fuer Ja / Nein PARAMETER
#define NEIN               0     // nicht aktiv
#define JA                 1     //  aktiv

// internel clock
#define TAKT200MS          2     // Systemtakt 200ms
#define TAKT1000MS        10     // Systemtakt 1000ms

/*#define STD_MOD_FREIGABE          0     // Modifikationszustand unbeeinflusst
#define STD_MOD_AUS               1     // Modifikationszustand ausgeschaltet
#define STD_MOD_EIN               2     // Modifikationszustand eingeschaltet
*/



/*
 * Signalgruppen
 */
// bSource -> SigWunschLesenSig, SigWunschLesenTkr
#define AL_AI_SIPL                      (UTINY)0
#define AL_AI_ZENTRALE                  (UTINY)1
#define AL_AI_BEDIENER                  (UTINY)2


#define SIG_SW                          (UTINY)2    // ANW_SIG_SETZEN
#define UEB_S_F                         (UTINY)2
#define UEB_F_S                         (UTINY)3
#define DEF_FARBE                             -1    // aus max_dek.h
#define DEF_U_FARBE                          255    // -1 as a byte
#define DEF_ZUSTAND                           -1    // aus max_dek.h


// SigWunschLesenSig
typedef struct { SSHORT wTX;      // 0 bis Umlaufzeit in 1/10 Sek. minus 10, wird bei laufenden Spl um 10 aufaddiert, hat im Auszustand den Wert (-1)
                 UTINY  bSource;  // muss nicht geladen werden, da es nie abgefragt wird
                 UTINY  bZustand; // im Auszustand am besten auf SPERREN setzen
                 UTINY  bFarbe;   // 0 ... 15
               } SIGSW_STRUCT;

// SigWunschLesenTkr
typedef struct { UTINY bSigNr;           // 1 ... MAX_SIGNALGRUPPEN
                 UTINY bZustand;
                 UTINY bFarbe;           // 0 ... 15, aus Farbkombinationsliste
               } SIGSW;

typedef struct { SSHORT wTX;
                 UTINY  bSource;
                 UTINY  bAnzSig;         // Anzahl der Signalgruppen fuer diese Tkr
                 SIGSW  sSigWunsch[MAX_SIGNALGRUPPEN];
               } TKRSW_STRUCT;


/*
 * UebergangsdauerLesen
 */
// bArt -> UebergangsdauerLesen
#define AL_AI_UESEQ_FS                  (UTINY)0
#define AL_AI_UESEQ_SF                  (UTINY)1

/*
 * BetriebszustandLesen, neu
 */

// bSiplNrAktuell -> BetriebszustandLesen       1 ... 32, 255
// bSiplNrGefordert -> BetriebszustandLesen     1 ... 32, 255
// bZplNr -> BetriebszustandLesen               1 ... 32, 255

// bSondereingriff -> BetriebszustandLesen
#define AL_AI_KEIN_SONDEREINGRIFF       (UTINY)0x00   // Es liegt kein Sondereingriff vor
#define AL_AI_FEUERWEHR_P1              (UTINY)0x01   // Sondereingriff Feuerwehr Plan 1
#define AL_AI_FEUERWEHR_P2              (UTINY)0x02   // Sondereingriff Feuerwehr Plan 2
#define AL_AI_FEUERWEHR_P3              (UTINY)0x03   // Sondereingriff Feuerwehr Plan 3
#define AL_AI_BAHN_VB                   (UTINY)0x04   // Sondereingriff Bahn Vollbeeinflussung
#define AL_AI_BAHN_TB                   (UTINY)0x05   // Sondereingriff Bahn Teilbeeinflussung

/*
 * ListeAuswaehlen
 */
// ListeAuswaehlen, bArt
#define AL_AI_ZZ     (UTINY)0 // ZZ-Matrix
#define AL_AI_MIF    (UTINY)1 // Mindestfreigabeliste
#define AL_AI_MIS    (UTINY)2 // Mindestsperrliste

/*
 * Sondereingriff
 */
// bMode -> Sondereingriff
#define AL_AI_SO_START          (UTINY)0
#define AL_AI_SO_ENDE           (UTINY)1

// bGrund -> Sondereingriff
#define AL_AI_SO_FEUERWEHR_1    (UTINY)0
#define AL_AI_SO_FEUERWEHR_2    (UTINY)1
#define AL_AI_SO_FEUERWEHR_3    (UTINY)2
#define AL_AI_SO_BAHN_VB        (UTINY)3   // Sondereingriff Bahn Vollbeeinflussung
#define AL_AI_SO_BAHN_TB        (UTINY)4   // Sondereingriff Bahn Teilbeeinflussung


// basic data, bZentralenTyp
#define KEINE_ZENTRALE     0     // Kein Zentralenanschluss
#define CANTO_1_3         66     // Canto 1.3 - Zentrale als Zentralenmodul
#define DVI35             35     // DVI35 Zentrale (Fa. Dambach)
#define OCIT_2_0          64     // OCIT 2.0 - Zentrale als Zentralenmodul
#define VNETS_ZENTRALE    31     // VNETS - Zentrale als Zentralenmodul
#define OCIT_ZENTRALE     32     // OCIT - Zentrale als Zentralenmodul
#define SONDER_ZENTRALE   99     // (Sonderzentrale - Zentrale

// basic data, bSystemTakt
#define TAKT200MS          2     // Systemtakt 200ms
#define TAKT1000MS        10     // Systemtakt 1000ms





/*
 * TXChange(Absolut)
 */
// Returnwerte -> TXChange
#define ACTUAL_NOT_VALID                ((SLONG)-23)    // wenn eine Veraenderung des TX derzeit nicht moeglich ist
// SUCCESS                              -> bereits definiert
// ERROR_PARAMETER                      -> bereits definiert

// Returnwerte -> TXChangeAbsolut
// SUCCESS                              -> bereits definiert
// ERROR_PARAM                          -> bereits definiert

/*
 * VsgDatenLesen
 */
// Returnwerte -> VsgDatenLesen

// #define VD_DETPAR               2   // VD_Obj AL_sDetPar
// #define VD_PARAMETER            3   // VD_Obj AL_sParameter
// #define VD_ZZ_MATRIZEN          4   // VD_Obj AL_aZZMatrix
// #define VD_VZ_MATRIX_BB         5   // VD_Obj AL_aVZMatrixBB
// #define VD_VZ_MATRIX_EE         6   // VD_Obj AL_aVZMatrixEE
// #define VD_FARBKOMBINATIONEN    9   // VD_Obj AL_sFarbkombinationen
// #define VD_UEBERGANGSSEQ_SF     10  // VD_Obj AL_sUebergangssequenzen_SF
// #define VD_UEBERGANGSSEQ_FS     11  // VD_Obj AL_sUebergangssequenzen_FS
// #define VD_UE_SEQ_MATRIX_SF     12  // VD_Obj AL_sUeSeeqMatrix_SF
// #define VD_UE_SEQ_MATRIX_FS     13  // VD_Obj Al_sUeSeqMatrix_FS
// #define VD_FEUERWEHRPLAN        14  // VD_Obj AL_sFeuerwehrplan
// #define VD_SOFTKEY              16  // VD_Obj AL_sSoftkey
// #define VD_EINSCHALTFOLGEN      17  // VD_Obj AL_sEinschaltfolgen
// #define VD_AUSSCHALTFOLGEN      18  // VD_Obj AL_sAusschaltfolgen
// #define VD_PHASEN               19  // VD_Obj AL_sPhasen
// #define VD_PHASEN_MATRIX        20  // VD_Obj AL_bPhasenUebergangsMatrix
// #define VD_PHASENUEBERGANG      21  // VD_Obj AL_sPhasenuebergang
// #define VD_PHASENDATEN          22  // VD_Obj AL_sPhasendaten
// #define VD_SFERNDATEN           23  // VD_Obj AL_sSFerndaten, entfaellt
// #define VD_DETEKTOR             25  // VD_Obj AL_sDetektor
// #define VD_STDKALENDER          26  // VD_Obj AL_sStdKalender
// #define VD_ARCHIV               27  // VD_Obj AL_sArchiv
// #define VD_TAGESPLAN            28  // VD_Obj AL_sTagesplan
// #define VD_BUESTRAPLAN          29  // VD_Obj AL_sBuestraplan
// #define VD_TABUSTEUERUNG        30  // VD_Obj AL_sTabuStrg
// #define VD_OEV_4PUNKT           31  // VD_Obj AL_sOEV_4PunktSignal
// #define VD_PERMISSIV            32  // VD_Obj AL_sPermissiv
// #define VD_SENLI                33  // VD_Obj AL_aSenli
// #define VD_TPZUORDNUNG          34  // VD_Obj AL_sTPZuordnung
// #define VD_TAE                  35  // VD_Obj AL_sVsgTae
// #define VD_BAZ                  36  // VD_Obj AL_sVsgBaz
// #define VD_ZE_INIT              37  // VD_Obj AL_sZEInit
// #define VD_VSG_IO               38  // VD_Obj AL_sVsgIO
// #define VD_IO_INIT              39  // VD_Obj AL_sIOInit
// #define VD_DETUEBERWACHUNG      40  // VD_Obj AL_sDetUebwListe
// #define VD_ROTFAHRER            41  // VD_Obj AL_sRotfahrer
// #define VD_VSG_COM              42  // VD_Obj AL_sVsgCom
// #define VD_AUSGANG              43  // VD_Obj AL_sAusgang
// #define VD_LEDUEBW              44  // VD_Obj AL_sLEDUebw
// #define VD_KOPFDATEN            45  // VD_Obj AL_sKopfdaten


/**
 * Detektorparameter (VD_DETPAR)
 */
/*typedef PACKED_STRUCT
{
    UTINY bGl_alpha1;                       //!< Glaettungsfaktor fuer "Aufwaerts"-Glaettung
    UTINY bGl_alpha2;                       //!< Glaettungsfaktor fuer "Abwaerts"-Glaettung
    UTINY bSchwelle;                        //!< Mindestbelegung des Detektors fuer Belegungsmeldung (in 10 ms Einheiten, 0 = keine Ueberwachung)
    UTINY bModus;                           //!< Modus des Detektors fuer Berechnung der abgeleiteten Werte
    UTINY bPlausiBer[MAX_DETUEBW_BEREICHE]; //!< Verweis auf den jeweils zugeordneten Ueberwachungsbereich
} DET_DIV_INIT; */                            //!< Glaettungs- und Plausibilitaetsdaten fuer Detektoren

/*typedef PACKED_STRUCT
{
    UTINY bReserve1;                        //!< nicht mehr genutzt - Anzahl an versorgten seriellen Detektoren
    UTINY bReserve2;                        //!< nicht mehr genutzt - Anzahl an versorgten Detektoren
    DET_DIV_INIT sDet[MAX_DETEKTOREN];      //!< Parametersatz fuer den jeweiligen Detektor
} DET_PAR;*/

/**
 * Geraeteparameter  (VD_PARAMETER)
 */
typedef PACKED_UNION
    {
    USHORT          wGesamt;      //!< Bitfeld zur Konfiguartion der Sonderfunktionen
    PACKED_STRUCT
        {
        USHORT      wSK16 : 1;    //!< Sonderkennung 16 - unbenutzt
        USHORT      wRCP  : 1;    //!< Aktivierung des RCP (russ. BAZ)
        USHORT      wFAS  : 1;    //!< Aktivierung der FAS-Daten (Funktionsueberwachungs- und Analysesystem)
        USHORT      wFHH  : 1;    //!< Hamburg Kennung
        USHORT      wKSB  : 1;    //!< Keine Stauchbereiche fuer Stretch berechnen
        USHORT      wSOP  : 1;    //!< Optimierte Stretchberechnung abschalten
        USHORT      wTLC  : 1;    //!< TL-Fehler 90 in Fehler 18 zur Zentrale umsetzen
        USHORT      wSON  : 1;    //!< Sipl Online ueber Befa15 mit oder ohne fuehrende Klammern uebertragen
        USHORT      wFMU  : 1;    //!< ZZ/Minzeit/Feindlichkeits - Fehlermeldungen bei Sipl-Wechsel nicht unterdruecken
        USHORT      wSEV  : 1;    //!< Einschaltverhinderung nach Sisistoerung und anschl. Netzausfall
        USHORT      wOBG  : 1;    //!< Oesterreichisches Bediengeraeteverhalten gefordert
        USHORT      wABS  : 1;    //!< Aus Gelb blinken sofort gefordert bei Abschaltung ueber BAZ
        USHORT      wADS  : 1;    //!< Aus Dunkel sofort gefordert bei Abschaltung ueber BAZ
        USHORT      wASU  : 1;    //!< Aus - Stoerungsunterscheidung
        USHORT      wADU  : 1;    //!< Aus-Dunkel Unterscheidung
        USHORT      wZAM  : 1;    //!< Zentralenausmarkierung
        } sArt;
    } SONDERKENNUNG;

/**
 * OCIT bzw Canto Modifikationen auf Ausgaenge mappen
 */
typedef PACKED_STRUCT
{
    UTINY       bModID;     //!< Nummer der Modifikation [MIN: 1][MAX: 16][DEF: MAX_UTINY]
    UTINY       bAusgang;   //!< Ausgang der geschalten werden soll [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE-1]
} MOD_MAP;

/**
 * Wechsel von Eingangszustaenden auf Sondermeldungen mappen
 */
typedef PACKED_STRUCT
{
    UTINY       bEingang;           //!< relevanter Eingang [MIN: 0][MAX: MAX_DETEKTOREN-1][DEF: MAX_UTINY]
    UTINY       bSondermeldungFS;   //!< Meldung die abgesetzt wird bei steigender Flanke [MIN: 1][MAX: 32][DEF: MAX_UTINY]
    UTINY       bSondermeldungFF;   //!< Meldung die abgesetzt wird bei fallender Flanke [MIN: 1][MAX: 32][DEF: MAX_UTINY]
    UTINY       bZusInfo;           //!< Zusatzinfo fuer Sonderloesungen (z.B. zur Unterscheidung der Meldungsart) [MIN: 0][MAX: MAX_UTINY][DEF: MAX_UTINY]
} SONDERM_MAP;

/**
 * Geraeteparameter
 */
/*typedef PACKED_STRUCT
  {
    SSHORT      wUmlaufKontrolle;                 //!< Zeitwert bis zum Ansprechen der Umlaufkontrolle, 0 = Ausgeschaltet [DEF: 1800][MIN:0][MAX: 10000] [ZEIT: 1, 0.1]
    SSHORT      wEinschaltVerz;                   //!< Zeitwert fuer eine Verzoegerung der Wiedereinschaltung nach Netzwiederkehr [DEF: 110][MIN:0][MAX: 3000][ZEIT: 1, 0.1]
    SSHORT      wAllesGelbDauer;                  //!< Zeitwert fuer die Dauer der "Alles Gelb" Zeit bzw. Umlaufabhaengig [DEF: 80] [ZEIT: 1, 0.1][MIN:0][MAX:600]
    UTINY       bZZOptimal;                       //!< ZZ-Programm soll Uebergaenge optimieren [DEF: JA]
    UTINY       bHandPlan;                        //!< Signalplan der fuer Handsteuerung herangezogen wird [MIN: 0][MAX: MAX_SIGNALPLAENE-1][DEF: MAX_UTINY]
    UTINY       bNotPlan;                         //!< Not-Signalplan der im Fehlerfall herangezogen wird [MIN: 0][MAX: MAX_SIGNALPLAENE-1][DEF: MAX_UTINY]
    UTINY       bReserve;                         //!< Reservebyte [DEF: 0]
    UTINY       bStdAusZustand;                   //!< Standardauszustand des Geraets [DEF: AUS_BILD_NORMAL]
    UTINY       bLandeskennung;                   //!< Kennung fuer das Land in dem die Anlage in Betrieb geht [DEF: DEUTSCH]
    UTINY       bAusZZFehler;                     //!< Verhalten im Falle eines Zwischenzeitfehlers im Steuerteil[DEF: GV_SCHIEBEN][BOCB: CB0077]
    UTINY       bAusMinGruenFehler;               //!< Verhalten im Falle eines Mindestgruenzeitfehlers im Steuerteil [DEF: GV_SCHIEBEN][BOCB: CB0077]
    UTINY       bAusMinRotFehler;                 //!< Verhalten im Falle eines Mindestrotzeitfehlers im Steuerteil [DEF: GV_SCHIEBEN][BOCB: CB0077]
    UTINY       bAusFeindlichFehler;              //!< Verhalten im Falle einer Feindlichkeit im Steuerteil [DEF: GV_SCHIEBEN] [REF: CB0077]
    UTINY       bEinschaltVerfahren;              //!< Kennung welches Einschaltverfahren benutzt werden soll [DEF: EA_STANDARD] [REF: CB0059]/
    UTINY       bAusschaltVerfahren;              //!< Kennung welches Ausschaltverfahren benutzt werden soll [DEF: EA_STANDARD] [REF: CB0059][READONLY:]
    UTINY       bUmschaltVerfahren;               //!< Kennung welches Umschaltverfahren benutzt werden soll [DEF: UV_GSP] [REF: CB0027]
    UTINY       bSyncVerfahren;                   //!< Kennung welches Synchronisierverfahren benutzt werden soll [DEF: SYNC_GSP] [REF: CB0035]
    UTINY       bSyAbweichung;                    //!< Max. Abweichung des TX von der Refli ohne Protokollierung des Synchronisierens (in Sekunden), 255 = keine Protokollierung
                                                  // !<[DEF: MAX_UTINY][MIN: 0][MAX: MAX_UTINY]
    UTINY       bStretchUmlaufAnz;                //!< Anzahl Umlaeufe fuer Synchronisierung [DEF: 1][MIN: 1][MAX: MAX_STRETCHUMLAEFE]
    UTINY       bVASteuerVerfahren;               //!< Steuerverfahren der Verkehrsabhaengigkeit [DEF: VA_PDM][REF: CB0030]
    UTINY       bVAMaxAnzTakte;                   //!< Zahl der Systemtakte die das AWP maximal fuer einen Durchlauf benoetigen darf
    SONDERKENNUNG   wSk;                          //!< Bit-Feld zur Parametrierung von Sonderfunktionen
    MOD_MAP         sModMap[MAX_MODMAP];          //!< Versorgung fuer ein moegliches Mapping von Modifikationen auf Ausgaenge
    SONDERM_MAP     sSonMeldMap[MAX_SONMELD_MAP]; //!< Versorgung fuer ein moegliches Mapping von Eingangs-Zustandsaenderungen auf Sondermeldungen
    UTINY           bDummy[8];                    //!< Fuellbytes zur Reserve [DEF: MAX_UTINY]
  } PARAMETER;*/

/**
 * ZZ- und VZ-Parameter (VD_ZZ_MATRIZEN, VD_VZ_MATRIX_BB, VD_VZ_MATRIX_EE)
 */
typedef PACKED_STRUCT
  {
    char   aName[STD_TEXT];                             //!< Bezeichnung der Signalgruppenmatrix [ASCII][NULLT:][MAX:32][INSTANZNAME:]
    SSHORT wWert[MAX_SIGNALGRUPPEN][MAX_SIGNALGRUPPEN]; //!< [DEF: MAX_SSHORT] [MIN: -1200] [MAX: MAX_ZWISCHENZEIT] [ZEIT: 1, 0.1]
  } SG_MATRIX;


/**
 * Farbkombinationsparameter (VD_FARBKOMBINATIONEN)
 * Achtung - Reihenfolge umgestellt, da Bitfelder mit dem MSB beginnen [DEF: 0 ]
 */
typedef PACKED_STRUCT
  {
    USHORT reserve        : 1;   //!< derzeit nicht benutzt [(Bit 2^15) [DEF: 0]

    USHORT sig_5_frequenz : 1;   //!< 0 : 1Hz-Blinken, 1 : 2Hz-Blinken [(Bit 2^14) [DEF: 0]
    USHORT sig_5_blinken  : 1;   //!< 0 : permanent, 1 : blinken  [(Bit 2^13) [DEF: 0]
    USHORT sig_5_hell     : 1;   //!< 0 : dunkel, 1 : hell - derzeit nicht benutzt  [(Bit 2^12)[DEF: 0]

    USHORT sig_4_frequenz : 1;   //!< 0 : 1Hz-Blinken, 1 : 2Hz-Blinken [(Bit 2^11) [DEF: 0]
    USHORT sig_4_blinken  : 1;   //!< 0 : permanent, 1 : blinken  [(Bit 2^10)[DEF: 0]
    USHORT sig_4_hell     : 1;   //!< 0 : dunkel, 1 : hell - derzeit nicht benutzt  [(Bit 2^9) [DEF: 0]

    USHORT sig_3_frequenz : 1;   //!< Geber Unten, 0:=1Hz-Blinken, 1:=2Hz-Blinken [(Bit 2^8)
    USHORT sig_3_blinken  : 1;   //!< Geber Unten, 0:=Dauerein, 1:=Blinken [(Bit 2^7)
    USHORT sig_3_hell     : 1;   //!< Geber Unten, 0:=Dunkelbeginn, 1:=Hellbeginn [(gruen) (Bit 2^6)

    USHORT sig_2_frequenz : 1;   //!< Geber Mitte, 0:=1Hz-Blinken, 1:=2Hz-Blinken [(Bit 2^5)
    USHORT sig_2_blinken  : 1;   //!< Geber Mitte, 0:=Dauerein, 1:=Blinken [(Bit 2^4)
    USHORT sig_2_hell     : 1;   //!< Geber Mitte, 0:=Dunkelbeginn, 1:=Hellbeginn [(gelb) (Bit 2^3)

    USHORT sig_1_frequenz : 1;   //!< Geber Oben, 0:=1Hz-Blinken, 1:=2Hz-Blinken [(Bit 2^2)
    USHORT sig_1_blinken  : 1;   //!< Geber Oben, 0:=Dauerein, 1:=Blinken [(Bit 2^1)
    USHORT sig_1_hell     : 1;   //!< Geber Oben, 0:=Dunkelbeginn, 1:=Hellbeginn [(rot) (Bit 2^0)
  } FBKOM;

/**
 * Struktur zum Speichern gueltiger Signalbilder
 */
typedef PACKED_STRUCT
  {
    FBKOM  sFarbe;          //!< Signalisierung
    UTINY  bOTECFarbCode;   //!< zugeordneter Farbcode gemaess OTEC / OCIT Definition
    char   bZeichen;        //!< ASCII-Zeichen fuer Visualisierung [ASCII]
    char   aBezeichnung[STD_TEXT];//!< Bezeichnung der Signalisierung [MAX:32][NULLT:]
  } VALID_FBKOM;

/**
 * Uebergangssequenzen (VD_UEBERGANGSSEQ_SF, VD_UEBERGANGSSEQ_FS)
 */
/*typedef PACKED_STRUCT
  {
    SSHORT wGesamtdauer;                         //!< Summe der Einzeldauer aller Bilder [ZEIT: 1, 0.1] [CALC:][DEF: MAX_SSHORT][MIN:0][MAX:MAX_UEB_DAUER]
    UTINY  bUebergangsbild[MAX_UEB_PRO_SEQUENZ]; //!< Farbe des Bildes der Uebergangssequenz [DEF: MAX_UTINY][DATAOBJ: VD_FARBKOMBINATIONEN] [MIN: 0] [MAX: MAX_VALID_FBKOM-1]
    SSHORT wBilddauer[MAX_UEB_PRO_SEQUENZ];      //!< Dauer des Bildes der Uebergangssequenz [DEF: MAX_SSHORT] [ZEIT: 1, 0.1][MIN:0][MAX:MAX_UEBSEQUENZ]
    char   aName[STD_TEXT];                      //!< Bezeichnung der Uebergangssequenz [ASCII][NULLT:][MAX:32]
  } UESEQ;*/

/**
 * UebergangssequenzMatrix (VD_UE_SEQ_MATRIX_SF, VD_UE_SEQ_MATRIX_FS)
 */
typedef PACKED_STRUCT
    {
        UTINY   bUebSequenz [MAX_SIGNALGRUPPEN]; //!< [DEF: MAX_UTINY][MAX: MAX_UESEQ-1]
        char    aName[STD_TEXT];                 //!< Bezeichnung der Uebergangsmatrix [ASCII][NULLT:][MAX:32]
    } UEBSEQMATRIX;

/**
 * Versorgungsstruktur fuer einen Buestra- bzw. auch Feuerwehrplan
 */
typedef PACKED_STRUCT
  {
    char   aSignalplanName[STD_TEXT]; //!< Bezeichnung des Sonderplans [ASCII][NULLT:][MAX:32]
    SSHORT wEzp;                      //!< Einstiegszeitpunkt fuer den Plan [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF ]
    SSHORT wAzp;                      //!< Ausstiegszeitpunkt fuer den Plan [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF ]
    SSHORT wGsp;                      //!< Haltezeitpunkt [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF ]
    SSHORT wTu;                       //!< Umlaufzeit [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF + 10 ]
    SSHORT wVersion;                  //!< Version des Signalplans [DEF: 1][MIN:0][MAX:999]
    UTINY bNrMinFreigabeListe;        //!< Nummer der Mindestfreigabezeitliste [DEF: 0][MIN: 0] [MAX: MAX_MINDESTFREIGABELISTEN - 1]
    UTINY bNrMinSperrListe;           //!< Nummer der Mindestsperrliste [DEF: 0][MIN: 0] [MAX: MAX_MINDESTSPERRLISTEN - 1]
    UTINY bFS_Nr;                     //!< Nummer der aktuellen Uebergangssequenzen Freigeben --> Sperren [DEF: 0][MIN: 0] [MAX: MAX_UESEQ_KOMBINATIONEN - 1]
    UTINY bSF_Nr;                     //!< Nummer der aktuellen Uebergangssequenzen Sperren --> Freigeben [DEF: 0][MIN: 0] [MAX: MAX_UESEQ_KOMBINATIONEN - 1]
    UTINY bZZ_Nr;                     //!< Nummer der fuer den Signalplan gueltigen ZZ-Matrix [DEF: 0][MIN: 0] [MAX: MAX_ZZ_MATRIZEN-1]
    UTINY bVZ_NrBB;                   //!< Nummer der fuer den Signalplan gueltigen VZ-Matrix [DEF: MAX_UTINY][MIN: 0] [MAX: MAX_VZ_MATRIZEN-1]
    UTINY bVZ_NrEE;                   //!< Nummer der fuer den Signalplan gueltigen VZ-Matrix [DEF: MAX_UTINY][MIN: 0] [MAX: MAX_VZ_MATRIZEN-1]
    UTINY bDummy;                     //!< Fuellbyte
    SIGRUDEFS sSiGruDefs[MAX_SIGNALGRUPPEN];     //!< Festlegungen fuer die einzelnen Signalgruppen
  } SONDERPLAN;

/**
 * Softkey (VD_SOFTKEY)
 */
/*typedef PACKED_STRUCT
  {
    char        aSysPW[STD_TEXT];        //!< Systempasswort zur Authentifizierung und Pruefung von Berechtigungen  [ASCII:][NULLT:][MAX:32]
    char        aSoftKey[EXTENDED_TEXT]; //!< Pfad fuer Datei mit Lizenzschluesselinformationen [ASCII:][NULLT:]
    char        aReserve[EXTENDED_TEXT]; //!< Reserveschluessel fuer spaetere Verwendung [ASCII:][NULLT:]
  } SOFTKEY;*/

/**
 * Ein- und Ausschaltfolgen (VD_AUSSCHALTFOLGEN, VD_EINSCHALTFOLGEN)
 */
/*typedef PACKED_STRUCT
{
    UTINY bFarbe[MAX_EA_SP];    //!< gewuenschte Signalisierung [DEF: MAX_UTINY][MIN: 0][MAX: MAX_VALID_FBKOM - 1]
    SSHORT wZeit[MAX_EA_SP];    //!< relativer Schaltzeitpunkt bezogen auf die Gesamtdauer des EA-Plans [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF]
} SCHALTPUNKT;    //!< Schaltpunkte in EA-Plaenen
*/

/**
 * Struktur fuer einen Ein- / Ausschaltplan
 */
/*typedef PACKED_STRUCT
{
    char    cName[STD_TEXT];    //!< Bezeichnung des Sonderplans [ASCII][NULLT:][MAX:32]
    USHORT  wGesamtDauer;       //!< Gesamtdauer der Folge [MIN:0][MAX: MAX_UMLAUF + 10]
    USHORT  wSisiScharf;        //!< Zeitpunkt ab dem der Zustandswechsel in der Signalsicherung erfolgt [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF]
    USHORT  wZZScharf;          //!< Zeitpunkt ab dem der Zustandswechsel in der ZZ-Ueberwachung erfolgt [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF]
    USHORT  wSE_OK;             //!< Zeitpunkt ab dem Sondereingriffe erlaubt bzw. nicht mehr erlaubt sind [DEF: MAX_SSHORT][MIN: 0][MAX: MAX_UMLAUF]
    SCHALTPUNKT sSP[MAX_SIGNALGRUPPEN]; //!< Schaltpunkte fuer die einzelnen Signalgruppen
} EAPLAN;*/

/**
 * Handphasen (VD_PHASEN)
 * Definition der Handphasen
 */
typedef PACKED_STRUCT
  {
    char  aName[STD_TEXT];           //!< Bezeichnung der Handphase [ASCII][NULLT:][MAX:32]
    UTINY bFarbe[MAX_SIGNALGRUPPEN]; //!< Farbkombination [MIN: 0] [MAX: MAX_VALID_FBKOM - 1][DEF: MAX_UTINY]
  } PHASEN;

/**
 * Uebergangsmatrix (VD_PHASEN_MATRIX)
 */
//typedef UTINY   PHASEN_MATRIX [MAX_PHASEN][MAX_PHASEN];

/**
 * Phasenuebergaenge (VD_PHASENUEBERGANG:)
 * Definition eines Wechsels im Phasenuebergang
 */
/*typedef PACKED_STRUCT
  {
    USHORT  wZeitpunkt[MAX_ZEITEN_PHAUEB]; //!< Zeitpunkte fuer Wechsel Zustand oder Farbe gerechnet vom Phasenwechselbeginn[MIN: 0][MAX:MAX_UMLAUF + 10][DEF: MAX_SSHORT]
    UTINY   bFarbe[MAX_ZEITEN_PHAUEB];     //!< Farbkombinationen[MIN: 0][MAX: MAX_VALID_FBKOM - 1] [DEF: MAX_UTINY]
  } SIGWECHSEL;*/

/**
 * Definition eines Phasenuebergang
 */
/*typedef PACKED_STRUCT
  {
    USHORT  wDauer;                 //!< Gesamtdauer[DEF: MAX_SSHORT][MAX:MAX_UMLAUF + 10]
    UTINY   bFS_KomNr;              //!< Nr. des Uebergangs FS [DEF: 0][MIN: 0] [MAX: MAX_UESEQ_KOMBINATIONEN - 1]
    UTINY   bSF_KomNr;              //!< Nr. des Uebergangs SF [DEF: 0][MIN: 0] [MAX: MAX_UESEQ_KOMBINATIONEN - 1]
    UTINY   bUrsprungsPhase;        //!< Nummer der aktuellen Phase vor dem Uebergang[MIN: 0][MAX: MAX_PHASEN-1][DEF: MAX_UTINY]
    UTINY   bZielPhase;             //!< Nummer der Zielphase nach dem Uebergang[MIN: 0] [MAX: MAX_PHASEN-1][DEF: MAX_UTINY]
    SIGWECHSEL sSigWechsel[MAX_SIGNALGRUPPEN];  //!< Schaltzeiten der einzelnen Signalgruppenwechsel
    char    aName[STD_TEXT];        //!< Bezeichnung des Phasenuebergangs[ASCII][NULLT:][MAX:32]
  } PHASENUEBERGANG;*/

/**
 * Ablaufparameter Phasensteuerung (VD_PHASENDATEN)
 */
typedef PACKED_STRUCT
  {
    UTINY bESBFolge;            // Nummer der EAbilderfolge fuer globales Einschalten [DEF: 0][MIN: 0][MAX: MAX_EAPLAENE - 1]
    UTINY bASBFolge;            // Nummer der EAbilderfolge fuer globales Ausschalten [DEF: 0][MIN: 0] [MAX: MAX_EAPLAENE - 1]
    UTINY bPhaseNachAUS;        // Phasennummer nach AUS[DEF: MAX_UTINY][MIN: 0][MAX: MAX_PHASEN - 1]
    UTINY bPhaseNachAR;         // Phasennummer nach Alles Rot[DEF: MAX_UTINY][MIN: 0][MAX: MAX_PHASEN - 1]
    UTINY bNrMinFreigabeListe;  // Nummer der Mindestfreigabezeitliste [DEF: 0][MIN: 0][MAX: MAX_MINDESTFREIGABELISTEN - 1]
    UTINY bNrMinSperrListe;     // Nummer der Mindestsperrliste [DEF: 0][MIN: 0] [MAX: MAX_MINDESTSPERRLISTEN - 1]
    UTINY bFS_Nr;               // Nummer der gueltigen Uebergangssequenzen Freigeben --> Sperren [DEF: 0][MIN: 0][MAX: MAX_UESEQ_KOMBINATIONEN - 1]
    UTINY bSF_Nr;               // Nummer der gueltigen Uebergangssequenzen Sperren --> Freigeben [DEF: 0][MIN: 0] [MAX: MAX_UESEQ_KOMBINATIONEN - 1]
    UTINY bZZ_Nr;               // Nummer der fuer die Phasensteuerung gueltigen ZZ-Matrix[DEF: 0][MIN: 0][MAX: MAX_ZZ_MATRIZEN-1]
    UTINY bVZ_NrBB;             // Nummer der fuer die Phasensteuerung gueltigen VZ-Matrix[DEF: MAX_UTINY][MIN: 0][MAX: MAX_VZ_MATRIZEN-1]
    UTINY bVZ_NrEE;             // Nummer der fuer die Phasensteuerung gueltigen VZ-Matrix[DEF: MAX_UTINY][MIN: 0][MAX: MAX_VZ_MATRIZEN-1]
    UTINY bDummy;               // Fuellbyte
  } PHASENDATEN;  // Daten fuer den Steuermodus PHASENSTEUERUNG


/**
 * Detektorparameter (VD_DETEKTOR)
 * Physikalische Versorgung fuer Detektoren
 */
typedef PACKED_STRUCT
{
    char  aBezeichnung[STD_TEXT];   //!< Anwenderbezeichnung des Detektors [ASCII][NULLT:][MAX:32]
    UTINY bAnschlussArt;            //!< Physikalischer Anschluss des logischen Detektors [DEF: DET_NICHTVORHANDEN] [REF: CB0002]
    UTINY bAnschlussPlatz;          //!< Anschlussplatz (Platteradresse, Pluskabel) [DEF: 0][MIN: 0][MAX: 8]
    UTINY bAnschlussKanal;          //!< Kanal innerhalb der physikalischen Anschlusseinheit [DEF: 0][MIN: 1][MAX: MAX_UTINY]
    UTINY bTyp;                     //!< Typ des Detektors [REF: CB0003] [DEF: EINFACH_SCHLEIFE]
    UTINY bExtIdx;                  //!< Index fuer Erfassung erweiterter Detektorwerte (nur max. 10 Doppelschleifen) [DEF: MAX_UTINY][MIN: 0][MAX: 9]
    UTINY bFehlerEingang;           //!< Fehlereingang des Detektors 0 = Standard, 255 = keine Fehlererkennung [DEF: 0][MIN: 0][MAX: MAX_UTINY]
    UTINY bAbstandHL;               //!< Abstand des Detektors zur Haltelinie (in Meter) [DEF: 0][MIN: 0] [MAX: MAX_UTINY]
    UTINY bDetLen;                  //!< Laenge des Detektors (in Dezimeter) [DEF: 10][MIN: 0] [MAX: MAX_UTINY]
    UTINY bSigZuordnung[2];         //!< Zuordnung des Detktors zur Signalgruppe [DEF: MAX_UTINY] [MIN: 0] [MAX: MAX_SIGNALGRUPPEN-1]
} VSG_DET;

/**
 * Standardkalender (VD_STDKALENDER)
 * Einzelner Sondertag
 */
/*typedef PACKED_STRUCT
{
    char     aSondertagName[STD_TEXT]; //!< Name des Sonder- oder Feiertags [ASCII][NULLT:]
    UTINY    bTag;                     //!< Tag 1...31 [MIN: 1][MAX: 31][DEF: MAX_UTINY]
    UTINY    bMonat;                   //!< Monat 1...12 [MIN: 1][MAX: 12][DEF: MAX_UTINY]
    UTINY    bJahr;                    //!< Jahr 0...99 [MIN: 0][MAX: 99][DEF: MAX_UTINY]
    UTINY    bPrio;                    //!< Prioritaet des Sondertages [MIN: 1][MAX: 9][DEF: 0]
    UTINY    bTP;                      //!< fuer diesen Sondertag geltender Tagesplan[DEF: MAX_UTINY][MIN: 0] [MAX: MAX_TAGESPLAENE-1]
    UTINY    bDummy;                   //!< Fuellbyte
} SONDERTAG_A;*/

/**
 * Jaehrlich wiederkehrender Sondertag
 */
/*typedef PACKED_STRUCT
{
    char     aSondertagName[STD_TEXT]; //!< Name des Sonder- oder Feiertags [ASCII][NULLT:]
    USHORT   wCode;                    //!< Tagescodierung nach OCIT Standard [MIN: 0][MAX: 9999][DEF: MAX_USHORT]
    UTINY    bPrio;                    //!< Prioritaet des Sondertages [MIN: 1][MAX: 9][DEF: 0]
    UTINY    bTP;                      //!< fuer diesen Sondertag geltender Tagesplan[DEF: MAX_UTINY][MIN: 0][MAX: MAX_TAGESPLAENE-1]
} SONDERTAG_J;*/

/**
 * Zeitbereiche mit besonderer Wochenplanzuordnung (z.B. Ferien)
 */
/*typedef PACKED_STRUCT
{
    char     aFerienName[STD_TEXT];    //!< Name der Ferien [ASCII][NULLT:]
    UTINY    bTagBeginn;               //!< Erster Ferientag 1...31 [MIN: 1][MAX: 31][DEF: MAX_UTINY]
    UTINY    bMonatBeginn;             //!< Beginnmonat der Ferien 1...12 [MIN: 1][MAX: 12][DEF: MAX_UTINY]
    UTINY    bJahrBeginn;              //!< Jahr auf das sich die folgenden Eintraege beziehen [MIN: 0][MAX: 99][DEF: MAX_UTINY]
    UTINY    bTagEnde;                 //!< Letzter Ferientag 1...31 [MIN: 1][MAX: 31][DEF: MAX_UTINY]
    UTINY    bMonatEnde;               //!< Endemonat der Ferien 1...12 [MIN: 1][MAX: 12][DEF: MAX_UTINY]
    UTINY    bJahrEnde;                //!< Jahr auf das sich die folgenden Eintraege beziehen [MIN: 0][MAX: 99][DEF: MAX_UTINY]
    UTINY    bPrio;                    //!< Prioritaet des Sondertages [MIN: 1][MAX: 9][DEF: 0]
    UTINY    bWPlan;                   //!< Gueltige Wochenplanzuordnung [DEF: 0]  [MIN: 0][MAX: MAX_WOCHENPLAENE-1]
} JAUT_ZEITBEREICH;*/

/*typedef PACKED_STRUCT
{
    UTINY   bTag;             //!< Tag 1...31 [MIN: 1][MAX: 31][DEF: MAX_UTINY]
    UTINY   bMonat;           //!< Monat 1...12 [MIN: 1][MAX: 12][DEF: MAX_UTINY]
} FEIERTAG;*/

/**
 * Wechsel Winter-/Sommerzeit
 */
typedef PACKED_STRUCT
{
    UTINY    bMonat_Beginn;   //!< Monat in dem auf Sommerzeit umgestellt wird [MIN: 1][MAX: 12][DEF: 3]
    UTINY    bWoche_Beginn;   //!< Woche innerhalb des Monats der Umstellung [MIN: 1][MAX: 5][DEF: 5]
    UTINY    bTag_Beginn;     //!< Wochentag an dem die Umstellung erfolgt [REF: CB0018] [DEF: SON]
    UTINY    bMonat_Ende;     //!< Monat in dem auf Winterzeit umgestellt wird [MIN: 1][MAX: 12][DEF: 10]
    UTINY    bWoche_Ende;     //!< Woche innerhalb des Monats der Umstellung [MIN: 1][MAX: 5][DEF: 5]
    UTINY    bTag_Ende;       //!< Wochentag an dem die Umstellung erfolgt [REF: CB0018] [DEF: SON]
} SOMMERZEIT;

/**
 * Liste zur Generierung des Jahreskalenders
 */
/*typedef PACKED_STRUCT
{
    SONDERTAG_A         sSondertag[MAX_SONDERTAGE];       //!< Einmalige Sondertage mit festem Datum
    SONDERTAG_J         sFeiertag_F[MAX_FEIERTAGE];       //!< Jaehrliche Feier- und Sondertage
    JAUT_ZEITBEREICH    sFerien[MAX_FERIENEINTRAEGE];     //!< Ferientermine
    FEIERTAG            sOsterSonntag[MAX_OSTERSONNTAGE]; //!< Liste mit Daten von allen Ostersonntagen
    SOMMERZEIT          sSz;                              //!< Wechsel Winter- / Sommerzeit
} STD_KALENDER;*/

/**
 * Archivparameter (VD_ARCHIV)
 * Definition der Steuerflags fuer die Empfaenger von Tagebuchmeldungen
 */
typedef PACKED_STRUCT
{
    UTINY fReserve  : 1;        //!< Reserve fuer weitere Ziele einer Tagebuchmeldung [DEF: 0]
    UTINY fRPC      : 1;        //!< Kennung fuer Tabumeldung an Remote-PC [DEF: JA][REF: CB0026]
    UTINY fWatch    : 1;        //!< Kennung fuer Tabumeldung an Watch [DEF: NEIN][REF: CB0026]
    UTINY fATU      : 1;        //!< Kennung fuer Tabumeldung an ATU [DEF: NEIN][REF: CB0026]
    UTINY fPC       : 1;        //!< Kennung fuer Tabumeldung an PC [DEF: JA][REF: CB0026]
    UTINY fVSR      : 1;        //!< Kennung fuer Tabumeldung an VSR [DEF: JA][REF: CB0026]
    UTINY fBAZ      : 1;        //!< Kennung fuer Tabumeldung an BAZ [DEF: JA][REF: CB0026]
    UTINY fArchiv   : 1;        //!< Kennung fuer Tabumeldung an Archiv [DEF: JA][REF: CB0026]
} TABUKENNUNG;

/**
 * Definition einer Standard-Archivdatei
 */
typedef PACKED_STRUCT
  {
    USHORT      wSize;          //!< Maximale Groesse der Datei in KB [FAKTOR: 2] [DEF: 20] [MIN: 20][MAX: 8000][OTEXT: Groesse][fuer Entw. in 512Byte Bloecken
    UTINY       bMode;          //!< zu verwendender Speichermode [DEF: ANHAENGEN] [REF: CB0051] [OTEXT: Modus]
    UTINY       blocation;      //!< Ort an dem die Datei abgelegt werden soll [DEF: SRAM][REF: CB0079][OTEXT: Ort]
    UTINY       bEntryLen;      //!< Strukturlaenge eines einzelnen Eintrags der Datei 0 = nicht aktiv [DEF: 0] [OTEXT: Eintragslaenge]
    UTINY       bBackup;        //!< Backupmodus [DEF: KEIN_BACKUP] [REF: CB0078]
    char        cDateiName[STD_TEXT]; //!< Dateiname der zu verwendenden Datei (relativ zum Standardarchivpfad) [ASCII:][NULLT:][MAX:32]
  } AR_STD_JOB;

/**
 * Definition der anzulegenden Standard-Archivdateien
 */
typedef PACKED_STRUCT
  {
    ULONG       lArSpeicherGroesse; //!< Groesse des Archivspeichers im SRAM (in KB) [REF: CB0076] [DEF:AR_STD]
    UTINY       bMaxActiveJobs;     //!< Max. aktive Auftraege [INVISIBLE:][DEF: 20][MIN: 10][MAX: 40]
    UTINY       bArchivVollEvent;   //!< freier Speicher fuer Betriebstagebuchmeldung Archiv voll [DEF: 5][MIN: 0][MAX: 50]
    TABUKENNUNG bTabuZiele;         //!< Versorgung der Ziele fuer eine Betriebstagebuchmeldung [DEF: 0x0B] [NEWPAGE: Tagebuch]
    UTINY       bReserve;           //!< Reservebyte [INVISIBLE:][DEF: 0][MIN: 0][MAX: MAX_UTINY]
    AR_STD_JOB  sTabuStd;           //!< Betriebstagebuchspeicher fuer Standard - Meldungen
    AR_STD_JOB  sTabuSisi;          //!< Betriebstagebuchspeicher fuer Sisi - Meldungen
    AR_STD_JOB  sOPNV;              //!< Betriebstagebuchspeicher fuer OePNV Telegramme
    AR_STD_JOB  sSIZULI;            //!< Unfallspeicher (TABU_SIZULI)
    AR_STD_JOB  sBZ;                //!< Betriebstagebuchspeicher fuer Betriebszustaende
    AR_STD_JOB  sRotFahrer;         //!< Betriebstagebuchspeicher fuer Rotlichtfahrer
    AR_STD_JOB  sAfd;               //!< Betriebstagebuchspeicher fuer OeV-Rohtelegramme
    AR_STD_JOB  sZSStat;            //!< Betriebstagebuchspeicher fuer Detektor Standard Werte
    AR_STD_JOB  sFREI1;             //!< Betriebstagebuchspeicher fuer freie Datei (FREI1)
    AR_STD_JOB  sFREI2;             //!< Betriebstagebuchspeicher fuer freie Datei (FREI2)
    AR_STD_JOB  sFREI3;             //!< Betriebstagebuchspeicher fuer freie Datei (FREI3)
    AR_STD_JOB  sFREI4;             //!< Betriebstagebuchspeicher fuer freie Datei (FREI4)
    AR_STD_JOB  sFREI5;             //!< Betriebstagebuchspeicher fuer freie Datei (FREI5)
    AR_STD_JOB  sFREI6;             //!< Betriebstagebuchspeicher fuer freie Datei (FREI6)
  }  VSG_ARCHIV;

/*
 * Tagesplanparameter (VD_TAGESPLAN)
 */
/*typedef PACKED_STRUCT
  {
    ULONG       lUhrzeit;               // Sekunden ab Mitternacht zu der die Aktion ausgefuehrt werden soll [MIN: 0][MAX: 86399][DEF: MAX_ULONG]
    USHORT      wAuftrag;               // Entspricht der functionality im Message-Header [REF: CB0049] [DEF: KEIN_AUFTRAG]
    UTINY       bZiel;                  // Entspricht der destination im Message-Header [REF: CB0103][DEF: MAX_UTINY]
    UTINY       bAktionsTyp;            // Spezifiziert die Abwicklung des Auftrags [REF: CB0050][DEF: AT_UNVERSORGT]
    UTINY       bAnzParameter;          // Anzahl der nachfolgend gueltigen Parameter [DEF: 0] [MIN: 0] [MAX: MAX_TPE_PAR]
    UTINY       bProprietaer;           // Aktionstyp ist herstellerspezifisch und bleibt nach Standardversorgungen erhalten [REF: CB0026][DEF: NEIN]
    UTINY       bParameter[MAX_TPE_PAR];// Parameter fuer den Auftrag [DEF: 255]
  } TP_EINTRAG_STD;     // Universalstruktur fuer Tagesplaneintraege (neues dem Standard angepasstes Format) [CONTROL:]

typedef PACKED_STRUCT
{
    char    aBezeichnung[STD_TEXT];             // Name des Tagesplanes [INSTANZNAME:][ASCII][MAX:32][NULLT:]
    TP_EINTRAG_STD  sPlan[MAX_TP_EINTRAEGE];    // Aufbau eines Tagesplan
} TAGESPLAN; // Tagesplan in neuer Struktur angelehnt an OCIT Standard
*/

/**
 * Tagebuchsteuerung (VD_TABUSTEUERUNG)
 * Steuerung der Empfaenger von Tagebuchmeldungen
 */
/*typedef PACKED_UNION
  {
    ULONG    lTabuSelector;       //!< Struktur als Wort
    PACKED_STRUCT
    {
        ULONG      lRes1       : 4;     //!< Reserve [MIN: 0][MAX: 15][DEF: 0]
        ULONG      lRPC        : 4;     //!< Steuerparameter fuer Uebertragung der Meldung an den Remote-PC [MIN: 0][MAX: 15][DEF: PAR_ANZ_8] [REF: CB0060]
        ULONG      lATU        : 4;     //!< Steuerparameter fuer Uebertragung der Meldung an die ATU [MIN: 0][MAX: 15][DEF: PAR_ANZ_8] [REF: CB0060]
        ULONG      lWatch      : 4;     //!< Steuerparameter fuer Uebertragung der Meldung an eine Waehlzentrale [MIN: 0][MAX: 15][DEF: ERR_UNCRIT] [REF: CB0067]
        ULONG      lArchiv     : 4;     //!< Steuerparameter fuer Eintrag der Meldung ins Archiv [MIN: 0][MAX: 15][DEF: PAR_ANZ_8] [REF: CB0060]
        ULONG      lZentrale   : 4;     //!< Steuerparameter fuer Uebertragung der Meldung an die Zentrale [MIN: 0][MAX: 15][DEF: PAR_ANZ_8] [REF: CB0060]
        ULONG      lHP         : 4;     //!< Steuerparameter fuer Uebertragung der Meldung an das BAZ [MIN: 0][MAX: 15][DEF: PAR_ANZ_8] [REF: CB0060]
        ULONG      lPC         : 4;     //!< Steuerparameter fuer Uebertragung der Meldung an den PC [MIN: 0][MAX: 15][DEF: PAR_ANZ_8] [REF: CB0060]
    } sTabuSelector;
  } TABU_STRG;*/

/*typedef PACKED_STRUCT
  {
    TABU_STRG   sStdFehler[MAX_TABUTEXTE];     // Filter fuer Standardfehlernummern
    TABU_STRG   sSisiFehler[MAX_SISITEXTE];    // Filter fuer Signalsicherungsfehlernummern
  } TABUSTRGLISTE;*/

/**
 * OEV-4Punktsignal (VD_OEV_4PUNKT)
 */
typedef PACKED_STRUCT
  {
    char  bName[STD_TEXT];  //!< Name des OEV-4Punktsignals [ASCII:][MAX:32][NULLT:]
    UTINY bSGLinks;         //!< 1. Zugehoerige Signalgruppe [DEF: MAX_UTINY] [MIN: 0] [MAX: MAX_SIGNALGRUPPEN-1]
    UTINY bSGMitte;         //!< 2. Zugehoerige Signalgruppe [DEF: MAX_UTINY] [MIN: 0] [MAX: MAX_SIGNALGRUPPEN-1]
    UTINY bSGRechts;        //!< 3. Zugehoerige Signalgruppe [DEF: MAX_UTINY] [MIN: 0] [MAX: MAX_SIGNALGRUPPEN-1]
    UTINY bSGPseudo;        //!< Dummy Signalgruppe (nur bei OEV-4Punktsignalen fuer eine Richtung [DEF: MAX_UTINY] [MIN: 0] [MAX: MAX_SIGNALGRUPPEN-1]
    UTINY bTyp;             //!< Typinformation der Signalgruppe [REF: CB0304][DEF: SIG_UNVERSORGT]
    UTINY bUebergang;       //!< Signal benutzt einen Uebergang [REF: CB0306][DEF: OHNE_UEBERGANG]
    UTINY bRotSignal;       //!< Art des Gesperrt-Signals [REF: CB0305][DEF: ROT_3FELDIG]
    UTINY bTKZuordnung;     //!< Teilkreuzungszuordnung des OEV-4Punktsignals [REF: CB0043] [DEF: NICHTVORHANDEN]
  } OEV_4PUNKT;

/**
 * Permissivsignal (VD_PERMISSIV)
 */
typedef PACKED_STRUCT
{
    char  bName[STD_TEXT];  //!< Name des Permissivsignals [ASCII:][MAX:32][NULLT:]
    UTINY bHauptSG;         //!< Hauptsignalgruppe dem das Permissivsignal zugeordnet ist [DEF: MAX_UTINY] [MIN: 0] [MAX: MAX_SIGNALGRUPPEN-1]
    UTINY bPermissivSG;     //!< zugehoeriges Permissivsignal [DEF: MAX_UTINY] [MIN: 0] [MAX: MAX_SIGNALGRUPPEN-1]
    UTINY bTyp;             //!< Typ des Permissivsignals [DEF: PERMTYP_OR] [REF: CB0307]
    UTINY bBedFreiFarbe;    /*!< OR-Typ: Farbe des Hauptsignals bzw. AND-Typ: Farbe des Permissivsignals welche den bedingten Freigabezustand signalisiert
 [DEF: FK_DUNKEL] [MIN: 0] [MAX: MAX_VALID_FBKOM - 1] */
} PERMISSIV;

/**
 * Quitungsanzeigen (VD_SENLI)
 * Setzen von Ausgaengen aufgrund von Eingangs- und Ausgangszustaenden (Senli - Funktionalitaet)
 */
typedef PACKED_STRUCT
  {
    UTINY       bEingang;               //!< logischer Eingang der gesetzt werden muss (Anforderungstaster) [DEF: MAX_UTINY][MIN: 0][MAX: MAX_DETEKTOREN - 1]
    UTINY       bSigGruppe;             //!< Nummer der relevanten Signalgruppe [DEF: MAX_UTINY] [MIN: 0] [MAX: MAX_SIGNALGRUPPEN-1]
    UTINY       bBedingung;             //!< Farbzustand der relevanten Signalgruppe  (z.B. Signalgruppe auf Rot) [DEF: MAX_UTINY][MIN: 0][MAX: MAX_VALID_FBKOM - 1]
    UTINY       bQuittung1;             //!< logischer Ausgang der gesetzt werden soll, sofern Bedingung erfuellt ist [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE - 1]
    UTINY       bQuittung2;             //!< logischer Ausgang der gesetzt werden soll, sofern Bedingung erfuellt ist [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE - 1]
    UTINY       bQuittung3;             //!< logischer Ausgang der gesetzt werden soll, sofern Bedingung erfuellt ist [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE - 1]
    UTINY       bQuittung4;             //!< logischer Ausgang der gesetzt werden soll, sofern Bedingung erfuellt ist [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE - 1]
    UTINY       bQuittung5;             //!< logischer Ausgang der gesetzt werden soll, sofern Bedingung erfuellt ist [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE - 1]
    char        aBezeichnung[STD_TEXT]; //!< Name der Quittungsbedingung [MAX:32][NULLT:]
  } QUITTUNGSTAB;

/**
 * Tagesplaene (VD_TPZUORDNUNG)
 * Struktur fuer einen Wochenplan nach OCIT Standard
 */
/*typedef PACKED_STRUCT
  {
    char    aBezeichnung[STD_TEXT];   //!< Name des Wochenplanes[ASCII][MAX:32][NULLT:]
    UTINY   bTPNr[MAX_TP_TAGE];       //!< Zuordnung des Tagesplans fuer den entsprechenden Wochentag [MIN: 0][MAX: MAX_TAGESPLAENE-1][DEF: 0]
  } WOCHENPLAN;*/

/**
 * Definition der Wochenplaene an normalen Wochentagen, Ferienzeiten u. weiteren speziellen Zeitbereichen
 */
/*typedef PACKED_STRUCT
  {
    WOCHENPLAN  sWP_Std;              //!< Standard-Zuordnung der Tagesplaene fuer den entsprechenden Wochentag
    WOCHENPLAN  sWP_Ferien;           //!< Standard-Zuordnung der Tagesplaene fuer den entsprechenden Ferientag
    WOCHENPLAN  sWP_2;                //!< 1. Alternativ-Zuordnung der Tagesplaene fuer entsprechende Zeitbereiche
    WOCHENPLAN  sWP_3;                //!< 2. Alternativ-Zuordnung der Tagesplaene fuer entsprechende Zeitbereiche
  } TP_STD_ZUORDNUNG;*/

/**
 * AFD-Parameter (VD_TAE)
 */
typedef PACKED_STRUCT
  {
    USHORT      wMP_Nr;     //!< Meldepunktnummer aus dem OeV-Funktelegramm [DEF: MAX_USHORT]
    USHORT      wL_Nr;      //!< Liniennummer (Einschraenkung), welche fuer den Meldepunkt relevant ist [DEF: MAX_USHORT][MAX:999]
    USHORT      wR_Nr;      //!< Routennummer (Einschraenkung), welche fuer den Meldepunkt relevant ist [DEF: MAX_USHORT][MAX:999]
  } MELDEPUNKT;

/**
 * Versorgungsdaten fuer die TAE -Task
 */
/*typedef PACKED_STRUCT
  {
    UTINY       bWiederholDauer;      //!< Maximale Dauer fuer Erkennung eines Wiederholungstelegramms (in 100 ms) [DEF: 30] [MIN: 0][MAX: 50]
    UTINY       bModulationsart;      //!< Kennung fuer Einstellung der Modulationsart der Funktelegramme [DEF: MA_NO_AFD] [REF: CB0046]
    UTINY       bAnzahlMp;            //!< Anzahl der versorgten Meldepunkte [DEF: 0][MIN: 0][MAX: MAX_TAE_MP]
    UTINY       bFunktelegrammTyp;    //!< Kennung fuer den Typ der Funktelegramme [DEF: NO_TEL] [REF: CB0048]
    UTINY       bUebwStartStunde;     //!< Beginn des Zeitraums fuer Ueberwachung auf eintreffende TAE-Telegramme [DEF: 6][MIN: 0][MAX: 23]
    UTINY       bUebwStartMinute;     //!< Beginn des Zeitraums fuer Ueberwachung auf eintreffende TAE-Telegramme [DEF: 0][MIN: 0][MAX: 59]
    UTINY       bUebwEndeStunde;      //!< Ende des Zeitraums fuer Ueberwachung auf eintreffende TAE-Telegramme [DEF: 22][MIN: 0][MAX: 23]
    UTINY       bUebwEndeMinute;      //!< Ende des Zeitraums fuer Ueberwachung auf eintreffende TAE-Telegramme [DEF: 0][MIN: 0][MAX: 59]
    USHORT      wTimeout;             //!< Timeoutzeit nach der auf Fehler erkannt wird (in Minuten) [DEF: 60][MIN: 0][MAX: 1440]
    USHORT      wSpezial;             //!< Sonderinformation fuer Sonderbehandlungen (z.B. VNetS) [REF: CB0082][DEF: OEV_STANDARD]
    MELDEPUNKT  sMpListe[MAX_TAE_MP]; //!< Liste mit Meldepunkten welche weitergeleitet werden sollen
  } VSG_TAE;*/

/**
 * BAZ-Parameter (VD_BAZ)
 * Initialisierungsdaten fuer die Steuerung im BAZ
 */
typedef PACKED_STRUCT
  {
    UTINY       bAbtastZyklus;  //!<Abtastintervall der Tastatur in 20ms-Einheiten [DEF: 2][MIN: 1] [MAX: 50]
    UTINY       bEntprellung;   //!<Grundentprellung eingeschaltet [REF: CB0026][DEF: JA]
    UTINY       bAnzSpalten;    //!<Anzahl Spalten der Tastatur - Matrix [DEF: 3][MIN: 0] [MAX: 3]
    UTINY       bAnzReihen;     //!<Anzahl Reihen der Tastatur - Matrix [DEF: 7][MIN: 0] [MAX: 8]
    UTINY       bFTaste1;       //!<Tastaturcode der 1. Funktionstaste [DEF: 22][MIN: 1] [MAX: 24]
    UTINY       bFTaste2;       //!<Tastaturcode der 2. Funktionstaste [DEF: 0][MIN: 1] [MAX: 24]
    UTINY       bFTaste3;       //!<Tastaturcode der 3. Funktionstaste [DEF: 0][MIN: 1] [MAX: 24]
    UTINY       bTM_Std;        //!<Tastaturabfragemodus von Standardtasten [REF: CB0044][DEF: TM_TIEF_FL]
    UTINY       bTM_Fkt;        //!<Tastaturabfragemodus von Funktionstasten [REF: CB0044][DEF: TM_HOCH_FL]
    UTINY       bMaxTastenDown; //!<Maximale Anzahl an gleichzeitig gedrueckten Tasten [DEF: 2][MIN: 1] [MAX: 4]
    UTINY       bAnzLEDs;       //!<Anzahl an Tastatur-LED's [DEF: 22][MIN: 0] [MAX: 32]
    UTINY       bTSKontakt;     //!<Tastenkennung fuer Tuerschliesskontakt [DEF: 19][MIN: 1] [MAX: 24]
  } INIT_BAZ;

/**
 * Initialisierungsdaten fuer die Steuerung im BAZ
 */
typedef PACKED_STRUCT
  {
    UTINY       bAbtastZyklus;  //!< Abtastintervall der Eingaenge in 20ms - Einheiten [DEF: 5][MIN: 1] [MAX: 50]
    UTINY       bEntprellung;   //!< Grundentprellung bei Eingangsabtastung eingeschaltet [REF: CB0026][DEF: JA]
    UTINY       bAnzInputs;     //!< Anzahl an Eingaengen ( 8er Schritte ) [DEF: 0][MIN: 1] [MAX: MAX_UTINY]
    UTINY       bAnzOutputs;    //!< Anzahl Ausgaengen ( 8er Schritte ) [DEF: 0][MIN: 1] [MAX: MAX_UTINY]
    UTINY       bAbtastmodus;   //!< Abtastmodus der Eingaenge [REF: CB0072][DEF: IM_AKTUELL_POS]
    UTINY       bOutputMode;    //!< Moduseinstellung fuer Ausgangsansteuerung [REF: CB0073][DEF: INVERTIEREN]
    UTINY       bInputOffset;   //!< Offset ab welchem Eingangs-Portbyte eingelesen wird [DEF: 0][MIN: 0] [MAX: 63]
    UTINY       bOutputOffset;  //!< Offset ab welchem Ausgangs-Portbyte gelesen wird [DEF: 0][MIN: 0] [MAX: 63]
  } INIT_PAR_IO;

/**
 * Versorgungsdaten fuer die BAZ-Steuerung
 */
/*typedef PACKED_STRUCT
  {
    UTINY       bStdTasten[MAX_PANEL_TASTEN];     //!< Tastaturcodes fuer Standardtasten[DEF: 0][MIN: 1] [MAX: MAX_UTINY]
    UTINY       bSonderTasten[MAX_PANEL_TASTEN];  //!< Tastaturcodes fuer Sondertasten (nur 8 nutzbar) [DEF: 0][MIN: 1] [MAX: MAX_UTINY]
    UTINY       bTastaturLED[MAX_BAZ_LEDS];       //!< Nummern der Tastatur - LED's [DEF: 255][MIN: 0] [MAX: 31]
    INIT_BAZ    sVsgBAZ;                          //!< Versorgung des BAZ
    INIT_PAR_IO sVsgParIO;                        //!< Versorgung der parallelen IO's der BAZ
  } VSG_BAZ;*/

/**
 * Zeitempfangsfunktionalitaeten-Parameter (VD_ZE_INIT)
 * Initialisierung fuer Zeitempfang (DCF, GPS, Detektor-Sy)
 */
/*typedef PACKED_STRUCT
  {
    UTINY   bZeitEmpfangArt;       //!< Art des Zeitempfangs [DEF: NO_ZE][REF: CB0024][PAGENAME: Allgemeines]
    STINY   bZeitKorrektur;        //!< Zeitdifferenz (in Stunden) gegenueber der empfangenen Zeit [DEF: 0][MIN: -12][MAX: 12]
    STINY   bZeitzone;             //!< Zeitzone (Abweichung von der GMT-Zeit) in der sich das Geraet befindet [DEF: 1][MIN: -12][MAX: 12]
    UTINY   bRueckrechenVerfahren; //!< Rueckrechenverfahren fuer die Referenzzeiten [DEF: RRV1_1] [REF: CB0029]
    SSHORT  wRefliVersatz;         //!< Zeitwert fuer einen generellen Versatz der Referenzzeiten [DEF: 0]
    SLONG   lLaengengrad;          //!< Position des Geraets in Millisekunden - wichtig fuer GPS - Empfang [DEF: 0]
    SLONG   lBreitengrad;          //!< Position des Geraets in Millisekunden - wichtig fuer GPS - Empfang [DEF: 0]
    USHORT  wZeitSyEingang;        //!< Eingang fuer Zeitsynchronisierung [MIN: 0][MAX: MAX_DETEKTOREN-1][DEF: MAX_USHORT]
    UTINY   bSyStunde;             //!< Uhrzeit (Stunde) auf die synchronisiert werden soll [DEF: 12][MIN: 0][MAX: 23]
    UTINY   bSyMinute;             //!< Uhrzeit (Minute) auf die synchronisiert werden soll [DEF: 0][MIN: 0][MAX: 59]
    UTINY   bSySekunde;            //!< Uhrzeit (Sekunde) auf die synchronisiert werden soll [DEF: 0][MIN: 0][MAX: 59]
    UTINY   bZQPrioritaet[MAX_PRIO_FELDER]; //!< Prioritaeten der verschiedenen Uhrzeitquellen [DEF: TIME_UNUSED] [REF: CB0016]
    USHORT  wZeitAbweichung;       //!< Maximal zulaessige Abweichung der Systemzeit von der tatsaechlichen Zeit ohne Tabu-Eintrag in Sek. [DEF: 2][MIN: 1]
    USHORT  wMaxDCFAusfall;        //!< Zulaessige Ausfallzeit des DCF / GPS ohne Tagebucheintrag (in Minuten) [DEF: 10]
    USHORT  wZQstabilDauer;        //!< Maximale Ausfallsdauer einer Zeitquelle in Min. bis Prioritaet zurueckgestuft wird [DEF: 30]
    USHORT  wZSAbfrageZyklus;      //!< Abfragezyklus fuer den Zeitserver in Minuten (0 = keine Abfrage) [DEF: 8][MIN: 0][MAX: 1440]
   } ZE_INIT;*/

/**
 * IO-Parameter(VD_VSG_IO)
 */
typedef PACKED_UNION
    {
    USHORT          wIO;           //!< Bitfeld zur Konfiguartion des Ports als Ein- bzw.als Ausgang
    PACKED_STRUCT
        {
        USHORT      wP16  : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069][LABELOFFSET: -1]
        USHORT      wP15  : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP14  : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP13  : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP12  : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP11  : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP10  : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP9   : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP8   : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP7   : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP6   : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP5   : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP4   : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP3   : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP2   : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        USHORT      wP1   : 1;     //!< Konfiguration des Ports [DEF: EINGANG][REF: CB0069]
        } sIO;
    } BFD_IO;

/**
 * statische IO-Versorgung - nicht nachversorgba
 */
/*typedef PACKED_STRUCT
{
    UTINY       bPlatterTyp[MAX_PLATTER];     //!< Typ des Platters mit entsprechender BFD-Adresse [REF:CB0052][DEF: PT_NOTINSTALLED]
    BFD_IO      sBfd[MAX_PLATTER];            //!< IO Konfiguration der BFD-Ports mit entsprechender BFD-Adresse [NEWPAGE: Platterkonfiguration]
    UTINY       bBAZInputs[MAX_BAZ_INPUTS];   //!< Zuordnung BAZ-Eingang zu logischem Detektor [DEF: MAX_UTINY][MIN: 0][MAX: MAX_DETEKTOREN - 1]
    UTINY       bBAZOutputs[MAX_BAZ_OUTPUTS]; //!< Zuordnung BAZ-Ausgang zu logischem Detektor [DEF: MAX_UTINY][MIN: 0][MAX: MAX_DETEKTOREN - 1]
} VSG_IO;*/

/**
 * Standardaufgaben der IO-Bearbeitung-Parameter (VD_IO_INIT)
 */
/*typedef PACKED_STRUCT
  {
    UTINY       bAusgang;           //!< Ausgang, der ggf. gesetzt werden soll [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE-1]
    UTINY       bRMKanal;           //!< Rueckmeldekanal, der ggf. gesetzt werden soll [DEF: MAX_UTINY][MIN: 0][MAX: MAX_RM_KANAELE-1]
    UTINY       bActivLevel;        //!< Zustand des Ausgangs der gesetzt werden soll [DEF: OP_SIG_ON][REF: CB0040]
    UTINY       bSondermeldung;     //!< Sondermeldung, die erzeugt werden soll [DEF: 0][MIN: 1][MAX: 32]
  } SYSEVENT;*/

/**
 * Versorgungsdaten fuer Einschaltblockierungen
 */
typedef PACKED_STRUCT
{
    UTINY bGesamtEin;               //!< Eingang fuer Einschaltblockierung fuer Gesamtkreuzung [DEF: MAX_UTINY][MIN: 0][MAX: MAX_DETEKTOREN-1]
    UTINY bFehlerEintrag;           //!< Eingang fuer Blockierung der Eintragung von Fehlermeldungen[DEF: MAX_UTINY][MIN: 0][MAX: MAX_DETEKTOREN-1]
    UTINY bTKEin[MAX_TKR];          //!< Eingang fuer Einschaltblockierung fuer Teilkreuzungen 1-4 [DEF: MAX_UTINY][MIN: 0][MAX: MAX_DETEKTOREN-1]
} EA_BLOCK;

/**
 * Versorgungsdaten fuer Standardaufgaben der IO-Bearbeitung
 */
/*typedef PACKED_STRUCT
{
    USHORT      wFWEingriff[MAX_FEUERWEHRPLAENE]; //!< Zuordnung von Eingaengen zur Ausloesung von Feuerwehreingriffen[DEF: MAX_USHORT][MIN: 0][MAX: MAX_DETEKTOREN-1]
    USHORT      wBahnEingriff[MAX_BUESTRAPLAENE]; //!< Zuordnung von Eingaengen zur Ausloesung von Bahneingriffen[DEF: MAX_USHORT][MIN: 0][MAX: MAX_DETEKTOREN-1]
    UTINY       bQuitArt;           //!< Typ der Quittungslampenlogik [DEF: AL_QART_WARTEN][REF: CB0037]
    UTINY       bQuitMode;          //!< Art der Anzeige der Quittungslampen (statisch / blinkend) [DEF: OP_SIG_ON][REF: CB0040]
    UTINY       bRotFahrerMode;     //!< Art der Rotfahrerdetektion (fallende Flanke / steigende Flanke) [DEF: RF_FLANKE_FALLEND][REF: CB0081]
    UTINY       bBAZ_TKSteuerung;   //!< Teilknoten welcher ueber die BAZ-Tastenkombination T_FKT u. T_AUS_B geschalten wird [DEF: MAX_UTINY][MIN: 1][MAX: MAX_TKR]
    SSHORT      wMexwaIntervall;    //!< Zeitraum zum Aufaddieren (Bilden) fuer TASS-Detektorwerte (in Sekunden), 0 = Umlaufdauer [DEF: 900] [ZEIT: 1, 0.1][MIN:0][MAX: MAX_UMLAUF + 10 ]
    UTINY       bTassGlAlpha_1;     //!< TASS Glaettungsfaktor fuer steigende Tendenz [DEF: GF_EINS] [REF: CB0001]
    UTINY       bTassGlAlpha_2;     //!< TASS Glaettungsfaktor fuer fallende Tendenz [DEF: GF_EINS] [REF: CB0001]
    EA_BLOCK    sBlockierung;       //!< Versorgung fuer Blockierungen
    SYSEVENT    sSysEvent[MAX_SYSEREIGNISSE];   //!< Konfiguration fuer parallele Meldung von Systemereignissen
    UTINY       bWatch;             //!< Geraet haengt an einer Waehlzentrale [REF: CB0075][DEF: NO_REMOTE_CON]
    UTINY       bTelNr;             //!< Index auf eine versorgte Telefonnummer [DEF: MAX_UTINY][MIN: 1][MAX: 3]
    UTINY       bSGBlinkmodus;      //!< Blinkabschaltung der Signalgruppen in Abhaengigkeit vom TK-Zustand [REF: CB0074][DEF: BM_STANDARD]
    UTINY       bAusgangTK1;        //!< Ausgang der in Abhaengigkeit vom Blinkmodus gesetzt wird [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE-1]
    UTINY       bAusgangTK2;        //!< Ausgang der in Abhaengigkeit vom Blinkmodus gesetzt wird [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE-1]
    UTINY       bAusgangTK3;        //!< Ausgang der in Abhaengigkeit vom Blinkmodus gesetzt wird [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE-1]
    UTINY       bAusgangTK4;        //!< Ausgang der in Abhaengigkeit vom Blinkmodus gesetzt wird [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE-1]
    UTINY       bUSVAusgang;        //!< Ausgang zum Abschalten der USV [DEF: MAX_UTINY][MIN: 0][MAX: MAX_AUSGAENGE - 1]
    UTINY       bNK1;               //!< SIO wo der Nachbarknoten angeschlossen ist [DEF: NOT_CONNECTED][REF: CB0080]
    UTINY       bNK2;               //!< SIO wo der Nachbarknoten angeschlossen ist [DEF: NOT_CONNECTED][REF: CB0080]
    USHORT      wTuerKontakt;       //!< Zuordnung eines Eingangs zur Ausloesung einer Meldung Geraetetuer-Kontakt[DEF: MAX_USHORT][MIN: 0][MAX: MAX_DETEKTOREN-1]
    USHORT      wEVUKontakt;        //!< Zuordnung eines Eingangs zur Ausloesung einer Meldung EVU-Tuer-Kontakt[DEF: MAX_USHORT][MIN: 0][MAX: MAX_DETEKTOREN-1]
    USHORT      wWartungszeit;      //!< Zeitdauer (in Min.) fuer eine Geraetewartung bis diese wieder automatisch aufgeloest wird [DEF: 120][MIN: 0][MAX: MAX_USHORT]
    UTINY       bWartungsverhalten; //!< Verhalten fuer Meldungen und Ist-Vektoren bei Wartungseingriff [DEF: SRV_SEND_ONLY_VECTOR] [REF: CB0087]
    UTINY       bDummy[25];         //!< Fuellbytes zur Reserve [DEF: MAX_UTINY]
} IO_DIV_INIT;*/

/**
 * Ueberwachungszeitraum fuer einen Detektor-Parameter (VD_DETUEBERWACHUNG)
 * Definition eines Ueberwachungszeitraums fuer Detektoren
 */
typedef PACKED_STRUCT
  {
    USHORT wMaxBelegung;            //!< Maximale Belegungsdauer innerhalb des angegebenen Zeitbereichs (in Minuten) [DEF: 0][MIN: 0][MAX: 1440]
    USHORT wMaxLuecke;              //!< Maximale Lueckendauer innerhalb des angegebenen Zeitbereichs (in Minuten) [DEF: 0][MIN: 0][MAX: 7200]
    USHORT wFlatterSchwelle;        //!< Minimaler zeitlicher Abstand zwischen identischen Flanken (in x/100 Sekunden bzw. x*10ms, 0=deaktiviert) [DEF: 0][MIN: 0][MAX: 300]
    char   aBezeichnung[STD_TEXT];  //!< Anwenderbezeichnung des Ueberwachungsbereichs [ASCII][NULLT:][MAX:32]
  } DET_UEBW;

/**
 * Rotlichtfahrer-Parameter (VD_ROTFAHRER)
 * Versorgungsstruktur fuer einen Rotlichtfahrer
 */
typedef PACKED_STRUCT
  {
    UTINY       bSigGruppe; //!< Nummer der relevanten Signalgruppe [DEF: MAX_UTINY] [MIN: 0] [MAX: MAX_SIGNALGRUPPEN-1]
    UTINY       bBedingung; //!< Farbzustand der relevanten Signalgruppe  (z.B. Signalgruppe auf Rot) [DEF: MAX_UTINY][MIN: 0][MAX: MAX_VALID_FBKOM - 1]
    USHORT      wDelay;     //!< Verzoegerungszeit, wie lange der Zustand bereits gesetzt sein muss (in 10 ms) [DEF: 0] [MIN: 0] [MAX: 1000]
    USHORT      wDetektor;  //!< Detektor der ansprechen muss [DEF: MAX_USHORT] [MIN: 0] [MAX: MAX_DETEKTOREN - 1]
  } VSG_ROTFAHRER;

/**
 * Kommunikation-Parameter (VD_VSG_COM)
 */
typedef PACKED_STRUCT
  {
    UTINY       bUsage;       //!< Nutzung des entsprechenden Device [REF: CB0068][DEF: SIO_RESERVED]
    UTINY       bBaudrate;    //!< Baudrate [REF: CB0038][DEF: BAUD_19200]
    UTINY       bDatabits;    //!< Datenbits [REF: CB0063][DEF: MM_8_DATENBITS]
    UTINY       bParity;      //!< Paritaet [REF: CB0054][DEF: MM_NO_PARITY]
    UTINY       bStopbits;    //!< Stopbits [REF: CB0062][DEF: MM_1_STOPBIT]
    UTINY       bProtokoll;   //!< Protokoll das standardmaessig eingestellt wird [DEF: NO_PROTOKOLL][REF: CB0055]
    UTINY       bLogicDest;   //!< Zugeordnete logische Destination [DEF: BETA][REF: CB0103]
    UTINY       bReserve;     //!< Fuellbyte [INVISIBLE:][DEF: 0]
  } SIO_INFO;

typedef PACKED_STRUCT
  {
    SIO_INFO    sSioInt1;     //!< Informationen zur Nutzung des 8270-internen Uarts 1
    SIO_INFO    sSioInt2;     //!< Informationen zur Nutzung des 8270-internen Uarts 2
    SIO_INFO    sSioInt3;     //!< Informationen zur Nutzung des 8270-internen Uarts 3
    SIO_INFO    sSioInt4;     //!< Informationen zur Nutzung des 8270-internen Uarts 4
    SIO_INFO    sSioInt5;     //!< Informationen zur Nutzung des 8270-internen Uarts 5
    SIO_INFO    sSioInt6;     //!< Informationen zur Nutzung des 8270-internen Uarts 6
    SIO_INFO    sSioExt1;     //!< Informationen zur Nutzung des 1 Uarts des externen Schnittstellenbausteins
    SIO_INFO    sSioExt2;     //!< Informationen zur Nutzung des 2 Uarts des externen Schnittstellenbausteins
    SIO_INFO    sSioExt3;     //!< Informationen zur Nutzung des 3 Uarts des externen Schnittstellenbausteins
    SIO_INFO    sSioExt4;     //!< Informationen zur Nutzung des 4 Uarts des externen Schnittstellenbausteins
    SIO_INFO    sSioExt5;     //!< Informationen zur Nutzung des 1 Uarts des 2. externen Schnittstellenbausteins
    SIO_INFO    sSioExt6;     //!< Informationen zur Nutzung des 2 Uarts des 2. externen Schnittstellenbausteins
    SIO_INFO    sSioExt7;     //!< Informationen zur Nutzung des 3 Uarts des 2. externen Schnittstellenbausteins
    SIO_INFO    sSioExt8;     //!< Informationen zur Nutzung des 4 Uarts des 2. externen Schnittstellenbausteins
  } VSG_COM;

/**
 * Logische Ausgaenge-Parameter (VD_AUSGANG)
 * Versorgungsdaten fuer logische Ausgaenge
 */
typedef PACKED_STRUCT
  {
    char  aBezeichnung[STD_TEXT];  //!< Anwenderbezeichnung des Ausgangs [ASCII][NULLT:][MAX:32]
    UTINY bAnschlussArt;           //!< Physikalischer Anschluss des logischen Ausgangs [DEF: IO_NICHTVORHANDEN] [REF: CB0006]
    UTINY bAnschlussPlatz;         //!< Anschlussplatz (Platteradresse, PHM-Modul, Pluskabel) [DEF: 0][MIN: 0][MAX: 8]
    UTINY bAnschlussKanal;         //!< Kanal innerhalb der physikalischen Anschlusseinheit [DEF: 0][MIN: 1][MAX: MAX_UTINY]
    UTINY bReserve;                //!< Reservebyte [DEF: 0]
  } VSG_OUTPUT;


/*
 * BetriebsartenWunschLesen
 */
// bSteuermodus: siehe BetriebszustandLesen
// bBedienmodus: siehe BetriebszustandLesen

// bBedienung -> BetriebsartenWunschLesen
#define AL_AI_NICHT_GEWUENSCHT          (UTINY)0
#define AL_AI_ANGEFORDERT               (UTINY)1
#define AL_AI_ZUGETEILT                 (UTINY)2

// bTkrState -> BetriebsartenWunschLesen
#define AL_AI_TKR_EIN                   (UTINY)0
#define AL_AI_TKR_AUS_NORMAL            (UTINY)1
#define AL_AI_TKR_AUS_DUNKEL            (UTINY)2
#define AL_AI_TKR_AUS_GELB              (UTINY)3
#define AL_AI_TKR_UNUSED                (UTINY)255

// bAwpState -> BetriebsartenWunschLesen
#define AL_AI_AWP_EIN                   (UTINY)3
#define AL_AI_AWP_AUS                   (UTINY)2
#define AL_AI_AWP_RD_EIN                (UTINY)4

// bSiplNrGefordert: 1 ... 32
// bPrio:            0 ... 6 (Prioritaet des Bedieners)

// BetriebsartenWunschLesen
typedef struct { UTINY   bSteuermodus;
                 UTINY   bBedienung;
                 UTINY   bTkrState[MAX_TKR];
                 UTINY   bAwpState;
                 UTINY   bSiplNrGefordert;              // 1 ... 32|64
                 UTINY   bPrio;                         // Prioritaet des Bedieners : 0 ... 6
               } BASTRUCT;

/*
 * BildeUebergangsDaten
 */
typedef struct
  {
    UTINY bZustand;
    UTINY bFarbe;
    USHORT wTime;
  } SIGZUST;

typedef struct          /* Speicherbereich des von Signalwunsch bilden und der VA bestimmten Sigwunsches */
  {
    UTINY bZustand;     /* Zustand der Signalgruppe (Sperren , Freigeben) */
    UTINY bFarbe;       /* Referenz auf VALID_FBKOM */
  } SIGWUNSCH;

typedef struct
  {
    SSHORT   wEndeMinFrei_FF;  // Ende der Mindestzeit von FREIGABE
    SSHORT   wFreigabeBeginn;  // Beginn Freigabe
    SSHORT   wEndeMinFrei_SF;  // Ende der Mindestfreigabezeit
    SSHORT   wSperrBeginn;     // Beginn Sperrzustand
    SSHORT   wSperrEnde;       // Ende der Mindest-Sperrzeit
  } SIGNALSCHALTZEITEN_TYPE;

typedef struct
  {
     UTINY  bErrCode;              // Fehleridentfier (SUMI_KEIN_ERR |
                                   //                  SUMI_ZZ_ERR |
                                   //                  SUMI_MIN_GRUEN_ERR |
                                   //                  SUMI_MIN_ROT_ERR |
                                   //                  SUMI_VZ_BB_ERR |
                                   //                  SUMI_VZ_EE_ERR |
                                   //                  SUMI_ZZ_VZ_BB_ERR |
                                   //                  SUMI_VZ_VZ_EE_ERR)
     UTINY aRelMX[MAX_ERR_REL_MX]; // Index auf relevante Matrix
     UTINY aRelSG[MAX_ERR_REL_SG]; // Index auf relevante Signalgruppe
  } SUMIERR_TYPE;

typedef struct
  {
    BOOLEAN                  bNextSigbild;
    SIGZUST                  sSigAkt[MAX_SIGNALGRUPPEN];
    SIGWUNSCH                *pSigWunsch;
    UTINY                    bMode;
    UTINY                    bArt;
    BOOLEAN                  MitSchutz;
    UTINY                    bZZMatrix;             // auch bei Lizenzierung
    UTINY                    bVZBBMatrix;
    UTINY                    bVZEEMatrix;
    UTINY                    bMinSperrListe;        // auch bei Lizenzierung
    UTINY                    bMinFreiListe;         // auch bei Lizenzierung
    UTINY                    bFS_Nr;
    UTINY                    bSF_Nr;
    UTINY                    bAnzAktSG;             // Anzahl der aktiven Signalgruppen
    UTINY                    bAktSG[MAX_SIGNALGRUPPEN]; //Nummern der aktiven Signalgruppen
    SIGWUNSCH                *pSigNeu;
    BOOLEAN                  *pSigInNeu;
    SSHORT                   wRestartUebergang;
    SIGNALSCHALTZEITEN_TYPE  *sSchaltzeiten;
    SUMIERR_TYPE             *pErr;
  } BUEBDATEN;

/*
 * ZeitzaehlerLesen
 */
// bZaehlerArt ->  ZeitzaehlerLesen
#define ZAEHLER_1S                      ((UTINY)0x01) // 10 ms Takt innerhalb der Geraetesekunde
#define ZAEHLER_TX                      ((UTINY)0x02) // TX Zaehlerstand des aktuellen Signalplans
#define ZAEHLER_ST                      ((UTINY)0x03) // 10 ms Takt innerhalb eines Systemtakts
#define ZAEHLER_EL                      ((UTINY)0x04) // 10 ms endlos - Zaehler, wird nur bei Reset geloescht
#define ZAEHLER_SS                      ((UTINY)0x05) // 10 ms Takt innerhalb der Signalsicherungssekunde
#define ZAEHLER_RR                      ((UTINY)0x06) // abgelaufene Sekunden gemaess versorgtem Rueckrechenverfahren
#define ZAEHLER_UTC                     ((UTINY)0x07) // abgelaufene Sekunden seit dem 1.1.1970 0:00 Uhr (UTC-Zeit)

/*
 * BZWechselSperren, BZWechselFreigeben
 */
// bArt
#define AL_AI_BZW_SIPLWECHSEL            (UTINY)0
#define AL_AI_BZW_AUSSCHALTEN            (UTINY)1
#define AL_AI_BZW_EINSCHALTEN            (UTINY)2
#define AL_AI_BZW_SYNCHRONISIEREN        (UTINY)3
#define AL_AI_BZW_VERSATZ_BB             (UTINY)4
#define AL_AI_BZW_VERSATZ_EE             (UTINY)5
#define AL_AI_BZW_AUSSCHALTEN_SOFORT     (UTINY)6
#define AL_AI_BZW_EINSCHALTEN_NACH_RESET (UTINY)7 // Einschaltverhinderung bleibt nach Reset bestehen

// bPara fuer bArt = AL_AI_BZW_SIPLWECHSEL (d.h. welches Verfahren des Sipl-Wechsels wird freigegeben/gesperrt) :
#define AL_AI_BZW_SIPLW_GSP              (UTINY)0
#define AL_AI_BZW_SIPLW_STRETCH          (UTINY)1

// bPara fuer bArt = AL_AI_BZW_SYNCHRONISIEREN (d.h. welches Verfahren der Synchronisierung wird freigegeben/gesperrt) :
#define AL_AI_BZW_SYNCHRO_GSP            (UTINY)0
#define AL_AI_BZW_SYNCHRO_STRETCH        (UTINY)1

// bPara == Teilknoten

/*
 * AWP_LEDSetzen -> gibt es diese Funktion bei sX
 */
// AWP_LEDSetzen -> Anzeige, dass VA lebt
#define LED_AUS                         ((UTINY)0x00) // Schaltet die angegebene Led aus
#define LED_AN                          ((UTINY)0x01) // Schaltet die angegebene Led an
// Selektion der VA Led
#define LED_VA_ONBOARD                  ((UTINY)0x00) // D3 auf der BBS
#define LED_VA_DIAG1                    ((UTINY)0x01) // rote   Led auf dem Diagnosemodul
#define LED_VA_DIAG2                    ((UTINY)0x02) // gruene Led auf dem Diagnosemodul

/*
 * BAZTastaturLesen
 */
// BAZTastaturLesen
#define BAZ_STD_TASTEN                   ((UTINY)0x00)  // liest aus der Liste der Standardtasten
#define BAZ_SONDER_TASTEN                ((UTINY)0x01)  // liest aus der Liste der Sondertasten
#define BAZ_STD_TASTEN_NOT_DEL           ((UTINY)0x10)  // liest aus der Liste der Standardtasten, ohne loeschen
#define BAZ_SONDER_TASTEN_NOT_DEL        ((UTINY)0x11)  // liest aus der Liste der Sondertasten, ohne loeschen

/*
 * structure for ta stage information
 */
typedef struct
  {
    UTINY       bAktPhase;      // current stage
    UTINY       bNextPhase;     // next stage
    UTINY       bDauerAktPhase; // duration of current stage
    UTINY       bUebDauer;      // duration of current stage transition
  } AWP_PHASEN_INFO;   // structure for ta stage information

/*
 * ToDo: car2x
 */
typedef struct
  {
    UBYTE id;                 // Intersection related ID of Signal Group
    UBYTE Colour;             // Actual colour of Signal Group
                              // "0"(unknown), "1"(dark), "2"(red), "3"(red_amber),"4"(green),
                              // "5"(amber), "6"(green blinking), "7"(amber blinking)
    UBYTE nextColour;         // next colour, same coding as actual colour
    UBYTE colourResidualMin;  // Min residual time in seconds
    UBYTE colourResidualMax;  // Max residual time in seconds
                              // coding of time values
                              // Min, Max = "255"  => no time value available
                              // Min = Max < "255"   => precise value of residual time (typically for steady fixed time control)
                              // Min < Max < "255"   => value range of residual time (occurs at traffic actuated local control)
    UBYTE status;             // Actual status of Signal Group:  "0"(unknown), "1"(NoRightofWay), "2"(RightofWay)
    UBYTE nextStatus;         // Next status of signal Group, same coding as actual status
    UBYTE statusResidualMin;  // minimal residual time of Signal State
                              // meaning of residual time value
                              // if actual Signal Status is RoW then value predicts time to end of RoW
                              // if actual Signal Status is NoRoW then value predicts time to RoW
    UBYTE statusResidualMax;  // maximal residual time of Signal State
                              // same meaning and coding rules as for minimal residual time values
  } TC_SIGNAL_GROUP_STATE;

/*
 * typedefs and declarations for device values 
 */


// start functions
extern int AWP_Init(void);
extern int AWP_Funktion(void);

// Funktionen fuer Signalgruppen
extern SLONG SigWunschLesenSig (UTINY bSigNr, SIGSW_STRUCT *pSW);
extern SLONG SigWunschLesenTkr (UTINY bTkrNr, TKRSW_STRUCT *pSW);
extern SLONG SigZustandLesenSig (UTINY bSigNr, SIGSZ_STRUCT *pSZ);

// Zustand der Sg anhand der Farbe ermitteln

extern SLONG UebergangsdauerLesen (UTINY bArt, UTINY bSigNr, SSHORT *pDauer);

extern BYTE  DetStoe_Geraet (WORD det_nr);	// not over iTAC

extern SLONG PortLesen (UTINY bVonPortNr, UTINY bBisPortNr, UTINY *pPortfeld);
extern SLONG PortSetzen (UTINY bPortNr, UTINY bState);


extern SLONG ListeAuswaehlen(UTINY bArt, UTINY bNr);

extern SLONG Sondereingriff (UTINY bMode, UTINY bGrund);
extern SLONG TXChange (SSHORT wWert);
extern SLONG TXChangeAbsolut (SSHORT wWert);  // wird bei FESA verwendet


extern void  resetStatus (); // reset all TA (detail) states


// bei Lizenzierung dem Hersteller ueberlassen, wie er das macht, wird bei PDMe in einer Hilfsfunktion aufgerufen
extern SLONG BZWechselSperren (UTINY bArt, UTINY bPara);
// bei Lizenzierung dem Hersteller ueberlassen, wie er das macht, wird bei PDMe in einer Hilfsfunktion aufgerufen
extern SLONG BZWechselFreigeben (UTINY bArt, UTINY bPara);

extern SLONG BetriebsartenWunschLesen (UTINY bEinheit, BASTRUCT *pBA);

extern SLONG BildeUebergangsDaten(BUEBDATEN *pueb_daten);
extern SLONG BildeUebergang(BUEBDATEN *pueb_daten);

//------------------------------------------------------------------------------------------

//ToDo Funktionalitaet wird noch benoetigt

extern SLONG ZeitzaehlerLesen(UTINY bZaehlerArt);  // liefert verschiedene Zaehler (TX, UTC, ...)



// ab hier muss geprueft werden:
//  -> welche Funktionen werden gebraucht
//  -> welche Funktionen muessen durch Ersatzfunktionen ersetzt werden (aufgrund des Steuergeraetes)
//  -> in welcher Reihenfolge muessen die (Ersatz)Funktionen erstllt werden
// extern SLONG HandrastpunktWeiterschalten(void);  // nicht sofort
// extern SLONG AblaufdauerLesen (UTINY bArt, UTINY bTkrNr, SSHORT *wAktDauer, SSHORT *wRestDauer);
// extern SLONG ZZAbfragen (UTINY bArt, ZZSTRUCT *pZZ);
// extern SLONG StatusLesen (UTINY bArt, UTINY *pStatus);
// extern SLONG AWPVersionSchreiben (VA_VERSION_INFO *pInfo);
extern SLONG HoleSpeicher (void** pAdresse, ULONG lGroesse);
extern SLONG GibSpeicherFrei (void* pAdresse);
extern SLONG setRetentiveMemoryPointer(void **pRetentiveMemoryPointer);
// extern SLONG SysDatenLesen (UTINY bObjID, UTINY bIndex, void* pObject);
// extern SLONG RefregisterLesen (UTINY bSipl, SSHORT *pRefZeit, SSHORT *pTuZeit);
// extern SLONG AWP_LEDSetzen(UTINY bLED, UTINY bZustand);
// extern SLONG BAZEingangLesen (UTINY bNr, UTINY bLaenge, UTINY *pErgebnisfeld);
// extern SLONG BAZAusgangSetzen (UTINY bNr, UTINY bLaenge, UTINY *pFeld);
// extern SLONG BAZTastaturLesen (UTINY bArt, UTINY bNr, UTINY bLaenge, UTINY *pMode, UTINY *pFeld);
// extern SLONG BAZDisplayBeschreiben (UTINY bZeile, UTINY bLaenge, UTINY *pFeld);
// extern SLONG PhasenInfoSetzen (AWP_PHASEN_INFO *pInfo);
// extern SLONG SollZustandLesen(UTINY bEinheit, SOLL_ZUSTAND *pBA);
// extern SLONG BefaRueckmeldungen(UTINY bArt, UTINY bNr, UTINY bLaenge, UTINY *pFeld);
// extern void Set_VaAusRed(UTINY *pVaAusRed);

//------------------------------------------------------------------------------------------

// noch nicht in api
// extern void UNFALLSPEICHER_SICHERN(void);
// extern WORT UNFALLSPEICHER;  -> Info ob Sicherung gut ging oder nicht

// ab hier muss geprueft werden:
//  -> welche Daten werden gebraucht
//  -> wie werden die Daten ausgetauscht
// extern volatile SYS_COUNTER     *pSysCnt;
// extern volatile DIAG_BUF        *pDiagBuf;
// extern volatile OUTPUT_BUF      *pOutBuf;
// extern volatile INPUT_BUF       *pInBuf;
// extern volatile CONFIG_BUF      *pConfigBuf;
// extern volatile VSG_BUF         *pVsgBuf;
// extern volatile SYS_STATE_INFO  *pSysStateInfo;
// extern volatile ZENTRALEN_INFO  *pZentralenInfo;
// extern volatile IO_BUF          *pIOBuf;
// extern volatile KMELD_INFO      *pKMeldBuf;
// extern volatile VA_STATE_INFO   *pVaStateInfo;
// extern volatile SBILD_STATUS    *pSGBildStatus;
// es muss noch geklaert werden:
//  -> welche Informationen werden beim Sitraffic C900 ueber Messages ausgetauscht
//  -> welche Informationen werden auch beim sX gebaucht
//  -> wie werden die Informationen beim sX ausgetauscht

//ToDo not longer used?
// Event AL_EV_SOLLZUSTAND
/*typedef PACKED_STRUCT
  {
    UTINY bSteuermodus;
    UTINY bTkrZustandNeu[MAX_TKR];
    UTINY bSiplNrNeu;
    UTINY bVAZustandNeu;
  } AL_EV_SOLL_STRUCT;
*/
#if defined __cplusplus
}
#endif

#endif
