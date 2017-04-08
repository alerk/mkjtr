rel/api_signalling.o: ../src/start/api_signalling.cpp \
 ../src/start/awpWrapper.h ../src/start/modonoffhelper.h CControl.h \
 ../src/util/what.h ../include/tac_kernel.h ../include/def_ger.h \
 ../include/instregistry.h ../src/cctrl/Config.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/util/repdef.h \
 ../src/util/report.h
rel/modonoffhelper.o: ../src/start/modonoffhelper.cpp CControl.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/util/what.h \
 ../include/instregistry.h ../include/DataReceiver.h \
 ../src/start/modonoffhelper.h ../src/util/repdef.h ../src/util/report.h
rel/TAC_main.o: ../src/start/TAC_main.cpp ../include/DataReceiver.h \
 CControl.h ../src/util/what.h ../include/evhttpadapter.h \
 ../include/instregistry.h ../src/mv/Detectors.h ../include/tac_kernel.h \
 ../include/def_ger.h ../src/cctrl/htmlHandler.h \
 ../src/start/awpWrapper.h ../src/start/modonoffhelper.h \
 ../src/cctrl/cltDeviceValues.h ../src/cctrl/PublicTransport.h \
 ../src/util/CControlEnums.h ../src/cctrl/Archive.h ../include/api_va.h \
 ../include/tac_kernel.h ../src/cctrl/LampStates.h ../src/util/repdef.h \
 ../src/util/report.h ../src/start/api_TaRunmode.h \
 ../src/util/TestAndDiagnostics.h ../src/util/PublicTransportHttp.h
rel/awpWrapper.o: ../src/start/awpWrapper.cpp ../src/start/awpWrapper.h \
 ../src/start/modonoffhelper.h CControl.h ../src/util/what.h \
 ../include/tac_kernel.h ../include/def_ger.h ../include/instregistry.h \
 ../include/DataReceiver.h ../src/cctrl/Config.h \
 ../src/cctrl/cltDeviceValues.h ../src/cctrl/wrappedCctrlDtos.h \
 ../src/cctrl/PublicTransport.h ../src/util/CControlEnums.h \
 ../src/start/statistics.h ../src/util/repdef.h ../src/util/report.h \
 ../src/util/StateMessage.h ../include/evhttpadapter.h \
 ../src/cctrl/api_operating.h
rel/api_time.o: ../src/start/api_time.cpp ../include/instregistry.h \
 ../src/util/what.h ../src/cctrl/wrappedCctrlDtos.h CControl.h \
 ../src/util/repdef.h ../src/util/report.h ../src/start/tac_time.h \
 ../include/tac_kernel.h ../include/def_ger.h
rel/api_TaRunmode.o: ../src/start/api_TaRunmode.cpp CControl.h \
 ../src/start/awpWrapper.h ../src/start/modonoffhelper.h \
 ../src/util/what.h ../include/tac_kernel.h ../include/def_ger.h \
 ../include/instregistry.h ../src/cctrl/Config.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/util/repdef.h \
 ../src/util/report.h ../src/start/api_TaRunmode.h \
 ../include/evhttpadapter.h
rel/CControlEnums.o: ../src/util/CControlEnums.cpp \
 ../src/util/CControlEnums.h CControl.h ../src/util/what.h
rel/PublicTransportHttp.o: ../src/util/PublicTransportHttp.cpp \
 ../src/util/what.h ../include/instregistry.h ../src/cctrl/htmlHandler.h \
 ../include/evhttpadapter.h ../include/tac_kernel.h ../include/def_ger.h \
 ../src/cctrl/PublicTransport.h ../src/util/CControlEnums.h CControl.h \
 ../src/util/TacEnums.h ../src/util/PublicTransportHttp.h
rel/TestAndDiagnostics.o: ../src/util/TestAndDiagnostics.cpp \
 ../src/util/json.hpp ../include/instregistry.h ../src/util/what.h \
 CControl.h ../src/cctrl/cltDeviceValues.h ../include/tac_kernel.h \
 ../include/def_ger.h ../src/util/repdef.h ../src/util/report.h \
 ../src/util/TacEnums.h ../src/util/TestAndDiagnostics.h \
 ../include/evhttpadapter.h
rel/TacDemoHttp.o: ../src/util/TacDemoHttp.cpp ../src/util/what.h \
 ../src/util/TacDemoHttp.h ../include/evhttpadapter.h
