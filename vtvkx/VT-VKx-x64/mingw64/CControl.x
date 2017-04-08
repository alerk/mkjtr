/* definition of SITRAFFIC C10 C-Control interface */

/** latest release version of C-Control interface 
 */
const CCONTROL_RELEASE_VERS = 1;
/** data lease timeout in milliseconds.<br></br> 
 * used by all subscribe methods of {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl)} c-control service}.
 * if subscriber don't {@link CControl_CcontrolService_CltIF#renew_8(String, int) renew} their leases within LEASE_TIMEOUT_MS the ccontrol service remove that subscription. 
 */
const LEASE_TIMEOUT_MS = 120000;

/* C-Control uses UTF-8 encoding to transmit strings ! 
 * const String CCtrlCharacterEncoding="UTF-8";
 */
/** default time in ms to be used with {@link CControl_CcontrolService_CltIF#addDesiredStageSwitchingEvents_8(ExternalStageSwitchingEventArray) stage}
 *  and {@link CControl_CcontrolService_CltIF#addDesiredSgSwitchingEvents_8(ExternalSgSwitchingEventArray) signal group} switching events. */ 
const SWITCHING_EVENT_DEFAULT_VALID_DURATION = 40000;
/** time in ms of cycle that shall be used to refresh 
 * {@link CControl_CcontrolService_CltIF#addDesiredStageSwitchingEvents_8(ExternalStageSwitchingEventArray) stage}
 *  or {@link CControl_CcontrolService_CltIF#addDesiredSgSwitchingEvents_8(ExternalSgSwitchingEventArray) signal group}
 *   switching events if a static signalisation is desired. */
const SWITCHING_EVENT_REFRESH_INTERVAL = 20000;


/* --------------- publish/subscribe DTO definitions ---------------- */
/**
 * defines data types that are (un)subscribeable by 
 * {@link CControl_CcontrolService_CltIF#unsubscribe4Tdt_8(String, int, MkjtricDataTypeCctrl)} or 
 * {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl)} ;
 * its param inetadr address or hostname where data has to be sent, 
 * that is where a {@link CControl_CcontrolDataReceiver_CltIF} service method receives the data. 
 * the param port UDP port number where the {@link CControl_CcontrolDataReceiver_CltIF} service method is listening to receive tha data.
 * <p>we recommend to use {@link com.sitraffic.fd.ic.ccontrolClient.DataReceiver} in conjunction with 
 * {@link com.sitraffic.fd.ic.ccontrolClient.DataReceiverStarter} to receive spontaneous data.</p>
 * @see <a href="doc-files/genericSubscriptionForDeliveryOfSpontaneousData.png">generic subscription for delivery of spontaneous data</a>
 */
enum MkjtricDataTypeCctrl
{
  /** only valid at unsubscribe(), refers to all subscribed traffic data types. */
  TDT_ALL=-1,
  /** no traffic data type. leads to an error if used with 
   * {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl)} or 
   * {@link CControl_CcontrolService_CltIF#unsubscribe4Tdt_8(String, int, MkjtricDataTypeCctrl)} */
  TDT_None=0,
  /** subscribe for detector raw values.<br></br>
   * data receiver service method: {@link CControl_CcontrolDataReceiver_CltIF#detStates_8}.
   */
  TDT_DETRAW=1, 
  /** subscribe for actual signal indications (IstBilder).<br></br>
   * data receiver service method: {@link CControl_CcontrolDataReceiver_CltIF#actualStatus_8}
   */
  TDT_ACTUALSTATUS=2, 
  /** subscribe for requested status (Soll Betriebszustand).<br></br>
   * data receiver service method: {@link CControl_CcontrolDataReceiver_CltIF#requestedStatus_8(RequestedStatusCctrl, int)}
   */
  TDT_REQUESTEDSTATUS=3, 
  /** subscribe for actual signal indications (IstBilder).<br></br>
   * data receiver service method: {@link CControl_CcontrolDataReceiver_CltIF#actualSignalIndications_8}
   */
  TDT_ACTUALSIGNALINDICATIONS=4, 
  /** subscribe for target stage (Soll Phase). it is only actuated if we are in a stage related signal program.<br></br>
   * data receiver service method: {@link CControl_CcontrolDataReceiver_CltIF#targetStage_8}
   */
  TDT_TARGETSTAGE=5, 
  /** subscribe for infrequent archive insert events. needs additional {@link TdtSubscriptionInfoCctrl}
   * the param selector enumeration value of type {@link ArchiveSelectorCctrl} to select the archive for insert events. Insert events are NOT supported for
   * archives with frequent inserts to avoid overload by archive requests:
   * <ul>
   * <li>{@link ArchiveSelectorCctrl#AS_SignalIndications SignalIndications}</li>
   * <li>{@link ArchiveSelectorCctrl#AS_DetectorValues DetectorValues}</li>
   * <li>{@link ArchiveSelectorCctrl#AS_DeviceValues DeviceValues}</li>
   * </ul>
   * param count number of archive inserts to wait before sending an event; 0 to disable archive events. Count has a different meaning when subscribing insert events for archives:
   * <ul>
   * <li>{@link ArchiveSelectorCctrl#AS_DetectorAggregation DetectorAggregation}: Bit0=0: mask signalingCycle values; Bit1=0: mask fixed cycle values; Bit31-2: number of unmasked archive inserts to wait.</li>
   * <li>{@link ArchiveSelectorCctrl#AS_Amli AMLi}: Bit0=0: mask extended R09 values; Bit1=0: mask raw R09 values; Bit31-2: number of unmasked archive inserts to wait.</li>
   * </ul>
   * {@link RetCodeCctrl#RC_OK} is returned if succeeded, no subscription taken place otherwise.
   */
  TDT_ARCHIVEEVENTS=6, 
  /** subscribe for intersection rides (OEPNV Fahrten). there are always sent all active rides if at least one change has been detected/estimated.<br></br>
   * data receiver service method: {@link CControl_CcontrolDataReceiver_CltIF#intersectionRides_8} 
   */
  TDT_INTERSECTIONRIDES=7,
  /** subscribe for device values.<br></br>
   * data receiver service method: {@link CControl_CcontrolDataReceiver_CltIF#deviceValues_8}.
   */
  TDT_DEVICEVALUES=8,
  /** subscribe for ride events (R09 telegrams).<br></br>
   * data receiver service method: {@link CControl_CcontrolDataReceiver_CltIF#rideEvent_8}.
   */
  TDT_RIDEEVENT=9

};

/* ------ detmanager/DetectorStates, DetectorValues ------ */
/** data transport object that transmits detected edges. 
 * needed by {@link CControl_CcontrolDataReceiver_CltIF#detStates_8} */
struct DetDataStruct{
			 /** sub-seconds in milliseconds relative to their enclosing {@linkplain DetRawValuesExchg#absTimeDevTicks}.<br></br>
			  * the absolute device tick time of this edge = subTicks + enclosing {@linkplain DetRawValuesExchg#absTimeDevTicks}. */
			 short subTicks;
			 /** new value of occupation: <ul>
			  *  <li>false, 0=gap</li>
			  *  <li> true, 1==occupied</li></ul> */
			 bool  value;			
};
/** data transport object that transmits the detector values of one detector channel.<br></br>
 * there are multiple {@linkplain DetDataStruct} dto's reflecting the multiple changes since last update (100ms typical polling interval).
 * needed by {@link CControl_CcontrolDataReceiver_CltIF#detStates_8} 
 */
struct DetValuesPerChannel {
	/** absolute device ticks in ms of last change of this detector occupancy value */
    hyper  TicksOfLastChange;	  		  
    /** detector channel number */
	short  nr;					  		  
	/** last value of detection: false, 0=gap/ true, 1==occupied */
	bool   value;		  				  
	/** relative to absDevTickTime of enclosing {@linkplain DetRawValuesExchg} */
	struct DetDataStruct detDatas <256>;  
};

/** 
 * Definitions used by {@linkplain DetStatusCctrl} defining the status of a single detector.
 */
enum DetectorErrorCodeCctrl
{
	/** None */
	DEC_NONE=0,
	/** Peripheral is not available */  
	DEC_PERIPHERAL_NOT_AVAILABLE=1,
	/** Input is not available. */
	DEC_INPUT_NOT_AVAILABLE=2,
	/** Input initialization has failed. */  
	DEC_INPUT_INIT_FAILED=3,
	/** Loop is corrupted. */  
	DEC_LOOP_CORRUPTED=4,
	/** Mapped and analyzed peripheral type do not match. */  
	DEC_PERIPHERAL_TYPE_MISMATCH=8,
	/** Connection types of a peripheral are incompatible. */  
	DEC_CONNECTION_TYPE_INCOMPATIBLE=9,
	/** Peripheral firmware version is not supported or not compatible with actual CBU version. */
	DEC_PERIPHERAL_FW_NOT_COMPATIBLE=10,
	/** There could be pending input data due to transfer troubles. */ 
	DEC_PENDING_INPUT_DATA=16,
	/** There are lost input data due to the event-logger overflow. */  
	DEC_LOST_INPUT_DATA=17,
	/** No rising edge for a long time. */  
	DEC_PERMANENT_GAP=32,
	/** No falling edge for along time. */
	DEC_PERMANENT_OCCUPANCY=33,
	/** Too many edges in a period. */ 
	DEC_FLUTTER_ERROR=34,
	/** UNDEFINED. */
	DEC_UNDEFINED=99  
};
/**
 * Data transport object that transmits the status of a detector.
 */
struct DetStatusCctrl {
	/** detector channel number */
	int         channel;
	/** quality attribute of the associated detector value(s), e.g:<ul>
	 *   <li>0% value not reliable,</li> 
	 *   <li>100% value fully reliable</li></ul> */
	unsigned char quality;			
	/** detector error Code reported by detector, 0==OK, no Error */
	DetectorErrorCodeCctrl errorCode;
};

/** data transport object for detector raw values and their status.<br></br> 
 * after subscribing for all Detectors DetValuePerChannel is sent,
 * then only those detectors that contain changes are reported.
 * it is up to the responsibility of the client (CcontrolDataReceiver implementation)
 * to handle a lost package, that may be detected by a not monotone sequence number 
 * (at CcontrolDataReceiver::void detStates(DetRawValuesExchg values, int seqNr)=1;)
 * {@linkplain CControl_CcontrolDataReceiver_CltIF#detStates_8}
 */
struct DetRawValuesExchg {
	/** absolute device ticks in ms */
	hyper	absTimeDevTicks;					
	/** values and deltas, optional */
	struct DetValuesPerChannel detDatas <256>;	
	/** status, sent initial after subscription, then typically each second */
	struct DetStatusCctrl detStatus<256>;			
};

/** generic return-codes of C-Control service calls. */
enum RetCodeCctrl {
	/** RPC call failed because of some RPC related reason. */
	RC_RPC_ERROR = -1,
	/** call succeeded */		
	RC_OK = 0,
	/** RPC call failed because of no further specified reason. */
	RC_ERROR = 1,
	/** used by RequestedStatusManagerIF. 
	 * signals that there has been passed an invalid priority value. The call is discarded. */
	RC_InvalidPrio=2, 		 
	/** used by RequestedStatusManagerIF. 
	 * signals that there has been passed an invalid duration value. The call is discarded. */
	RC_InvalidDuration=3,
	/** used by RequestedStatusManagerIF 
	 * signals that there has been passed an invalid key value (e.g. unknown subintersection number). The call is discarded. */
	RC_InvalidKey=4,
	/** incompatible version of CControl interface of communication partner. */
	RC_UnsupportedReleaseVersion=5,
	/** used by RequestedStatusManagerIF 
	 * signals that there has been passed an invalid value (e.g. unknown signal plan number). The call is discarded. */
	RC_UnknownValue=99
};

/* ------ actualstatus ----------------------*/
/** definitions used by actualstatus defining the operating mode (Betriebsart) 
 *  @see <a href="https://svnocit.de/dok/trunk/html/ocitf_r.html#KNOTENBETRIEBSART1">OCIT KNOTENBETRIEBSART 1:138</a> */ 
enum IntersectionOperatingModeCctrl
{
  IOS_SPECIAL=0,
  /** encoding for operating mode traffic actuated */ 
  IOS_TRAFFIC_ACUATED=1, 
  /** encoding for operating mode manual stop, that is there is somebody manually selecting phases... */
  IOS_MANUAL_STOP=2, 
  /** signals an locally constant operating mode chosen. */
  IOS_LOCAL_FIX=3, 
  /** the time of day scheduler (Jahresautomatik) has chosen the operating mode */
  IOS_LOCAL_TIME_SCHEDULE=4, 
  /** the control center (Zentrale) has chosen the operating mode */
  IOS_CONTROL_CENTER=5
};
/** definitions used by actualstatus defining the summary alarm (Sammelstoerung)
 *  @see <a href="https://svnocit.de/dok/trunk/html/ocitf_r.html#SAMMELSTOERUNG1">OCIT SAMMELSTOERUNG 1:69</a> */ 
enum SummaryAlarmCctrl
{
  /** 	no fault, all ok */
  SA_NO_ALARM=0, 
  /** 	if there is at least one fault, device works eventually restricted */
  SA_ALARM=1,
  /**	there is at least one fault, that causes a switch off */
  SA_ALARM_CAUSING_SWITCH_OFF=2, 
  /**	there is at least one fault, that causes a partly switch off of a sub intersection */ 
  SA_ALARM_CAUSING_PARTLY_SWITCH_OFF=3, 
  /** there is an intern fault, for example problem with configuration or intern communication fault there is an intern fault, for example problem with configuration or intern communication fault between Canto-adaption unit and intersection controller */ 
  SA_INTERNAL_ALARM=4
};
/** defines the type of a modification that is part of the macroscopic control structure (actualstatus, requestedstatus) */ 
enum TypeOfModificationCctrl
{
  /** modification type for traffic actuation */
  TOM_ModTD=0, 
  /** modification type for public transport priorisation  */
  TOM_ModPublicTransport=1, 
  /** modification type for individual traffic actuation  */
  TOM_ModTDIndividualMkjtric=2, 
  /** index of synchronization on/off requested state modification. */
  TOM_ModSynchronisationState=3,

  TOM_ModPrj1=4, TOM_ModPrj2=5, TOM_ModPrj3=6, TOM_ModPrj4=7, 
  TOM_ModPrj5=8, TOM_ModPrj6=9, TOM_ModPrj7=10, TOM_ModPrj8=11, TOM_ModPrj9=12, TOM_ModPrj10=13, TOM_ModPrj11=14, TOM_ModPrj12=15, TOM_ModPrj13=16
};
/** state of sub intersection */
enum SubIntersectionDetailStateCctrl
{
  /** 	no state set or undefined state */
  SDS_NONE=0,
  /** 	sub intersection is to switch into intersection state of whole intersection. 
   * Sub intersection is not switched off. That means real sub intersection On / Off state is equal to intersection state of total intersection. 
   * <p>if for example whole intersection is OffDark and sub intersection is switched on, sub intersection is also OffDark, 
   * because it is switched like the whole intersection</p> */
  SDS_ON=1,
  /**  signal groups are flashing, which are chosen by configuration, normally vehicle signal groups of secondary direction, other signal groups are dark */
  SDS_OFF_DEFAULT=2, 
  /** 	all signal groups of secondary direction of sub intersection are flashing */
  SDS_OFF_FLASH_SECONDARY_DIRECTION=3, 
  /** 	all signal groups of sub intersection are dark */
  SDS_OFF_DARK=4, 
  /** 	all signal groups of sub intersection are flashing */
  SDS_OFF_FLASH_ALL=5, 
  /** the controller is waiting for the syncronous switch on time. */
  SDS_WAIT4SYNC_ON=10,
  /** 	the controller is showing the switch on symbol sequence. */ 
  SDS_SWITCHON_PATTERN=11, 
  /** 	the controller is waiting for the switch off time. */
  SDS_WAIT4_OFF=12, 
  /** 	the controller is showing the switch off symbol sequence.*/
  SDS_SWITCHOFF_PATTERN=13, 
  /** the controller has switched off due to failure and is showing special fault off symbols (e.g. blinking 2 HZ).*/
  SDS_FAULT_OFF=14,
  /** the cbu is running in emergency mode, the omc is switching the subintersection back into normal operation. */
  SDS_RECOVER_FROM_EMERGENCY_MODE=15
};
/** definitions used by actualstatus defining the value of an modification for TD, public transport, orientation tact ...
 *  @see <a href="https://svnocit.de/dok/trunk/html/ocitf_r.html#ModEinAusZustand1">OCIT ModEinAusZustand 1:65</a> */ 
enum ModOnOffStateCctrl
{
  /** no state set, undefined state or local state choice given free */
  MOOS_NONE=0, 
  /** modification set off */
  MOOS_OFF=1, 
  /** modification set on */
  MOOS_ON=2
};
/** definitions used by @{linkplain#ActualStatusCctrl} defining the on/off state of the main intersections. */
enum IntersectionStateCctrl
{
  /** no intersection state known or defined. */
  IS_NONE=0, 
  /** main intersection is on.<br></br>
   *  note the intersection is only running a signalplan if main intersection and sub intersection is on and a signal plan is defined. */
  IS_ON=1, 
  /** main intersection is in default off.
   *  signal groups chosen by configuration (typically vehicle signal groups of secondary direction) are flashing normally, the remaining signal groups are dark */
  IS_OFF_DEFAULT=2,
  /** intersection is switched off flashing secondary direction and for all active sub intersections applies respectively:<br></br>
   *  vehicle signal groups of secondary direction are flashing, the remaining signal groups are dark */
  IS_OFF_FLASH_SECONDARY_DIRECTION=3, 
  /** intersection is to be switched off dark and all signal groups of active sub intersections are dark respectively */ 
  IS_OFF_DARK=4, 
  /** intersection is to be switched off flashing all and all signal groups of active sub intersections are flashing respectively */
  IS_OFF_FLASH_ALL=5, 
  /** intersection controller is manually controlled. the stages sequence is taken of configuration, they are continued manually. */ 
  IS_MANUAL_CYCLIC=6,
  /** intersection controller is manually controlled. the stage sequence and the continuation of stages is controlled manually. */
  IS_MANUAL_ACYCLIC=7,
  /** OMC is in an only via restart recoverable error condition. e.g. no ticks from OMC received. */
  IS_FATAL_ERROR=8,
  /** the cbu is running in emergency mode, the omc is switching the intersection back into normal operation. */
  IS_RECOVER_FROM_EMERGENCY_MODE=9
};
/** transaction tagged value sub-intersection detail state at C-Control interface {@linkplain ActualStatusCctrl}<br></br>
 *  data transport object for partial intersection detail status.*/
struct TTVSubIntersectionDetailStateCctrl 
{
	/** sub-intersection number, see hlc_base.xsd IntersectionVersion/PartialIntersectionList/No */
	int subintersectionNo;
	/** system job id or transaction number. gives a hint why this state has been established. it consists of a originator and a sequence number 
	 *  see <a href="doc-files/TransactionId.htm">TransactionId doc</a> */
	int sysJobId;
	/** sub-intersection detail state value */
	enum SubIntersectionDetailStateCctrl value; 
};

/** data transfer object for transaction tagged on off modification value */
struct TTVModOnOffStateCctrl
{
	/** system job id or transaction number. gives a hint why this state has been established. it consists of a originator and a sequence number 
	 *  see <a href="doc-files/TransactionId.htm">TransactionId doc</a> */
	int sysJobId;
	/** type of modification */
	enum TypeOfModificationCctrl modType;
	/** modification number, 
	 *  only relevant with project specific modifications ( modType={@linkplain TypeOfModificationCctrl#TOM_ModPrj1} .. {@linkplain TypeOfModificationCctrl#TOM_ModPrj13})
	 */
	int modNo;
	/** value of this modification */
	enum ModOnOffStateCctrl value;	
};
/** wraps variable length array of {@linkplain TTVModOnOffStateCctrl} */
typedef struct TTVModOnOffStateCctrl TTVModOnOffStateCctrlA<16>;

