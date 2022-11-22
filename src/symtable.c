/**
 * @file symtable.c
 * @author Vojtěch Kališ (xkalis03)
 * @brief binary tree implementation
 *
 * @date of last update:   14th October 2022
 * 
 * UPDATE NOTES: 20-09-2022:  - basic functionality done
 *               28-09-2022:  - tried implementing deletes ("try" being the operative word here)
 *               14-10-2022:  - minor updates, testing (deletes are still not completely functional but won't probably even be used anyway)
 */

/**
 * TODO: - strdup var and func names ('key' and 'string')
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "symtable.h"

//bintree INIT FUNCTION
void bintree_init(bintree_node *root){
    if (root != NULL){
        *root = NULL;
    }
}

//bintree DISPOSE FUNCTION
//recursive
void bintree_dispose(struct bintree_node *root){
    //printf("DISPOSE: root pointing at [%d] %s\n", root->node_data->id, root->node_data->key);
    bintree_dispose_internal(root);
    bintree_init(&root);
}
void bintree_dispose_internal(struct bintree_node *root){
    if (root == NULL){
        return;
    }
  
    //dispose of both subtrees
    bintree_dispose_internal(root->l);
    bintree_dispose_internal(root->r);
    
    //then delete the node
    //printf("DELETING: [%d] %s\n", root->node_data->id, root->node_data->key);
    if (root->node_type == function){
        bintree_dispose(root->local_symtab);
        dll_destroy(root->node_data->args_list);
    }
    free (root->node_data); //free node data
    free (root); //free node
}

//
struct bintree_node* bintree_node_nullifyinfo(struct bintree_node *root){
    root->node_data->arg_cnt = 0;
    root->node_data->vars_cnt = 0;
    if (root->node_type == function){
        while (dll_is_empty(root->node_data->args_list) == false){
            dll_delete_first(root->node_data->args_list);
        }
    } 
    root->node_data->rtype = ARG_TYPE_ERROR;
    root->node_data->init_type = ARG_TYPE_ERROR;
    root->node_data->curr_type = ARG_TYPE_ERROR;
    root->node_data->string[0] = '\0';
    root->node_data->value = 0;
    root->node_data->codegen_was_def = false;

    return root;
}

//bintree NODE INSERT FUNCTION
//non-recursive
struct bintree_node* bintree_insert(struct bintree_node *root, size_t id, char key[], bintree_node_type type_of_node){
    struct bintree_node *tmp, *par, *ptr, *search;

    ptr = root;
    par = NULL;

    if ((search = bintree_search_by_key(root, key)) != NULL){ //if node with given key already exists
        dbgprint("DEBUG:  in bintree_insert:  node with given key already exist, please do bintree_search before bintree_insert and update the returned node\nNOTHING WAS INSERTED");
        return root;
    }

    while (ptr != NULL){
        par = ptr;
        if (id < ptr->node_data->id){
            ptr = ptr->l;
        } else if (id > ptr->node_data->id ){
            ptr = ptr->r;
        } else {
            dbgprint("ERROR:  in bintree_insert:  trying to insert a node with an already existing id");
            return root;
        }
    }

    tmp = (struct bintree_node *)malloc(sizeof(struct bintree_node));
    if (tmp == NULL){
        dbgprint("ERROR:  in bintree_insert:  malloc failed");
        free(tmp);
        return NULL;
    }
    //!!! strdup (data);
    bintree_data data = (bintree_data)malloc(sizeof(struct bintree_data));
    if (data == NULL){
        dbgprint("ERROR:  in bintree_insert:  malloc failed");
        free(tmp);
        free(data);
        return NULL;
    }
    tmp->node_data = data;
    tmp->node_data->id = id;
    tmp->node_type = type_of_node;
    //printf("tmp->node_data->id = %d\n", tmp->node_data->id);
    strcpy(tmp->node_data->key, key);
    //printf("tmp->node_data->key = %s\n", tmp->node_data->key);
    tmp->l = NULL;
    tmp->r = NULL;

    if (par == NULL){
        root = tmp;
    } else if (id < par->node_data->id){
        par->l = tmp;
    } else {
        par->r = tmp;
    }

    return root;
}

//bintree NODE DELETE BY ID FUNCTION
//non-recursive
struct bintree_node* bintree_delete_by_id(struct bintree_node *root, size_t id){
    struct bintree_node *par; //parent node ptr
    struct bintree_node *ptr;

    ptr = root;
    par = NULL;

    while (ptr != NULL){
        //printf("\n? scanning: [%d]\n\n",ptr->node_data->id);
        if (id == ptr->node_data->id){
            if (ptr->l == NULL && ptr->r == NULL){ //leaf
                if (ptr != root){
                    if (ptr == par->l){
                        par->l = NULL;
                    } else {
                        par->r = NULL;
                    }
                } else { //no parent (is root)
                    root = NULL;
                }
                free(ptr->node_data);
                free(ptr);
            } else if (ptr->l == NULL || ptr->r == NULL){ //one child
                struct bintree_node *next = ptr->r;
                if (ptr != root){
                    if (ptr == par->l){
                        par->l = next;
                    } else {
                        par->r = next;
                    }
                } else { //no parent (is root)
                    root = next;
                }
                free(ptr->node_data);
                free(ptr);
            } else {
                ;
            }
            break;
        }
        par = ptr;
        if (id > ptr->node_data->id){
            ptr = ptr->r;
        } else {
            ptr = ptr->l;
        }
    }

    if (ptr == NULL){
        dbgprint("WARNING:  in bintree_delete_by_id:  no node with ID %ld found", id);
    }

    return root;
}

//bintree NODE DELETE BY KEY FUNCTION
//non-recursive (using the principle of inorder traversal)
struct bintree_node* bintree_delete_by_key(struct bintree_node *root, char key[]){
    struct bintree_node *ptr, *tmp; //parent, current pointer, auxiliary
    int cmp = 0;

    ptr = root;
    
    while (ptr != NULL){
        if (ptr->l != NULL){ //something's to the left
            tmp = ptr->l; //explore it with tmp
            while (tmp->r != NULL && tmp->r != ptr){
                tmp = tmp->r;
            }
    
            if (tmp->r == ptr){
                tmp->r = NULL;
                if ((cmp = strcmp(ptr->node_data->key, key)) == 0){
                    bintree_delete_by_id(root, ptr->node_data->id);
                    break;
                }
                ptr = ptr->r;
            } else {
                tmp->r = ptr;
                ptr = ptr->l;
            }
        } else {
            if ((cmp = strcmp(ptr->node_data->key, key)) == 0){
                bintree_delete_by_id(root, ptr->node_data->id);
                break;
            }
            ptr = ptr->r;
        }
    }

    //printf(" ptr pointing at [%d] %s\n", ptr->node_data->id, ptr->node_data->key);
    //if (par != NULL) {printf(" par pointing at [%d] %s\n", par->node_data->id, par->node_data->key);}
    //printf(" root pointing at [%d] %s\n", root->node_data->id, root->node_data->key);

    if (ptr == NULL){
        dbgprint("WARNING:  in bintree_delete_by_key:  no node with KEY \"%s\" found", key);
    }

    return root;
}





//bintree NODE SEARCH BY ID FUNCTION
//non-recursive
//FOR THE PURPOSES OF THIS PROJECT, IT'S BETTER NOT TO USE THIS (not optimized for the purposes of this project)
struct bintree_node* bintree_search_by_id (struct bintree_node *root, size_t id){
    while (root != NULL){
        //printf("id is %d\n", id);
        //printf("root->node_data->id is %d\n", root->node_data->id);
        if (id < root->node_data->id){
            root = root->l; /*Move to left child*/
        } else if (id > root->node_data->id){
            root = root->r;  /*Move to right child */
        } else { /*id found*/
            return root;
        }
    }

    return NULL;
}

