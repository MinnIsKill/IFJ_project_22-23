BIN=test
CC=gcc
FLAGS=-std=c99 -Werror -Wall -Wextra -pedantic -DDEBUG

PROJECT=../../src/
INCLUDE=$(PROJECT)char_buffer.c $(PROJECT)lex.c $(PROJECT)token.c $(PROJECT)vector.c

all: $(BIN)

$(BIN):
	$(CC) $(FLAGS) -o $(BIN) ./main.c $(INCLUDE)

clean:
	rm -rf $(BIN)