/** data transfer object used with {@linkplain CControl_CcontrolDataReceiver_CltIF#actualStatus_8(ActualStatusCctrl, int)} 
 *  to provide the actual status (Betriebszustand) to C-Control clients. 
 *  @see <a href="doc-files/genericSubscriptionForDeliveryOfSpontaneousData.png">genericSubscriptionForDeliveryOfSpontaneousData</a> */
struct ActualStatusCctrl
{
	/** device tick time in ms, since last start of CBC when this actual status has changed. */
	hyper tickTimeOfLastChange;
	/** intersection controller global fault state (Sammelfehler) */
 	enum SummaryAlarmCctrl globalFaultState;

	/** transaction id of {@linkplain #operatingMode}, gives a hint to originator of this status.
	 *  @see <a href="doc-files/TransactionId.htm">TransactionId doc</a> */
 	int operatingModeSysJobId;
 	/** actual operating mode of this intersection controller. */ 
 	enum IntersectionOperatingModeCctrl operatingMode;

	/** transaction id of {@linkplain #signalProgram}, gives a hint to originator of this spl.
	 *  @see <a href="doc-files/TransactionId.htm">TransactionId doc</a> */
 	int signalProgramSysJobId;
 	/** number of actual or last running signal program. */ 
 	int signalProgram;
	
	/** transaction id of {@linkplain #intersectionOnOffState}, gives a hint to originator of this intersectionOnOffState.
	 *  @see <a href="doc-files/TransactionId.htm">TransactionId doc</a> */
	int intersectionOnOffStateSysJobId;
	/** actual main intersection On Off state value */
	enum IntersectionStateCctrl intersectionOnOffState;
  
  	/** variable length list of sub-intersection states. 
  	 *  The number of sub-intersections is dependent of the configuration hlc_base.xsd IntersectionVersion/PartialIntersectionList.
  	 *  there is typically at least one partial intersection and so one subIntersectionState */
  	TTVSubIntersectionDetailStateCctrl subIntersectionStates<4>;

	/** transaction id of {@linkplain #specialIntervention}, gives a hint to originator of this specialIntervention.
	 *  @see <a href="doc-files/TransactionId.htm">TransactionId doc</a> */
	int  specialInterventionSysJobId;
	/** special intervention number. if no special intervention is engaged specialIntervention==0 */
	int	 specialIntervention;
  
	/** list of modifications, containing traffic actuation related and project specific modifications.
	 *  <p>the index of the first 4 modifications is always the order of {@link TypeOfModificationCctrl#getEncoding()} that is:<ol>
	 *  <li>TOM_ModTD(0)</li><li>TOM_ModPublicTransport(1)</li><li>TOM_ModTDIndividualMkjtric(2)</li><li>TOM_ModSynchronisationState(3)</li><ol>
	 *  the above mentioned modification are always set if at least TOM_ModTD is not {@link ModOnOffStateCctrl#MOOS_NONE})
	 *  </p>
	 */
	struct TTVModOnOffStateCctrl modifications<17>;

};

/* ------ standard messages ------ */
/** Message parameter type information according to Canto and OCIT-O */
enum MsgParamTypeInfoCctrl
{
	/** Parameter data type int8_t. */
	MPTI_BYTE=0,
	/** Parameter data type uint8_t. */
	MPTI_UBYTE=1,
	/** Parameter data type int16_t. */
	MPTI_SHORT=2,
	/** Parameter data type uint16_t. */
	MPTI_USHORT=3,
	/** Parameter data type int32_t. */
	MPTI_LONG=4,
	/** Parameter data type uint32_t. */
	MPTI_ULONG=5,
	/** Parameter data type float. */
	MPTI_FLOAT=6,
	/** Parameter data type double. */
	MPTI_DOUBLE=7,
	/** Parameter data type uint8_t[] with length &lt; 65535. */
	MPTI_STRING=8,
	/** Parameter data type bool_t. */
	MPTI_BOOL=10,
	/** Parameter data type transaction identifier (uint32_t). */
	MPTI_SYSJOBID=13,
	/** Parameter data type SHA1 checksum (uint8_t[20]). */
	MPTI_CHECKSUM=14,
	/** Parameter data type UTC time stamp (int64_t). */
	MPTI_UTC=15,
	/** Parameter data type OCIT-O domain reference. */
	MPTI_REFERENCE=16,
	/** Parameter enum used to transport TA status to firmware and enable nls translation, 
	 * pass ordinal as well it is needed by OCIT/CANTO-control center process */ 
	MPTI_ENUM=17,
	/** Parameter data type int64_t. to be displayed as number */
	MPTI_INT64=18
};

/** Parameter data type OCIT-O domain reference. */
struct MsgParamTypeRefCctrl
{
	/** Licensee identifier */
	int member;
	/** Object type: <code>member</code> and <code>oType</code> define a unique type (domain) */
	int oType;
	/** additional instance addressing path; depends on runtime vales of <code>member</code> and <code>oType</code>*/
	char path<>;
};
struct MsgParamTypeEnumCctrl
{
        /** ordinal number for use by OCIT/CANTO-control center process */
        int ordinal;
        /** enumeration value name, used as default nls value and key for nls-translation at sX-GUI. */
        string name<>;
};

/** Value of message parameter; up to now integer values or string values are supported as message parameters */
union MsgParameterValueCctrl switch ( MsgParamTypeInfoCctrl typeInfo )
{
  case MPTI_BYTE:
  case MPTI_UBYTE:
  case MPTI_SHORT:
  case MPTI_USHORT:
  case MPTI_LONG:
  case MPTI_ULONG:
  case MPTI_SYSJOBID:
	int intValue;
  case MPTI_BOOL:
	bool boolValue;
  case MPTI_UTC:
	hyper timeStamp;
  case MPTI_FLOAT:
	float fValue;
  case MPTI_DOUBLE:
	double dValue;
  case MPTI_CHECKSUM:
	char checkSum<20>;
  case MPTI_STRING:
	string strValue<>;
  case MPTI_REFERENCE:
	MsgParamTypeRefCctrl reference;
  case MPTI_ENUM:
        MsgParamTypeEnumCctrl enumValue;
  case MPTI_INT64:
        hyper int64Value;
};

/** Describes a single message parameter of type int or string */
struct MsgParameterCctrl
{
	/** Symbolic name of message parameter for debugging or logging purpose */
	string name<>;
	/** Value of message parameter */
	MsgParameterValueCctrl value;
};


/* ------ status/CommonStates ------ */

/**
 * A single Canto 1.3 common status device variable as provided by the C10 archive
 */
struct CommonStatusCctrl
{
	/** device tick time in ms, since last start of CBC when this common status has changed. */
	hyper tickTimeOfLastChange;
	/** OType value of a common status device variable OITD 59.n; possible values for n: [401..654] */
	int otype;
	/**
	 * Value of CommonState; Valid values for all device variables are: 254=NOT_APPLICABLE and 255=UNKNOWN;
	 * Other values [0..253] defined by Canto 1.3 depending on <code>otype</code>. */
	int value;
};

/* ------ status/LampStates ------ */

/** 
 * Definitions used by {@linkplain LampStatusCctrl} defining the status of a single lamp addressed
 * by signal group, head, and chamber.
 */
enum LampFaultStateCctrl
{
	/** Lamp or all lamps of a head were checked without failure. */
	LFS_OK=0,
	/** Single Lamp addressed by signal group number, head, and chamber is defect. */
	LFS_DEFECT=1,
	/** At least on Lamp of group of lamps addressed by signal group number and chamber is defect. */
	LFS_GROUP_DEFECT=2,
	/** Lamp addressed by signal group number, head, and chamber is not monitored. */
	LFS_NOT_MONITORED=3,
	/** Lamp addressed by signal group number, head, and chamber is not used resp. connected. */
	LFS_UNUSED=4,
	/** Fault state of a single Lamp addressed by signal group number, head, and chamber is not known. */
	LFS_UNKNOWN=255
};

/** 
 * Definitions used by {@linkplain LampStatusCctrl} defining the time constraints of checking a single color.
 */
enum LampFaultTimeConstraintsCctrl
{
	/** lamps were checked less than one hour before. */
	LFTC_LAST_HOUR=0,
	/** lamps were checked less than one day and more than one hour before. */
	LFTC_LAST_DAY=1,
	/** lamps were checked more than one hour before. */
	LFTC_SOMETIMES=2,
	/** lamps were not checked since last start. */
	LFTC_NEVER=3
};


/**
 * A lamp fault state of a single signal group as provided by the C10 archive.
 */
struct LampStatusCctrl
{
	/** device tick time in ms, since last start of CBC when this lamp status has changed. */
	hyper tickTimeOfLastChange;
	/** signal group number as configured with hlc_base.xml of signal group this LampStatus represents. */
	int sgNo;
	/** Time constraints for red lamps. */
	enum LampFaultTimeConstraintsCctrl tcRed;
	/** Time constraints for amber lamps. */
	enum LampFaultTimeConstraintsCctrl tcAmber;
	/** Time constraints for green lamps. */
	enum LampFaultTimeConstraintsCctrl tcGreen;
	/** Lamp fault states for up to 4 read heads */ 
	enum LampFaultStateCctrl redHeads<4>;
	/** Lamp fault states for up to 4 amber heads */ 
	enum LampFaultStateCctrl amberHeads<4>;
	/** Lamp fault states for up to 4 green heads */ 
	enum LampFaultStateCctrl greenHeads<4>;
};

/* ------ detmanager/DetectorAggregation ------ */

/** Structure from speed and volume for vehicle class depending values. */
struct SpeedVolumeCctrl
{
	/** Average speed of vehicle class in km/h */
	short speed;
	/** Volume of vehicle class in (vehicle/h) */
	int volume;
};

/**
 * Data transport object for single aggregated detector value.
 */
struct DetectorAggregationCctrl
{
    /** detector channel number */
	short  nr;
	/** Volume of vehicles in vehicle/h */
	int  volume;
	/** Level of occupancy rate in 0.01% units.*/
	int occupanceRate;
	/** Quality of this value in % */
	unsigned char quality;
	/** average speed and volume of vehicles in classes. index is a VehicleType. 
	 * In case the detector does NOT provide extended information this array is empty (length 0). 
	 * In case the detector provides extended information this array has the length 9. 
	 * The length SHALL be 0 or 9 (other length values are not allowed).
	 * Vehicle type indices:
	 * <ol start=0>
	 * <li>Automobile</li>
	 * <li>Automobile with trailer</li>
	 * <li>Truck</li>
	 * <li>Truck with trailer</li>
	 * <li>Bus</li>
	 * <li>Not classified vehicle</li>
	 * <li>Bike</li>
	 * <li>Delivery van</li>
	 * <li>Trailer truck</li>
	 * </ol>
	 */
	SpeedVolumeCctrl sv<>;
};

/** Properties of fixed aggregation cycle. */
struct AggregationCycleCctrl
{
	/** Cycle time in ms. */
	int samplingInterval;
	/** Offset to standard Canto reference point in time in ms. Offset is calculated modulo samplingInterval. */
	int offset;
};

/**
 * Union DTO to transfer depending on the {@link RetCodeCctrl} a result of type {@link AggregationCycleCctrl}.
 * <p>
 * Used by {@link CControl_CcontrolService_CltIF#getAggregationCycle_8}. If <code>error</code> is not {@link RetCodeCctrl#RC_OK},
 * no {@link AggregationCycleCctrl} instance is transfered! 
 * </p>
 */
union AggregationCycleResultCctrl switch (enum RetCodeCctrl retCode) {
	case RC_OK:
		AggregationCycleCctrl ac;
	default:
		void;
	};

/**
 * Union DTO to transfer depending on the {@link RetCodeCctrl} the maximum aggregation cycle time of signalling cycle based aggregation.
 * <p>
 * Used by {@link CControl_CcontrolService_CltIF#getAggregationSignalling_8}. If <code>error</code> is not {@link RetCodeCctrl#RC_OK},
 * no value is transfered! 
 * </p>
 */
union AggregationSignallingResultCctrl switch (enum RetCodeCctrl retCode) {
	case RC_OK:
		int maxSamplinInterval;
	default:
		void;
	};

/* ------ detmanager/DetectorValues ------ */

/** data transport object that transmits the detector status and values of one detector channel.<br></br>
 * There are multiple {@linkplain DetDataStruct} dto's reflecting the multiple changes since last update (100ms typical polling interval).
 * needed by  {@link CControl_CcontrolService_CltIF#archiveGetLaterThan_8}, {@link CControl_CcontrolService_CltIF#archiveGetYoungest_8},
 * or {@link CControl_CcontrolService_CltIF#archiveGetOldest_8} to avoid merging of detector values with detector states using multiple
 * RPC calls. 
 */
struct DetRawValuesGradedCctrl {
    /** detector channel number */
	short  nr;					  		  
	/** last value of detection: false, 0=gap/ true, 1==occupied */
	bool   value;		  				  
	/** quality attribute of the associated detector value(s), e.g:<ul>
	 *   <li>0% value not reliable,</li> 
	 *   <li>100% value fully reliable</li></ul> */
	unsigned char quality;			
	/** detector error Code reported by detector, 0==OK, no Error */
	DetectorErrorCodeCctrl errorCode;
	/** absolute device ticks in ms of last change of this detector occupancy value */
    hyper  TicksOfLastChange;	  		  
	/** relative to absDevTickTime of enclosing {@linkplain DetRawValuesExchg} */
	struct DetDataStruct detDatas <256>;  
};

/* ------ sgcontrol/SignalIndications ------ */
/** DTO with per signal group informations (IstBilder) */ 
struct SignalIndicationCctrl {
	/** signal group number as configured with hlc_base.xml of signal group this SignalIndication represents. */
	int sgNo;
	/** color that is shown by signal group {@linkplain #sgNo}. <a href="doc-files/SignalGroupColorEncoding.html">SgColorEncoding</a> 
	 * <p>if  {@link  CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} with {@link TdtSubscriptionInfoCctrl#withRedGreenStates}==true
	 * the RedGreenState &lt;&lt; 12  is ored into ocitColor.<br></br>
	 * RedGreenState numbers are defined as follows:<ol start="0">
	 *     <li><b>unknown</b>,</li>
	 *     <li><b>red</b> blocked (gesperrt)</li>
	 *     <li><b>green</b> clearance (freigegeben)</li>
	 *     <li><b>redGreen</b> transition blocked-->clearance (Uebergang sperr-frei)</li>
	 *     <li><b>greenRed</b> transition clearance-->blocked (Uebergang frei-sperr)</li>
	 * </ol></p> 
	 * */
	int ocitColor;
	/** device Tick time in ms of colorStart = baseTickTime - colorStartTime*100<br></br>
	 *  with {@linkplain SignalIndicationsCctrl#baseTicktime} of enclosing {@linkplain SignalIndicationsCctrl} DTO.  */
	int  colorStartTime;
};
/** DTO for transmitting the signal indications of all signal-groups at a time (IstBilder).<br></br>
 *  used with {@linkplain CControl_CcontrolDataReceiver_CltIF#actualSignalIndications_8} */
struct SignalIndicationsCctrl {
 	/** OMC device tick time milliseconds, when this value was sent by CBC. 
 	 * OMC device tick time is started at last coldstart of OMC. */	
	hyper baseTicktime;
	/** difference between utc ({@linkplain System#currentTimeMillis() }) and device tick time at reception of this signal indication values. 
	 *  Unit milliseconds. */
	hyper baseUtcMinusTicktime;
	/** cycle time counter in milliseconds */
	hyper tx;								
	/** one element per configured signal group */
	struct SignalIndicationCctrl sgPatterns <64>;  
};

/* ------ structures for other methods ------ */

/** 
 * byte array of traffic actuation control parameter file
 */
struct MkjtricActuationComponentParams
{
	opaque componentParams<>;
};

union MkjtricActuationComponentParamsResult switch (enum RetCodeCctrl retCode) {
case RC_OK:
	MkjtricActuationComponentParams tacp;
default:
	void;
};

/** nls aware initiator tag for use with ExternalSwitchingEvents. it provides a further specification of the reason why the event was generated. 
 * in its way inside the firmware further information is added. */
struct InitiatorCCtrl {
  /** format string, currently only %s are allowed. */
  string format<>;
  /** initialtor parameters */
  MsgParameterValueCctrl params<>;
};

/** data transport object for signal group related signal indication control.
 * needed by signal group oriented traffic actated control (VA). 
 */
struct ExternalSgSwitchingEvent {    

    /** microscopic priority, 0=lowest, 10 JAUT, 20 local Mkjtric actuation, device GUI 40...
     * the traffic actated control has to use the priority dependant of its  {@link RequestedStatusPriorityCctrl macroscopic priority} */
    int priority;    
    /** device tick time when to start this signal indication, -1 means as soon as possible. */
    hyper startTickTime;
    /** number of ms that this switching event stays valid after startTickTime (if not overwritten). */
    hyper validDuration;
    /** initiator is an arbitrary message that is associated with this switching event */
    InitiatorCCtrl initiator;
    
    /** reference to signal group that is influenced by this switching event. */
    int signalGroupNumber;
    /**  <a href="doc-files/SignalGroupColorEncoding.html">color</a> that is shown by signal group {@linkplain #signalGroupNumber} 
     * if this switching event is realized. */
    int signalIndication;
    /** true if transition patterns shall be inserted if necessary  by the ControlCore. */
    bool isWithTransition;
    /** true if minimum time and intergreen time constraint corrections shall be done by the ControlCore. */
    bool isWithCorrection;
};
/** variabel length list of {@link ExternalSgSwitchingEvent}.
 * needs to be a typedef because of jrpcgen restrictions. */
typedef struct ExternalSgSwitchingEvent ExternalSgSwitchingEventArray <>; 

/** data transport object for stage related signal indication control.
 * needed by stage oriented traffic actated control (phasenorientierte VA). 
 */
struct ExternalStageSwitchingEvent {    
    /** microscopic priority, 0=lowest, 10 JAUT, 20 local Mkjtric actuation, device GUI 40...
     * the traffic actated control has to use the priority dependant of its  {@link RequestedStatusPriorityCctrl macroscopic priority} */
    int priority;
    /** device tick time when to start this stage, -1 means as soon as possible. */
    hyper startTickTime;
    /** number of ms that this switching event stays valid after startTickTime (if not overwritten). */
    hyper validDuration;
    /** initiator is an arbitrary message that is associated with this switching event */
    InitiatorCCtrl initiator;

    /** number of desired stage, refers to configuration (hlc_base.xml), {@link CfgStage#stageNo} */
    int stageNumber;
};
/** variabel length list of {@link ExternalStageSwitchingEvent}.
 * needs to be a typedef because of jrpcgen restrictions. */
typedef struct ExternalStageSwitchingEvent ExternalStageSwitchingEventArray <>; 

/* --- TargetStage values --- */
/** dynamic value containing stage informations. 
 * clients may obtain these values by {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribing} for 
 * {@link MkjtricDataTypeCctrl#TDT_TARGETSTAGE} at C-Control Service.
 * <p>java C-Control clients SHALL use {@link com.sitraffic.fd.ic.ccontrolClient.DataReceiver} in conjunction with {@link com.sitraffic.fd.ic.ccontrolClient.DataReceiverStarter}.</p>
 */
