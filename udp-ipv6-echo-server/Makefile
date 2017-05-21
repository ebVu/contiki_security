UIP_CONF_IPV6=1
UIP_CONF_RPL=1

CONTIKI_PROJECT = udp-echo-server test

all: $(CONTIKI_PROJECT)

CONTIKI = ../../..
TARGET_LIBFILES += -L. -laes
include $(CONTIKI)/Makefile.include