//bintree NODE SEARCH BY KEY FUNCTION
//non-recursive (basically just inorder traversal)
//FOR THE PURPOSES OF THIS PROJECT, DISREGARDS 'prog_b' NODES
struct bintree_node* bintree_search_by_key (struct bintree_node *root, char key[]){
    struct bintree_node *tmp; //forward checker
    struct bintree_node *found = NULL;
 
    while (root != NULL){
        if (root->l != NULL){
            tmp = root->l;
            while (tmp->r != NULL && tmp->r != root){
                tmp = tmp->r;
            }
 
            if (tmp->r == root){
                tmp->r = NULL;
                if (strcmp(root->node_data->key, key) == 0){
                    if (found == NULL){
                        found = root;
                    }
                }
                root = root->r;
            } else {
                tmp->r = root;
                root = root->l;
            }
        } else {
            if (strcmp(root->node_data->key, key) == 0){
                if (found == NULL){
                    found = root;
                }
            }
            root = root->r;
        }
    }

    return found;
}

const char* bintree_fnc_arg_type_tostr(arg_type type){
    switch (type){
        case int_t:     return "int";     //int
        case float_t:   return "float";   //float
        case string_t:  return "string";  //string
        case void_t:    return "void";    //void

        case nint_t:    return "?int";    //?int
        case nfloat_t:  return "?float";  //?float
        case nstring_t: return "?string"; //?string

        default: return "ERROR"; //!!!
    }
}

