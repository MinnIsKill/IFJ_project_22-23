/**
 * @file symtable_tests.c
 * @author Vojtěch Kališ (xkalis03)
 * @brief binary tree testing implementation
 *
 * @date of last update:   14th October 2022
 */

/** 
 * gcc -std=c99 -Wall -Wextra -Werror -g symtable.c symtable_tests.c -lm -o symtable_tests
 * valgrind --leak-check=full ./symtable_tests
**/

#include "./../../src/symtable.h"
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
    if ((test = bintree_insert(test, "haha", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "hihi", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "hehe", variable)) == NULL){ bintree_dispose(test); return false;}
    bintree_inorder_saveoutput(test, output);
    bintree_dispose(test);
    return (strcmp(output,"[0]:haha | [1]:hihi | [2]:hehe") == 0) ? true : false;
}

bool test_search(struct bintree_node* test, struct bintree_node* node){
    memset(output, 0, sizeof(output));
    if ((test = bintree_insert(test, "hmmmm", variable)) == NULL){ bintree_dispose(test); return false;}
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

    if ((test = bintree_insert(test, "one", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "two", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "three", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "four", variable)) == NULL){ bintree_dispose(test); return false;}

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

    if ((test = bintree_insert(test, "one", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "two", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "three", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "four", variable)) == NULL){ bintree_dispose(test); return false;}

    bintree_dispose(test);
    bintree_init(&test);

    if ((test = bintree_insert(test, "one", variable)) == NULL){ bintree_dispose(test); return false;}
    
    if ((node = bintree_search_by_id(test, 0)) != NULL){
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

//0-4
    if ((test = bintree_insert(test, "a", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "b", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "c", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "d", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "e", variable)) == NULL){ bintree_dispose(test); return false;}
//5-9
    if ((test = bintree_insert(test, "f", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "g", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "h", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "i", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "j", variable)) == NULL){ bintree_dispose(test); return false;}
//10-14
    if ((test = bintree_insert(test, "k", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "l", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "m", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "n", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "o", variable)) == NULL){ bintree_dispose(test); return false;}
//15-19
    if ((test = bintree_insert(test, "p", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "q", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "r", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "s", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "t", variable)) == NULL){ bintree_dispose(test); return false;}
//20-24
    if ((test = bintree_insert(test, "u", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "v", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "w", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "x", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "y", variable)) == NULL){ bintree_dispose(test); return false;}
//25-29
    if ((test = bintree_insert(test, "z", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "A", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "B", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "C", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "D", variable)) == NULL){ bintree_dispose(test); return false;}
//30-34
    if ((test = bintree_insert(test, "E", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "F", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "G", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "H", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "I", variable)) == NULL){ bintree_dispose(test); return false;}
//35-39
    if ((test = bintree_insert(test, "J", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "K", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "L", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "M", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "N", variable)) == NULL){ bintree_dispose(test); return false;}
//40-44
    if ((test = bintree_insert(test, "O", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "P", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "Q", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "R", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "S", variable)) == NULL){ bintree_dispose(test); return false;}
//45-49
    if ((test = bintree_insert(test, "T", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "U", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "V", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "W", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "X", variable)) == NULL){ bintree_dispose(test); return false;}

    if ((node = bintree_search_by_id(test, 25)) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "O")) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_id(test, 18)) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "e")) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_id(test, 40)) != NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((node = bintree_search_by_key(test, "A")) != NULL){
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

    if ((test = bintree_insert(test, "a", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "a", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((node = bintree_search_by_id(test, 1)) == NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }
    if ((test = bintree_insert(test, "b", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "c", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((test = bintree_insert(test, "c", variable)) == NULL){ bintree_dispose(test); return false;}
    if ((node = bintree_search_by_id(test, 3)) == NULL){
        strcat(output,"succ ");
    } else {
        strcat(output,"fail ");
    }

    bintree_dispose(test);
    return strcmp(output,"succ succ ") == 0 ? true : false;
}


/************************
*   TESTS COLLECTIONS   *
************************/
void test_symtable_basics(bintree_node test, bintree_node node){
    if (test_treeinit(test) == true)             {basic_passed++;} else {basic_failed++; strcat(failed_list, "test_treeinit\n");}
    if (test_insert_dispose(test) == true)       {basic_passed++;} else {basic_failed++; strcat(failed_list, "test_insert_dispose\n");}
    if (test_search(test, node) == true)         {basic_passed++;} else {basic_failed++; strcat(failed_list, "test_search\n");}
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