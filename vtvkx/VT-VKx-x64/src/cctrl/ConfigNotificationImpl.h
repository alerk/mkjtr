///////////////////////////////////////////////////////////
//  ConfigNotificationImpl.h
//  Implementation of the Class ConfigNotificationImpl
//  Created on:      15-Mrz-2017 09:58:49
//  Original author: Peter Zelger
///////////////////////////////////////////////////////////

#if !defined(EA_171B7F62_8FA5_4a12_BC6B_E2661E3F8BA4__INCLUDED_)
#define EA_171B7F62_8FA5_4a12_BC6B_E2661E3F8BA4__INCLUDED_

#include "tac_kernel.h"
#include "CControl.h"

/**
 * one instance registered at instregistry.
 * 
 * it stores and invokes the TA_Config function pointers.
 * it implements the ccontrol service functions:
 * <ul>
 * 	<li>CheckRespondCctrl checkConfig(<font color="#7f0055"><b>int</b></font>
 * configTypes)=20;</li>
 * 	<li><font color="#7f0055"><b>void</b></font> activate(<font
 * color="#7f0055"><b>int</b></font> sysJobId)=21;</li>
 * 	<li><font color="#7f0055"><b>void</b></font> cleanupConfigCheck(<font
 * color="#7f0055"><b>void</b></font>)=22;</li>
 * </ul>
 * 
 * further it manages an instance for Config and Detector holding the draft
 * configuration version.
 */

 struct TacMsgList
   {
     typedef std::vector<CfgMessageCctrl> msgs_t;
     msgs_t msgs;

     TacMsgList() : msgs(){}
     void tacMsgListAddMsg(TacMsgSeverity severity, const char * format, ...);
     void moveMsgsTo(CheckRespondCctrl & chkRespond);
   };

namespace sitraffic
{
class Config;
class Detectors;
class DataReceiver;

class ConfigNotificationImpl //: public tacConfigNotification
{
    Config *pConfigDraft;
    Detectors * pDetectorsDraft;
    ta_checkConfig_t fpcheck;
    ta_activateConfig_t fpactivate;
    ta_cleanupConfig_t fpcleanup;
    bool isDraftChanged;
    void updateDrafts();
    ConfigNotificationImpl& operator=(const sitraffic::ConfigNotificationImpl&)=delete;
    ConfigNotificationImpl(const sitraffic::ConfigNotificationImpl&)=delete;
public:
	ConfigNotificationImpl();
	~ConfigNotificationImpl();

	void tacRegisterConfigNotifications(ta_checkConfig_t fcheck, ta_activateConfig_t factivate, ta_cleanupConfig_t fcleanup);
	void doCheckConfig(int configTypes, CheckRespondCctrl * pCheckRespondCctrl);
	void doActivate(int sysJobId);
	void doCleanupConfigCheck();

	// management of draft configuration
	const Config & getDraftConfig() const;
	const Detectors & getDraftDetectors() const;

	// register at cctrl
	CLIENT * registerAtCctrl(CLIENT * clnt, const DataReceiver & dr);

};
}
#endif // !defined(EA_171B7F62_8FA5_4a12_BC6B_E2661E3F8BA4__INCLUDED_)