struct TargetStage
{

  /**
   * number of target/actual stage. Number 0 signals unknown stage. If {@link #isStageSignalIndication} ==true
   * the target Stage is equal to actual stage.
   */
  int stageNo;

  /**
   * device tick time ms when the actual stage has been started. duration (ms) of actual stage may be calculated
   * by implementation from start tick time of actual stage.
   */
  hyper stageStartTime;

  /**
   * number of target Stage in case an actual stage transition controlled by phaser is in progress. 
   * if there is no stage transition running 0 is returned.
   */
  int nextStageNo;

  /**
   * device tick time ms when actual stage transition has been started.
   */
  hyper stageTransitionStartTime;

  /**
   * duration of current stage transition if any.
   * in case there is a on the fly calculated stage transition this value may not be correct.
   */
  hyper stageTransitionDuration;

  /**
   * true if no stage transition in progress and actual signal indication matches the actual stage.
   */
  bool isStageSignalIndication;

};
typedef TargetStage * TargetStagePtr;

/**
 * definitions used at addCheckDesiredStageSwitchingEvents_8 to specify the allowd violation types.
 * the definition may be used as bitfield.
 */
enum TypeOfViolation {
  /** no violation */
  TOV_None=0,
  /** not configured stage transitions are generated by shifter. */
  TOV_PhaserMissingStageTransition=1,
  /** stage min time violation */
  TOV_PhaserStageMintimeViolation=2,
  /** */
  TOV_PhaserStageSkipped=4,
  /** */
  TOV_ShifterIgtViolation=8,
  /** */
  TOV_ShifterColorMintimeViolation=16,
  /** */
  TOV_ShifterOffsettimeViolation=32,
  /** */
  TOV_ShifterColorSkipped=64,
  /** */
  TOV_EventDroppedAtInputCheck=128,
  /** */
  TOV_InvalidSgcontrolState=256
};

/* --- RequestedStatusIF --- */

/**
 * on/off modification state definition for requested status.
 */
enum RequestedModificationStateCctrl {
  /** no modification state, that is request is to be cleared */
  RMS_NONE = 0,
  /** modification is off. */
	RMS_OFF = 1,
  /** modification is on. */
	RMS_ON = 2
};

/** sub-intersection state definitions used at requested status. */
enum RequestedNodeStateCctrl {
  /**
   * sub-intersection is to be switched on to signal program defined by object TargetSignalProgram and for all active sub
   * intersections applies respectively: signal program defined by ActualSignalProgram is executed
   */
  RNS_ON=1,
  /**
   * sub-intersection is to be switched off default and for all active sub intersections applies respectively: signal
   * groups chosen by configuration are flashing normally vehicle signal groups of secondary direction, the remaining
   * signal groups are dark
   */
  RNS_OFF_DEFAULT=2,
  /**
   * sub-intersection is to be switched off flashing secondary direction and for all active sub
   * intersections applies respectively: vehicle signal goups of secondary direction are flashing, the remaining signal
   * groups are dark "Aus-Blinken".
   */
  RNS_OFF_FLASH_SECONDARY_DIRECTION=3,
  /**
   * sub-intersection is to be switched off dark and all signal groups of active sub intersections are dark
   * respectively "Aus-Dunkel".
   */
  RNS_OFF_DARK=4,
  /**
   * sub-intersection is to be switched off flashing all and all signal groups of
   * active sub intersections are flashing respectively " AUS-Zustand Alles Gelb-Blinken ".
   */
  RNS_OFF_FLASH_ALL=5,
  /**
   * partial intersection must be shut down due to a pending signal monitoring error.
   * set by firmware only !
   */
  RNS_FAULT_SHUTDOWN=6

};
/** requested state for whole intersection. */
enum RequestedMainNodeStateCctrl {
  /**
   * intersection is to be switched on to signal program defined by object TargetSignalProgram and for all active sub
   * intersections applies respectively: signal program defined by ActualSignalProgram is executed
   */
  RMNS_ON=1,
  /**
   * intersection is to be switched on off default and for all active sub intersections applies respectively: signal
   * groups chosen by configuration are flashing normally vehicle signal groups of secondary direction, the remaining
   * signal groups are dark
   */
  RMNS_OFF_DEFAULT=2,
  /**
   * "Aus-Blinken". intersection is to be switched on off flashing secondary direction and for all active sub
   * intersections applies respectively: vehicle signal goups of secondary direction are flashing, the remaining signal
   * groups are dark
   */
  RMNS_OFF_FLASH_SECONDARY_DIRECTION=3,
  /**
   * "Aus-Dunkel". intersection is to be switched on off dark and all signal groups of active sub intersections are dark
   * respectively
   */
  RMNS_OFF_DARK=4,
  /**
   * " AUS-Zustand Alles Gelb-Blinken ". intersection is to be switched on off flashing all and all signal groups of
   * active sub intersections are flashing respectively
   */
  RMNS_OFF_FLASH_ALL=5,
  /**
   * manual cyclic ON (based on Signalplan) control. 
   */
  RMNS_MANUAL_CYCLIC=6,
  /**
   * acyclic manual control ON, desired phase may be selected manually.
  */
  RMNS_MANUAL_ACYCLIC=7
};

/** definitions for {@link RequestedStatusCctrl#modState} */
enum RequestedStatusModNoCctrl {
	/** index of traffc actuated on/off requested state modification. */
  RSMN_VAState=0,
  /** index of traffc actuated public transport optimization on/off requested state modification. */
	RSMN_OEVState=1,
	/** index of traffc actuated individual traffic optimization on/off requested state modification. */
	RSMN_IVState=2,
	/** index of synchronization on/off requested state modification. */
	RSMN_SynchronisationState=3,
	/** index of project specific on/off requested state modification number 0. */
	RSMN_Proj00=4,
  /** index of project specific on/off requested state modification number 1. */
	RSMN_Proj01=5,
  /** index of project specific on/off requested state modification number 2. */
	RSMN_Proj02=6,
  /** index of project specific on/off requested state modification number 3. */
	RSMN_Proj03=7,
  /** index of project specific on/off requested state modification number 4. */
	RSMN_Proj04=8,
	/** index of project specific on/off requested state modification number 5. */
	RSMN_Proj05=9,
  /** index of project specific on/off requested state modification number 6. */
	RSMN_Proj06=10,
  /** index of project specific on/off requested state modification number 7. */
	RSMN_Proj07=11,
  /** index of project specific on/off requested state modification number 8. */
	RSMN_Proj08=12,
  /** index of project specific on/off requested state modification number 9. */
	RSMN_Proj09=13,
  /** index of project specific on/off requested state modification number 10. */
	RSMN_Proj10=14,
  /** index of project specific on/off requested state modification number 11. */
	RSMN_Proj11=15,
  /** index of project specific on/off requested state modification number 12. */
	RSMN_Proj12=16
};

/** DTO needed for {@link RequestedNodeStateCctrl}.
 * It is used for multiple cases, there are put the enum's encoding intto {@link TimeConstrainedTransactionTaggedIntValueCctrl#value}.
 */
struct TimeConstrainedTransactionTaggedIntValueCctrl {
  /** value of this TimeConstrainedTransactionTaggedIntValueCctrl, its meaning is depending of the context (the embedding structure). */
  int   value;
  /** transaction id of this switching request, gives a hint to originator of this status. 
   * If this switching request gets realized, the sysJobId appears at {@link ActualStatusCctrl} and {@link ActualStatusDtoCctrl}.
   * @see <a href="doc-files/TransactionId.htm">TransactionId doc</a> */
	int   sysJobId;
	/** priority of this switching request. Fill in here values obtained by {@link RequestedStatusPriorityCctrl#getEncoding()}. */
	int   priority;
	/** device time in milliseconds how long this switching request stays valid. (it expires at {@link #validUntilDeviceTime}+1. */  
	hyper validUntilDeviceTime;
};
/** DTO representing project specific modifications at Requested Status. 
 * they have additional to {@link TimeConstrainedTransactionTaggedIntValueCctrl} a {@link #modNo} and a  {@link #modType} */
struct TcTTVModOnOffStateCctrl{
  /** value of this TimeConstrainedTransactionTaggedIntValueCctrl, its meaning is depending of the context (the embedding structure). */
  RequestedModificationStateCctrl   value;
  /** transaction id of this switching request, gives a hint to originator of this status. 
   * If this switching request gets realized, the sysJobId appears at {@link ActualStatusCctrl} and {@link ActualStatusDtoCctrl}.
   * @see <a href="doc-files/TransactionId.htm">TransactionId doc</a> */
  int   sysJobId;
  /** priority of this switching request. Fill in here values obtained by {@link RequestedStatusPriorityCctrl#getEncoding()}. */
  int   priority;
  /** device time in milliseconds how long this switching request stays valid. (it expires at {@link #validUntilDeviceTime}+1. */  
  hyper validUntilDeviceTime;  
  /** type of modification */
  RequestedStatusModNoCctrl modType;
  /** modification number, 
   *  only relevant with project specific modifications ( modType={@linkplain TypeOfModificationCctrl#TOM_ModPrj1} .. {@linkplain TypeOfModificationCctrl#TOM_ModPrj13})
   */
  int modNo;
};

/** DTO used to pass requested status to C-Control. the requested status contains macroscopic switching requests. */
struct RequestedStatusCctrl {
	/** value = RequestedMainNodeStateCctrl */ 
   TimeConstrainedTransactionTaggedIntValueCctrl intersectionState;
   
   /** value = RequestedModificationStateCctrl
    * index is  RequestedStatusModNoCctrl */
   TcTTVModOnOffStateCctrl modState<17>;

   /** value = int signal program number */
   TimeConstrainedTransactionTaggedIntValueCctrl signalProgram;
   
   /** value = int special intervention number */
   TimeConstrainedTransactionTaggedIntValueCctrl specialIntervention;
   
   /** value = RequestedNodeStateCctrl
    * index is partial intersection number 0..3 */
   TimeConstrainedTransactionTaggedIntValueCctrl subIntersectionState[4];
};

/** priority values used at RequestedStatusIF and RequestedStatusManagerIF methods. 
 * priorities SHALL be &gt; 0 and &lt; RSP_Internal 
 */
enum RequestedStatusPriorityCctrl
{
  /** priority of time of year automatic (JAUT) */
  RSP_JAUT = 10,
  /** priority of local traffic actuation in normal mode. */
  RSP_TA=20,
  /** priority of central system in normal mode. */
  RSP_CentralSystem=30,
  /** priority of remote service PC */
  RSP_ServicePC=35,
  /** priority of manual control via device GUI or BAZ */
  RSP_Manual = 40,
  /** priority of manual blocking control */
  RSP_ManualBlocking = 50,
  /** priority of special intervention by central system */
  RSP_SiCentralSystem = 110,
  /** priority of special intervention by local traffic actuation */
  RSP_SiTA = 120,
  /** priority of special intervention by detector */
  RSP_SiDetector = 130,
  /** priority of special intervention by device GUI */
  RSP_SiGUI = 140,
  /** priority of special intervention by railroad */
  RSP_SiTrain = 150,
  /** priority used by {@link CControl_CcontrolService_CltIF#prepare4Shutdown2Update_8}.
   * to avoid macroscopic race condition (time between switched off and shutdown for SW update).
   */
  RSP_SwUpdateLock=899,
  /** priority used by FaultLockManager to keep a macroscopic state blocked.
   * on Simo operating state  NORMAL the FaultLockManager removes this lock.  */
  RSP_MainsFaultLock = 900,  
  /** priority used to prevent intersection controller from accidentally switch on. */
  RSP_FaultLock = 910,
  /** priority used by license manager to enforce intersection off or TA off in case of missing license. */
  RSP_LicenseLock = 920,
  /** maximal priority at CControl, use for tests only */
  RSP_MaxPrio = 999,
  /** maximum priority used for internal operation (eg. on-/off sequences) 
   *  MUST NOT be used by external programs */
  RSP_Internal=1000
};

/* --- End RequestedStatusIF --- */


/* --- definitions for ConfigAccess --- */

/** definition used for {@link CControl_CcontrolDataReceiver_CltIF#checkConfig_8 ConfigConsumer} values for use as Bitfield to specify the configurations to check.
 * currently there are 2 configuartion files at one version (draft, inventory, archive). ConfigType is used to distinguish between them. */
enum ConfigType {
  /** address the traffic related configuration. */
	CT_BASE=1, 
	/** address the systemc related configuration. */
	CT_SYSTEM=2,
	/** address the traffic actuation control code. */
	CT_TACODE=4,
	/** address the traffic actuation control parameter. */
	CT_TAPARAM=8
};
/** Result of C-Control client Config check. (ConfigConsumer function {@link CControl_CcontrolDataReceiver_CltIF#checkConfig_8 ConfigConsumer} */
enum CheckResult {
  /** config consumer was not affected by the configuration to check. */
	CR_NOT_AFFECTED=0, 
  /** config consumer is able to activate the configuration to check on the fly, that is without restart or reset. */
	CR_ON_THE_FLY=2, 
  /** restart TA process is required */
	CR_TA_RESTART=3,
  /** config consumer is able to activate the configuration to check only at next restart or reset. 
   * an external (connected via C-Control) config consumer has to manage its restart at activation itself. 
   * There is no further action taken by firmware. */
	CR_RESTART=4,
	/** config consumer has detected a problem inside the configuration to check that makes it impossible for this config consumer to operate with it.
	 * returning this value prevents transition from draft to iunventory of the configuration to check. */
	CR_REFUSE=8
};
/** used to identify a C-Control Config Consumer */
enum UnitId {
	/** for use by firmware control. */
	UID_FIRMWARE_CCONTROL=2,
	/** for use by traffic actuated control. */
	UID_TRAFFIC_ACTUACTED_CCONTROL=3,
	/** for use by control center communication process. */
	UID_CENTRAL_ADAPTER_CCONTROL=4
};

/** Severity definitions at C Control for messages and status. */
enum SeverityCctrl {
  /** SHALL not be used by client */
  SMS_UNKNOWN=0,   /*  --> "black" */
  /** to be used with informations if used with status marked with green symbol */
  SMS_OK=1,        /*  --> "green" */
  /** to be used with hints if used with status marked with blue symbol */
  SMS_HINT=2,      /*  --> "blue" */
  /** to be used with warnings if used with status marked with yellow symbol */
  SMS_WARNING=3,   /*  --> "yellow" */
  /** to be used with errord if used with status marked with orange symbol */
  SMS_ERROR=4,     /*  --> "orange" */
  /** severe failures if used with status, mapped to ERROR since c-control clients shall not be sources of faults. */
  SMS_FAULT=5      /*  --> "red" */
};
/**
 * DTO used to transport a configuration message as result with checkConfig and status attachments.
 */
struct CfgMessageCctrl {  
  /**
   * severity of message.
   */
  SeverityCctrl severity;
  /** timestamp of the message in ms device ticks. */
  hyper ticks;
  /** transaction id, gives a hint to originator of this status/message.
   *  @see <a href="doc-files/TransactionId.htm">TransactionId doc</a> 
   */
  int sysjobid;
  /** (NLS) message with params. */
  InitiatorCCtrl msg; 
};

/** Respond of C-Control client Config check. (ConfigConsumer function {@link CControl_CcontrolDataReceiver_CltIF#checkConfig_8 ConfigConsumer}. */
struct CheckRespondCctrl {
  /** Result of C-Control client Config check. */
  CheckResult result;
  /** optional messages that explain result. */
  CfgMessageCctrl Messages<>;
};
/* --- End definitions for ConfigAccess --- */
/** DTO to report status of c-control client */
struct CltStatusMsgsCctrl {
  /** type of clt */
  enum UnitId unit;
  /** msgs detail state attachments to set. SHALL contain a Nls key as format */
  CfgMessageCctrl msgs<>;
};

/* --- definitions config structures ---*/	

/* definitions of signal group types. */
enum SignalGroupDetailTypeCctrl {
	/** vehicle 3 aspects */
	SGDT_VEH_3Aspect=1,
	/** vehicle 2 aspects */
	SGDT_VEH_2Aspect=2,
	/** vehicle 1 aspects */
	SGDT_VEH_1Aspect=3,
	SGDT_RightTurners_2Aspect=4,
	SGDT_RightTurners_1Aspect=5,
	SGDT_LeftTurners_2Aspect=6,
	SGDT_LeftTurners_1Aspect=7,
	SGDT_BUS_3Aspect=8,
	SGDT_BUS_2Aspect=9,
	SGDT_BUS_1Aspect=10,
	SGDT_TRAM_3Aspect=11,
	SGDT_TRAM_2Aspect=12,
	SGDT_TRAM_1Aspect=13,
	SGDT_Cyclist_3Aspect=14,
	SGDT_Cyclist_2Aspect=15,
	SGDT_Cyclist_1Aspect=16,
	SGDT_PED_3Aspect=17,
	SGDT_PED_2Aspect=18,
	SGDT_PED_1Aspect=19,
	SGDT_BLIND_PERS_SIG=20,
	SGDT_SPEED_SIG=21,
	SGDT_FLASHER=22,
	SGDT_Warning_Flasher=23,
	SGDT_Double_Flasher=24,
	SGDT_CONTACT_SIG=25,
	SGDT_DOOR_CLOSE_SIG=26,
	SGDT_Cyclist_Ped_SIG=29,
	SGDT_ALTERNATE_FLASHER=30,
	/**
	 * signal group type with 3 aspects for right-turning vehicles 
	 */
	SGDT_RightTurners_3Aspect=31,
	/**
	 * signal group type with 3 aspects for left-turning vehicles 
	 */
	SGDT_LeftTurners_3Aspect=32,
	/**
	 * signal group type with 3 aspects for cylclist and/or pedestrians 
	 */
	SGDT_Cyclist_Ped_SIG_3Aspect=33,
	/**
	 * signal group with 1 aspect for cylclist and/or pedestrians 
	 */
	SGDT_Cyclist_Ped_SIG_1Aspect=34,
	/**
	 * signal group with red light camera including a defined wiring
	 */
	SGDT_Red_Light_Camera_Kfz=35,
	SGDT_PED_BLIND_SIG=249,
	SGDT_PT_4POINT_SIG=250,
	SGDT_PERMISSIVE_AND=251,
	SGDT_PERMISSIVE_OR=252,
	SGDT_NO_TYPE=255
};

