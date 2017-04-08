#################################################################
#
# Makefile for VT-VKx
#
#################################################################

#################################################################
# cross builds include file crosscompile.env at current directory!
# for native target builds no file crosscompile.env shall exist in
# current directory.
#################################################################
-include crosscompile.env

#################################################################
# default build is Release!
#################################################################
ifndef CFG
CFG=Release
else
CFG=Debug
endif

#################################################################
#
# common settings
#
#################################################################

PLATFORM := $(notdir $(shell pwd))
export NO_CLEARTOOL := 1

ifeq ($(DO_DIR)X,X)
	DO_DIR = ../do/
endif

DO_SUBDIR = rpm-tdsx/target

#################################################################
# common tool settings
#################################################################

LD=$(CXX)
RM=rm -f

ifeq (X$(RPCGEN), X)
RPCGEN := rpcgen
endif

#################################################################
# common flags
#################################################################

RPCGENFLAGS := -C -N -M

WARNFLAGS= -Wextra -Weffc++ -Woverloaded-virtual -Wsign-promo -Wmissing-format-attribute -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wconversion -Wcast-qual
COMMONCFLAGS = -Wall -I. -I../src -I../include -I../extern/$(PLATFORM)/include
CPPFLAGS = -std=c++11

LDFLAGS += -L../extern/$(PLATFORM)/lib
LIBS	= -levent -lpthread 

ifneq ($(SYSROOT)X,X)
COMMONCFLAGS+=--sysroot=$(SYSROOT)
endif

#################################################################
# versions
#################################################################

TARGET_VERSION=0

#################################################################
# dirs
#################################################################

TARGET_TAC_PDMX_DIR=opt/ext/tac/pdmx
TARGET_HTTP_DIR=opt/httpd
TARGET_TAC_HTTP_DIR=$(TARGET_HTTP_DIR)/tac
TARGET_CFG_INVENTORY_DIR=opt/jappl/data/config/inventory

ifdef DEST_LIB_DIR
INSTALL_DIR = $(DEST_LIB_DIR)/lib
else
INSTALL_DIR = ../lib/$(PLATFORM)
endif

#################################################################
# Release version settings
#################################################################
ifeq "$(CFG)"  "Release"
ifdef DEST_DIR 
OUT_DIR:=$(DEST_DIR)/rel
else
OUT_DIR:=rel
endif
CFLAGS+=-g -O2 -finline-functions -fpic -DNDEBUG $(COMMONCFLAGS)

else
#################################################################
# Debug version settings
#################################################################
ifeq "$(CFG)"  "Debug"
ifdef DEST_DIR
OUT_DIR:=$(DEST_DIR)/work
else
OUT_DIR:=work
endif
CFLAGS+=-g -O0 -fpic -D_DEBUG -DVERBOSE $(COMMONCFLAGS)

else
#################################################################
# abort built on selecting an unknown configuration
#################################################################
$(error Invalid configuration "$(CFG)" specified: supported is only "Debug"|"Release"!)
endif
endif

CXXFLAGS += $(CPPFLAGS) $(CFLAGS) $(WARNFLAGS)

#################################################################
# define target dependent settings
#################################################################

TEST := $(OUT_DIR)/tacKernelDemo
DLL := tackernel

ifeq ($(PLATFORM), mingw64)
	CFLAGS += -DWIN32 -DONCRPCDLL -I /$(PLATFORM)/include -D__USE_MINGW_ANSI_STDIO=1 -mno-ms-bitfields
	LIBS += -loncrpc -lws2_32
	LDDFLAGS = $(LDFLAGS) --shared -Wl,-soname=lib$(DLL).dll
	TARGET := lib$(DLL).dll
ifeq ($(CC), cc)
	CC:= gcc
endif
else
	LDDFLAGS = $(LDFLAGS) --shared -Wl,-soname=lib$(DLL).so.$(TARGET_VERSION)
	TARGET := lib$(DLL).so
endif

#################################################################
# setup searchpath for sources and headers
#################################################################

vpath %cpp ../src/:../src/start:../src/cctrl:../src/mv:../src/util
vpath %c .
vpath %x .

#################################################################
#  source and header files
#################################################################

SOURCE_FILES := $(shell find ../src -mindepth 2 -type f -name '*.c' -o -name '*.cpp')

HEADER_FILES := $(shell find ../include ../src -type f -name '*.h')

TEST_SOURCES = ../src/TA_main.cpp

RPC_SOURCES = CControl_xdr.c CControl_clnt.c CControl_svc.c

DEPENDS = $(OUT_DIR)/$(DLL).d

#################################################################
# default rule!
#################################################################

.PHONY: all
all: bin install 

#################################################################
# static pattern rules
#################################################################

$(OUT_DIR)/CControl_xdr.o: CControl_xdr.c
	$(CC) $(CFLAGS) -Wno-unused-variable -o $@ -c $<

