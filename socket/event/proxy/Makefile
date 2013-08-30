CFLAGS = -g -D_REENTRANT -Wall -pedantic -Ievthr/
LDFLAGS = -levent -levent_pthreads
#LDFLAGS = -levent 
CC = gcc

TARGETS = server client

all: $(TARGETS)

server: src/server.o evthr/evthr.o
	$(CC) $(LDFLAGS) -o $@ $^
client: src/client.o
	$(CC) -o $@ $^
src/server.o : src/server.c
src/client.o : src/client.c
evthr/evthr.o : evthr/evthr.c

clean:
	$(RM) $(TARGETS)