/** DTO to provide traffic related configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfg_8} and {@link CControl_CcontrolService_CltIF#getDraftSelectedCfg_8}. */
struct CfgSgInfo {
  /** signal group number, unique inside one intersection controller. */
	int sgNo;
	/** dynmic length list of color codes that signal clearance state (typically green light). */
	int greenStateColors<>;
  /** dynmic length list of color codes that signal block state (typically red light). */
	int redStateColors<>;
};
/** DTO used to provide signal group transitions and min times */
struct CfgSgStateTransitionElementCCtrl {
	/** OCIT color code. */
	int color; 
	/** duration [100ms] of this color inside this SG Transition. */
	int duration_100ms;
};
/** Sg state-mintime or state-transition element. Is referenced by the signal plan. */
struct CfgSgStateTransitionCCtrl {
	/** number of this sg state transition. unique within this controller at a single valid configuration. */
	int no;
	/** state transition elements in the order of their appearance. */
	CfgSgStateTransitionElementCCtrl elements<>;
};
/** DTO to provide extendes configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfg_8} and {@link CControl_CcontrolService_CltIF#getDraftSelectedCfg_8}. */
struct CfgSgInfo2CCtrl {
	/**
	 * signal group number
	 */
	int sgNo;
	/**
	 * IntersectionVersion/SignalGroupList/ShortName
	 */
	string name<>;
	/**
	 * IntersectionVersion/SignalGroupList/PartialIntersectionRef
	 */
	int partialIntersectionNo;
	/** detailed type of signal group */
	SignalGroupDetailTypeCctrl sgType;
	/**
	 * IntersectionVersion/SignalGroupList/MainDirection
	 */
	bool isMainDirection;
	/**
	 * ocit color code of all signal indications that signal go.
	 * starts with standard green state color.
	 */
	int greenStateColors<>;
	/**
	 * ocit color code of all signal indications that signal wait.
	 * starts with standard red state color.
	 * if color is part of a SgStateTransition color is a transitional color.
	 */
	int redStateColors<>;
	/** signal group state transitions from green-state to red-state. the first one is the standard transition. */
	CfgSgStateTransitionCCtrl transGnRd<>;
	/** signal group state transitions from red-state to green-state. the first one is the standard transition. */
	CfgSgStateTransitionCCtrl transRdGn<>;
	/** there is a SgTransitionElement for each GreenState-color. It describes the minimum time to use. */
	CfgSgStateTransitionCCtrl minGreen<>;
	/** there is a SgTransitionElement for each RedState-color. It describes the minimum time to use. */
	CfgSgStateTransitionCCtrl minRed<>;
};


/** DTO to provide traffic related parameter of signalgroup--detector association configuration with 
 * {@link CControl_CcontrolService_CltIF#getSelectedCfg_8} and {@link CControl_CcontrolService_CltIF#getDraftSelectedCfg_8}. */
struct CfgSgDetParam {
  /** signal group number, unique inside one intersection controller. */
	int sgNo;
	/** detector number, unique inside one intersection controller. */
	int detNo;
	/** time in ms after change to RedState of the sg with sgNo when the demands are reset */
	int demandResetThreshold; 
	/** time in ms if running Gap exceedes this value, no more traffic is assumed.
	 * if detector with detNo detects a gap in ms that is greater than this value, the current stage may be abandoned. */
	int runningGapThreshold; 
};
/** DTO to provide the signalisation associated with a stage with
 * {@link CControl_CcontrolService_CltIF#getSelectedCfg_8} and {@link CControl_CcontrolService_CltIF#getDraftSelectedCfg_8}. */
struct CfgSignalisation {
  /** signal group number, unique inside one intersection controller. */
	int sgNo;
  /** color that is shown by signal group {@linkplain #sgNo}. <a href="doc-files/SignalGroupColorEncoding.html">SgColorEncoding</a> */
	int color;
};

/** DTO to provide the signalisation associated with a stage with
 * {@link CControl_CcontrolService_CltIF#getSelectedCfg_8} and {@link CControl_CcontrolService_CltIF#getDraftSelectedCfg_8}. */
struct CfgStage {
  /** number of stage (Phase), unique inside a config version of a intersection controller. */
	int stageNo;
	/** signalisation within this stage. */
	struct CfgSignalisation signalisations<>;
};
/** association attribute<signalplan, stage>, gives signal plan specific stage attributes
 * StageSequence V2, extends {@link CfgStageSequenceElement} with fixedOrderStage, 
 * @since SX V2, config schema version 8 */ 
struct CfgStageSequenceElement2 {
  /** unique number of stage sequence element inside this stage sequence plan. */
  int stageSequenceElementNo;
  /** reference to stage {@link CfgStage#stageNo} */
  int stageNo;  
  /** minimum stage duration, relative time inside signaling cycle, 100ms resolution. for configuration tool only, not used by SLC. */
  int minDuration_100ms; 
  /** maximum stage duration, relative time inside signaling cycle, 100ms resolution. stage transition duration is not included. */
  int maxDuration_100ms;  
  /** if true the corresponding stage is extendable by SLC, (Bemessungsphase) */
  bool isExtendable;     
  /** if value is true, stage is mandatory, otherwise false (on demand); 
   * if value is false, at least one of the lists {@link #sgOnDemandList} or {@link #demandedByPtDirection} must not be empty;
   * if value is true, SgOnDemandlList must be empty */
  bool isMandatory;
  /** minimal stage duration for SLC, only relevant if isExtendable is true. */
  int extendableStageDuration_100ms; 
  /** give time of this stage if skipped to previous stage */
  bool isAddTimeToPredecessor;
  /** give time of this stage if skipped to next stage */
  bool isAddTimeToFollower;
  /** if isFixedOrderStage is true the stage is only switched if the previous stage is a demand stage */
  bool isFixedOrderStage;
  /** list of public transport directions that may demand this stage sequence element */
  int demandedByPtDirection<>;
  /** list of signal group references, (= signal group numbers).  sgOnDemandList contains all signal groups that may demand this stage sequence. 
   * if this list is empty, the stage is no demand stage. */ 
  int sgOnDemandList<>;
};
/** times when signalindication begins to show */
struct CfgStageTransitionSwitchingTime
{
  /** reference to signalgroup to switch by signal group number */
  int sgNo; 
  /**  <a href="doc-files/SignalGroupColorEncoding.html">color</a> that is shown by signal group <code>sgNo</code> 
    * if this switching time is realized. */
  int signalIndication;
  /** device tick time when to start this signal indication, relative to start of stage transition. */
  int time_100ms;
};
/** data transport object for stage transition that is an association attribute between <signalprogram, fromStage, toStage>.
  * it contains the signal group switching events and the duration of this sequence.*/
struct CfgStageTransition
{  
  /** number of stage transition, unique inside embedding StageSequencePlan. */
  int stageTransitionNo;
  /** information about how this stage transition was calculated.  */
  bool isCalculatedWithMinGreen;
  /** reference to ending stage; at the view of the stage transmission this is the start-stage.*/
  int fromStageNo;
  /** reference to starting stage; at the view of the stage transmission this is the end-stage.*/
  int toStageNo;
  /** duration of this stage transition in 100ms units. */
  int duration_100ms;
  /** switching times of this stage transition. */
  CfgStageTransitionSwitchingTime switchingTimeList<>;
};
/** DTO used for definition stage oriented signal plan configuration data. */
struct CfgStageSequencePlan2 {
  /** signalprogram number, unique inside a config version of a intersection controller. */
  int splNo;  
  /** cycle time in 100ms units. */
  short tu_100ms;
  /** refernce to synchronisation {@link CfgStageSequenceElement#stageSequenceElementNo stageSequenceElement} */
  int syncStageSequenceElementNo; 
  /** reference to stage sequence element inside {@link #stageSequences} describing an idle stage (i.e. an all red stage); */
  int idleStageSequenceElementNo;
  /** switching/synchronisation point */
  short gsp_100ms;
  /** if true the signal plan is desiged to run synchonized, if false this signalplan will not be run synchronized.<br></br>
   * the c10 synchronizes only if<ul>
   * <li> this flag is true AND</li>
   * <li> there is a valid ReCalculationMethod provided AND</li>
   * <li> the Requested Status SynchronizationState is ON.</li>
   * </ul>
   */
  bool isSynchronized;
  /** variable length list for definition of stage oriented signal program. */
  struct CfgStageSequenceElement2 stageSequences<>;
  /** variable length list for definition of stage oriented signal program's stage transitions. */
  struct CfgStageTransition stageTransitions<>;
  /** offset in time of this signal plan in 100ms units. */
  int offset_100ms;  
  /** maximal stage extension in case of public transport demand, in 100ms units */
  int maxExtensionPT_100ms;
  /** duration of the associated activation routine in 100ms units. */
  int activationRoutineDuration_100ms;
  /** duration of the associated deactivation routine in 100ms units. */
  int deactivationRoutineDuration_100ms;
};

/** DTO that provides extended information about signal programs. 
 * See also {@link CfgStageSequencePlan2 stage oriented spl}, {@link CfgInfoSignalPlanCctrl} 
 * needed to generate VD_SIGNALPLAN needed by PDMx. */
struct CfgSignalplanInfoCCtrl {
  /** signalprogram number, unique inside a config version of a intersection controller. */
  int splNo;  
  /** signal plan (short) name */
  string name<>;
  /** cycle time in 100ms units. */
  short tu_100ms;
  /** switching/synchronisation point */
  short gsp_100ms;
  /** offset in time of this signal plan in 100ms units. */
  short offset_100ms;  
  /** value of synchronous cycle time counter when activation program has to be finished in order to switch on synchronously in 100ms units. 
   * may be -1 in case no activationPointOfTime is given. */
  short activationPointOfTime_100ms;
  /** value of synchronous cycle time counter when deactivation program has to be started in order to switch on synchronously in 100ms units. */
  short deactivationPointOfTime_100ms;
  /** number of intergreentime matrix to use with this signalprogram. */
  int itRef;
  /** reference of signal group specific min green entry. sgNo *100 + listNo */ 
  int minGreenRefs<>;
  /** reference of signal group specific min red entry. sgNo *100 + listNo */ 
  int minRedRefs<>;
  /** reference of signal group specific green to red transition entry. sgNo *100 + listNo */ 
  int transGnRdRefs<>;
  /** reference of signal group specific red to green transition entry. sgNo *100 + listNo */ 
  int transRdGnRefs<>;
  /** switching times of signal group oriented signal plan (not present within stage oriented signal plans).
   * The switching times mark the end times of the given signal indications.
   * if the switching time==0 a continous signalisation is encoded.
   */
  struct CfgStageTransitionSwitchingTime switchingTimeList<>;
};

/* --- sync Info -- */
/** determines the recalculation method, that is the reference point in time for calculation of start of cycle */
enum ReCalculationMethodCctrl {
  /** no recalculation is done */
  RCM_NO = 0,
  /** recalculation reference is 1.1.1980 00:00:00 */
  RCM_UTC = 1,
  /** recalculation reference is 1.1. of current year */ 
  RCM_1_1 = 2,
  /** recalculation reference is 1.1.1980 00:00:00 */
  RCM_1980 = 3,
  /** recalculation reference is 00:00:00 of current day */
  RCM_0_0 = 4
};
/** defined signal plan synchronization methods */
enum SyncMethodCctrl {
  /** Synchronisierung wie C900 nach Signalplanwechsel am  GSP zu Beginn des 2. Umlaufs */
  SM_GSP_SIEMENS = 0,
  /** Synchronisierung am 1. GSP nach Signalplanwechsel */
  SM_GSP_RILSA = 4,
  /** Synchronisierung Stretch */
  SM_STRETCH = 3
};
/** DTO to get info about synchronization */
struct CfgSyncInfoCctrl {
  /** define recalculation point in time */
  ReCalculationMethodCctrl reCalculationMethod;
  /** used signal plan synchronization method */
  SyncMethodCctrl syncMethod;
  /** allowed number of cycles till controller is synchronous. */
  int syncCyclesAllowed;
  /** time offset for the whole intersection in 100ms units */
  int globalRefTimeShift_100ms;
};
/* --- /sync Info -- */

/* --- Intergreentime Matrix --- */
/** DTO to get intergreentime matrix entries. */
struct CfgInterGreenTimeElementCctrl {
  /** signal group number of entering traffic (Einfahrsignalgruppen-Nummer <256) */
  int enteringSgNo;
  /** signal group number of clearing traffic (Raeumsignalgruppen-Nummer <256) */
  int clearingSgNo;
  /** intergreentime value in 100ms units, >0. */
  int value;  
};
/** DTO to get a single intergreentime table. */
struct CfgInterGreenTimeTableCctrl {
  /** unique number of this intergreeentime table. */
  int igTableNo;
  /** list of (all) elements defining this intergreentime table. 
   * the SG tuples that are not reported here do not have an intergreentime relation. */
  CfgInterGreenTimeElementCctrl elements<>;
};
/* --- /Intergreentime Matrix --- */

/** definition of selectable config queries and ret code of 
 * {@link CControl_CcontrolService_CltIF#getSelectedCfg_8} and {@link CControl_CcontrolService_CltIF#getDraftSelectedCfg_8(String, int, CfgSelector)} . */
enum CfgSelector {
  /** there was an error retrieving the config, no config returned. */
  CS_CfgError=0,
  /** select {@link CfgSgInfo} */
  CS_CfgSgInfo=1, 
  /** select {@link CfgSgDetParam} */
  CS_CfgSgDetParam=2, 
  /** select {@link CfgStage} */
  CS_CfgStages=3, 
  /** select configuration parameter system_pule_ms, that is 1000 for each second. */
  CS_CfgSystem_pulse_ms=5,
  /** select configuration parameter influencing {@link CfgSyncInfoCctrl synchronization} */
  CS_CfgSyncInfoCctrl=6,
  /** select extended  {@link CfgStageSequencePlan2}
   * @since SX V2
   */
  CS_CfgStageSequencePlan2=7,
  /** select all intergreentime tables. */
  CS_CfgInterGreenTimeTables=8,
  /** select all {@link CfgSgInfo2CCtrl extended signal group infos}. */
  CS_CfgSgInfo2CCtrl=9,
  /** select all {@link CfgSignalplanInfoCCtrl signal program infos}. */
  CS_CfgSignalplanInfoCCtrl=10,
  /** select all {@link CfgDetectorParamCctrl detector infos}. */ 
  CS_CfgDetectorParamCctrl=11,
  /** select general controller informations. */
  CS_CfgGeneralParamCctrl=12
  };


/* --- config structures - assorted configinfo objects --*/	
/* --- definitions config structures ---*/

/** DTO used at {@link CControl_CcontrolService_CltIF#getSelectedCfgInfo_8}.*/	
struct CfgInfoBaseCctrl {
  /** number of element, its semantic depends on embedding structure. */
	int no;
	/** shortname of element, its semantic depends on embedding structure. */
	string shortname<>;
  /** name of element, its semantic depends on embedding structure. */
	string name<>;
};



/** DTO providing attributes of the configured control center connection. */
struct CfgInfoConnection {
  /** i.e. CANTO_1_3_66  */
	string communicationType<>;  
	/** i.e. System_CM_3   */
	string controlPrinciple<>;
};

/** DTO to provide traffic related configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfgInfo_8}. */
struct CfgInfoDetectorCctrl { 
  /** element signalhead no shortname name of signalhead select {@link CfgInfoBaseCctrl} */
  CfgInfoBaseCctrl cfgInfoBaseDet;
   /** channelNo unique  unique inside a config version of a intersection controller. */
  int channelNo;
  /** detType enum of c10model */
  string modelType<>;
};

/** DTO to provide Detector related configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfg_8}. */
struct CfgDetectorParamCctrl{
    /** base Detector informations */
    CfgInfoDetectorCctrl base;
    /** filter parameter may be used to calculate BA value, while rising.
     * possible values 1, 0.5, 0.25, 0.125
     */
    float SmothingFactorUp;
    /** filter parameter may be used to calculate BA value, while falling.
     * possible values 1, 0.5, 0.25, 0.125
     */
    float SmothingFactorDown;
    /** interval in seconds for creation of aggregated values, 0=cycle specific aggregation. */ 
    int Mexwainterval;
};

/** currently known intersection conmtroller types */
enum CfgIntersectionControllerTypeCctrl {
  /** controller type not known, abnormal situation */
  ICT_UNKNOWN=0,
  /**
   * low voltage (40V) controller with a maximum of 32 signal groups
   * 
   */
  ICT_C_10_L_61=61,
  /**
   * high voltage (230V) controller with a maximum of 32 signal groups
   * 
   */
  ICT_C_10_H_62=62,
  /**
   * low voltage (40V) controller with more than 32 signal groups
   * 
   */
  ICT_C_10_LX_63=63,
  /**
   * high voltage (230V) controller with more than 32 signal groups
   * 
   */
  ICT_C_10_HX_64=64,
  /**
   * very low voltage (24V) controller with a maximum of 32 signal groups
   * 
   */
  ICT_C_10_V_65=65,
  /**
   * very low voltage (24V) controller with more than 32 signal groups
   * 
   */
  ICT_C_10_VX_66=66
};
/** DTO to provide general configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfg_8}. */
struct CfgGeneralParamCctrl {
    enum CfgIntersectionControllerTypeCctrl controllerType;
    /** base configuration: ShortName */
    string      name<32>;
    /** trafic control center number. */
    int         tccNo;
    /** intersection controler number, unique for this tcc. */
    int         guiNo;
    /** field device number (in case of OCIT, CANTO) -1 if not defined */
    int         fnr;
    /** interval between successive TA invocations in ms valid range: 100..1000 */
    int         systemPulse_ms;
    /** control center connection type, empty string in case there is no control center connection. */
    string      centerType<16>;
};

/** DTO that transports configuration info that may be requested */
union SelectedConfig switch ( enum CfgSelector selector )  {
        case CS_CfgSgInfo:
                CfgSgInfo       sgInfos<>;
        case CS_CfgSgDetParam:
                CfgSgDetParam sgDetParams<>;
        case CS_CfgStages:
                CfgStage        stages<>;
        case CS_CfgStageSequencePlan2:
          CfgStageSequencePlan2 stageSequencePlans2<>;
        case CS_CfgSystem_pulse_ms:
          int systemPulseMs;
        case CS_CfgSyncInfoCctrl:
          CfgSyncInfoCctrl syncInfo;
        case CS_CfgInterGreenTimeTables:
          CfgInterGreenTimeTableCctrl igTables<>;
        case CS_CfgSgInfo2CCtrl:
                CfgSgInfo2CCtrl sgInfos2<>;
        case CS_CfgSignalplanInfoCCtrl:
          CfgSignalplanInfoCCtrl signalplanInfos<>;
        case CS_CfgDetectorParamCctrl:
          CfgDetectorParamCctrl detectorParams<>;
        case CS_CfgGeneralParamCctrl:
          CfgGeneralParamCctrl generalParams;
        case CS_CfgError:
                enum RetCodeCctrl ret;
        default:
                enum RetCodeCctrl ret1;
};

/** DTO to provide traffic related configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfgInfo_8}. */
struct SgHeadColorCctrl {	
  /**  i.e. red,amber,green  */
  string color<> ;
};

/** DTO to provide traffic related configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfgInfo_8}. */
struct CfgInfoSignalHeadCctrl {
  /** element signalhead no shortname name of signalhead select {@link CfgInfoBaseCctrl} */
  CfgInfoBaseCctrl cfgInfoBaseSignalHead;
  /** chamber colors av. in signalhead   select {@link SgHeadColorCctrl}  */
  SgHeadColorCctrl chamberColors<> ;
};

/** DTO to provide traffic related configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfgInfo_8}. */
struct CfgInfoSignalGroupCctrl {
  /** element signalgroup no shortname name  select {@link CfgInfoBaseCctrl} */ 
  CfgInfoBaseCctrl cfgInfoBaseSignalGroup;
  /** sgType enum of c10model */
  string sgType<>;
  /** sgDetailType enum of c10model */
  string sgDetailType<>;
  /**signalhead(s) represnting signalgroup  select {@link CfgInfoSignalHeadCctrl} */
  CfgInfoSignalHeadCctrl cfgInfoSignalHead<>;
};

/** DTO to provide traffic related configuration info at {@link CControl_CcontrolService_CltIF#getSelectedCfgInfo_8}. */
struct CfgInfoSignalPlanCctrl {
  /**  select {@link CfgInfoBaseCctrl} element signalplan */ 
  CfgInfoBaseCctrl cfgInfoBaseSignalPlan;
  /** tu   time in 100ms */
  int tu;
  /** uzp  time in 100ms */
  int gsp;
  /** cycle time monitor threshold in 100ms. (Grenzwert der Umlaufzeitkontrolle)  
   *  0 is cycle monitor is disabled. */
  int cycleTimeMonitorThreshold_100ms; 
  /** if true the signal plan is desiged to run synchonized, if false this signalplan will not be run synchronized.<br></br>
   * the c10 synchronizes only if<ul>
   * <li> this flag is true AND</li>
   * <li> there is a valid ReCalculationMethod provided AND</li>
   * <li> the Requested Status SynchronizationState is ON.</li>
   * </ul>
   */
  bool isSynchronized;
};



