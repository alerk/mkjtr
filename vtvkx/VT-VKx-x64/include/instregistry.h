/*
 * instregistry.h
 * used to store/lookup central used objects
 *
 *  Created on: 31.08.2015
 *      Author: zelgerpe
 */

#ifndef INSTREGISTRY_H_
#define INSTREGISTRY_H_

#include "util/what.h"
DEFINE_WHATINFO_HEADER;

extern "C" struct SignalIndicationsCctrl;
extern "C" struct ActualStatusCctrl;
extern "C" struct RequestedStatusCctrl;

namespace sitraffic
{

class DataReceiver;
class EvHttpServer;
class Detectors;
class Config;
class AwpWrapper;
class CltDeviceValueProviders;
class RequestedStatusWCctrl;
class ActualStatusWCctrl;
class SignalIndicationsWCctrl;
class ModOnOffHelperStateHandler;
class PublicTransport;
class ConfigNotificationImpl;
class Archive;

// Singleton
class InstRegistry
{
	// Singleton instance
	static InstRegistry inst;
	DataReceiver * dr;
	EvHttpServer * ehs;
	SignalIndicationsWCctrl *si;
	Detectors *dets;
	Archive *arch;
	Config *config;
	AwpWrapper *awpWrapper;
	ModOnOffHelperStateHandler *_statusModsHandler; // used to send actual status-modifications only on change
	ActualStatusWCctrl * pactualStatus;
	RequestedStatusWCctrl * prequestedStatus;
	CltDeviceValueProviders * pcltDeviceValueProviders;
	PublicTransport *publicTransport;
	ConfigNotificationImpl *pConfigNotification;

	InstRegistry() :
		dr(0), ehs(0), si(0), dets(0), arch(0), config(0), awpWrapper(0),  _statusModsHandler{},
		pactualStatus(0), prequestedStatus(0),	pcltDeviceValueProviders(0), publicTransport(0), pConfigNotification(0)
	{
	}

public:
	static InstRegistry & Instance()
	{
		return inst;
	}

	void set(EvHttpServer &d)
	{
		ehs = &d;
	}

	EvHttpServer & getEvHttpServer() const
	{
		return *ehs;
	}

	void set(DataReceiver &d)
	{
		dr = &d;
	}

	DataReceiver & getDataReceiver() const
	{
		return *dr;
	}

	void set(SignalIndicationsWCctrl & sic)
	{
		si = &sic;
	}

	SignalIndicationsWCctrl & getSi() const
	{
		return *si;
	}

	void set(Detectors & ds)
	{
		dets = &ds;
	}

	Detectors & getDetectors() const
	{
		return *dets;
	}

    void set(Archive & ar)
    {
        arch = &ar;
    }

    Archive & getArchive() const
    {
        return *arch;
    }

	void set(Config & c)
	{
		config = &c;
	}

	Config & getConfig() const
	{
		return *config;
	}

	void set(AwpWrapper & aw)
	{
		awpWrapper = &aw;
	}

	AwpWrapper & getAwpWrapper() const
	{
		return *awpWrapper;
	}

	void set(ModOnOffHelperStateHandler & moohsh)
	{
		_statusModsHandler = &moohsh;
	}
	ModOnOffHelperStateHandler & getModOnOffHelperStateHandler()
	{
		return *_statusModsHandler;
	}

	void set(PublicTransport & pt) {
		publicTransport = &pt;
	}

	PublicTransport & getPublicTransport() {
		return *publicTransport;
	}

	void set(ActualStatusWCctrl & as) { pactualStatus=&as; }
	ActualStatusWCctrl & getActualStatus() const { return *pactualStatus; }
	void set(RequestedStatusWCctrl & rs) { prequestedStatus=&rs; }
	RequestedStatusWCctrl & getRequestedStatus() const { return *prequestedStatus;}

	void set(CltDeviceValueProviders & rs) { pcltDeviceValueProviders=&rs; }
	CltDeviceValueProviders & getCltDeviceValueProviders() const { return *pcltDeviceValueProviders;}

	void set(ConfigNotificationImpl & cn) { pConfigNotification=&cn; }
	ConfigNotificationImpl & getConfigNotification() const { return *pConfigNotification;}

	/**
	 * check if initial data are received.
	 * @return true if necessary pointers are set, to safely start AWP_Init().
	 */
	bool isReady4Init()
	{
            return dr && ehs && si && dets && config && awpWrapper && pactualStatus && prequestedStatus && pcltDeviceValueProviders;
	}

};

}
#endif /* INSTREGISTRY_H_ */
