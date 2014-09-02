CC=gcc
INCLUDE_PATH=$(JUMBO_ROOT)/include
LIBRARY_PATH=$(JUMBO_ROOT)/lib
C_FLAGS=-I$(INCLUDE_PATH)
LD_FLAGS=-L$(LIBRARY_PATH) -levent
SOURCES=$(wildcard *.c)
OBJS=$(patsubst %c, %o, $(SOURCES))

all: server

server: $(OBJS)
	$(CC) $(LD_FLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(C_FLAGS) -c $^
clean:
	rm -rf *.o
	rm -rf server
