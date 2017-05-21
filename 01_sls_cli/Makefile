#CFLAGS=-g -Wall -W -I../../..
CFLAGS=-g -Wall -W 

sls_cli: sls_cli.o net.o noerr.o


#net.o: ../net.c
net.o: net.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<

#noerr.o: ../../noerr/noerr.c
noerr.o: noerr.c
	$(COMPILE.c) $(OUTPUT_OPTION) $<
