/*
 * api_mv.cpp
 *
 *  Created on: 22.10.2015
 *      Author: philippf
 */

#include "Detectors.h"
#include "instregistry.h"
#include "DataReceiver.h"
#include <stdexcept>
#include "util/repdef.h"
#include "evhttpadapter.h"
#include <sstream>


#include "util/what.h"
DEFINE_WHATINFO;


//----------------------------------------------------------------------------------------------------------------
/*
 2.2.1	DetektorLesen
Syntax:	SLONG DetectorRead(int iFromDetNo, int iToDetNo, DET_MEASURED_DATA * pResultArray);

Beschreibung:		Die Funktion DetektorRead liefert fuer den angegebenen Bereich von iFromDetNo bis iToDetNot alle Detektorwerte der Struktur DETEKTOR.
Bei nicht versorgten Detektoren innerhalb des Bereiches wird bei wDetIdx 0 eingetragen.
Fuer PDM werden die Werte wZS, wBA, wSS, wLS, wLU und wError benoetigt. Fuer den TL-Anwender stehen alle Werte zur Verfuegung.
Die Werte fuer wHoldTime, wLastGap, wDriveDuration, wSpeed, wVehLength, wVehType und wStatus stehen im sX derzeit nicht zur Verfuegung.

Uebergabeparameter:
iFromDetNo	Startindex in die Detektorfeld-Liste, ab der kopiert wird (Start mit 1)
iBisDetNr	Endindex bis zu dem einschliesslich kopiert wird.(letzter vorhandener Detektor)
pResultArray	Zielpointer fuer die Anwenderdaten.
	pErgebnisfeld zeigt auf eine Variable DETEKTOR	detektoren[MAXDET]
	typedef struct
	{
USHORT	wDetIdx;	Logische Detektornummer
USHORT	wError;		Detektorstoerung (Defines siehe unten)
USHORT	wQuality	Qualitaet
USHORT	wAU;		aktueller Detektorzustand
USHORT	wZS;		Zaehlwert
USHORT 	wBU;		Summe der Belegtzeiten
USHORT 	wBA;		geglaettete Belegtzeitensumme
USHORT 	wBS;		sekuendlicher Belegtwert
USHORT 	wSS;		Stichproben Summenwert
USHORT 	wLS;		laufende Luecke
USHORT 	wLU;		letzte abgeschlossene Luecke
USHORT 	wLA;		geglaettete Luecke
USHORT 	wA;			Puffer Original Zaehlwert (MEXWA-Intervall)
USHORT 	wB;			Mittlere Verkehrsstaerke (MEXWA-Intervall)
USHORT 	wC;			Puffer Original Belegungswert (MEXWA-Intervall)
USHORT 	wD;			Mittlere Belegungszeit (MEXWA-Intervall)
USHORT	wHoldTime;	Belegungsdauer durch das Fahrzeug in10ms
USHORT	wLastGap;	Letzte Luecke der Schleife vor dem Fahrzeug in 10ms
USHORT	wDriveDuration;	Fahrdauer von Schleife A->B in ms
USHORT	wSpeed;		Ermittelte Geschwindigkeit des Fahrzeugs in km/h
USHORT	wVehLength;	Ermittelte Laenge des Fahrzeugs in 0,1 m
USHORT	wVehType;	Ermittelter Typ des Fahrzeugs (nach TLS)
USHORT	wStatus;	Statusbyte 0=aktuell, >0 Alter des Fzg. In Sekunden
}    DET_MEASURED_DATA;

Defines fuer wError (auch TL-Anwender):
DET_OK				Detekor ist in Ordnung
DET_ERR_BELEGUNG	Detekor ist fehlerhaft aufgrund von Belegungsueberwachung
DET_ERR_LUECKE		Detekor ist fehlerhaft aufgrund von Lueckenueberwachung
DET_ERR_HW			Detekor ist fehlerhaft aufgrund von Hardwareueberwachung
Rueckgabe: 	SUCCESS					bei Erfolg
			ERROR_BEREICHSPARAMETER	bei Fehler in Start- bzw. Endindex
 *
 */

