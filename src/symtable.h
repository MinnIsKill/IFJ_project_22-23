/**
 * @file symtable.h
 * @author Vojtěch Kališ (xkalis03)
 * @brief binary tree implementation
 *
 * @date of last update:   28th July 2022
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct bintree_data{
    int id;
    char key[32];
};
typedef struct bintree_data* bintree_data;

/* node structure */
struct bintree_node{
    struct bintree_data *nodeData;
    struct bintree_node *r; //right node ptr
    struct bintree_node *l; //left node ptr
};
typedef struct bintree_node* bintree_node;


//bintree INIT FUNCTION
void bintree_init();

//bintree DISPOSE FUNCTION
void bintree_dispose(struct bintree_node*);
//bintree DISPOSE AUX FUNCTION
void bintree_dispose_internal(struct bintree_node*);

//bintree NODE INSERT FUNCTION
struct bintree_node* bintree_insert(struct bintree_node*, int, char[]);

//bintree NODE DELETE BY ID FUNCTION
struct bintree_node* bintree_delete_by_id(struct bintree_node*, int);
//bintree NODE DELETE BY KEY FUNCTION
struct bintree_node* bintree_delete_by_key(struct bintree_node*, char[]);
void bintree_delete_aux(struct bintree_node *ptr, struct bintree_node *parent, char position_from_parent[]);

//bintree NODE SEARCH BY ID FUNCTION
struct bintree_node* bintree_search_by_id(struct bintree_node*, int);

//bintree NODE SEARCH BY KEY FUNCTION
struct bintree_node* bintree_search_by_key(struct bintree_node*, char[]);

//bintree INORDER PRINT FUNCTION
void bintree_inorder(struct bintree_node*);
//bintree INORDER PRINT FUNCTION WHICH SAVES OUTPUT INTO PASSED STRING
void bintree_inorder_saveoutput(struct bintree_node*, char[]);

#endif
