/** 
 * gcc -std=c99 -Wall -Wextra -Werror -g symtable.c symtable_test.c -lm -o symtable_test
 * valgrind --leak-check=full ./symtable_test
**/

#include "symtable.h"
#include <assert.h>

//counters
size_t basic_passed = 0;
size_t basic_failed = 0;
size_t adv_passed = 0;
size_t adv_failed = 0;
//test variables
bintree_node test;
bintree_node node;
char output[2048];
char failed_list[1024];

/************************
*      BASIC TESTS      *
************************/
// !!!ALL TESTS ASSUME TESTING TREE TO BE EMPTY ON ENTER!!!
bool test_treeinit(struct bintree_node* test){
    bintree_init(&test);
    bintree_inorder_saveoutput(test, output);
    return strcmp(output,"") == 0 ? true : false;
}

bool test_insert_dispose(struct bintree_node* test){
    memset(output, 0, sizeof(output));
    test = bintree_insert(test, 1, "haha");
    test = bintree_insert(test, 2, "hihi");
    test = bintree_insert(test, 3, "hehe");
    bintree_inorder_saveoutput(test, output);
    bintree_dispose(test);
    return (strcmp(output,"[1]:haha | [2]:hihi | [3]:hehe") == 0) ? true : false;
}

bool test_search(struct bintree_node* test, struct bintree_node* node){
    memset(output, 0, sizeof(output));
    test = bintree_insert(test, 0, "hmmmm");
    if ((node = bintree_search_by_id(test, 0)) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "hmmmm")) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    bintree_dispose(test);
    return strcmp(output,"succ succ ") == 0 ? true : false;
}

/**bool test_delete_by_id(bintree_node test, bintree_node node){
    memset(output, 0, sizeof(output));
    test = bintree_insert(test, 4, "I'm");
    test = bintree_insert(test, 1, "A");

    if ((node = bintree_search_by_id(test, 1)) != NULL){
        bintree_delete_by_id(test, 1);
        //bintree_inorder(test);
        if ((node = bintree_search_by_id(test, 1)) == NULL){
            strcat(output,"succ ");
        } else {
            strcat(output,"fail ");
        }
    } else {
        strcat(output,"fail ");
    }

    if ((node = bintree_search_by_id(test, 4)) != NULL){
        //bintree_inorder(test);
        bintree_delete_by_id(test, 4);
        //bintree_inorder(test);
        if ((node = bintree_search_by_id(test, 4)) == NULL){
            strcat(output,"succ ");
        } else {
            strcat(output,"fail ");
        }
    } else {
        strcat(output,"fail ");
    }
    bintree_dispose(test);
    printf("%s\n",output);
    return strcmp(output,"succ succ ") == 0 ? true : false;
}**/

/**bool test_delete_by_key(bintree_node test, bintree_node node){
    memset(output, 0, sizeof(output));
    test = bintree_insert(test, 2, "Barbie");
    test = bintree_insert(test, 3, "Girl");

    if ((node = bintree_search_by_key(test, "Barbie")) != NULL){
        bintree_delete_by_key(test, "Barbie");
        if ((node = bintree_search_by_key(test, "Barbie")) == NULL){
            strcat(output,"succ ");
        } else {
            strcat(output,"fail ");
        }
    } else {
        strcat(output,"fail ");
    }

    if ((node = bintree_search_by_key(test, "Girl")) != NULL){
        bintree_delete_by_key(test, "Girl");
        if ((node = bintree_search_by_key(test, "Girl")) == NULL){
            strcat(output,"succ ");
        } else {
            strcat(output,"fail ");
        }
    } else {
        strcat(output,"fail ");
    }
    bintree_dispose(test);
    printf("%s\n",output);
    return strcmp(output,"succ succ ") == 0 ? true : false;
}**/



/************************
*     ADVANCED TESTS    *
************************/
// !!!ALL TESTS ASSUME TESTING TREE TO BE EMPTY ON ENTER!!!

bool test_search_after_dispose(struct bintree_node* test, struct bintree_node* node){
    memset(output, 0, sizeof(output));

    test = bintree_insert(test, 1, "one");
    test = bintree_insert(test, 2, "two");
    test = bintree_insert(test, 3, "three");
    test = bintree_insert(test, 4, "four");

    bintree_dispose(test);
    bintree_init(&test);

    if ((node = bintree_search_by_id(test, 1)) == NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "three")) == NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    bintree_dispose(test);
    return strcmp(output,"succ succ ") == 0 ? true : false;
}

bool test_insert_after_dispose(struct bintree_node* test, struct bintree_node* node){
    memset(output, 0, sizeof(output));

    test = bintree_insert(test, 1, "one");
    test = bintree_insert(test, 2, "two");
    test = bintree_insert(test, 3, "three");
    test = bintree_insert(test, 4, "four");

    bintree_dispose(test);
    bintree_init(&test);

    test = bintree_insert(test, 1, "one");
    
    if ((node = bintree_search_by_id(test, 1)) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "two")) == NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    
    bintree_dispose(test);
    return strcmp(output,"succ succ ") == 0 ? true : false;
}

