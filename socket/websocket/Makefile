CC = gcc
CFLAGS = -g -Idep
RM = rm -rf

all : main

main : main.o dep/http_parser.o dep/base64_enc.o dep/sha1.o
	$(CC) -o $@ $^

clean : 
	$(RM) *.o dep/*.o main
