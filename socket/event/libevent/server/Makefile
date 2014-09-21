CC=gcc
INCLUDE_PATH=$(JUMBO_ROOT)/include
LIBRARY_PATH=$(JUMBO_ROOT)/lib
C_FLAGS=-I$(INCLUDE_PATH) -g
LD_FLAGS=-L$(LIBRARY_PATH) -levent
SOURCES=$(wildcard *.c)
OBJS=$(patsubst %c, %o, $(SOURCES))
OBJ=server
RM=rm -rf


all: $(OBJ)

$(OBJ): $(OBJS)
	$(CC) $(LD_FLAGS) $(OBJS) -o $@

%.o: %.c
	$(CC) $(C_FLAGS) -c $^
clean:
	$(RM) $(OBJS) $(OBJ)
