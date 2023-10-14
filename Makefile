CC=clang
CFLAGS=-g -O0 -Wall -Wextra -Werror -O2 -std=c99 -pedantic

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS)

lexer: lexer.o
	$(CC) ./lexer.c -o lexer
	./lexer