/** definition of selectable config queries and ret code of 
 * {@link CControl_CcontrolService_CltIF#getSelectedCfgInfo_8}. */
enum CfgInfoSelector { 
  /** there was an error retrieving the config, no config returned. */
  CS_CfgInfoError=0, 
  /** select {@link SelectedCfgInfo#infoIntersection} {@link CfgInfoBaseCctrl} */
  CS_CfgInfoIntersection=1, 
  /** select {@link SelectedCfgInfo#partialIntersections} {@link CfgInfoBaseCctrl} */
  CS_CfgInfoPartialIntersections=2,  
  /** select {@link SelectedCfgInfo#signalingGroups} {@link CfgInfoBaseCctrl} */
  CS_CfgInfoSignalingGroups=3, 
  /** select {@link SelectedCfgInfo#signalPlans} {@link CfgInfoBaseCctrl} */
  CS_CfgInfoSignalPlans=4, 
  /** select {@link SelectedCfgInfo#cfgDetectors} {@link CfgInfoBaseCctrl} */
  CS_CfgInfoDetectors=5, 
  /** select {@link SelectedCfgInfo#cfgSubs} {@link CfgInfoBaseCctrl} */
  CS_CfgInfoSubsystem=6, 
  /** select {@link SelectedCfgInfo#cfgCon} {@link CfgInfoConnection} */
  CS_CfgInfoConnection=7,
  /** select {@link SelectedCfgInfo#cfgSignalGroups} {@link CfgInfoSignalGroupCctrl} */
  CS_CfgInfoSGCctrl=8,
  /** select {@link SelectedCfgInfo#cfgSignalPlans} {@link CfgInfoSignalPlanCctrl} */
  CS_CfgInfoSPCctrl=9, 
  /** select {@link SelectedCfgInfo#cfgDets} {@link CfgInfoDetectorCctrl} */
  CS_CfgInfoDetCctrl=10
  };


/* --- Union SelectedConfigInfo  ---*/	
/* @param selector CfgInfoSelector assorted configinfo datasubsets from Inventory! */
/*  --- */
/** union DTO that transports configuration info that has be requested.
 * there is at most one field dependant of {@link  SelectedCfgInfo#selector} set. */
union SelectedCfgInfo switch ( enum CfgInfoSelector selector )  {
	case CS_CfgInfoIntersection:
		CfgInfoBaseCctrl infoIntersection;
	case CS_CfgInfoPartialIntersections:
		CfgInfoBaseCctrl partialIntersections<>;
	case CS_CfgInfoSignalingGroups:
		CfgInfoBaseCctrl	signalingGroups<>;
	case CS_CfgInfoSignalPlans:
		CfgInfoBaseCctrl signalPlans<>;
	case CS_CfgInfoDetectors:
	  CfgInfoBaseCctrl cfgDetectors<>;
	case CS_CfgInfoSubsystem:
	  CfgInfoBaseCctrl cfgSubs;
  case CS_CfgInfoConnection:
		CfgInfoConnection cfgCon;
	case CS_CfgInfoSGCctrl:
		CfgInfoSignalGroupCctrl	cfgSignalGroups<>;
	case CS_CfgInfoSPCctrl:
		CfgInfoSignalPlanCctrl cfgSignalPlans<>;
	case CS_CfgInfoDetCctrl:
		CfgInfoDetectorCctrl cfgDets<>;
  case CS_CfgInfoError:
		enum RetCodeCctrl ret;
	default:
		enum RetCodeCctrl ret1;
};

/* --- End definitions config structures ---*/	

/* --- DeviceVariable related DTO --- */
/** oitd index DTO, used as key for subscription and delivery of device variables at C-Control interface. */
struct OitdCctrl {
  /** OCIT Member number. */
  int member;
  /** OCIT object type number */
  int otype;
  /** index of this key */
  int index;
};
typedef struct OitdCctrl OitdCctrlA<>;


/** key-value structure used to deliver DeviceVariables. */
struct DeviceValueCctrl {
  /** key of delivered device variable value. */
  OitdCctrl oitd;
  /** tick time of the device value */
  hyper tickTime;
  /** delivered device variable value. */
  int value;
};

/** device value delivery DTO. */
struct DeviceValuesCctrl {
  /** dev ticks in ms of the following changed device values. */
  hyper timestamp; 
  /** (changed) device values. */ 
  DeviceValueCctrl dv<>;
};

/* --- End DeviceVariable related DTO --- */
/* --- device variable meta data --*/

/** dto for transport of natural language string mappings. */
struct NlsElementCctrl {
  /** key of nls text, shll be unique inside a language mapping. 
   * keys may be built from format strings that appear at status {@link InitiatorCCtrl#format messages} or device variable meta definitions. */
  string key<>;
  /** nls text repolacement for key. */
  string value<>;
};
/** nls mapping for a single language. 
 * used for device variables and status messages */
struct NlsMappingCctrl {
  /** lang 2 char language code e.g. en for english, de for german... */
  string lang<2>;
  /** mappings for that language */
  NlsElementCctrl map<>;
};
typedef struct NlsMappingCctrl NlsMappingsCctrl<>;

/** classifies the index of the device variable. 
 * used to associate the device value with a domain object.
 * that is the number of instances is equal to the number of refered elements. */
enum DeviceValueIndexType {
  /** a device value per field device, that is only one instance per field device, index=0 */
  DVI_Fd=1,
  /** a device value per detector. */
  DVI_Detector=2,
  /** a device value per signal group. */
  DVI_SG=3,
  /** a device value per signal program. */
  DVI_SignalProgram=4,
  /** a device value per stage (Phase). */
  DVI_Stage=5,
  /** a device value per partial intersection (Teilknoten). */
  DVI_PartialIntersection=6,
  /** a device value per non signal group output. */
  DVI_Output=7,
  /** public transport control point (Meldpunkt) */
  DVI_PublicTransportCP=10,
  /** public transport group */
  DVI_PublicTransportGroup=11,
  /** public transport signal group */
  DVI_PublicTransportSgGroup=12,
  /** unknown other index type */
  DVI_other=99
};

/** DTO used to transport device value meta/type informations from c-control client into firmware. */
struct DeviceValueTypeCctrl {
  /** will be replaced by matching NlsElementCctrl.value where NlsElementCctrl.key==name.
   * if no nls key is found, name is used as is. */ 
  string name<>;
  /** will be replaced by matching NlsElementCctrl.value where NlsElementCctrl.key==longName. */ 
  string longName<>;
  /** will be replaced by matching NlsElementCctrl.value where NlsElementCctrl.key==description. */ 
  string description<>;
  /** OCIT member that defined this device value type */
  int member;
  /** OCIT type number of this device value definition */
  int otype;
  /** association of this device value to domain object instance */ 
  enum DeviceValueIndexType dvit;
  /** lower bound of index, only relevant if dvit is DVI_other or unknown */
  int minIndex;
  /** upper bound of index, only relevant if dvit is DVI_other or unknown */
  int maxIndex;
  /** minimal value */
  int minValue;
  /** maximal value */
  int maxValue;
};

/** dto used to return data at getDeviceValuesUsedIndex */
typedef int DeviceValuesUsedIndicesCctrl <>;

/** DTO to transport device value related type information from c-control client to firmware. */
struct DeviceValuesMetaDataCctrl {
  /** distinction between control centerinterface or traffic actuation */
  enum UnitId providerType;
  /** name of traffic actuation or control center inteface implementation,
   * SHALL correspopnd to dvpName passed to 
   * {@link CControl_CcontrolService_CltIF#registerDeviceVariableProvider_8(String inetadr, int port, int tcpPort, String dvpName, String dvpVersion)} */
  string name<>;
  /** version of traffic actuation or control center inteface implementation 
   * SHALL correspopnd to dvpVersion passed to 
   * {@link CControl_CcontrolService_CltIF#registerDeviceVariableProvider_8(String inetadr, int port, int tcpPort, String dvpName, String dvpVersion)} */
  string vers<>;
  /** meta definitions per device value type. */
  DeviceValueTypeCctrl dvt<>;
};
typedef struct DeviceValuesMetaDataCctrl DeviceValuesMetaDataCctrlA<>;

/* --- CantoNetConfig --- */

/** Canto network configuration parameter from hlc_system.xml. */
struct CantoNetConfigDtoCctrl {
	/**
	 * Describes the type of communication profile of the controllers central connection:
	 * UNDEFINED, CANTO_V34, CANTO_E, CANTO_E_P, CANTO_I_D, CANTO_I_G, CANTO_I_U, or CANTO_I_W.
	 */
	string profileType<>;
	/**
	 * Optional detailled description of controller's communication profile. E.g. Name of service provider.
	 */
	string profileDetails<>;
	/**
	 * Nominal up stream bit rate in kbit/s (controller to control center).
	 */
	int upstreamSpeed;
	/**
	 * Nominal down stream bit rate in kbit/s (control center to controller).
	 */
	int downstreamSpeed;
	/**
	 * Control center's external ip address of the VPN tunnel as it is configured at the controller.
	 */
	string vpnGateway<>;
	/**
	 * Control center's port number of the VPN tunnel. 
	 */
	int vpnPortNumber;
	/**
	 * VPN network protocol: UPD or UDP6. 
	 */
	string vpnProtocol<>;
};

/**
 * Union DTO to transfer depending on the {@link RetCodeCctrl} the Canto network configuration parameter of type {@link CantoNetConfigDtoCctrl}.
 * <p>
 * Used by {@link CControl_CcontrolService_CltIF#getCantoNetConfig_8}. If <code>error</code> is not {@link RetCodeCctrl#RC_OK},
 * no {@link CantoNetConfigDtoCctrl} instance is transfered! 
 * </p>
 */
union CantoNetConfigCctrl switch (enum RetCodeCctrl retCode) {
  case RC_OK:
    CantoNetConfigDtoCctrl cantoNetConfig;
  default:
    void;
};
/* --- End CantoNetConfig --- */

/* --- Version information of C10 components --- */

/** Version information about MkjtricActuationControlDeployments. */
struct MkjtricActuationVersionCctrl {
	/** Internal(sitraffic) tac deployment name. */
	string taName<>;
	/** Version and release of traffic actuation process. The format of the version string depends on the type of the tac deployment name. */
	string taVersion<>;
};

/** Complete version information of all C10 components. */
struct VersionsDtoCctrl {
	/** Intersection controller type */
	string	type<>;
	/** optinal name of customer */
	string customerName<>;
	/** Version number of configuration. */
	int		configVersion;
	/** Version number of planning. */
	int		planningVersion;	
	/** Build number of configuration */
	int		configBuildNo;
	/** Version and release of firmware like x.y.z.t-release with integer values for x, y, z, t, and release */
	string	fwVersion<>;
	/** Version and Release of CBC. Format of string according to <code>fwVersion</code>.*/ 
	string	cbcVersion<>;
	/** Version and Release of CHX. Format of string according to <code>fwVersion</code>.*/ 
	string	chxVersion<>;
	/** Version and Release of CMU. Format of string according to <code>fwVersion</code>.*/ 
	string	cmuVersion<>;
	/** List of MkjtricActuationControlDeployments including their versions.*/
	MkjtricActuationVersionCctrl	taVersions<>;
};

/**
 * Union DTO to transfer depending on the {@link RetCodeCctrl} the complete {@link VersionsDtoCctrl version information} of all C10 components.
 * <p>
 * Used by {@link CControl_CcontrolService_CltIF#getVersions_8}. If <code>error</code> is not {@link RetCodeCctrl#RC_OK},
 * no {@link VersionsDtoCctrl} instance is transfered! 
 * </p>
 */
union VersionsCctrl switch (enum RetCodeCctrl retCode) {
  case RC_OK:
	  VersionsDtoCctrl dto;
  default:
    void;
};


/* --- End version information of C10 components --- */

/**
 * parameter of {@link CControl_CcontrolService_CltIF#doControlCenterIf_8(ProcessControl)}. 
 */
enum ProcessControl {
  /** stop control center process(es) if any. */
  PC_start=0,
  /** start control center process(es) if any. */
  PC_stop=1,
  /** restart control center process(es) if any. */
  PC_restart=2
};

/* --- Begin definitions for ArchiveAccess --- */

/**
 * Definition of selectible archive instances and RPC results for {@link CControl_CcontrolService_CltIF#archiveGetLaterThan_8}, 
 * {@link CControl_CcontrolService_CltIF#archiveGetYoungest_8}, or {@link CControl_CcontrolService_CltIF#archiveGetOldest_8}.
 */
enum ArchiveSelectorCctrl {
  /** the archive instance does not exist, or is not acessible by Cctrl. */
  AS_NoArchive=0,
  /** select {@link ActualStatusDtoCctrl ActualStatus} archive. */
  AS_ActualStatus=1,
  /** select {@link StandardMessageDtoCctrl StandardMessage} archive. */
  AS_StandardMessage=2,
  /** select {@link CommonStatesDtoCctrl CommonStates} archive. */
  AS_CommonStates=11,
  /** select {@link LampStatesDtoCctrl LampStates} archive. */
  AS_LampStates=12,
  /** select {@link DetectorStatesDtoCctrl DetectorStates} archive. */
  AS_DetectorStates=13,
  /** select {@link SignalIndicationsDtoCctrl SignalIndications} archive. */
  AS_SignalIndications=21,
  /** select {@link DetectorValuesDtoCctrl DetectorValues} archive. */
  AS_DetectorValues=22,
  /** select {@link DeviceValuesDtoCctrl DeviceValues} archive. */
  AS_DeviceValues=23,
  /** select {@link DetectorAggregationDtoCctrl DetectorAggregation} archive. */
  AS_DetectorAggregation=24,
  /** select {@link AmliDtoCctrl Amli} archive. */
  AS_Amli=25
  };

/**
 *  archive key values of a single archive DTO. 
 */
struct ArchiveKeysCctrl {
	/**  time difference measured in milliseconds, elapsed since midnight, January 1, 1970 UTC. */
	hyper timeStamp;
	/** CBC Device tick time in ms, since last start of CBC when one of these common states has changed. */
	hyper tickTimeOfLastChange;
	/** Archive primary key value */
	hyper primaryKey;
};

/** 
 * A data transfer object returned by archive requests. It includes the archive DTO keys and a {@linkplain ActualStatusCctrl actualStatus} instance.
 * It is required by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct ActualStatusDtoCctrl
{
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
	/** actual status at this time */ 
 	ActualStatusCctrl status;
};

/** 
 * A data transfer object returned by archive requests. It includes the archive DTO keys and describes a 
 * standard message acording to the Canto 1.3 standard as well as a OCIT-O standard message. It is required 
 * by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct StandardMessageDtoCctrl
{
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
	/** 
	 * transaction identifier of source that initiated this message.
     * see <a href="doc-files/TransactionId.htm">TransactionId doc</a>
	 */
	int transactionNo;
	/** licensee identifier */
	int member;
	/** Identification and <code>licensee identifier</code> are defined as unique objecttype (Domain) */
	int otype;
	/** Severity of message: 0: info; 1: warning; 2: error; 3: fatal */
	int severity;
	/** Symbolic name of message for debugging or logging purpose */
	string name<>;
	/** List of message parameters. */
	MsgParameterCctrl parameters<>;
};

/** 
 * A data transfer object returned by archive requests. It includes the archive DTO keys and a set
 * of Canto 1.3 {@linkplain DeviceValueCctrl device variables} also known as C10 device values.
 * It is required by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct DeviceValuesDtoCctrl {
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
    /** (changed) device values. */ 
	DeviceValueCctrl dv<>;
};

/** 
 * A data transfer object returned by archive requests. It includes the archive DTO keys and a {@linkplain SignalIndicationsCctrl signalIndications}
 * instance. It is required by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct SignalIndicationsDtoCctrl
{
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
	/** actual status at this time */ 
 	struct SignalIndicationsCctrl signalIndications;
};

/** 
 * A data transfer object returned by archive requests. It includes the archive DTO keys and a set
 * {@linkplain DetRawValuesGradedCctrl raw values} instances for all configured detectors.
 * It is required by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct DetectorValuesDtoCctrl
{
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
	/** Array of all known detector values */
	DetRawValuesGradedCctrl detectorValues<>;
};

/** 
 * A data transfer object returned by archive requests. It includes the archive DTO keys and 
 * {@linkplain DetectorAggregationCctrl aggregated detector values} for all summable
 * detectors. It is required by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct DetectorAggregationDtoCctrl
{
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
	/** length of aggregation interval */
	int cycleLength;
	/** <code>true</code> in case of cyclic aggregation with fixed interval; <code>false</code> in case of signalling cycle based aggregation. */
	bool isFixedCycle;
	/** Array of aggregated detector values. */
	DetectorAggregationCctrl aggregationEntries<>;
};

/** 
 * A data transfer object returned by archive requests. It includes the archive DTO keys and a set of Canto 1.3
 * {@linkplain LampStatusCctrl  lamp fault} device variables for all configured signal groups.
 * It is required by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct LampStatesDtoCctrl
{
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
	/** Array of lamp states for each signal group. */
	LampStatusCctrl lampStates<>;
};

/**
 * A data transfer object returned by archive requests. It includes the archive DTO keys and a set of Canto 1.3
 * {@linkplain DetStatusCctrl  detector fault} device variables for all configured detectors.
 * It is required by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct DetectorStatesDtoCctrl
{
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
	/** Array of all known detector states */
	DetStatusCctrl detectorStates<>;
};

/** 
 * A data transfer object returned by archive requests. It includes the archive DTO keys and a set of Canto 1.3
 * {@linkplain CommonStatusCctrl  common status} device variables.
 * It is required by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct CommonStatesDtoCctrl
{
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
	/** Array of all known common states */
	CommonStatusCctrl commonStates<>;
};

/** Selector for optional R09 attribute values */
enum R09AttrSelCctrl {
	/** Imbalance in seconds >0 too late; <0 too early. */
	R09_Imbalance = 0,
	/** Priority: 0=none, 1=normal, 2=high, 3=highest */
	R09_Priority = 1,
	/** Requested direction; 0=default */
	R09_ManDir = 2,
	/** Length of vehicle */
	R09_VehicleLen = 3,
	/** Number (identifier) of vehilce */
	R09_VehicleNo = 4,
	/** Number (identifier) of company running public transport vehicle */
	R09_CompanyNo = 5,
	/** Number (identifier) of public transport detector */
	R09_PtDetNo = 6,
	/** signal group number influenced by this R09 event */
	R09_SignalGroup = 7,
	/** public transport direction number, sent with intersectionRide. */
	R09_PtDirectionNo = 8,
	/** unknown attribute for fault handling only */
	R09_Unknown = 255
};

/** Optional R09 attribute entry. */
struct R09AttributeCctrl {
	/** key of optional R09 attribute */
	R09AttrSelCctrl selector;
	/** value of optional R09 attribute */
	int value;
};

/** Source of ride event */
enum RideEventSourceCctrl {
	/** Local PT telegram receiver board AFD */
	RESC_AFD = 0,
	/** Local detector for emergency checkin */
	RESC_Det = 1,
	/** Intermodal transport control system (formerly RBL) or AVLS (Automatic Vehicle Location System); request received via control center. */
	RESC_ITCS = 2,
	/** Forced check-out event by ride manager in case of a missing check-out event. */
	RESC_RideManager = 3,
	/** Event create by GUI for simulation. */
	RESC_SimGui = 4,
	/** Event created by traffic actuated control process. */
	RESC_Ta=5,
	/** None of the above defined values. */
	RESC_unknown = 255
};