rel/TacEnums.o: ../src/util/TacEnums.cpp ../src/util/TacEnums.h \
 ../include/tac_kernel.h ../include/def_ger.h ../src/util/what.h
rel/reportDialog.o: ../src/util/reportDialog.cpp CControl.h \
 ../include/instregistry.h ../src/util/what.h ../include/DataReceiver.h \
 ../include/evhttpadapter.h ../src/cctrl/htmlHandler.h \
 ../include/tac_kernel.h ../include/def_ger.h ../src/cctrl/Config.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/util/StateMessage.h \
 ../src/util/repdef.h ../src/util/report.h
rel/StateMessage.o: ../src/util/StateMessage.cpp CControl.h \
 ../include/instregistry.h ../src/util/what.h ../include/DataReceiver.h \
 ../include/evhttpadapter.h ../src/cctrl/htmlHandler.h \
 ../include/tac_kernel.h ../include/def_ger.h ../src/cctrl/Config.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/cctrl/cltDeviceValues.h \
 ../src/util/TestAndDiagnostics.h ../src/util/StateMessage.h \
 ../src/util/repdef.h ../src/util/report.h
rel/instregistry.o: ../src/util/instregistry.cpp ../include/instregistry.h \
 ../src/util/what.h
rel/evDispatchThread.o: ../src/util/evDispatchThread.cpp \
 ../include/evhttpadapter.h ../src/util/what.h ../include/instregistry.h \
 ../src/util/report.h
rel/evhttpadapter.o: ../src/util/evhttpadapter.cpp ../include/evhttpadapter.h \
 ../src/util/what.h ../extern/linux_hlc/include/event2/event.h \
 ../extern/linux_hlc/include/event2/event-config.h \
 ../extern/linux_hlc/include/event2/util.h \
 ../extern/linux_hlc/include/event2/event_struct.h \
 ../extern/linux_hlc/include/event2/keyvalq_struct.h \
 ../extern/linux_hlc/include/event2/http.h \
 ../extern/linux_hlc/include/event2/http_struct.h \
 ../extern/linux_hlc/include/event2/buffer.h
rel/report.o: ../src/util/report.cpp ../src/util/what.h ../src/util/props.h \
 ../src/util/report.h ../src/util/TestAndDiagnostics.h \
 ../include/evhttpadapter.h
rel/what.o: ../src/util/what.cpp ../include/evhttpadapter.h \
 ../src/util/what.h ../src/cctrl/htmlHandler.h ../include/tac_kernel.h \
 ../include/def_ger.h
rel/Detectors.o: ../src/mv/Detectors.cpp ../src/mv/Detectors.h CControl.h \
 ../include/tac_kernel.h ../include/def_ger.h ../src/util/what.h \
 ../include/instregistry.h ../src/cctrl/Config.h \
 ../include/evhttpadapter.h ../src/cctrl/htmlHandler.h \
 ../src/util/repdef.h ../src/util/report.h
rel/api_mv.o: ../src/mv/api_mv.cpp ../src/mv/Detectors.h CControl.h \
 ../include/tac_kernel.h ../include/def_ger.h ../src/util/what.h \
 ../include/instregistry.h ../include/DataReceiver.h ../src/util/repdef.h \
 ../src/util/report.h ../include/evhttpadapter.h
rel/Config.o: ../src/cctrl/Config.cpp ../src/cctrl/Config.h CControl.h \
 ../include/tac_kernel.h ../include/def_ger.h ../src/util/what.h \
 ../src/cctrl/htmlHandler.h ../include/evhttpadapter.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/cctrl/Archive.h \
 ../include/api_va.h ../include/tac_kernel.h ../src/cctrl/LampStates.h \
 ../include/instregistry.h ../include/DataReceiver.h \
 ../src/mv/Detectors.h ../src/util/repdef.h ../src/util/report.h \
 ../src/util/report.h
rel/ConfigNotificationImpl.o: ../src/cctrl/ConfigNotificationImpl.cpp \
 ../src/cctrl/Config.h CControl.h ../include/tac_kernel.h \
 ../include/def_ger.h ../src/util/what.h ../include/instregistry.h \
 ../include/DataReceiver.h ../src/cctrl/ConfigNotificationImpl.h \
 ../src/mv/Detectors.h ../src/util/repdef.h ../src/util/report.h
rel/DataReceiver.o: ../src/cctrl/DataReceiver.cpp ../include/DataReceiver.h \
 CControl.h ../src/util/what.h ../src/cctrl/Config.h \
 ../include/tac_kernel.h ../include/def_ger.h ../src/mv/Detectors.h \
 ../src/util/repdef.h ../src/util/report.h ../include/instregistry.h \
 ../src/cctrl/LocalResolv.h ../src/cctrl/ConfigNotificationImpl.h
