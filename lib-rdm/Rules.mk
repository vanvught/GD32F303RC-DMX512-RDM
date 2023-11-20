EXTRA_INCLUDES=../lib-rdmsensor/include ../lib-rdmsubdevice/include ../lib-dmx/include ../lib-properties/include ../lib-lightset/include
EXTRA_INCLUDES+=../lib-hal/include ../lib-network/include ../lib-display/include 

ifneq ($(MAKE_FLAGS),)
	ifeq (,$(findstring NODE_ARTNET,$(MAKE_FLAGS)))
  	ifeq ($(findstring RDM_RESPONDER,$(MAKE_FLAGS)), RDM_RESPONDER)
  		EXTRA_SRCDIR+= src/responder
  		EXTRA_INCLUDES+=../lib-dmxreceiver/include
  	endif
	endif
	
	ifneq (,$(findstring NODE_RDMNET_LLRP_ONLY,$(MAKE_FLAGS)))
 		EXTRA_SRCDIR += src/llrp
	endif
else
	ifeq (, $(wildcard $('../lib-network/src/noemac')))
		DEFINES+=NO_EMAC
	else
		EXTRA_SRCDIR+=src/llrp
	endif
	EXTRA_INCLUDES+=../lib-dmxreceiver/include
	DEFINES+=ENABLE_RDM_MANUFACTURER_PIDS
	DEFINES+=RDM_RESPONDER
	EXTRA_SRCDIR+=src/responder
endif