/** General public transport ride event. */
struct RideEventCctrl {
	/** Source of public transport ride event */
	RideEventSourceCctrl source;
	/** Reporting point number */
	int rpNo;
	/** Number of public transport line */
	int lineNo;
	/** Number of vehicle serving this line */
	int runNo;
	/** Number of destination of this vehicle.*/
	int destinationNo;
	/** Set of additional R09 attributes */
	R09AttributeCctrl attrs<>;
};

/** Type of AMLI event */
enum AmliEventTypeCctrl {
	/** Normal check-in event */
	AET_CheckIn = 0,
	/** Check out event without AMLI green-red-state state transition. */
	AET_CheckOutRaw = 1,
	/** Check out event including AMLI green-red-state state transition. */
	AET_CheckOutAmli = 2,
	/** Emergency check-in event.*/
	AET_EmergencyCheckIn = 3,
	/** Forced check-out event.*/
	AET_ForcedCheckOut = 4,
	/**
	 * Public transport request was received by traffic actuation but blocked by a high-priority request. Time stamp of
	 * event is the time the TA received the high-priority request.
	 */
	AET_Blocked = 5,
	/** None of the above defined values.*/
	AET_unknown = 15
};

/** Specifies kind of AMLI check-out event */
enum AmliCheckOutTypeCctrl {
	/**
	 * Event is a check in event.<br>
	 * red-green-transition=0; green-red-transition=0
	 */
	ACOR_CheckIn = 0,
	/**
	 * Public transport signal group at red state before first check-in event and green-red-transition after check-out event.<br>
	 * red-green-transition=[1..253]; green-red-transition=[1..253]
	 */
	ACOR_OK = 1,
	/**
	 * Check out event received after green-red-transition and configured red time of public transport signal groups.<br>
	 * red-green-transition=0; green-red-transition=0
	 */
	ACOR_TooLate = 2,
	/**
	 * Check out event while intersection was switched off.<br>
	 * red-green-transition=0; green-red-transition=255
	 */
	ACOR_Off = 3,
	/**
	 * Check out event received during activation routine with public transport signal group at green-state and before
	 * green-red-transition.<br>
	 * red-green-transition=255; green-red-transition=[1..253]
	 */
	ACOR_SwitchOnOk = 4,
	/**
	 * Check out event received during activation routine does not affect change of public transport signal group, i.e no
	 * green-red-transition 15s after this event.<br>
	 * red-green-transition=255; green-red-transition=254
	 */
	ACOR_SwitchOnNA = 5,
	/**
	 * Public transport signal group at green state before first check-in event and at least 15s after check-out event.<br>
	 * red-green-transition=254; green-red-transition=254
	 */
	ACOR_NotAffected = 6,
	/**
	 * Public transport signal group at green state before first check-in event and green-red-transition after check-out event.<br>
	 * red-green-transition=254; green-red-transition=[1..253]
	 */
	ACOR_StageExtended = 7,
	/**
	 * Public transport signal group at green state before first check-in event and after check-out event of next ride received.<br>
	 * red-green-transition=254; green-red-transition=0
	 */
	ACOR_CheckOutByNextNA = 8,
	/**
	 * Public transport signal group at red state before first check-in event and at green state least 15s after check-out event.<br>
	 * red-green-transition=[1..253]; green-red-transition=254
	 */
	ACOR_StageUnmodified = 9,
	/**
	 * Public transport signal group at red state before first check-in event and check-out event of next ride received.<br>
	 * red-green-transition=[1..253]; green-red-transition=0
	 */
	ACOR_CheckOutByNext = 10,
	/** None of the above defined values. */
	ACOR_unknown = 255
};

/**
 * A data transfer object returned by archive requests. It includes the archive DTO keys and the AMLI public transport events.
 * It is required by {@linkplain CControl_CcontrolService_CltIF#archiveGetLaterThan_8},
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetOldest_8}, or 
 * {@linkplain CControl_CcontrolService_CltIF#archiveGetYoungest_8}.
 */
struct AmliDtoCctrl {
	/**  archive key values: time stamp, CBC device tick, and primary key */
	ArchiveKeysCctrl keys;
	/** Public transport event */
	RideEventCctrl event;
	/** At check-in: Time difference in milliseconds between required green state and check-in event. */
	int greenTime;
	/** Flag indicating the public transport modification of the traffic actaution is runnig, or not. */
	bool ptModActive;
	/** Cycle time the ride event was received. */
	int tx;
	/** Number of running signal program */
	int spNo;
	/** actual stage */
	int actStage;
	/** requested stage */
	int nextStage;
	/** 
	 * At check-in event: estimated traveltime in milliseconds from check-in to check-out reporting point number.
	 * At check-out event: time diffrence between check-out event and last check-in events in milliseconds.
	 */
	int travelTime;
	/** Cycle time of begin of green state. */
	int txGNS;
	/** Cycle time of end of green state.*/
	int txGNE;
	/** At AMLI check-out: Time difference in milliseconds between end of green state and check-out event. */
	int amliTimeout;
	/** Number of configured public transport direction applied by this event. */
	int ptDirNo;
	/** Type of AMLI event */
	AmliEventTypeCctrl type;
	/** Sub type of AMLI check-out event */
	AmliCheckOutTypeCctrl coType;
};

/** 
 * Union DTO needed to transfer a single instance of an archive request result. The enumeration value {@link ArchiveSelectorCctrl} 
 * selects between the different types of archived data.
 */
union ArchiveDtoCctrl switch ( enum ArchiveSelectorCctrl selector ) {
	case AS_ActualStatus:
	ActualStatusDtoCctrl asDto;
	case AS_StandardMessage:
	StandardMessageDtoCctrl smDto;
	case AS_CommonStates:
	CommonStatesDtoCctrl csDto;
	case AS_LampStates:
	LampStatesDtoCctrl lsDto;
	case AS_DetectorStates:
	DetectorStatesDtoCctrl dsDto;
	case AS_DetectorValues:
	DetectorValuesDtoCctrl dvDto;
	case AS_SignalIndications:
	SignalIndicationsDtoCctrl siDto;
	case AS_DeviceValues:
	DeviceValuesDtoCctrl devVDto;
	case AS_DetectorAggregation:
	DetectorAggregationDtoCctrl daDto;
	case AS_Amli:
	AmliDtoCctrl amDto;
	default:
	void;
};

/**
 * Union DTO to transfer depending on the {@link RetCodeCctrl} a single archive requests result of type {@link ArchiveDtoCctrl}.
 * <p>
 * Used by {@link CControl_CcontrolService_CltIF#archiveGetYoungest_8}, or {@link CControl_CcontrolService_CltIF#archiveGetOldest_8}
 * to return a single archived instance. If <code>error</code> is not {@link RetCodeCctrl#RC_OK}, no {@link ArchiveDtoCctrl} instance
 * is transfered! 
 * </p>
 */
union ArchiveResultCctrl switch (enum RetCodeCctrl retCode) {
	case RC_OK:
		ArchiveDtoCctrl dto;
	default:
		void;
	};
	

/**
 * Union DTO to transfer depending on the {@link RetCodeCctrl} multiple archive request results of type {@link ArchiveDtoCctrl}.
 * <p>
 * Used by {@link CControl_CcontrolService_CltIF#archiveGetLaterThan_8} to return a variable length array of {@link ArchiveDtoCctrl} 
 * objects. A variable length array of size 0 is valid result, too. 
 * If <code>error</code> is not {@link RetCodeCctrl#RC_OK}, no {@link ArchiveDtoCctrl} variable lenth array
 * is transfered! 
 * </p>
 */
union ArchiveResultArrayCctrl switch (enum RetCodeCctrl retCode) {
	case RC_OK:
		ArchiveDtoCctrl dtos<>;
	default:
		void;
	};

/**
 * DTO for {@link TdtSubscriptionInfoCctrl} needed by {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl)}.
 */
struct ArchiveSubscribeInfoCctrl
{
  /** selector enumeration value of type {@link ArchiveSelectorCctrl} to select the archive for insert events. Insert events are NOT supported for
   * archives with frequent inserts to avoid overload by archive requests:
   * <ul>
   * <li>{@link ArchiveSelectorCctrl#AS_SignalIndications SignalIndications}</li>
   * <li>{@link ArchiveSelectorCctrl#AS_DetectorValues DetectorValues}</li>
   * <li>{@link ArchiveSelectorCctrl#AS_DeviceValues DeviceValues}</li>
   * </ul>
   */
  enum ArchiveSelectorCctrl selector;
  /** number of archive inserts to wait before sending an event; 0 to disable archive events. Count has a different meaning when subscribing insert events for archives:
   * <ul>
   * <li>{@link ArchiveSelectorCctrl#AS_DetectorAggregation DetectorAggregation}: Bit0=0: mask signalingCycle values; Bit1=0: mask fixed cycle values; Bit31-2: number of unmasked archive inserts to wait.</li>
   * <li>{@link ArchiveSelectorCctrl#AS_Amli AMLi}: Bit0=0: mask extended R09 values; Bit1=0: mask raw R09 values; Bit31-2: number of unmasked archive inserts to wait.</li>
   * </ul>
   */  
  int count;  
};
/**
 * DTO needed by {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl)} to pass additional subscription information.
 * currently those are only in case of archive info events necessary {@link MkjtricDataTypeCctrl#TDT_ARCHIVEEVENTS}.
 */
union TdtSubscriptionInfoCctrl switch ( MkjtricDataTypeCctrl tdt )
{
  case TDT_ARCHIVEEVENTS:
    ArchiveSubscribeInfoCctrl archiveInfo;
  case TDT_ACTUALSIGNALINDICATIONS:
    bool withRedGreenStates;
    default:
      void;
};

/* --- End definitions for ArchiveAccess --- */
/* --- definitions for public transport --- */

/**
 * R09 or R10 telegram buffer in network byte order.
 */
typedef opaque R09TelegramCctrl<>;

/** list of amli telegrams used to send from TAC to firmware */
typedef AmliDtoCctrl AmliDtosCctrl<>;

/* --- End definitions for public transport --- */

/**
	 * type of ride
	 */
	enum TypOfRideCctrl
	{
	  /**
	   * individual traffic (e.g. trucks ?)
	   */
	  TOR_IT=1,
	  /**
	   * public transport
	   */
	  TOR_PT=2,
	  /**
	   * Bahn ?
	   */
	  TOR_Train=3,
	  /**
	   * vehicle with very important person
	   */
	  TOR_Vip=4,
	  /**
	   * emergency vehicle route
	   */
	  TOR_EVR=5
	};
	/** definitions that select type of {@link RideExtensionCctrl} */
	enum RideExtensionSelector {
	  /** select no extension */
	  RES_none=0,
	  /** select ride extension estimatedGreenDurationNeeded. */
	  RES_estimatedGreenDurationNeeded=1,
    /** select ride extension priority. */
	  RES_priority=2,
    /** select ride extension vehicleType. */
	  RES_vehicleType=3
	};
	/** to be used by IntersectionRide to enable compatible extensions. 
   *
   * estimated needed duration of green for the associated public transport
   * direction-signal group.
   * 
   * duration in ms. 
   * int estimatedGreenDurationNeeded;
   * int priority;
   * TypeOfRide vehicleType;
  */
	union RideExtensionCctrl switch(enum RideExtensionSelector selector) {
	  case RES_estimatedGreenDurationNeeded:
	    int estimatedGreenDurationNeeded;
	  case RES_priority:
	    int priority;
	  case RES_vehicleType:
	    TypOfRideCctrl vehicleType;
	  default:
	    void;
	};
  /**
   * public transport ride data transport object at C-Control interface.
   * needed by TA
   */
	struct IntersectionRideCctrl {

	  /**
	   * identifier for an intersection ride instance unique since last cold start.
	   * May be used to find same ride at subsequent updates.
	   */
	  hyper id;
	  /** public transport direction number. Corresponds with values configured with {@link CfgStageSequenceElement2#demandedByPtDirection} */
	  int ptDirectionNo;
	  /**
	   * number of signal group that needs green. 
	   */
	  int ptSgNo;
	  /**
	   * estimated earliest time when green for the associated public transport
	   * direction-signal group is needed.
	   * OmcTick in ms.
	   */
	  hyper estimatedStartTimeOfGreen;
	  /** list of future extensions. */
	  RideExtensionCctrl extensionList<>;
	};
	/** subscribeable public transport data object */
	struct IntersectionRidesCctrl {
	  /** list of active rides. */
	  struct IntersectionRideCctrl activeRideList<>;
	  /**
	   * OMC ticktime of last list update
	   */
	  hyper timeOfLastUpdate;
	};
/* --- End definitions for public transport --- */
	
/* ----- CycleTimeline dto's ----- */
/** possible types of specific structures representing CycleTimeline interval types. */
enum CycleTimelineEntryTypeCctrl {
  /** indicates an interval when the cycle time counter increments in 100ms incremets, is reset to 0 when cycleTime is reached. */
  CTE_Cyclic=0,
  /** indicates an interval when the cycle time counter is constant. */
  CTE_Stalling=1,
  /** indicates an interval with no defined cycle time counter */
  CTE_Invalid=2
};
/** {@link CycleTimelineEntryTypeCctrl#CTE_Cyclic} specific datastructure for {@link CycleTimelineEntryCctrl} */
struct CteCyclicCctrl {
  /** cycle time in ms */
  int cycleTime;
  /** device tick in ms of start of cycle, typically less than {@link CycleTimelineEntryCctrl#startTick start of intervall} */
  hyper cycleStartTime;
};
/** {@link CycleTimelineEntryTypeCctrl#CTE_Stalling} specific datastructure for {@link CycleTimelineEntryCctrl} */
struct CteStallingCctrl {
  /** the constant value this entry stalles to */ 
  int cycleTimeCounter;
};
/** DTO for cycle timeline intervall type specific data. */
union CteSpecificCctrl switch(enum CycleTimelineEntryTypeCctrl cteType) {
  /* cyclic entry specific datastructure for {@link CycleTimelineEntryCctrl} */
  case CTE_Cyclic:   CteCyclicCctrl cyclic;
  /* stalled rep. conctant entry specific datastructure for {@link CycleTimelineEntryCctrl} */
  case CTE_Stalling: CteStallingCctrl stalling;
  /* invalid/undefined entry specific datastructure for {@link CycleTimelineEntryCctrl} */
  case CTE_Invalid:  void;
  default: void;
};  
/** DTO describing a cycle-timeline interval. */
struct CycleTimelineEntryCctrl {
  /** device tick in ms of start of this interval */
  hyper startTick;
  /** {@link RequestedStatusPriorityCctrl priority} of switching event that caused this entry. */
  int prio;
  /** number of signal program to be running at this cycle timeline entry . */ 
  int signalProgram;
  /** transaction id of {@linkplain #signalProgram}, gives a hint to originator of this signalProgram.
   *  @see <a href="doc-files/TransactionId.htm">TransactionId doc</a> */
  int signalProgramSysJobId;
  /** intervall type specific data. */
  CteSpecificCctrl s;
};
/** sorted list of CycleTimelineEntry, used to inform client about trend of TX */ 
struct CycleTimelineCctrl {
  /** device tick time (omctick) of creation */ 
  hyper baseTick;
  /** sorted (entry.startTick) list of relevant cycle time determing segments. */
  CycleTimelineEntryCctrl entries<>;
};
/* ----- end CycleTimeline dto's ----- */

/* ----- begin Lock dto's ----- */
/** TA lock state */
enum TaLockStateCctrl {
  /** lock state is not available */
  TLS_NOT_AVAILABLE=0,
  /** lock state is lock */
  TLS_LOCK=1,
  /** lock state is unlock */
  TLS_UNLOCK=2,
  /** lock state is do not care */
  TLS_DO_NOT_CARE=3
};

/** struct of TA locks */ 
struct TaLocksCctrl {
  TaLockStateCctrl subIntersection[4];
};

/* ----- end Lock dto's ----- */



/* ------- program definitions ------- */
/** to be implemented by C10 traffic actuation and control center process.
 * we recommend to use {@link com.sitraffic.fd.ic.ccontrolClient.DataReceiver} in conjunction with {@link com.sitraffic.fd.ic.ccontrolClient.DataReceiverStarter} to do so. */
