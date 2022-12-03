#include "dll.h"

/** 
 * gcc -std=c99 -Wall -Wextra -Werror -g dll_tests.c -lm -o dll_tests
 * 
 * gcc -std=c99 -Wall -Wextra -Werror -g dll.c dll_tests.c -lm -o dll_tests
 * valgrind --leak-check=full ./dll_tests
**/



int main(){
    fprintf(stderr,"==========================================================\n");
    fprintf(stderr,"                   TEST #1 - dll_create                   \n");
    fprintf(stderr,"==========================================================\n");
    fprintf(stderr,"-- try initializing a double-linked list, then printing it\n");
    dll test = dll_create();
    dll_print_forwards(test);
    dll_print_backwards(test);
    fprintf(stderr,"  -- if nothing was printed, everything's fine\n");
    fprintf(stderr,"\n");
/**/
    fprintf(stderr,"==========================================================\n");
    fprintf(stderr,"  TEST #2 - dll_insert_first && dll_insert_last && prints \n");
    fprintf(stderr,"==========================================================\n");
/**/
    fprintf(stderr,"-- try inserting a few links\n");
    dll_insert_first(test, 0, "first", int_t);
    dll_insert_last(test, 10, "second", int_t);
    dll_insert_first(test, -5, "third", int_t);
    dll_insert_last(test, -80, "fourth", int_t);
    dll_insert_first(test, 100, "fifth", int_t);
//
    fprintf(stderr,"  -- try printing the list forwards\n");
    dll_print_forwards(test);
    fprintf(stderr,"  -- try printing the list backwards\n");
    dll_print_backwards(test);
/**/
    fprintf(stderr,"==========================================================\n");
    fprintf(stderr,"      TEST #3 - dll_insert_before && dll_insert_after     \n");
    fprintf(stderr,"==========================================================\n");
/**/
    fprintf(stderr,"-- try inserting a few links\n");
    dll_insert_before(test, 0, 1, "before_first", int_t);
    dll_insert_after(test, 10, 11, "after_second", int_t);
    dll_insert_before(test, -5, -6, "before_third", int_t);
    dll_insert_after(test, -80, -81, "after_fourth", int_t);
//
    fprintf(stderr,"  -- try printing the list forwards\n");
    dll_print_forwards(test);
/**/
    fprintf(stderr,"==========================================================\n");
    fprintf(stderr,"       TEST #4 - dll_delete_first && dll_delete_last      \n");
    fprintf(stderr,"==========================================================\n");
/**/
    fprintf(stderr,"-- try deleting the first and the two last links\n");
    dll_delete_first(test);
    dll_delete_last(test);
    dll_delete_last(test);
//
    fprintf(stderr,"  -- try printing the list forwards\n");
    dll_print_forwards(test);
/**/
    fprintf(stderr,"==========================================================\n");
    fprintf(stderr,"                TEST #4 - dll_delete_by_id                \n");
    fprintf(stderr,"==========================================================\n");
/**/
    fprintf(stderr,"-- try deleting links with IDs -5 and 10\n");
    dll_delete_by_id(test, -5);
    dll_delete_by_id(test, 10);
//
    fprintf(stderr,"  -- try printing the list forwards\n");
    dll_print_forwards(test);
//
    fprintf(stderr,"-- try deleting a nonexistent link\n");
    dll_delete_by_id(test, 999);

/**/
    fprintf(stderr,"==========================================================\n");
    fprintf(stderr,"                    FINAL - dll_dispose                   \n");
    fprintf(stderr,"==========================================================\n");
/**/
    fprintf(stderr,"-- try to free the entire list\n");
    dll_destroy(test);
    test = NULL;
    fprintf(stderr,"  -- dll_dispose ran successfully, try printing list\n");
    dll_print_forwards(test);
    fprintf(stderr,"  -- if errors were displayed (since list is destroyed == doesn't exist anymore), everything's fine\n");
    fprintf(stderr,"\n");

    return 0;
}