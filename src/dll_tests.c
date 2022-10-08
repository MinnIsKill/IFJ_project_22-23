#include "dll.h"

/** 
 * gcc -std=c99 -Wall -Wextra -Werror -g dll_tests.c -lm -o dll_tests
 * 
 * gcc -std=c99 -Wall -Wextra -Werror -g dll.c dll_tests.c -lm -o dll_tests
 * valgrind --leak-check=full ./dll_tests
**/



int main(){
    printf("==========================================================\n");
    printf("                   TEST #1 - dll_create                   \n");
    printf("==========================================================\n");
    printf("-- try initializing a double-linked list, then printing it\n");
    _dll test = dll_create();
    dll_print_forwards(test);
    dll_print_backwards(test);
    printf("  -- if nothing was printed, everything's fine\n");
    printf("\n");
/**/
    printf("==========================================================\n");
    printf("  TEST #2 - dll_insert_first && dll_insert_last && prints \n");
    printf("==========================================================\n");
/**/
    printf("-- try inserting a few links\n");
    dll_insert_first(test, 0, "first");
    dll_insert_last(test, 10, "second");
    dll_insert_first(test, -5, "third");
    dll_insert_last(test, -80, "fourth");
    dll_insert_first(test, 100, "fifth");
//
    printf("  -- try printing the list forwards\n");
    dll_print_forwards(test);
    printf("  -- try printing the list backwards\n");
    dll_print_backwards(test);
/**/
    printf("==========================================================\n");
    printf("      TEST #3 - dll_insert_before && dll_insert_after     \n");
    printf("==========================================================\n");
/**/
    printf("-- try inserting a few links\n");
    dll_insert_before(test, 0, 1, "before_first");
    dll_insert_after(test, 10, 11, "after_second");
    dll_insert_before(test, -5, -6, "before_third");
    dll_insert_after(test, -80, -81, "after_fourth");
//
    printf("  -- try printing the list forwards\n");
    dll_print_forwards(test);
/**/
    printf("==========================================================\n");
    printf("       TEST #4 - dll_delete_first && dll_delete_last      \n");
    printf("==========================================================\n");
/**/
    printf("-- try deleting the first and the two last links\n");
    dll_delete_first(test);
    dll_delete_last(test);
    dll_delete_last(test);
//
    printf("  -- try printing the list forwards\n");
    dll_print_forwards(test);
/**/
    printf("==========================================================\n");
    printf("                TEST #4 - dll_delete_by_id                \n");
    printf("==========================================================\n");
/**/
    printf("-- try deleting links with IDs -5 and 10\n");
    dll_delete_by_id(test, -5);
    dll_delete_by_id(test, 10);
//
    printf("  -- try printing the list forwards\n");
    dll_print_forwards(test);
//
    printf("-- try deleting a nonexistent link\n");
    dll_delete_by_id(test, 999);

/**/
    printf("==========================================================\n");
    printf("                    FINAL - dll_dispose                   \n");
    printf("==========================================================\n");
/**/
    printf("-- try to free the entire list\n");
    dll_destroy(test);
    test = NULL;
    printf("  -- dll_dispose ran successfully, try printing list\n");
    dll_print_forwards(test);
    printf("  -- if errors were displayed (since list is destroyed == doesn't exist anymore), everything's fine\n");
    printf("\n");

    return 0;
}