program CcontrolDataReceiver {
	version Sprint8R {
  /**
   * get C-Control interface protocol release version
   * this method may be called during a CcontrolService.subscribe4XXX call
   */
  int getReleasedVersionCcontrolDataReceiver(void)=1;
  
  /** service method for reception of spontaneous detector raw value changes.
   * if {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} for 
   * {@link MkjtricDataTypeCctrl#TDT_DETRAW} this method will be invoked on arrival of new det raw values 
   * that is typically each 100 ms.
   * <p>NOTE: the order of invocation (detStates, actualSignalIndications...) is unspecified and may vary while running.</p>
   * @param values  detector values and states that have changed.
   * @param seqNr   sequence number to detect loss of data. with each subscription (different tuple host,port) the service starts with seqNr=0.  
   */
	void detStates(DetRawValuesExchg values, int seqNr)=10;
  /** service method for reception of spontaneous SignalIndication value changes.
   * if {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} for 
   * {@link MkjtricDataTypeCctrl#TDT_ACTUALSIGNALINDICATIONS} this method will be invoked on arrival of new det signal indications 
   * from CBC, that is typically each 100 ms.
   * <p>NOTE: the order of invocation (detStates, actualSignalIndications...) is unspecified and may vary while running.</p>
   * @param values  SignalIndication that <b>might</b> have changed.
   * @param seqNr   sequence number to detect loss of data. with each subscription (different tuple host,port) the service starts with seqNr=0.  
   */
	void actualSignalIndications(SignalIndicationsCctrl values, int seqNr)=11;
  /** service method for reception of spontaneous ActualStatus value changes.
   * if {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} for 
   * {@link MkjtricDataTypeCctrl#TDT_ACTUALSTATUS} this method will be invoked on change of 
   * actual status at firmware. After subscription the initial actual status is sent (without change).
   * @param values  actual status that has changed (except initial update).
   * @param seqNr   sequence number to detect loss of data. with each subscription (different tuple host,port) the service starts with seqNr=0.  
   */
	void actualStatus(ActualStatusCctrl values, int seqNr)=12;
  /** service method for reception of spontaneous requested status value changes.
   * if {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} for 
   * {@link MkjtricDataTypeCctrl#TDT_REQUESTEDSTATUS} this method will be invoked on change of 
   * requested status at firmware. After subscription the initial requested status is sent (without change).
   * @param rs  requested status that has changed (except initial update).
   * @param seqNr   sequence number to detect loss of data. with each subscription (different tuple host,port) the service starts with seqNr=0.  
   */
	void requestedStatus(RequestedStatusCctrl rs, int seqNr)=13;
  /** service method for reception of spontaneous target stage value changes.
   * if {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} for 
   * {@link MkjtricDataTypeCctrl#TDT_TARGETSTAGE} this method will be invoked on change of 
   * target stage at firmware. After subscription the initial target stage is sent (without change).
   * @param ts  target stage that has changed (except initial update).
   * @param devTick omc tick in ms till when the ts data is valid.
   * @param seqNr   sequence number to detect loss of data. with each subscription (different tuple host,port) the service starts with seqNr=0.  
   */
	void targetStage(TargetStagePtr ts, hyper devTick, int seqNr)=14;

  /** service method for reception of spontaneous public transport rides value changes.
   * if {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} for 
   * {@link MkjtricDataTypeCctrl#TDT_INTERSECTIONRIDES} this method will be invoked on change of 
   * intersection ride list at firmware. After subscription the initial intersection ride list is sent (without change).
   * @param rides  intersection ride list that has changed (except initial update).
   * @param seqNr   sequence number to detect loss of data. with each subscription (different tuple host,port) the service starts with seqNr=0.  
   */
  void intersectionRides(IntersectionRidesCctrl rides, int seqNr)=15;

  /** service method for reception of spontaneous device value changes.
   * if {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} for
   * {@link MkjtricDataTypeCctrl#TDT_DEVICEVALUES} this method will be invoked on change of a subscribed device
   * value at firmware. After subscription the initial values of the subscribed device values are sent (without change).
   * @param values  device value list that has changed (except initial update).
   * @param seqNr   sequence number to detect loss of data. with each subscription (different tuple host,port) the service starts with seqNr=0.
   */
  void deviceValues(DeviceValuesCctrl values, int seqNr)=16;

  /** service method for reception of received ride event telegram (R09).
   * if {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} for
   * {@link MkjtricDataTypeCctrl#TDT_RIDEEVENT} this method will be invoked on change of a subscribed device
   * value at firmware.
   * @param r09     received ride event.
   * @param seqNr   sequence number to detect loss of data. with each subscription (different tuple host,port) the service starts with seqNr=0.
   */
  void rideEvent(RideEventCctrl r09, int seqNr)=17;

  /**
   * switch reporting of {@link CControl_CcontrolService_CltIF#allQueuesAreIdle_8(String, int, int)} on/off.
   * only relevant in conjunction with CBC simulator.
   * @param isReportRequested if true, client shall invoke allQueuesAreIdle_8() if it has processed all subscribed events.
   * @param seqNr   sequence number to detect loss of data. with each subscription (different tuple host,port) the service starts with seqNr=0.  
   */
  void reportAllQueuesAreIdle(bool isReportRequested, int seqNr)=30;

	/* --- ConfigConsumer --- */
    /** method for C-Control client as ConfigConsumer.<br></br>
     * ask a configuration consumer if given (new) configuration will be accepted.
     * the configuration to check may be obtained by <ul>
     * <li>{@link CControl_CcontrolService_CltIF#getDraftSelectedCfg_8(String, int, CfgSelector)}</li>
     * <li>{@link CControl_CcontrolService_CltIF#getDraftIntersectionVersionAsXmlString_8}</li>  
     * <li>{@link CControl_CcontrolService_CltIF#getDraftSystemConfigVersionAsXmlString_8}</li>
     * </ul>
     * @param configTypes is a bitfield with values of enum {@link ConfigType#getEncoding} ored together.  
     */
	  CheckRespondCctrl checkConfig(int configTypes)=20;
	
    /** method for C-Control client as ConfigConsumer.<br></br>
  	 * activates (new) configuration for a registered consumer
     * activation is only done if new configuration has been accepted by consumer before.
     * @param sysJobId  system job id of job that initiated this config change.
     *                  see <a href="doc-files/TransactionId.htm">TransactionId doc</a>  */
  	void activate(int sysJobId)=21;
    /** method for C-Control client as ConfigConsumer.<br></br>
  	 * clean up a previous checkConfig call if transaction has been canceled by user. */
  	void cleanupConfigCheck(void)=22; 
  	/* --- End ConfigConsumer --- */
  	
   /* --- Archive access --- */
  
   /** service method for reception of archive insert events.
   * if {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscribed} for 
   * {@link MkjtricDataTypeCctrl#TDT_ARCHIVEEVENTS} this method will be invoked after requested number of
   * firmware archive inserts.
   * @param selector enumeration value of type {@link ArchiveSelectorCctrl} indicates for which archive the insert event was sent.
   */
   void archiveInsertEvent(enum ArchiveSelectorCctrl selector)=140;
   /* --- End archive access --- */

   /* --- DeviceVariables --- */
   /*
    * @return wtt.xml as xmlstring
    * removed by refactoring use getDeviveValuesMetaData instead.
    * string getDeviceVariablesMetaData(void)=150;
    */
   /**
    * subscribe for delivery of passed oitds. Values are sent to firmware via {@link CControl_CcontrolService_CltIF#setDeviceValues_8(DeviceValuesCctrl)}.
    * unknown oitds are ignored.
    * @param oitds list of tuples (member, otype) to subscribe. 
    * @return RC_OK 
    */
   enum RetCodeCctrl subscribeDeviceVariables(OitdCctrlA oitds)=151;
   /**
    * un-subscribe for delivery of passed oitds. 
    * @param oitds list of tuples (member, otype) to unsubscribe. 
    * @return RC_OK if successful 
    */
   enum RetCodeCctrl unSubscribeDeviceVariables(OitdCctrlA oitds)=152;
   
   /**
    * @return meta data to device variables. Will be used in conjunction with {@link #getNlsDefinitions_8}
    */
   DeviceValuesMetaDataCctrl getDeviceValuesMetaData(void) = 153;
   /**
    * provides national language mappings for language lang.
    * the mappings apply to {@link DeviceValueTypeCctrl} and TA Status
    * @return language mappings for given lang. may be empty if lanuage is not supported.
    */
   NlsMappingsCctrl getNlsDefinitions(void) = 160;
   /* --- End DeviceVariables --- */

  	   
	}=8;
}=0x20000201;

