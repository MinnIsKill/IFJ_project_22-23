#Makefile
#author Jan Lutonský, xluton02

BIN=parser_test
CC=gcc
CFLAGS=-std=gnu11 -g -O0 
DBGFLAGS=-DINFO -DDEBUG
ERFLAGS=-Werror -Wall -Wextra -pedantic

all: $(BIN) $(BIN)_dbg

$(BIN): ./main.c ./../../src/parser_expr.c ./../../src/dll.c ./../../src/symtable.c ./../../src/lex.c ./../../src/char_buffer.c ./../../src/context.c ./../../src/token.c ./../../src/ast.c ./../../src/g_stack.c ./../../src/ast_stack.c
	$(CC) $(CFLAGS) $^ -o $@

$(BIN)_dbg: ./main.c ./../../src/parser_expr.c ./../../src/dll.c ./../../src/symtable.c ./../../src/lex.c ./../../src/char_buffer.c ./../../src/context.c ./../../src/token.c ./../../src/ast.c ./../../src/g_stack.c ./../../src/ast_stack.c
	$(CC) $(CFLAGS) $(DBGFLAGS) $^ -o $@