const char* bintree_fnc_args_list_tostr(arg_type* args_list, size_t arg_cnt, char list[]){
    char tmp[32];
    for (size_t i = 0; i < arg_cnt; i++){
        snprintf(tmp, sizeof(tmp), "%s ",bintree_fnc_arg_type_tostr(args_list[i]));
        strcat(list,tmp);
    }
    return list;
}



//bintree INORDER PRINT FUNCTION
//non-recursive
void bintree_inorder(struct bintree_node *root){
    struct bintree_node *tmp; //forward checker
    //if (root != NULL) {printf("INORDER: root is [%d] %s\n", root->node_data->id, root->node_data->key);}
 
    while (root != NULL){
        if (root->l != NULL){
            tmp = root->l;
            while (tmp->r != NULL && tmp->r != root){
                tmp = tmp->r;
            }
 
            if (tmp->r == root){
                tmp->r = NULL;
                fprintf(stdout,"    [%ld] %s\n", root->node_data->id, root->node_data->key);
                root = root->r;
            } else {
                tmp->r = root;
                root = root->l;
            }
        } else {
            fprintf(stdout,"    [%ld] %s\n", root->node_data->id, root->node_data->key);
            root = root->r;
        }
    }
}

//bintree INORDER PRINT FUNCTION WHICH SAVES OUTPUT INTO PASSED STRING
//non-recursive
void bintree_inorder_saveoutput(struct bintree_node *root, char output[]){
    struct bintree_node *tmp; //forward checker
    bool firstflag = false;
    //if (root != NULL) {printf("INORDER: root is [%d] %s\n", root->node_data->id, root->node_data->key);}
 
    while (root != NULL){
        if (firstflag == false){
            firstflag = true;
        } else {
            strcat(output, " | ");
        }
        if (root->l != NULL){
            tmp = root->l;
            while (tmp->r != NULL && tmp->r != root){
                tmp = tmp->r;
            }
 
            if (tmp->r == root){
                tmp->r = NULL;
                char tmp[128];
                snprintf(tmp, sizeof(tmp), "[%ld]:%s",root->node_data->id, root->node_data->key);
                strcat(output, tmp);
                root = root->r;
            } else {
                tmp->r = root;
                root = root->l;
            }
        } else {
            char tmp[128];
            snprintf(tmp, sizeof(tmp), "[%ld]:%s",root->node_data->id, root->node_data->key);
            strcat(output, tmp);
            root = root->r;
        }
    }
}

