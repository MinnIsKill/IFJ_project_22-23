#Makefile

BIN=test_run
CC=gcc
CFLAGS=-std=gnu11 -g -O0
LFLAGS=--coverage -lcmocka -Wl,--wrap=realloc,--wrap=calloc

$(BIN): ast_stack_tests.o ast.o token.o ast_stack.o
	$(CC) $(LFLAGS) $^ -o $@

ast_stack_test.o: ast_stack_test.c
	$(CC) $(CFLAGS) -c $< -o $@ 

ast_stack.o: ./../../src/ast_stack.c 
	$(CC) -DDEBUG --coverage $(CFLAGS) -c $< -o $@

ast.o: ./../../src/ast.c
	$(CC) $(CFLAGS) -c $< -o $@

token.o: ./../../src/token.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: tests report clean coverage

tests: $(BIN) 
	valgrind --leak-check=full ./$(BIN)

report: $(BIN)
	@env CMOCKA_XML_FILE=res.xml valgrind --leak-check=full --xml=yes --xml-file=val_res.xml ./$(BIN) -x

coverage: $(BIN) 
	./$(BIN)
	gcov ast_stack
	lcov --capture --directory . --output-file ast_stack.info
	genhtml ast_stack.info --output-directory ./out

clean:
	rm -f ./$(BIN)
	rm -f ./*.o
	rm -f ./*.xml
	rm -f ./*.gcov
	rm -f ./*.gcda
	rm -f ./*.gcno
	rm -f ./*.info
	rm -f -r ./out