/** implemented by C10 firmware C-Control provider. */
program CcontrolService {
  version Sprint8S {
  /**
   * get C-Control interface protocol release version
   */
  int getReleasedVersionCcontrolService(void)=1;
  
  /* --- publish subscriber related methods --- */
  /* use peer IpAdr and Port */
  /** subscribe for {@link MkjtricDataTypeCctrl} values.
   * <p>we recommend to use {@link com.sitraffic.fd.ic.ccontrolClient.DataReceiver} in conjunction with 
   * {@link com.sitraffic.fd.ic.ccontrolClient.DataReceiverStarter} to receive spontaneous data.</p>
   * @param inetadr address or hostname where data has to be sent, 
   * that is where the corresponding service method receives the data. 
   * @param port UDP port number where the corresponding service method is listening to receive tha data.
   * @param si further specification of subscription.
   * @return {@link RetCodeCctrl#RC_OK} if succeeded, no subscription taken place otherwise.
   * @see <a href="doc-files/genericSubscriptionForDeliveryOfSpontaneousData.png">generic subscription for delivery of spontaneous data</a>
   */
  enum RetCodeCctrl subscribe4Tdt(string inetadr, int port, TdtSubscriptionInfoCctrl si)=10;
  /**
   * stop traffic data value delivery subscribed with {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl)}}
   * @param inetadr address or hostname where data has to be sent, 
   * that is where the corresponding service method receives the data. 
   * @param port UDP port number where the corresponding service method is listening to receive tha data.
   * @param tdt specification of traffic data type to unsubscribe.
   * @return {@link RetCodeCctrl#RC_OK} if service has removed subscription. 
   */
  enum RetCodeCctrl unsubscribe4Tdt(string inetadr, int port, enum MkjtricDataTypeCctrl tdt)=11;  
  
  /**
   * only relevant in conjunction with cbcsimulator. 
   * this method signals firmware that the c-control client refered by inetadr:port has finished processing upto seqNr 
   *  needed at simulation to throttle tick generation at cbc-simulator.
   * @param inetadr inetadress of subscription of one of the subscribeXXX calls above 
   * @param port port number of subscription of one of the subscribeXXX calls above 
   */
  void allQueuesAreIdle(string inetadr, int port, int seqNr)=95;

  /** renew lease. All subscriptions above are renewed by this method.
   *  the lease expires after {@link CControl#LEASE_TIMEOUT_MS} (currently 2 minutes).
   * @see <a href="doc-files/genericSubscriptionForDeliveryOfSpontaneousData.png">generic subscription for delivery of spontaneous data</a> */ 
  enum RetCodeCctrl renew(string inetadr, int port)=99;
  
  /* --- other methods --- */
  /** microscopic signal group orientated control method. for use by signal group orinted traffic actuated control.
   * <p>if a traffic actuated control logic wants a signal group to stay at a static color/state it shall refresh this stage switching event 
   * every {@link CControl#SWITCHING_EVENT_REFRESH_INTERVAL}  with a duration of {@link CControl#SWITCHING_EVENT_DEFAULT_VALID_DURATION} .</p>
   * @param sgSwitchingEvents signal group related desired switching events.
   */
  void addDesiredSgSwitchingEvents(ExternalSgSwitchingEventArray sgSwitchingEvents)=100;				/* DesiredSignalIndication */
  /** microscopic stage oriented control method. for use by stage oriented traffic actuated control.
   * <p>if a traffic actuated control logic wants to stay at a static stage it shall refresh this stage switching event 
   * every {@link CControl#SWITCHING_EVENT_REFRESH_INTERVAL}  with a duration of {@link CControl#SWITCHING_EVENT_DEFAULT_VALID_DURATION} .</p>
   * @param stageSwitchingEvents desired stage switching events 
   */
  void addDesiredStageSwitchingEvents(ExternalStageSwitchingEventArray stageSwitchingEvents)=101;		/* DesiredStage */
  /**
   * 
   * @param notBeforeTime dev ticks in ms since when the earliest time is searched
   * @param toStage   stage number of stage transition end 
   * @return  earliest time after notBeforeTime when the stage transition may be started and played out without any shifter interventions,
   * that is the stage transition runs as computed. In case of parameter errors, {@linkplain Long#MAX_VALUE} is returned.
   * @throws Exception
   * @deprecated use {@link checkAddDesiredStageSwitchingEvents} instead
   */
  hyper getEarliestTime4StageTransition(hyper notBeforeTime, int toStage)=102;
 
  /** microscopic stage oriented control method. for use by stage oriented traffic actuated control.
   * <p>if a traffic actuated control logic wants to stay at a static stage it shall refresh this stage switching event 
   * every {@link CControl#SWITCHING_EVENT_REFRESH_INTERVAL}  with a duration of {@link CControl#SWITCHING_EVENT_DEFAULT_VALID_DURATION} .</p>
   * @param stageSwitchingEvents desired stage switching events. time of check starts with startTickTime of first switching event 
   * @param allowedTypesOfViolation bitfield with values of {@link TypeOfViolation} 
   *  e.g.: TypeOfViolation.TOV_PhaserMissingStageTransition.getEncoding()|TypeOfViolation.TOV_PhaserStageSkipped.getEncoding()
   * @return <ul>
   *          <li>true if check has passed and stage switching events have been added, 
   *          <li>false if check has failed and no switching events have been added.
   *          </ul>
   */
  bool checkAddDesiredStageSwitchingEvents(ExternalStageSwitchingEventArray stageSwitchingEvents, int allowedTypesOfViolation)=103;   

  
  /* --- RequestedStatusManagerIF --- */
  /** macroscopic control method for whole intersection on/off state.
   * controls {@link RequestedStatusCctrl}
   * @param priority operator priority, 0 is lowest
   * @param value value to change e.g.: RMNS_ON...;
   * @param sysJobId sysJobId of assigned from the operator to this change
   * @param durationInMs valid time for the change in milli seconds
   * @return {@link RetCodeCctrl#RC_OK} on success, otherwise no action is taken. 
   */
  enum RetCodeCctrl setIntersectionState(enum RequestedStatusPriorityCctrl priority, enum RequestedMainNodeStateCctrl value, int sysJobId, hyper durationInMs)=110;
  /** macroscopic control method
   * controls {@link RequestedStatusCctrl}
   * @param priority operator priority, 0 is lowest
   * @param value value to change
   * @param sysJobId sysJobId of assigned from the operator to this change
   * @param key 1..4
   * @param durationInMs valid time for the change in milli seconds
   * @return {@link RetCodeCctrl#RC_OK} on success, otherwise no action is taken. 
   */
  enum RetCodeCctrl setSubIntersectionState(enum RequestedStatusPriorityCctrl priority, enum RequestedNodeStateCctrl value, int sysJobId, int key, hyper durationInMs)=111;
   /** macroscopic control method
   * controls {@link RequestedStatusCctrl}
   * @param priority operator priority, 0 is lowest
   * @param value value to change
   * @param sysJobId sysJobId of assigned from the operator to this change
   * @param durationInMs valid time for the change in milli seconds
   * @return {@link RetCodeCctrl#RC_OK} on success, otherwise no action is taken. 
   */
  enum RetCodeCctrl setSignalProgram(enum RequestedStatusPriorityCctrl priority, int value, int sysJobId, hyper durationInMs)=112;
  /** macroscopic control method
   * controls {@link RequestedStatusCctrl}
   * @param priority operator priority, 0 is lowest
   * @param value value to change
   * @param sysJobId sysJobId of assigned from the operator to this change
   * @param durationInMs valid time for the change in milli seconds
   * @return {@link RetCodeCctrl#RC_OK} on success, otherwise no action is taken. 
   */
  enum RetCodeCctrl setSpecialIntervention(enum RequestedStatusPriorityCctrl priority, int value, int sysJobId, hyper durationInMs)=113;

  /** macroscopic control method
   * controls {@link RequestedStatusCctrl}
   * @param priority operator priority, 0 is lowest
   * @param value value to change
   * @param sysJobId sysJobId of assigned from the operator to this change
   * @param durationInMs valid time for the change in milli seconds
   * @return {@link RetCodeCctrl#RC_OK} on success, otherwise no action is taken. 
   */
  enum RetCodeCctrl setModVAState(enum RequestedStatusPriorityCctrl priority, RequestedModificationStateCctrl value, int sysJobId, hyper durationInMs)=115;

  /** macroscopic control method
   * controls {@link RequestedStatusCctrl}
   * @param priority operator priority, 0 is lowest
   * @param value value to change
   * @param sysJobId sysJobId of assigned from the operator to this change
   * @param durationInMs valid time for the change in milli seconds
   * @return {@link RetCodeCctrl#RC_OK} on success, otherwise no action is taken. 
   */
  enum RetCodeCctrl setModIVState(enum RequestedStatusPriorityCctrl priority, RequestedModificationStateCctrl value, int sysJobId, hyper durationInMs)=116;

  /** macroscopic control method
   * controls {@link RequestedStatusCctrl}
   * @param priority operator priority, 0 is lowest
   * @param value value to change
   * @param sysJobId sysJobId of assigned from the operator to this change
   * @param durationInMs valid time for the change in milli seconds
   * @return {@link RetCodeCctrl#RC_OK} on success, otherwise no action is taken. 
   */
  enum RetCodeCctrl setModOEVState(enum RequestedStatusPriorityCctrl priority, RequestedModificationStateCctrl value, int sysJobId, hyper durationInMs)=117;

  /** macroscopic control method controls {@link RequestedStatusCctrl}
   * @param priority operator {@link RequestedStatusPriorityCctrl priority}, 0 is lowest
   * @param value value to change
   * @param sysJobId sysJobId of assigned from the operator to this change
   * @param durationInMs valid time for the change in milli seconds
   */  
  enum RetCodeCctrl setSynchronizationState(enum RequestedStatusPriorityCctrl priority, RequestedModificationStateCctrl value, int sysJobId, hyper durationInMs)=121;

  /** macroscopic control method
   * controls {@link RequestedStatusCctrl}
   * @param priority operator priority, 0 is lowest
   * @param value value to change
   * @param sysJobId sysJobId of assigned from the operator to this change
   * @param key number of the modification to change (1..13)
   * @param durationInMs valid time for the change in milliseconds
   * @return {@link RetCodeCctrl#RC_OK} on success, otherwise no action is taken. 
   */
  enum RetCodeCctrl setProjectspecificModState(enum RequestedStatusPriorityCctrl priority, RequestedModificationStateCctrl value, int sysJobId, int key, hyper durationInMs)=118;

  /**
   * macroscopic control method,
   * controls {@link RequestedStatusCctrl}
   * drop all change requests of a given priority
   * 
   * @param priority
   */
  void dropPriority(enum RequestedStatusPriorityCctrl priority)=119;

  /** macroscopic control method, clears all desired macroscopic switching events out of {@link RequestedStatusCctrl}. 
   * handle with care, provided for tests. */
  void clearAll(void)=120;
  /* --- /RequestedStatusManagerIF --- */
  
  /** set actual status of traffic actuation process 
   * host, port SHALL be the one of a previous {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscription} for 
   * {@link MkjtricDataTypeCctrl#TDT_REQUESTEDSTATUS} 
   * @param host address or hostname of your subscription 
   * @param port UDP port number of your subscription
   * @param taStates a list of the ActualStatus modifications to change.
   * @return RC_OK if set,
   *        RC_InvalidKey if lease not found 
   */
  enum RetCodeCctrl setTaActualStatus(string host, int port, TTVModOnOffStateCctrlA taStates)=130;
  
  /** return of traffic actuation control parameter file from inventory as byte array
   * in case of any error, an empty byte array is returned 
   * @param componentName name of traffic actuated control
   * @return {@link MkjtricActuationComponentParams} parameter file as byte array
   */
  MkjtricActuationComponentParamsResult getComponentParams(string componentName)=131;

  /** return of traffic actuation control parameter file from draft as byte array
   * in case of any error, an empty byte array is returned 
   * @param componentName name of traffic actuated control
   * @return {@link MkjtricActuationComponentParams} parameter file as byte array
   */
  MkjtricActuationComponentParamsResult getDraftComponentParams(string componentName, string inetadr, int port)=132;
  
  /** status reporting of c-control clients. 
   * host, port SHALL be the one of a previous {@link CControl_CcontrolService_CltIF#subscribe4Tdt_8(String, int, TdtSubscriptionInfoCctrl) subscription} for 
   * {@link MkjtricDataTypeCctrl#TDT_REQUESTEDSTATUS} 
   * @param host address or hostname of your subscription of {@link CControl_CcontrolService_CltIF#registerDeviceVariableProvider_8(String, int, int, String, String)} 
   * @param port UDP port number of your subscription of {@link CControl_CcontrolService_CltIF#registerDeviceVariableProvider_8(String, int, int, String, String)}
   * @param status type of clt and detail state attachment messages to set.
   * */
  enum RetCodeCctrl setCltStatus(string host, int port, CltStatusMsgsCctrl status)=135;
  
  /* --- Archive access --- */
  
  /**
   * Get the oldest archived value from archive selected by {@link ArchiveSelectorCctrl}.
   * <p>
   * Oldest value means, that at the moment of the archive access the archive contains no element of the requested type with a time stamp
   * less than the time stamp of the returned value.
   * </p>
   * 
   * @param selector enumeration value of type {@link ArchiveSelectorCctrl} to select the archive to access.
   * @return {@link ArchiveResultCctrl} instance with first archived value.
   */
  ArchiveResultCctrl archiveGetOldest(enum ArchiveSelectorCctrl selector)=151;

  /**
   * Get the youngest archived value from archive selected by {@link ArchiveSelectorCctrl}.
   * <p>
   * Youngest value means, that at the moment of the archive access the archive holds no element of the requested type with a time stamp
   * greater than the time stamp of the returned value.
   * </p>
   * 
   * @param selector enumeration value of type {@link ArchiveSelectorCctrl} to select the archive to access.
   * @return {@link ArchiveResultCctrl} instance with last archived value.
   */
  ArchiveResultCctrl archiveGetYoungest(enum ArchiveSelectorCctrl selector)=152;

  /**
   * Get up to <code>maxCount</code> archived values from archive selected by {@link ArchiveSelectorCctrl}
   * with a time stamp greater than <code>start</code>.
   * 
   * @param selector enumeration value of type {@link ArchiveSelectorCctrl} to select the archive to access.
   * @param primaryKey Archive primary key value, or 0 for time based archive search with key <code>start</code>
   * @param start the time in UTC milliseconds from the epoch.
   * @param maxCount max number of values to retrieve from archive.
   * @return {@link ArchiveResultArrayCctrl} wraps a strictly increasing sorted array of values.
   */
  ArchiveResultArrayCctrl archiveGetLaterThan(enum ArchiveSelectorCctrl selector, hyper primaryKey, hyper start, int maxCount)=153;
  
  /**
   * Append firmware operating message C_ResetArchive to archive {@link ArchiveSelectorCctrl#AS_StandardMessage StandardMessage}
   * <p>
   * Reserved for use by Siemens control center communication process to implement message C_ResetArchive according to Canto 1.3 standard.
   * </p>
   *
   * @param sysJobId system job id or transaction number of control center communication process. It consists of a originator and a sequence number
   * @param archiveType identifier of Canto 1.3 transient archive instance.
   * @return {@link RetCodeCctrl#RC_OK} if succeeded; otherwise parameter <code>archiveType</code> is not a valid archive instance.
   */
  enum RetCodeCctrl addMsgResetArchive(int sysJobId, MsgParameterCctrl archiveType)=159;

  /* --- End archive access --- */
  
  /* --- Detector aggregation configuration */
  
  /** 
   * Get current aggregation interval and offset of cyclic detector aggregation.
   * 
   * @return {@link AggregationCycleResultCctrl} wraps {@link AggregationCycleCctrl configuration} of cyclic detector aggregation.
   */
  AggregationCycleResultCctrl getAggregationCycle(void)=161;
  
  /**
   * Set current aggregation interval and offset of cyclic detector aggregation.
   * 
   * @param samplingInterval new cycle time of cyclic detector aggregation in ms.
   * @param offset new offset to standard Canto reference point in time in ms. Offset is calculated modulo samplingInterval.
   * @return {@link RetCodeCctrl#RC_OK} if succeeded; otherwise configuration of cyclic detector aggregation remains unchanged.
   */
  enum RetCodeCctrl setAggregationCycle(int samplingInterval, int offset)=162;
  
  /** 
   * Get current maximum aggregation interval of signalling cycle based detector aggregation.
   * 
   * @return {@link AggregationSignallingResultCctrl} wraps maximum aggregation interval of signalling cycle based detector aggregation.
   */
  AggregationSignallingResultCctrl getAggregationSignalling(void)=163;

  /**
   * Set current maximum aggregation interval of signalling cycle based detector aggregation.
   * 
   * @param maxSamplingInterval new maximum aggregation interval of signalling cycle based detector aggregation.
   * @return {@link RetCodeCctrl#RC_OK} if succeeded; otherwise configuration of signalling cycle based detector aggregation remains unchanged.
   */
  enum RetCodeCctrl setAggregationSignalling(int maxSamplingInterval)=164;
  
  /* --- End detector aggregation configuration */
  
  /* --- PublicTransportRequest ----*/
  
  /**
   * Send a public transport request to the sX public transport manager.
   * @param sysJobId  transaction id of the request, gives a hint to originator of this request, see <a href="doc-files/TransactionId.htm">TransactionId doc</a>
   * @param telegram  Raw R09 or R10 telegram buffer in network byte order.
   * @return {@link RetCodeCctrl#RC_OK} if a valid public transport event was forwarded to the public transport manager;
   *         {@link RetCodeCctrl#RC_UnknownValue} if R09/R10 telegram is unknown or faulty and no event was created for the public transport manager.
   */
  enum RetCodeCctrl forwardPtRequest(int sysJobId, R09TelegramCctrl telegram)=170;
  
  /* --- End PublicTransportRequest ----*/
  /* ---- ConfigAccess ----*/
  /** 
   * THE FOLLOWING RPC CALL IS WITHOUT WARRANTY. DEFECTS CAUSED BY INCOMPATIBILITIES COULD OCCUR WHEN DOING SOFTWARE UPDATES. THE ENTIRE RISK OF USING THIS RPC CALL IS WITH YOU.
   * SHOULD THE COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY SERVICING, REPAIR OR CORRECTION.
   * @return xml string (utf-8) of inventory IntersectionVersion.
   * in case of any error, an empty string is returned */
  string getIntersectionVersionAsXmlString(void)=200;
  /** 
   * THE FOLLOWING RPC CALL IS WITHOUT WARRANTY. DEFECTS CAUSED BY INCOMPATIBILITIES COULD OCCUR WHEN DOING SOFTWARE UPDATES. THE ENTIRE RISK OF USING THIS RPC CALL IS WITH YOU.
   * SHOULD THE COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY SERVICING, REPAIR OR CORRECTION.
   * @return xml string (utf-8) of inventory SystemConfigVersion.
   * in case of any error, an empty string is returned */
  string getSystemConfigVersionAsXmlString(void)=201;
  /* -- ConfigAccess for ConfigConsumer ---*/
  /** 
   * THE FOLLOWING RPC CALL IS WITHOUT WARRANTY. DEFECTS CAUSED BY INCOMPATIBILITIES COULD OCCUR WHEN DOING SOFTWARE UPDATES. THE ENTIRE RISK OF USING THIS RPC CALL IS WITH YOU.
   * SHOULD THE COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY SERVICING, REPAIR OR CORRECTION.
   * @return xml string (utf-8) of draft IntersectionVersion to check inside ConfigConsumer.check().
   * in case of any error (e.g. no running config check), an empty string is returned 
   * inetadr, port SHALL be the one used for previous registerAsConfigConsumer() */
  string getDraftIntersectionVersionAsXmlString(string inetadr, int port)=202;
  /** 
   * THE FOLLOWING RPC CALL IS WITHOUT WARRANTY. DEFECTS CAUSED BY INCOMPATIBILITIES COULD OCCUR WHEN DOING SOFTWARE UPDATES. THE ENTIRE RISK OF USING THIS RPC CALL IS WITH YOU.
   * SHOULD THE COVERED CODE PROVE DEFECTIVE IN ANY RESPECT, YOU (NOT THE INITIAL DEVELOPER OR ANY OTHER CONTRIBUTOR) ASSUME THE COST OF ANY NECESSARY SERVICING, REPAIR OR CORRECTION.
   * @return xml string (utf-8) of draft SystemConfigVersion to check inside ConfigConsumer.check().
   * in case of any error (e.g. no running config check), an empty string is returned
   * inetadr, port SHALL be the one used for previous registerAsConfigConsumer() */
  string getDraftSystemConfigVersionAsXmlString(string inetadr, int port)=203;
   
  /**
   * Get complete version information of all C10 components.
   * @return {@link VersionsCctrl version} information of all C10 components. 
   */
  VersionsCctrl getVersions(void)=208;
  
  /**
   * Get Canto network configuration parameter from hlc_system.xml.
   * @return {@link CantoNetConfigCctrl} instance with Canto network configuration parameters.
   */
  CantoNetConfigCctrl getCantoNetConfig(void)=209;
   
  /** the inventory version is queried
   * in case of error the returned selector is CS_CfgError.
   * @return some traffic related configuration structures for use by traffic actuated control (VA)
   */
  SelectedConfig getSelectedCfg(enum CfgSelector)=240;  
  /** the draft version is queried
   * in case of error the returned selector is CS_CfgError.
   * @return some traffic related configuration structures for use by traffic actuated control (VA)
   */
  SelectedConfig getDraftSelectedCfg(string inetadr, int port, enum CfgSelector)=241;  



  /*--- Assorted ConfigData Info ---*/
  /* return csv string (utf-8) tuple number-shortname,  of c10 base_xml data inventory-version.*/
  /* in case of any error, an empty string is returned */
  /* refactoring removed 
  string getConfigDataInfoIntersectionAsCSV(void)=251;
  string getConfigDataInfoSubIntersectionAsCSV(void)=252;
  string getConfigDataInfoSignalprogramAsCSV(void)=253;
  string getConfigDataInfoDetectorAsCSV(void)=254;
  */
  
  /** query some inventory configuration data needed by control center process. 
   * @param selector which data to query
   * @return selected inventory configuration dependant of {@link SelectedCfgInfo#selector} */
  SelectedCfgInfo getSelectedCfgInfo(enum CfgInfoSelector selector)=255;
  /** @return int of c10 base_xml data inventory-version.
   * in case of any error, an negative int -1 is returned */
  int getConfigDataInfoZNr(void)=258;
  /** @return int of c10 base_xml data inventory-version.
   * in case of any error, an negative int -1 is returned */
  int getConfigDataInfoFNr(void)=259;
  
 
  /* ConfigNotification */
  /**
   * register a configuration consumer to configuration manager
   * a consumer may register/unregister for several different (partial) configurations
   * by passing a set of their corresponding configuration types 
   * Consumer is notified about changes in all registered configurations  
   * call is ignored if consumer is already registered for given configuration type
   * 
   * @param inetadr of client, a configuration consumer
   * @param port    of client, a configuration consumer
   * @param configTypeSet denotes the configuration type 
   * @param uid		unit identifier for registration
   */
  enum RetCodeCctrl registerAsConfigConsumer(string inetadr, int port, int configTypeSet, enum UnitId uid)=204;
  /**
   * unregister a configuration consumer from configuration manager if registered previously
   * a consumer may register/unregister for several different (partial) configurations
   * by passing a set of their corresponding configuration types 
   * call is ignored if consumer is already unregistered for given configuration type 
   * 
   * @param inetadr of client, a configuration consumer
   * @param port    of client, a configuration consumer
   * @param configTypeSet denotes the configuration type 
   */
  enum RetCodeCctrl unregisterAsConfigConsumer(string inetadr, int port, int configTypeSet)=205;

  /* --- DeviceVariables --- */
  /**
   * register a remote CControl Device Variable provider.
   * with this registration the firmware may call {@link CControl_CcontrolDataReceiver_CltIF#getNlsDefinitions_8()}
   * @param inetadr of client, a device variable provider
   * @param port    pport of client's subscription/lease, the udp server socket port.
   * @param tcpPort    tcp port of client, a device variable provider
   * @param dvpName name of the device variable provider
   * @param dvpVersion version of the device variable provider
   */
  enum RetCodeCctrl registerDeviceVariableProvider(string inetadr, int port, int tcpPort, string dvpName, string dvpVersion)=210;
  
  /**
   * updates subscribed, changed device values. in case of first invokation, the initial update there are all subscribed device values reported.
   * @param dvs collection of device values that have changed at timestamp.
   */
  void setDeviceValues(DeviceValuesCctrl dvs)=211; 

  /**
   * This method is used by an external process like the center communication process to subscribe to device variables identified by their oitds.
   * Values are not sent back to the external process by dedicated methods. Instead the external process has to call corresponding archive methods.
   * Unknown oitds are ignored.
   * @return RC_OK 
   */
  enum RetCodeCctrl subscribe4DeviceVariables(OitdCctrlA oitds)=212;
  
  /**
   * this method is used by an external process like the center communication process to un-subscribe to device variables identified by their oitds.
   * Unknown oitds are ignored.
   * @return RC_OK  
   */
  enum RetCodeCctrl unSubscribe4DeviceVariables(OitdCctrlA oitds)=213;
  
  /**
   * this method is used by an external process like the center communication process to un-subscribe all device variables
   * @return RC_OK  
   */
  enum RetCodeCctrl unSubscribeAllDeviceVariables(void)=214;
  
  /**
   * this method can be used by the central communication process in order to ensure that a device variable identified by its oitd exists
   * @return true if the device variable identified by its oitd is defined in the firmware, otherwise false
   */  
  bool checkIfOitdExists(OitdCctrl oitd)=215;
  
  /**
   * query all available device variable definitions
   * @return list of device value meta data of the available device variable provider (VA, FW)
   */
  DeviceValuesMetaDataCctrlA getDeviceValuesMetaDataList(void)=216;
  
  /**
   * query list of used indices for a given device value index type.
   * in case there is only one instance, index 0 is returned.
   * In case of unknown device value index type, an empty list is returned.
   * @param dvit index type to query list of used indices
   * @return list of used indices
   */
  DeviceValuesUsedIndicesCctrl getDeviceValuesUsedIndex(DeviceValueIndexType dvit)=217;
  
  /* --- End DeviceVariables --- */
  
  /**
   * force the spl number and cycle time value (TX) to a new value
   * 
   * @param signalProgramNumber number of related signalprogram
   * @param newCycleTimeCounterValue new value in milliseconds
   * @param atTickTime the device tick time when the cycle time value should jump to the new value
   * @param durationMs how long this request is valid
   * @param priority priority of the request
   * @param initiator initiator of the request, arbitrary message that is associated with this switching request
   * @param sysJobId  transaction id of the request, gives a hint to originator of this signal program request, see <a href="doc-files/TransactionId.htm">TransactionId doc</a>
   * @return OK on success
   */
  enum RetCodeCctrl setSplCycleTimeCounterTo(int signalProgramNumber,
    int newCycleTimeCounterValue, hyper atTickTime, int durationMs, int priority, InitiatorCCtrl initiator,
    int sysJobId)=222;
  /**
   * force the cycle time value (TX) to jump atTickTime to newCycleTimeCounterValue and remain there duration
   * milliseconds.
   * 
   * @param signalProgramNumber number of related signalprogram
   * @param newCycleTimeCounterValue new value in milliseconds
   * @param atTickTime the OMC-device tick time when the cycle time value should jump to the new value
   * @param durationMs number of milliseconds the TX stays at newCycleTimeCounterValue, SHALL be >=0
   * @param priority priority of the request
   * @param initiator initiator of the request, arbitrary message that is associated with this switching request
   * @param sysJobId  transaction id of the request, gives a hint to originator of this signal program request, see <a href="doc-files/TransactionId.htm">TransactionId doc</a>
   * @return OK on success RC_UnknownValue if not 0&lt;= newCycleTimeCounterValue &lt; cycleTime of current signal plan,
   *         RC_InvalidDuration if durationMs is &lt; 0, RC_InvalidKey if no signal program runs at the given atTickTime
   */
  enum RetCodeCctrl stallSplCycleTimeCounterTo(int signalProgramNumber,
    int newCycleTimeCounterValue, hyper atTickTime, int durationMs, int priority, InitiatorCCtrl initiator,
    int sysJobId)=223;
  /**
   * invalidate the cycle time value (TX) at atTickTime for duration milliseconds.
   * 
   * @param signalProgramNumber number of related signalprogram
   * @param atTickTime the device tick time when the cycle time value should jump to the new value
   * @param durationMs number of milliseconds the TX stays at newCycleTimeCounterValue, SHALL be >=0
   * @param priority priority of the request
   * @param initiator initiator of the request, arbitrary message that is associated with this switching request
   * @param sysJobId  transaction id of the request, gives a hint to originator of this signal program request, see <a href="doc-files/TransactionId.htm">TransactionId doc</a>
   * @return OK on success RC_UnknownValue if not 0&lt;= newCycleTimeCounterValue &lt; cycleTime of current signal plan,
   *         RC_InvalidDuration if durationMs is &lt; 0, RC_InvalidKey if no signal program runs at the given atTickTime
   */
  enum RetCodeCctrl invalidateCycleTimeCounterAt(int signalProgramNumber,
    hyper atTickTime, int durationMs, int priority, InitiatorCCtrl initiator, int sysJobId)=224;

  /**
   * reset the cycle monitor control (Umlaufzeitkontrolle).
   * for use by traffic actuated control if it switches an static signalisation.
   * @param partialIntersectionMask bitfield that passes the subintersections that's cycle monitor control has to be resetted.<br></br>
   *        set bit 2^partialIntersection number for each partial intersection that's cycle monitor control has to be resetted.
   *        e.g. to reset partial intersection 1 and 3 partialIntersectionMask = 2^1 + 2^3 = 2 + 8 = 10 = 0xa<br></br>
   *        to reset partial intersections 1,2,3,4 partialIntersectionMask = 2+4+8+16 = 30
   *        to reset all partial intersections pass 0xff
   * @return RC_OK if cycle monitor control has been resetted, or cycle monior control is disabled or not running.
   *  an error is only reported if cycle monitor control has not been reached.
   */
  enum RetCodeCctrl resetCycleMonitor(int partialIntersectionMask)=225;
  
  /**
   * lookup and return the resulting cycle timeline with priority less than the given priority.
   * @return cycle timeline, sorted 
   */
  CycleTimelineCctrl getCycleTimelineWithPriorityLT(int priority)=226;
  
  /**
   * reset a eventually set FaultLock from requested status, for use by control center process.
   * checks if there is a system fault it does not reset the fault lock.
   */
  void resetFaultLock(void)=230;

  /**
   * prepare C10 firmware for shutdown needed to update software.
   * @param useEmergencyOpMode if true try to engage emergency operation mode, if not possible return NOT ready 4 update,...
   * @param sysJobId system job id or transaction number to use to switch OFF/EOM gives a hint why this state has been established. 
   *  it consists of a originator and a sequence number 
   *  see <a href="doc-files/TransactionId.htm">TransactionId doc</a>
   * @return starting with 'ready 4 update' firmware is ready for update (intersection is OFF or in emergency Mode), 
   *  if starting with NOT c10 is NOT ready for update, the rest of the string tells about the reason.
   */
  string prepare4Shutdown2Update(bool useEmergencyOpMode, int sysJobId)=235;
  /**
   * prepare SX for shutdown needed to update traffic actuation software.
   * @param sysJobId system job id or transaction number to use to switch OFF/EOM gives a hint why this state has been established. 
   *  it consists of a originator and a sequence number 
   *  see <a href="doc-files/TransactionId.htm">TransactionId doc</a>
   * @return starting with 'ready 4 update' firmware is ready for update (intersection is OFF or in emergency Mode), 
   *  if starting with NOT c10 is NOT ready for update, the rest of the string tells about the reason.
   */
  string prepare4TaShutdown2Update(int sysJobId)=236;

  /**
   * query licensed features. with step 2 of C10 the following festures are defined:<ul>
   * <li>MoreThan8SG</li>
   * <li>SiemensVA</li>
   * <li>VS-Plus</li>
   * <li>ControlCenterIF</li>
   * <li>IBUTTON_SN returns the serail number of iButton attached at OMC. e.g.:'24 0 0 0 7 B4 69 37 '.<br></br>
   *     In case of invalid iButton/contents 'false' is returned.</li>
   * </ul>
   * @param feature name of feature that's license is to be queried.
   * @return license value of feature, in case feature is NOT licensed <b>false</b> is returned.
   */
  string isFeatureLicensed(string feature)=265;  
  
  /**
   * start/stop/restart control center interface proces(es), needed e.g. by firmware update.
   * @param  startStopRestart
   * @return RC_OK if operation done, 
   */
  enum RetCodeCctrl doControlCenterIf(enum ProcessControl startStopRestart)=270;
  
  /**
   * set TA lock(s) to control traffic actuation blocking of firmware
   * @param host address or hostname of your subscription 
   * @param port UDP port number of your subscription
   * @param locks setup of TA locks
   * @return RC_OK if operation done 
   */
  enum RetCodeCctrl setTaLock(string host, int port, TaLocksCctrl locks)=284;
  
  /**
   * pass AMLi telegrams from TA to firmware for archive and visualization.
   * @param amlis amli telegrams to hand over to firmware to be archived, visualized and sent to control center.
   * @return RC_OK if fw has taken over the telegrams.
   */
  enum RetCodeCctrl processedAMLi(AmliDtosCctrl amlis)=290;
  
  }=8;
}=0x20000202;