//bintree INORDER PRINT FUNCTION WHICH PRINTS ALL ASPECTS OF NODE
//non-recursive
void bintree_inorder_fullprint(struct bintree_node *root, bool called_from_inside){
    struct bintree_node *tmp; //forward checker
    //if (root != NULL) {printf("INORDER: root is [%d] %s\n", root->node_data->id, root->node_data->key);}
 
    while (root != NULL){
        if (root->l != NULL){
            tmp = root->l;
            while (tmp->r != NULL && tmp->r != root){
                tmp = tmp->r;
            }
 
            if (tmp->r == root){
                tmp->r = NULL;
                if (strcmp(root->node_data->key, ":b") == 0) { //if it's prog_body func
                    fprintf(stdout,"  [prog_body]\n");
                    fprintf(stdout,"      └─> contains variables: \n");
                    bintree_inorder_currvarsonly(root);
                } else if (root->node_type == function){
                    fprintf(stdout,"    [fnc]%s   args_cnt: %ld   rtype: %s   arguments:",root->node_data->key, root->node_data->arg_cnt, bintree_fnc_arg_type_tostr(root->node_data->rtype));
                    dll_print_forwards(root->node_data->args_list);
                    fprintf(stdout,"\n      └─> its local symtable contains: \n");
                    if (root->local_symtab == NULL){
                        fprintf(stdout,"\n");
                    } else {
                        bintree_inorder_fullprint(root->local_symtab, true);
                    }
                } else {
                    if (called_from_inside == true){ fprintf(stdout,"                                       ");}
                    fprintf(stdout,"[var]%s   type: %s\n",root->node_data->key, bintree_fnc_arg_type_tostr(root->node_data->curr_type));
                }
                root = root->r;
            } else {
                tmp->r = root;
                root = root->l;
            }
        } else {
            if (strcmp(root->node_data->key, ":b") == 0) { //if it's prog_body func
                fprintf(stdout,"  [prog_body]\n");
                fprintf(stdout,"      └─> contains variables: \n");
                bintree_inorder_currvarsonly(root);
            } else if (root->node_type == function){
                fprintf(stdout,"    [fnc]%s   args_cnt: %ld   rtype: %s   arguments:",root->node_data->key, root->node_data->arg_cnt, bintree_fnc_arg_type_tostr(root->node_data->rtype));
                dll_print_forwards(root->node_data->args_list);
                fprintf(stdout,"\n      └─> its local symtable contains: \n");
                if (root->local_symtab == NULL){
                    fprintf(stdout,"\n");
                } else {
                    bintree_inorder_fullprint(root->local_symtab, true);
                }
            } else {
                if (called_from_inside == true){ fprintf(stdout,"                                       ");}
                fprintf(stdout,"[var]%s   type: %s\n",root->node_data->key, bintree_fnc_arg_type_tostr(root->node_data->curr_type));
            }
            root = root->r;
        }
    }
}

//bintree INORDER PRINT FUNCTION WHICH PRINTS ONLY NODES WITH TYPE 'variable' OF PASSED SYMTABLE (without delving into local symtable)
//non-recursive
void bintree_inorder_currvarsonly(struct bintree_node *root){
    struct bintree_node *tmp; //forward checker
    //if (root != NULL) {printf("INORDER: root is [%d] %s\n", root->node_data->id, root->node_data->key);}
 
    while (root != NULL){
        if (root->l != NULL){
            tmp = root->l;
            while (tmp->r != NULL && tmp->r != root){
                tmp = tmp->r;
            }
 
            if (tmp->r == root){
                tmp->r = NULL;
                if (root->node_type == variable){
                    fprintf(stdout,"                             [%s]%s : ",bintree_fnc_arg_type_tostr(root->node_data->curr_type), root->node_data->key);
                    if (root->node_data->curr_type == int_t || root->node_data->curr_type == nint_t || root->node_data->curr_type == float_t || root->node_data->curr_type == nfloat_t){
                        fprintf(stdout,"%.2f\n",root->node_data->value);
                    } else if (root->node_data->curr_type == string_t || root->node_data->curr_type == nstring_t) {
                        fprintf(stdout,"%s\n",root->node_data->string);
                    } else {
                        fprintf(stdout,"NULL\n");
                    }
                }
                root = root->r;
            } else {
                tmp->r = root;
                root = root->l;
            }
        } else {
            if (root->node_type == variable){
                fprintf(stdout,"                             [%s]%s : ",bintree_fnc_arg_type_tostr(root->node_data->curr_type), root->node_data->key);
                if (root->node_data->curr_type == int_t || root->node_data->curr_type == nint_t || root->node_data->curr_type == float_t || root->node_data->curr_type == nfloat_t){
                    fprintf(stdout,"%.2f\n",root->node_data->value);
                } else if (root->node_data->curr_type == string_t || root->node_data->curr_type == nstring_t) {
                    fprintf(stdout,"%s\n",root->node_data->string);
                } else {
                    fprintf(stdout,"NULL\n");
                }
            }
            root = root->r;
        }
    }
}
