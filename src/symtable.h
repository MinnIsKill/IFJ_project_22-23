/**
 * @file symtable.h
 * @author Vojtěch Kališ (xkalis03)
 * @brief binary tree implementation
 *
 * @date of last update:   14th October 2022
 */

#ifndef SYMTABLE_H
#define SYMTABLE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "token.h"
#include "dll.h"
#include "vector.h"
#include "dbg.h"

/* argument and return types (for functions) */
typedef enum{
    int_t,     //int
    float_t,   //float
    string_t,  //string
    void_t,    //void

    nint_t,    //?int
    nfloat_t,  //?float
    nstring_t, //?string
    ARG_TYPE_ERROR
}arg_type;

/* type of node */
typedef enum{
    function,
    variable
}bintree_node_type;

/* node data */
struct bintree_data{
    size_t id;  //id
    char* key;  //string (var/fnc name)

    size_t vars_cnt; //for functions, counter for variables
    arg_type init_type; //for variables, type assigned during declaration
    arg_type curr_type; //for variables, current type
    float value;        //for int/float variables
    char string[1024];  //for string variables
                        //!!!dynamic strings would maybe be nice

    bool codegen_was_def; //for codegen, if var had been processed
    bool variadic_func;   //if function variadic
    //bool is_zero;

    struct dll *args_list; //names of arguments for functions (in order)
    arg_type rtype; //return type for functions
    size_t arg_cnt; //arguments counter for functions
};
typedef struct bintree_data* bintree_data;

/* node structure */
struct bintree_node{
    struct bintree_data *node_data;
    struct bintree_node *r; //right node ptr
    struct bintree_node *l; //left node ptr
    bintree_node_type node_type; //function or variable
    struct bintree_node *local_symtab;
};
typedef struct bintree_node* bintree_node;


//bintree INIT FUNCTION
void bintree_init(bintree_node *root);

//bintree DISPOSE FUNCTION
void bintree_dispose(struct bintree_node *root);
//bintree DISPOSE AUX FUNCTION
void bintree_dispose_internal(struct bintree_node *root);

struct bintree_node* bintree_node_nullifyinfo(struct bintree_node *root);

/** bintree NODE INSERT FUNCTION
 *  @param root = root node of binary tree
 *  @param key = string (name of inserted variable/function)
 *  @param type_of_node = variable/function
 *  @return: root if successful
 *           NULL if unsuccessful
**/
struct bintree_node* bintree_insert(struct bintree_node *root, size_t id, char key[], bintree_node_type type_of_node);

//bintree NODE DELETE BY ID FUNCTION
struct bintree_node* bintree_delete_by_id(struct bintree_node *root, size_t id);
//bintree NODE DELETE BY KEY FUNCTION
struct bintree_node* bintree_delete_by_key(struct bintree_node *root, char key[]);
void bintree_delete_aux(struct bintree_node *ptr, struct bintree_node *parent, char position_from_parent[]);

//bintree NODE SEARCH BY ID FUNCTION
struct bintree_node* bintree_search_by_id(struct bintree_node *root, size_t id);

//bintree NODE SEARCH BY KEY FUNCTION
struct bintree_node* bintree_search_by_key(struct bintree_node *root, char key[]);

//bintree enums string conversions
const char* bintree_fnc_arg_type_tostr(arg_type type);
const char* bintree_fnc_args_list_tostr(arg_type* args_list, size_t arg_cnt, char list[]);

//bintree INORDER PRINT FUNCTION
void bintree_inorder(struct bintree_node *root);
//bintree INORDER PRINT FUNCTION WHICH SAVES OUTPUT INTO PASSED STRING
void bintree_inorder_saveoutput(struct bintree_node *root, char output[]);
//bintree INORDER PRINT FUNCTION WHICH PRINTS ALL ASPECTS OF NODE
void bintree_inorder_fullprint(struct bintree_node *root, bool called_from_inside);
//bintree INORDER PRINT FUNCTION WHICH PRINTS ONLY NODES WITH TYPE 'variable' OF PASSED SYMTABLE (without delving into local symtable)
void bintree_inorder_currvarsonly(struct bintree_node *root);

#endif