bool test_insert_search_galore(struct bintree_node* test, struct bintree_node* node){
    memset(output, 0, sizeof(output));

    test = bintree_insert(test, 1, "a");
    test = bintree_insert(test, 2, "b");
    test = bintree_insert(test, 3, "c");
    test = bintree_insert(test, 4, "d");
    test = bintree_insert(test, 5, "e");
    test = bintree_insert(test, 6, "f");
    test = bintree_insert(test, 7, "g");
    test = bintree_insert(test, 8, "h");
    test = bintree_insert(test, 9, "ch");
    test = bintree_insert(test, 10, "i");
    test = bintree_insert(test, 11, "j");
    test = bintree_insert(test, 12, "k");
    test = bintree_insert(test, 13, "l");
    test = bintree_insert(test, 14, "m");
    test = bintree_insert(test, 15, "n");
    test = bintree_insert(test, 16, "o");
    test = bintree_insert(test, 17, "p");
    test = bintree_insert(test, 18, "q");
    test = bintree_insert(test, 19, "r");
    test = bintree_insert(test, 20, "s");
    test = bintree_insert(test, 21, "t");
    test = bintree_insert(test, 22, "u");
    test = bintree_insert(test, 23, "v");
    test = bintree_insert(test, 24, "w");
    test = bintree_insert(test, 25, "x");
    test = bintree_insert(test, 26, "y");
    test = bintree_insert(test, 27, "z");
    test = bintree_insert(test, 28, "ab");
    test = bintree_insert(test, 29, "cd");
    test = bintree_insert(test, 30, "ef");
    test = bintree_insert(test, 31, "gh");
    test = bintree_insert(test, 32, "chi");
    test = bintree_insert(test, 33, "jk");
    test = bintree_insert(test, 34, "lm");
    test = bintree_insert(test, 35, "no");
    test = bintree_insert(test, 36, "pq");
    test = bintree_insert(test, 37, "rs");
    test = bintree_insert(test, 38, "tu");
    test = bintree_insert(test, 39, "vw");
    test = bintree_insert(test, 40, "xy");

    if ((node = bintree_search_by_id(test, 25)) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "ab")) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_id(test, 18)) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "o")) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_id(test, 40)) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "f")) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }

    if ((node = bintree_search_by_id(test, 56)) == NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "nope")) == NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }

    bintree_dispose(test);
    return strcmp(output,"succ succ succ succ succ succ succ succ ") == 0 ? true : false;
}

// should throw an error and not insert the duplicate node
bool test_duplicate_insert(struct bintree_node* test, struct bintree_node* node){
    memset(output, 0, sizeof(output));

    test = bintree_insert(test, 1, "a");
    test = bintree_insert(test, 2, "a");
    test = bintree_insert(test, 1, "c");

    if ((node = bintree_search_by_id(test, 1)) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "a")) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "c")) == NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }

    bintree_dispose(test);
    return strcmp(output,"succ succ succ ") == 0 ? true : false;
}


/************************
*   TESTS COLLECTIONS   *
************************/
void test_symtable_basics(bintree_node test, bintree_node node){
    if (test_treeinit(test) == true)             {basic_passed++;} else {basic_failed++; strcat(failed_list, "test_treeinit\n");}
    if (test_insert_dispose(test) == true)       {basic_passed++;} else {basic_failed++; strcat(failed_list, "test_dispose\n");}
    if (test_search(test, node) == true)         {basic_passed++;} else {basic_failed++; strcat(failed_list, "test_insert_search\n");}
    //if (test_delete_by_id(test, node) == true)   {basic_passed++;} else {basic_failed++; strcat(failed_list, "test_delete_by_id\n");}
    //if (test_delete_by_key(test, node) == true)  {basic_passed++;} else {basic_failed++; strcat(failed_list, "test_delete_by_key\n");}
}

void test_symtable_advanced(bintree_node test, bintree_node node){
    if (test_search_after_dispose(test, node) == true)  {adv_passed++;} else {adv_failed++; strcat(failed_list, "test_search_after_dispose\n");}
    if (test_insert_after_dispose(test, node) == true)  {adv_passed++;} else {adv_failed++; strcat(failed_list, "test_insert_after_dispose\n");}
    if (test_insert_search_galore(test, node) == true)  {adv_passed++;} else {adv_failed++; strcat(failed_list, "test_insert_search_galore\n");}
    if (test_duplicate_insert(test, node) == true)      {adv_passed++;} else {adv_failed++; strcat(failed_list, "test_duplicate_insert\n");}
}


/************************
*          MAIN         *
************************/
int main (){
    fprintf(stdout, "---------------------------\n       SYMTABLE TESTS\n---------------------------\n");
    fprintf(stdout, "|debug prints| <-- error messages, because we're testing improper behavior as well\n\n");

    test_symtable_basics(test, node);
    test_symtable_advanced(test, node);
    bintree_dispose(test);

    fprintf(stdout, "\n- - - - - - - - - - - - - -\n|tests successfulness|\n\n");
    fprintf(stdout, "  basic tests passed: %ld\n  basic tests failed: %ld\n", basic_passed, basic_failed);
    fprintf(stdout, "  advanced tests passed: %ld\n  advanced tests failed: %ld\n", adv_passed, adv_failed);
    fprintf(stdout, "---------------------------\n");
    fprintf(stdout, "       failed tests\n---------------------------\n");
    if(strcmp(failed_list, "") != 0){fprintf(stdout, "%s\n", failed_list);}

    return 0;
}