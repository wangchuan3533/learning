CFLAGS = -g -D_REENTRANT -Wall -pedantic -I$(PREFIX)/include -Idep -Idep/evthr/ -Isrc/ -DDEBUG
LDFLAGS = -L$(PREFIX)/lib -levent -levent_pthreads -lcurl
CC = gcc
INSTALL = /usr/bin/install

SERVER = customerlive_tcp_server
SCRIPTS = scripts/customerlive scripts/customerlive_env

TARGETS = $(SERVER) client
OBJS = src/server.o src/client.o dep/evthr/evthr.o dep/sha1.o dep/http_parser.o dep/base64_enc.o

all: $(TARGETS)

$(SERVER): src/server.o dep/evthr/evthr.o dep/http_parser.o dep/sha1.o dep/base64_enc.o
	$(CC) $(LDFLAGS) -o $@ $^
client: src/client.o
	$(CC) -o $@ $^

clean:
	$(RM) $(TARGETS)
	$(RM) $(OBJS)
install:
	$(INSTALL) -d $(PREFIX)/bin
	$(INSTALL) -d $(PREFIX)/var/log
	$(INSTALL) -d $(PREFIX)/var/tmp
	$(INSTALL) $(SERVER) $(PREFIX)/bin
	$(INSTALL) $(SCRIPTS) $(PREFIX)/bin

