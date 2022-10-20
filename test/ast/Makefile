#Makefile
# author Jan Lutonský, xluton02

BIN=test_run
CC=gcc
CFLAGS=-std=gnu11 -g -O0
LFLAGS=--coverage -lcmocka -Wl,--wrap=malloc,--wrap=realloc,--wrap=strdup
ERFLAGS=-Werror -Wall -Wextra -pedantic

$(BIN): ast_tests.o ast.o token.o
	$(CC) $(LFLAGS) $^ -o $@

ast_test.o: ast_test.c
	$(CC) $(CFLAGS) -c $< -o $@ 

ast.o: ./../../src/ast.c 
	$(CC) $(ERFLAGS) -DDEBUG --coverage $(CFLAGS) -c $< -o $@

token.o: ./../../src/token.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: tests report clean coverage

tests: $(BIN) 
	valgrind --leak-check=full ./$(BIN)

report: $(BIN)
	@env CMOCKA_XML_FILE=res.xml valgrind --leak-check=full --xml=yes --xml-file=val_res.xml ./$(BIN) -x

coverage: 
	./$(BIN)
	gcov ast
	lcov --capture --directory . --output-file ast.info
	genhtml ast.info --output-directory ./out

clean:
	rm -f ./$(BIN)
	rm -f ./*.o
	rm -f ./*.xml
	rm -f ./*.gcov
	rm -f ./*.gcda
	rm -f ./*.gcno
	rm -f ./*.info
	rm -f -r ./out