//----------------------------------------------------------------------------------------------------------------
void fillDet(int detNr, const sitraffic::Detector & siDet, DET_MEASURED_DATA & det)
{
  if(det.wDetIdx != 0 && det.wDetIdx != detNr)
    Report(DET_ERROR, "wDetIdx:%d != detNr:%d", det.wDetIdx, detNr);

  det = siDet.getDmd();
}

//----------------------------------------------------------------------------------------------------------------
void nullDet(int /*detNr*/, DET_MEASURED_DATA & det)
{
  det.wDetIdx = (USHORT)0;

  // Report(DET_WARNING "DetectorRead: detNr:%d not found", detNr);

  det.wQuality = 0;
  det.wError = DET_NOT_USED;
  det.wStatus = 0;
}

//----------------------------------------------------------------------------------------------------------------
SLONG DetectorRead(USHORT wFromDetNo, USHORT wToDetNo, DET_MEASURED_DATA* pResultArray) // -> ersetzt DetektorLesen
{
  Report(DET_INFO, "wFromDetNo:%d wToDetNo:%d pResultArray:%p", wFromDetNo, wToDetNo, pResultArray);

  if (pResultArray == 0)
    return ERROR_BEREICHSPARAMETER ;

  SLONG result = SUCCESS;

  sitraffic::Detectors & sitrafficDets = sitraffic::InstRegistry::Instance().getDetectors();

  int i = 0;
  for (int detNr = wFromDetNo; detNr <= wToDetNo; detNr++, i++)
  {
    // sitraffic::Detector & siDet = sitrafficDets.getDets().find(detNr)->second;  // @c second is a copy of the second object
    // std::map<int, sitraffic::Detector> & detMap = (std::map<int, sitraffic::Detector> & )sitrafficDets.getDets();
    const sitraffic::Detector * siDet = sitrafficDets.getDet(detNr);
    if (siDet != NULL)
    {
      fillDet(detNr, *siDet, pResultArray[i]);
    }
    else
    {
      nullDet(detNr, pResultArray[i]);
      if (detNr == wFromDetNo || detNr == wToDetNo)
        result = ERROR_BEREICHSPARAMETER;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------
/*
 2.2.2	DetektorwertSetzen
SLONG DetectorValueSet (USHORT wDetNr, UTINY bArt, USHORT wNeuerWert);
Parameter:	wDetNr	ist der Index in die Detektorliste
			bArt	gibt die Art des Detektorwertes an, der geaendert werden soll
		DET_VALUE_SS
		DET_VALUE_LS
		DET_VALUE_LU
		DET_VALUE_ZS
	wNeuerWert	neuer Wert, mit dem der alte Wert ueberschrieben wird.
Rueckgabe:	SUCCESS	bei Erfolg
	ERROR_BEREICHSPARAMETER	bei Fehler wDetNr
	ERROR_DETEKTORART		bei ungueltigem Wert in bArt
Beschreibung:	Die Funktion DetektorwertSetzen ueberschreibt den angegebenen Detektorwert mit
dem angegebenen wNeuerWert.

 */
extern SLONG DetectorValueSet (USHORT detNr, UTINY bArt, USHORT wNeuerWert)
{
    sitraffic::Detectors & sitrafficDets = sitraffic::InstRegistry::Instance().getDetectors();

    Report(DET_DEBUG, "detNr:%d bArt:%d wNeuerWert:%d", detNr, bArt, wNeuerWert);

    const sitraffic::Detector * siDet = sitrafficDets.getDet(detNr);
    if(siDet == NULL)
    {
      Report(DET_WARNING, "detNr:%d not found", detNr);
      return ERROR_BEREICHSPARAMETER;
    }

    const DET_MEASURED_DATA & dmd = siDet->getDmd();
    DET_MEASURED_DATA & writableDmd = const_cast<DET_MEASURED_DATA &>(dmd);

    switch(bArt)
    {
      case DET_VALUE_SS: writableDmd.wSS = wNeuerWert; break;
      case DET_VALUE_LS: writableDmd.wLU = writableDmd.wLS; writableDmd.wLS = wNeuerWert; break; // store LS in LU
      case DET_VALUE_LU: writableDmd.wLU = wNeuerWert; break;
      case DET_VALUE_ZS: writableDmd.wZS = wNeuerWert; break;
      default:
        Report(DET_WARNING, "unknown bArt:%d", bArt);
        return ERROR_DETEKTORART;
    }

    return SUCCESS;
}




