/*~-*/
/*~XSF_LANGUAGE: C/C++*/
/*~T*/
/***************************************************************************
* Datei        DEF_GER.H                                                   *
****************************************************************************
* Verfasser:    Fischer                                                    *
* Erstellt :    08.15                                   Version 1.0        *
*                                                                          *
* Funktion :   Definieren des Steuergeraetes bei Lizensierung              *
*                                                                          *
* Aenderung:              ( Name )              ( Datum )                  *
*                                                                          *
*                      ( Beschreibung / Begruendung )                      *
*                                                                          *
* Version 1.0     MO MM R&D ITS TT Fischer         08.15                   *
*                                                                          *
*    Neuerstellung fuer sX                                                 *
*    Datei muss vom Lizenznehmer gestellt werden                           *
*                                                                          *
***************************************************************************/

/*~A*/
/*~+:Aenderungen*/
/*~K*/
/*~+:Ausgangsdatei : def_ger.h, Version 1.2 fuer fuer C900*/
/*~K*/
/*~+:Aenderungen in der Version 1.0*/
/*~+:*/
/*~+:xx.xx.xx  YYY*/
/*~+:   */
/*~E*/
/*~I*/
#ifndef _DEF_GER_
/*~T*/
#define _DEF_GER_
/*~T*/
#define _SITRAFFIC   // Kennung Siemens Steuergeraet
// #define _LIZENZ_XXXX     fuer Lizenznehmer, dann "#define _SITRAFFIC" auskommentieren
/*~I*/
#ifdef _SITRAFFIC
/*~A*/
/*~+:defines bei Siemens Steuergeraet*/
/*~T*/
// main entry of SX tac kernel, shall be moved into api_va.h
int tac_main(int argc, char *argv[], int(*awpinit)(void), int(*awpfunction)(void));

// ToDo: defines muessen noch angepasst werden
/*~T*/
#undef PACKED_UNION
#undef PACKED_STRUCT
#undef POST_PACKED

/*~I*/
#ifdef _SIMULA  // muss beim Compilieren fuer Vissim (in der dortigen Entwicklungsumgebung) gesetzt werden
/*~T*/
#define PACKED_UNION  union
#define PACKED_STRUCT struct
#define POST_PACKED
/*~O*/
/*~-*/
#else
/*~T*/
#define PACKED_UNION  union __attribute__((packed))
#define PACKED_STRUCT struct __attribute__((packed))
#define POST_PACKED
/*~-*/
#endif
/*~E*/
/*~T*/

/*~E*/
/*~-*/
#endif
/*~E*/
/*~I*/
#ifdef _LIZENZ_XXXX
/*~A*/
/*~+:defines bei Lizenznehmer Steuergeraet*/
/*~T*/
// ToDo: defines muessen noch angepasst werden
/*~T*/
#undef PACKED_UNION
#undef PACKED_STRUCT
#undef POST_PACKED

/*~T*/
#define PACKED_UNION  union
#define PACKED_STRUCT struct
#define POST_PACKED
/*~E*/
/*~-*/
#endif
/*~E*/
/*~-*/
#endif
/*~E*/
