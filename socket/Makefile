CFLAGS = -lm -pthread
FILES = model.h error.h error.c message.c

MAX ?= 500000
CARGS ?= 1 127.0.0.1 7777
SARGS ?= 127.0.0.1 7777

RUN_CLIENT = ./client $(CARGS)
RUN_SERVER = ./server $(SARGS)

all: server client

run_server: server
	$(RUN_SERVER)

run_client: client
	$(RUN_CLIENT)

timeit: client
	time ($(RUN_CLIENT))

client: $(FILES) client.c
	$(CC) $(CFLAGS) $^ -o $@ -D MAX=$(MAX)

server: $(FILES) server.c
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) client server