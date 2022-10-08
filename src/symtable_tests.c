#include "symtable.h"

/** 
 * gcc -std=c99 -Wall -Wextra -Werror -g symtable_tests.c -lm -o symtable_tests
 * 
 * gcc -std=c99 -Wall -Wextra -Werror -g bintree.c symtable_tests.c -lm -o symtable_tests
 * valgrind --leak-check=full ./symtable_tests
**/

_binTree_node test;  //test variable
_binTree_node node;

int main(){
    printf("==========================================================\n");
    printf("                TEST #1 - binTree_init                    \n");
    printf("==========================================================\n");
    printf("-- try initializing a binary tree, then printing it\n");
    binTree_init(test);
    binTree_inorder(test);
    printf("  -- if there's nothing here but this message, everything's fine\n");
    printf("\n");
/**/
    printf("==========================================================\n");
    printf("        TEST #2 - binTree_insert && binTree_search        \n");
    printf("==========================================================\n");
/**/
    printf("-- try inserting a node\n");
    test = binTree_insert(test, 0, "whale peepee");
    if ((node = binTree_search_by_id(test, 0)) != NULL){
        printf("  -- inserted node   [%d] %s   found by ID at address:   %p\n", node->nodeData->id, node->nodeData->key, node);
    } else {
        printf("  -- inserted node NOT found by id\n");
    }
    if ((node = binTree_search_by_key(test, "whale peepee")) != NULL){
        printf("  -- inserted node   [%d] %s   found by KEY at address:   %p\n", node->nodeData->id, node->nodeData->key, node);
    } else {
        printf("  -- inserted node NOT found by key\n");
    }
//
    printf("  -- try printing the tree\n");
    binTree_inorder(test);
/**/
    printf("------------------------------\n");
/**/
    printf("-- try inserting more nodes\n");
    test = binTree_insert(test, 3, "give");
    test = binTree_insert(test, 1, "never");
    test = binTree_insert(test, 5, "up");
    test = binTree_insert(test, 2, "gonna");
    test = binTree_insert(test, 4, "you");
    test = binTree_insert(test, 6, "h");
    test = binTree_insert(test, 7, "a");
    test = binTree_insert(test, 8, "ha");
    test = binTree_insert(test, 9, "h");
    test = binTree_insert(test, 10, "i");
    test = binTree_insert(test, 11, "hi");
//
    printf("  -- try printing the tree\n");
    binTree_inorder(test);
//
    printf("-- try searching for some of the nodes\n");
    if ((node = binTree_search_by_id(test, 2)) != NULL){
        printf("  -- node   [%d] %s   found by ID at address:   %p\n", node->nodeData->id, node->nodeData->key, node);
    } else {
        printf("  -- node NOT found by id\n");
    }
    if ((node = binTree_search_by_key(test, "you")) != NULL){
        printf("  -- node   [%d] %s   found by KEY at address:   %p\n", node->nodeData->id, node->nodeData->key, node);
    } else {
        printf("  -- node NOT found by key\n");
    }
//
    printf("-- try searching for some nonexistent nodes\n");
    if ((node = binTree_search_by_id(test, 15)) != NULL){
        printf("  -- node   [%d] %s   found by ID at address:   %p\n", node->nodeData->id, node->nodeData->key, node);
    } else {
        printf("  -- node NOT found by id\n");
    }
    if ((node = binTree_search_by_key(test, "whale")) != NULL){
        printf("  -- node   [%d] %s   found by KEY at address:   %p\n", node->nodeData->id, node->nodeData->key, node);
    } else {
        printf("  -- node NOT found by key\n");
    }
    printf("\n");
/**/
    printf("==========================================================\n");
    printf("  TEST #3 - binTree_delete_by_id && binTree_delete_by_key \n");
    printf("==========================================================\n");
/**/
    printf("-- try deleting some nodes\n");
    binTree_delete_by_id(test, 0);
    printf("  -- tried deleting node with ID 0, is it gone from the tree?\n");
    binTree_inorder(test);
    binTree_delete_by_key(test, "ha");
    binTree_delete_by_key(test, "hi");
    printf("  -- tried deleting nodes with KEYs \"ha\" and \"hi\", are they gone from the tree?\n");
    binTree_inorder(test);
    binTree_delete_by_id(test, 6);
    binTree_delete_by_id(test, 7);
    binTree_delete_by_id(test, 9);
    binTree_delete_by_id(test, 10);
    printf("  -- tried deleting nodes with IDs 6,7,9,10, are they gone from the tree?\n");
    binTree_inorder(test);
    printf("-- try deleting some nonexistent nodes\n");
    binTree_delete_by_id(test, 15);
    binTree_delete_by_key(test, "meow");
/**/
    printf("==========================================================\n");
    printf("                  FINAL - binTree_dispose                 \n");
    printf("==========================================================\n");
/**/
    printf("-- try to free the entire binary tree\n");
    binTree_dispose(test);
    test = NULL;
    printf("  -- binTree_dispose ran successfully, try printing tree\n");
    binTree_inorder(test);
    printf("  -- if nothing was printed, everything's fine\n");
    printf("\n");

    return 0;
}