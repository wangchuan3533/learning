CFLAGS = -DDEBUG -D_REENTRANT -Wall -pedantic -Idep/
LDFLAGS = -levent -levent_pthreads
#LDFLAGS = -levent 
CC = gcc

ifdef DEBUG
CFLAGS += -g
LDFLAGS += -g
endif

TARGETS = server

all: $(TARGETS)

server: src/server.o dep/threadpool.o
	$(CC) $(LDFLAGS) -o $@ $^
src/server.o : src/server.c
dep/threadpool.o : dep/threadpool.c
dep/thpool.o : dep/thpool.c

clean:
	$(RM) $(TARGETS)