rel/CControl_server.o: ../src/cctrl/CControl_server.cpp CControl.h \
 ../src/cctrl/Archive.h ../include/api_va.h ../include/def_ger.h \
 ../include/tac_kernel.h ../src/cctrl/LampStates.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/util/what.h \
 ../include/instregistry.h ../src/mv/Detectors.h ../include/tac_kernel.h \
 ../src/start/awpWrapper.h ../src/start/modonoffhelper.h \
 ../src/util/repdef.h ../src/util/report.h ../src/cctrl/cltDeviceValues.h \
 ../src/util/StateMessage.h ../src/cctrl/nlsPropertyProvider.h \
 ../src/cctrl/PublicTransport.h ../src/util/CControlEnums.h \
 ../src/cctrl/ConfigNotificationImpl.h
rel/wrappedCctrlDtos.o: ../src/cctrl/wrappedCctrlDtos.cpp \
 ../src/cctrl/wrappedCctrlDtos.h CControl.h ../src/util/what.h \
 ../src/util/repdef.h ../src/util/report.h
rel/nlsPropertyProvider.o: ../src/cctrl/nlsPropertyProvider.cpp \
 ../src/cctrl/nlsPropertyProvider.h CControl.h ../src/util/what.h \
 ../src/util/repdef.h ../src/util/report.h
rel/Archive.o: ../src/cctrl/Archive.cpp ../src/cctrl/Archive.h CControl.h \
 ../include/api_va.h ../include/def_ger.h ../include/tac_kernel.h \
 ../src/cctrl/LampStates.h ../src/start/awpWrapper.h \
 ../src/start/modonoffhelper.h ../src/util/what.h ../include/tac_kernel.h \
 ../include/instregistry.h ../src/cctrl/Config.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/util/repdef.h \
 ../src/util/report.h
rel/LampStates.o: ../src/cctrl/LampStates.cpp ../src/cctrl/LampStates.h
rel/api_operating.o: ../src/cctrl/api_operating.cpp ../src/start/awpWrapper.h \
 ../src/start/modonoffhelper.h CControl.h ../src/util/what.h \
 ../include/tac_kernel.h ../include/def_ger.h ../include/instregistry.h \
 ../src/cctrl/Config.h ../src/cctrl/wrappedCctrlDtos.h \
 ../src/util/repdef.h ../src/util/report.h ../include/DataReceiver.h
rel/cltDeviceValues.o: ../src/cctrl/cltDeviceValues.cpp \
 ../include/DataReceiver.h CControl.h ../src/util/what.h \
 ../src/util/repdef.h ../src/util/report.h ../src/cctrl/cltDeviceValues.h \
 ../include/tac_kernel.h ../include/def_ger.h ../include/instregistry.h \
 ../src/util/TestAndDiagnostics.h ../include/evhttpadapter.h \
 ../src/cctrl/Config.h
rel/api_Config.o: ../src/cctrl/api_Config.cpp ../include/instregistry.h \
 ../src/util/what.h ../src/cctrl/Config.h CControl.h \
 ../include/tac_kernel.h ../include/def_ger.h ../include/DataReceiver.h \
 ../src/cctrl/cltDeviceValues.h ../src/util/repdef.h ../src/util/report.h \
 ../src/cctrl/ConfigNotificationImpl.h
rel/api_PublicTransport.o: ../src/cctrl/api_PublicTransport.cpp \
 ../src/start/awpWrapper.h ../src/start/modonoffhelper.h CControl.h \
 ../src/util/what.h ../include/tac_kernel.h ../include/def_ger.h \
 ../include/instregistry.h ../src/util/repdef.h ../src/util/report.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/cctrl/PublicTransport.h \
 ../src/util/CControlEnums.h ../src/util/TacEnums.h \
 ../src/start/tac_time.h
rel/LocalResolv.o: ../src/cctrl/LocalResolv.cpp ../src/cctrl/LocalResolv.h \
 ../src/util/what.h
rel/htmlHandler.o: ../src/cctrl/htmlHandler.cpp ../src/cctrl/htmlHandler.h \
 ../include/evhttpadapter.h ../src/util/what.h ../include/tac_kernel.h \
 ../include/def_ger.h ../include/instregistry.h CControl.h \
 ../src/cctrl/wrappedCctrlDtos.h ../src/cctrl/Config.h
