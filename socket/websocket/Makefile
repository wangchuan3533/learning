CC = gcc
CFLAGS = -g -Idep

all : main

main : main.o dep/http_parser.o dep/base64_enc.o dep/sha1.o
	$(CC) -o $@ $^
