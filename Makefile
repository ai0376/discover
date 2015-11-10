TGT_C = client
TGT_S = server

CC = gcc
#CC = arm-linux-gnueabihf-gcc
DIR = ./src/

SRC_C = $(DIR)udpclient.c
SRC_S = $(DIR)udpserver.c
SRC_COMMON = $(DIR)udpcommon.c

OBJS_C = $(DIR)udpclient.o
OBJS_S = $(DIR)udpserver.o
OBJS_COMMON = $(DIR)udpcommon.o

all:clean $(TGT_C) $(TGT_S)

$(TGT_C): $(OBJS_C) $(OBJS_COMMON)
	$(CC) -o $@ $^
	
$(TGT_S): $(OBJS_S) $(OBJS_COMMON)
	$(CC) -o $@ $^

%o:%c
	$(CC) -c -o $@ $< 		
clean:
	@echo "--- Cleaning bin ---"
	$(RM) -rf $(DIR)*.o $(TGT_C) $(TGT_S)
