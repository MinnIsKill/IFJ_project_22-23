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

struct _binTree_data{
    int id;
    char key[32];
};
// what is this used for ?
//typedef struct _binTree_data* _binTree_data;

/* node structure */
struct _binTree_node{
    struct _binTree_data *nodeData;
    struct _binTree_node *r; //right node ptr
    struct _binTree_node *l; //left node ptr
};
// what is this used for ?
//typedef struct _binTree_node* _binTree_node;


//BINTREE INIT FUNCTION
void binTree_init();

//BINTREE DISPOSE FUNCTION
void binTree_dispose(struct _binTree_node*);
//BINTREE DISPOSE AUX FUNCTION
void binTree_dispose_internal(struct _binTree_node*);

//BINTREE NODE INSERT FUNCTION
struct _binTree_node* binTree_insert(struct _binTree_node*, int, char[]);

//BINTREE NODE DELETE BY ID FUNCTION
void binTree_delete_by_id(struct _binTree_node*, int);
//BINTREE NODE DELETE BY KEY FUNCTION
void binTree_delete_by_key(struct _binTree_node*, char[]);
//BINTREE NODE DELETE AUX FUNCTION
void binTree_delete_aux(struct _binTree_node *ptr, struct _binTree_node *par);

//BINTREE NODE SEARCH BY ID FUNCTION
struct _binTree_node* binTree_search_by_id(struct _binTree_node*, int);

//BINTREE NODE SEARCH BY KEY FUNCTION
struct _binTree_node* binTree_search_by_key(struct _binTree_node*, char[]);

//BINTREE INORDER PRINT FUNCTION
void binTree_inorder(struct _binTree_node*);

#endif
