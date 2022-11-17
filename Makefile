#Makefile
# this will defenetly change, just for testing

BIN=compiler
CC=gcc
CFLAGS=-std=gnu11 -g 
DBGFLAGS=-DINFO -DDEBUG
ERFLAGS=-Werror -Wall -Wextra -pedantic

all: $(BIN) $(BIN)_dbg

$(BIN): ./obj/fake_lex.o ./src/token.c ./src/g_stack.c ./src/ast_stack.c ./src/ast.c ./src/parser_expr.c ./src/parser.c ./src/main.c ./src/symtable.c ./src/dll.c ./src/semantic.c ./src/context.c ./src/lex.c ./src/char_buffer.c 
	$(CC) $(CFLAGS) $(ERFLAGS) $^ -o $@

$(BIN)_dbg: ./obj/fake_lex.o ./obj/fake_lex.o ./src/token.c ./src/g_stack.c ./src/ast_stack.c ./src/ast.c ./src/parser_expr.c ./src/parser.c ./src/main.c ./src/symtable.c  ./src/dll.c ./src/semantic.c ./src/parser_dbg.c ./src/context.c ./src/lex.c ./src/char_buffer.c 
	$(CC) $(CFLAGS) $(DBGFLAGS) $(ERFLAGS)  $^ -o $@

./obj/fake_lex.o: ./src/fake_lex.c
	$(CC) $(CFLAGS) -c $< -o $@

lex: 
	cd ./src/flex;flex lexer.l
