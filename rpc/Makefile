
# This is a template Makefile generated by rpcgen

# Parameters

CLIENT = minmax_client
SERVER = minmax_server

MAXDATA ?= 500000

SOURCES_CLNT.c = 
SOURCES_CLNT.h = 
SOURCES_SVC.c = 
SOURCES_SVC.h = 
SOURCES.x = minmax.x

TARGETS_SVC.c = minmax_svc.c minmax_server.c minmax_xdr.c 
TARGETS_CLNT.c = minmax_clnt.c minmax_client.c minmax_xdr.c error.c
TARGETS = minmax.h minmax_xdr.c minmax_clnt.c minmax_svc.c minmax_client.c minmax_server.c

OBJECTS_CLNT = $(SOURCES_CLNT.c:%.c=%.o) $(TARGETS_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(SOURCES_SVC.c:%.c=%.o) $(TARGETS_SVC.c:%.c=%.o)
# Compiler flags 

CFLAGS += -g -lm -pthread -I/usr/include/tirpc/ -D MAXDATA=$(MAXDATA)
LDLIBS += -lnsl -ltirpc
RPCGENFLAGS = -b

# Targets 

all : $(CLIENT) $(SERVER)

timeit: $(CLIENT)
	time (./$(CLIENT) $(ARGS))

$(TARGETS) : $(SOURCES.x) 
	rpcgen $(RPCGENFLAGS) $(SOURCES.x)

$(OBJECTS_CLNT) : $(SOURCES_CLNT.c) $(SOURCES_CLNT.h) $(TARGETS_CLNT.c) 

$(OBJECTS_SVC) : $(SOURCES_SVC.c) $(SOURCES_SVC.h) $(TARGETS_SVC.c) 

$(CLIENT) : $(OBJECTS_CLNT) 
	$(LINK.c) -o $(CLIENT) $(OBJECTS_CLNT) $(LDLIBS)

$(SERVER) : $(OBJECTS_SVC) 
	$(LINK.c) -o $(SERVER) $(OBJECTS_SVC) $(LDLIBS)

clean:
	 $(RM) minmax_client minmax_client.o

