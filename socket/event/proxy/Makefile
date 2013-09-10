CFLAGS = -g -D_REENTRANT -Wall -pedantic -Ievthr/ -Isrc/ -DDEBUG
LDFLAGS = -L/usr/local/lib -levent -levent_pthreads -lcurl
CC = gcc

TARGETS = server client
OBJS = src/server.o src/client.o evthr/evthr.o

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
	$(RM) $(OBJS)
