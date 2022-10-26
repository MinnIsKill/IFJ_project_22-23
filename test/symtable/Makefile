# Makefile for Symtable testing
# Author: Vojtěch Kališ (xkalis03)

CC     := gcc
STD    := -std=c99
LFLAGS := -fPIC -fprofile-arcs -ftest-coverage
CFLAGS := -Wall -Wextra -Werror -pedantic -g
REPORT := genhtml coverage.info --output-directory docs
RM     := rm -rf

FILE   := symtable
TFILE  := symtable_tests

GCOV   := gcov ./../../src/$(FILE).c
LCOV   := lcov --capture --directory . --output-file coverage.info

# default rule
# if only 'make' inserted, the first found rule is executed
# since this is supposed to do the same thing as 'make all', just run that
default: all

# phony rules
.PHONY: all
all: build testgen clean

.PHONY: build #compile, create an executable
build: ./../../src/$(FILE).c ./../../src/$(FILE).h $(TFILE).c
	$(CC) $(STD) $(LFLAGS) $(CFLAGS) ./../../src/$(FILE).c ./../../src/$(FILE).h $(TFILE).c -lm -o $(TFILE)

.PHONY: testgen #run file to generate .gcda files, then generate a HTML report using these files 
testgen: ./../../src/$(FILE).c ./../../src/$(FILE).h $(TFILE).c
	./$(TFILE)
	$(GCOV)
	$(LCOV)
	$(REPORT)

.PHONE: clean
clean: ./../../src/$(FILE).c ./../../src/$(FILE).h $(TFILE).c $(TFILE)
	$(RM) symtable_tests *.o *.so *.gcno *.gcda *.gcov coverage.info
	