$(OUT_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $<

$(OUT_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<

#################################################################
# create list of objects to build
#################################################################

OBJS=$(addprefix $(OUT_DIR)/, $(notdir $(patsubst %.cpp,%.o, $(filter %.cpp, $(SOURCE_FILES)))))
OBJS+=$(addprefix $(OUT_DIR)/, $(notdir $(patsubst %.c,%.o, $(filter %.c, $(SOURCE_FILES)))))
RPC_OBJS=$(addprefix $(OUT_DIR)/, $(notdir $(patsubst %.c,%.o, $(filter %.c, $(RPC_SOURCES)))))
TEST_OBJS=$(addprefix $(OUT_DIR)/, $(notdir $(patsubst %.cpp,%.o, $(filter %.cpp, $(TEST_SOURCES)))))

#################################################################
# rules section
#################################################################

$(OUT_DIR):
	-mkdir $(OUT_DIR)
	
$(INSTALL_DIR):
	-mkdir -p $(INSTALL_DIR)
	
#----------------------------------
# RPC source generation
#----------------------------------

CControl.h : CControl.x
	-rm -f $@
	$(RPCGEN) $(RPCGENFLAGS) -h -o CControl.h CControl.x

CControl_xdr.c : CControl.x CControl.h
	-rm -f $@
	$(RPCGEN) $(RPCGENFLAGS) -c -o CControl_xdr.c CControl.x

CControl_clnt.c : CControl.x CControl.h
	-rm -f $@;
	$(RPCGEN) $(RPCGENFLAGS) -l -o CControl_clnt.c CControl.x

# don't generate CcontrolService skeleton
CControl_svc.c : CControl.x CControl.h
	- rm -f $@;
	cat CControl.x|awk '/program CcontrolService/ {matched=1; next} 1==matched {next} {print}'>CControl.xx;$(RPCGEN) $(RPCGENFLAGS) -m -o CControl_svc.c CControl.xx

#----------------------------------
# prepare dir tree for rpm
#----------------------------------
rpm_dirs:
	rm -rf   $(DO_DIR)/$(DO_SUBDIR)/
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/bin
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/cfg/nls
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/lib
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/log
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_HTTP_DIR)/js/chart
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_HTTP_DIR)/js/tac
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_HTTP_DIR)/expert/pdmx
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_HTTP_DIR)/monitor/pdmx
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_HTTP_DIR)/operate/pdmx
	mkdir -p $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_HTTP_DIR)/service/pdmx

#----------------------------------
# target rules
#----------------------------------

$(DEPENDS): CControl.h $(SOURCE_FILES) $(TEST_SOURCES) $(HEADER_FILES)
	-$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $(SOURCE_FILES) $(TEST_SOURCES) $(HEADER_FILES) | sed 's:^\w:$(OUT_DIR)\/&:' > $(DEPENDS)

$(OUT_DIR)/$(TARGET): $(DEPENDS) $(RPC_SOURCES) $(OBJS) $(RPC_OBJS)
	@echo "Linking $@ ..."
	$(LD) $(LDDFLAGS) -o $@ $(OBJS) $(RPC_OBJS) $(LIBS)
	@echo "done"

$(TEST):  $(OUT_DIR)/$(TARGET) $(TEST_OBJS)
	@echo "Linking $@ ..."
	$(LD) $(LDFLAGS) -L$(OUT_DIR) -o $@ $(TEST_OBJS) -l$(DLL) $(LIBS)
	@echo "done"

$(INSTALL_DIR)/$(TARGET): $(OUT_DIR)/$(TARGET)
	install $(OUT_DIR)/$(TARGET) $(INSTALL_DIR)

prepare_rpm: bin rpm_dirs
	cp -f  ../start.sh $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/
	ln -s  /$(TARGET_CFG_INVENTORY_DIR)/pdmx.cod $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/bin/pdmx
	cp -f  ../cfg/profile_valib.txt $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/cfg/
	cp -rf ../cfg/nls/* $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/cfg/nls/
	cp -f  $(OUT_DIR)/$(TARGET) $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/lib/
	ln -s  $(TARGET) $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_PDMX_DIR)/lib/$(TARGET).$(TARGET_VERSION)
	cp -f  ../httpd/js/chart/Chart.js $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_HTTP_DIR)/js/chart/
	cp -f  ../httpd/js/tac/TestAndDiagnostics.js $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_HTTP_DIR)/js/tac/
	cp -f  ../httpd/tac/expert/expert.html $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_HTTP_DIR)/expert/pdmx/
	cp -f  ../httpd/tac/monitor/1f_Fehlerhandbuch_C800_C900_A012_dt.pdf $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_HTTP_DIR)/monitor/pdmx/
	cp -f  ../httpd/tac/monitor/1f_Fehlerhandbuch_C800_C900_A012_en.pdf $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_HTTP_DIR)/monitor/pdmx/
	cp -f  ../httpd/tac/operate/operate.html $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_HTTP_DIR)/operate/pdmx/
	cp -f  ../httpd/tac/service/service.html $(DO_DIR)/$(DO_SUBDIR)/$(TARGET_TAC_HTTP_DIR)/service/pdmx/

#----------------------------------
# symbolic rules
#----------------------------------

.PHONY: lib bin clean depend install
lib: $(OUT_DIR) $(OUT_DIR)/$(TARGET)

bin: $(OUT_DIR) $(TEST)

clean:
	-$(RM) $(OUT_DIR)/*.o $(TEST) $(OUT_DIR)/$(TARGET) $(RPC_SOURCES) CControl.h CControl.xx $(DEPENDS)

depend: $(OUT_DIR) $(DEPENDS)

install: lib $(INSTALL_DIR) $(INSTALL_DIR)/$(TARGET)


#################################################################
# include generated dependencies
#################################################################
ifneq ($(MAKECMDGOALS), clean)
    -include $(DEPENDS)
endif
