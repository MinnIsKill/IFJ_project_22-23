#Makefile

BIN=semantic_test
CC=gcc
CFLAGS=-std=gnu11 -g -O0
DBGFLAGS=-DINFO -DDEBUG
ERFLAGS=-Werror -Wall -Wextra -pedantic
SRC=../../src


all: $(BIN) $(BIN)_dbg

$(BIN): ./$(SRC)/token.c ./$(SRC)/g_stack.c ./$(SRC)/ast_stack.c ./$(SRC)/ast.c ./$(SRC)/parser_expr.c ./$(SRC)/parser.c ./$(SRC)/main.c ./$(SRC)/symtable.c ./$(SRC)/dll.c ./$(SRC)/semantic.c ./$(SRC)/context.c ./$(SRC)/lex.c ./$(SRC)/char_buffer.c 
	$(CC) $(CFLAGS) $(ERFLAGS) $^ -o $@

$(BIN)_dbg: ./$(SRC)/token.c ./$(SRC)/g_stack.c ./$(SRC)/ast_stack.c ./$(SRC)/ast.c ./$(SRC)/parser_expr.c ./$(SRC)/parser.c ./$(SRC)/main.c ./$(SRC)/symtable.c  ./$(SRC)/dll.c ./$(SRC)/semantic.c ./$(SRC)/parser_dbg.c ./$(SRC)/context.c ./$(SRC)/lex.c ./$(SRC)/char_buffer.c 
	$(CC) $(CFLAGS) $(DBGFLAGS) $(ERFLAGS)  $^ -o $@

.PHONY: clean

clean:
	rm -f ./$(BIN)
	rm -f ./$(BIN)_dbg
