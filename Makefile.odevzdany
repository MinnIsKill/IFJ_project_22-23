#Makefile

BIN=compiler
CC=gcc
CFLAGS=-std=gnu11 -g 
ERFLAGS=-Werror -Wall -Wextra -pedantic

all: $(BIN)

$(BIN): ./codegen.c ./token.c ./g_stack.c ./ast_stack.c ./ast.c ./parser_expr.c ./parser.c ./main.c ./symtable.c ./dll.c ./semantic.c ./context.c ./lex.c ./char_buffer.c 
	$(CC) $(CFLAGS) $(ERFLAGS) $^ -o $@