rel/TA_main.o: ../src/TA_main.cpp ../include/instregistry.h \
 ../src/util/what.h ../src/mv/Detectors.h CControl.h \
 ../include/tac_kernel.h ../include/def_ger.h \
 ../src/cctrl/cltDeviceValues.h ../src/util/repdef.h ../src/util/report.h \
 ../src/util/TestAndDiagnostics.h ../include/evhttpadapter.h \
 ../src/util/TacDemoHttp.h ../src/util/what.h
rel/api_va.o: ../include/api_va.h ../include/def_ger.h \
 ../include/tac_kernel.h
rel/instregistry.o: ../include/instregistry.h ../src/util/what.h
rel/tac_kernel.o: ../include/tac_kernel.h ../include/def_ger.h
rel/def_ger.o: ../include/def_ger.h
rel/evhttpadapter.o: ../include/evhttpadapter.h ../src/util/what.h
rel/DataReceiver.o: ../include/DataReceiver.h CControl.h ../src/util/what.h
rel/modonoffhelper.o: ../src/start/modonoffhelper.h CControl.h \
 ../src/util/what.h
rel/awpWrapper.o: ../src/start/awpWrapper.h ../src/start/modonoffhelper.h \
 CControl.h ../src/util/what.h ../include/tac_kernel.h \
 ../include/def_ger.h
rel/api_TaRunmode.o: ../src/start/api_TaRunmode.h ../include/evhttpadapter.h \
 ../src/util/what.h
rel/statistics.o: ../src/start/statistics.h ../src/util/what.h
rel/tac_time.o: ../src/start/tac_time.h ../src/util/what.h
rel/StateMessage.o: ../src/util/StateMessage.h ../src/util/what.h
rel/props.o: ../src/util/props.h ../src/util/what.h
rel/report.o: ../src/util/report.h ../src/util/what.h
rel/TacDemoHttp.o: ../src/util/TacDemoHttp.h ../include/evhttpadapter.h \
 ../src/util/what.h
rel/TestAndDiagnostics.o: ../src/util/TestAndDiagnostics.h \
 ../include/evhttpadapter.h ../src/util/what.h
rel/repdef.o: ../src/util/repdef.h ../src/util/report.h ../src/util/what.h
rel/CControlEnums.o: ../src/util/CControlEnums.h CControl.h \
 ../src/util/what.h
rel/PublicTransportHttp.o: ../src/util/PublicTransportHttp.h \
 ../include/evhttpadapter.h ../src/util/what.h
rel/what.o: ../src/util/what.h
rel/TacEnums.o: ../src/util/TacEnums.h ../include/tac_kernel.h \
 ../include/def_ger.h ../src/util/what.h
rel/Detectors.o: ../src/mv/Detectors.h CControl.h ../include/tac_kernel.h \
 ../include/def_ger.h ../src/util/what.h
rel/api_operating.o: ../src/cctrl/api_operating.h
rel/cltDeviceValues.o: ../src/cctrl/cltDeviceValues.h ../include/tac_kernel.h \
 ../include/def_ger.h ../src/util/what.h
rel/Config.o: ../src/cctrl/Config.h CControl.h ../include/tac_kernel.h \
 ../include/def_ger.h ../src/util/what.h
rel/LampStates.o: ../src/cctrl/LampStates.h
rel/nlsPropertyProvider.o: ../src/cctrl/nlsPropertyProvider.h CControl.h \
 ../src/util/what.h
rel/htmlHandler.o: ../src/cctrl/htmlHandler.h ../include/evhttpadapter.h \
 ../src/util/what.h ../include/tac_kernel.h ../include/def_ger.h
rel/PublicTransport.o: ../src/cctrl/PublicTransport.h \
 ../src/util/CControlEnums.h CControl.h ../src/util/what.h \
 ../include/tac_kernel.h ../include/def_ger.h
rel/Archive.o: ../src/cctrl/Archive.h CControl.h ../include/api_va.h \
 ../include/def_ger.h ../include/tac_kernel.h ../src/cctrl/LampStates.h
rel/LocalResolv.o: ../src/cctrl/LocalResolv.h ../src/util/what.h
rel/wrappedCctrlDtos.o: ../src/cctrl/wrappedCctrlDtos.h CControl.h \
 ../src/util/what.h
rel/ConfigNotificationImpl.o: ../src/cctrl/ConfigNotificationImpl.h \
 ../include/tac_kernel.h ../include/def_ger.h CControl.h
