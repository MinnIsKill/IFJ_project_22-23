#Makefile
# this will defenetly change, just for testing

BIN=compiler
CC=gcc
CFLAGS=-std=gnu11 -g 
DBGFLAGS=-DINFO -DDEBUG
ERFLAGS=-Werror -Wall -Wextra -pedantic

all: $(BIN) $(BIN)_dbg

$(BIN): lex ./obj/fake_lex.o
	$(CC) $(CFLAGS) $(ERFLAGS) ./src/char_buffer.c ./src/lex.c ./src/token.c ./src/ast_stack.c ./src/ast.c ./src/parser_expr.c ./src/parser.c ./src/main.c  -o $@

$(BIN)_dbg: lex ./obj/fake_lex.o
	$(CC) $(CFLAGS) $(DBGFLAGS) $(ERFLAGS) ./src/char_buffer.c ./src/lex.c ./src/token.c ./src/ast_stack.c ./src/ast.c ./src/parser_expr.c ./src/parser.c ./src/main.c  -o $@

./obj/fake_lex.o: ./src/fake_lex.c
	$(CC) $(CFLAGS) -c $< -o $@

lex: 
	cd ./src/flex;flex lexer.l
