# Abstract syntax tree test
Simple test suite for ast data structure.

## Dependencies
`cmocka` - testing framework\
`valgrind` - memory leak detection\
`gcov`+`lcov` - code coverage reports

## Makefile
make - compiles tests\
make tests - compile and run tests with valgrind\
make report - generate xml reports from cmocka and valgrind\
make coverage - run the test and generate code coverage report in ./out/index.html
