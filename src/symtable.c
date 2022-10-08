/**
 * @file symtable.c
 * @author Vojtěch Kališ (xkalis03)
 * @brief binary tree implementation
 *
 * @date of last update:   28th July 2022
 * 
 * UPDATE NOTES: 20-07-2022:  - basic functionality done
 *               28-07-2022:  - after some unavailability time and a following streak of laziness, had 
 *                              a look into the Deletes issue
 *                            - everything seems to be working, the remaining TODOs are minor enough to 
 *                              where they can be ignored for now and focus can be diverted elsewhere
 */

/**
 * TODO: - beautify prints
 *       - make dispose non-recursive (perhaps once stack is implemented?)
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "symtable.h"

//BINTREE INIT FUNCTION
void binTree_init(_binTree_node *root){
    if (root != NULL){
        *root = NULL;
    }
}

//BINTREE DISPOSE FUNCTION
//recursive
void binTree_dispose(struct _binTree_node *root){
    //printf("DISPOSE: root pointing at [%d] %s\n", root->nodeData->id, root->nodeData->key);
    binTree_dispose_internal(root);
    root = NULL;
}
void binTree_dispose_internal(struct _binTree_node *root){
    if (root == NULL){
        return;
    }
  
    //dispose of both subtrees
    binTree_dispose_internal(root->l);
    binTree_dispose_internal(root->r);
    
    //then delete the node
    //printf("DELETING: [%d] %s\n", root->nodeData->id, root->nodeData->key);
    free (root->nodeData);
    free (root);
}

//BINTREE NODE INSERT FUNCTION
//non-recursive
struct _binTree_node* binTree_insert(struct _binTree_node *root, int id, char key[]){
    struct _binTree_node *tmp, *par, *ptr;

    ptr = root;
    par = NULL;

    while (ptr != NULL){
        par = ptr;
        if (id < ptr->nodeData->id){
            ptr = ptr->l;
        } else if (id > ptr->nodeData->id ){
            ptr = ptr->r;
        } else {
            fprintf(stderr,"ERROR: trying to insert a node with an already existing ID\n");
            return root;
        }
    }

    tmp = (struct _binTree_node *)malloc(sizeof(struct _binTree_node));
    _binTree_data data = (_binTree_data)malloc(sizeof(struct _binTree_data));
    tmp->nodeData = data;
    tmp->nodeData->id = id;
    //printf("tmp->nodeData->id = %d\n", tmp->nodeData->id);
    strcpy(tmp->nodeData->key, key);
    //printf("tmp->nodeData->key = %s\n", tmp->nodeData->key);
    tmp->l = NULL;
    tmp->r = NULL;

    if (par == NULL){
        root = tmp;
    } else if (id < par->nodeData->id){
        par->l = tmp;
    } else {
        par->r = tmp;
    }

    return root;
}

//BINTREE NODE DELETE BY ID FUNCTION
//non-recursive
void binTree_delete_by_id(struct _binTree_node *root, int id){
    struct _binTree_node *par, *ptr;

    ptr = root;
    par = NULL;

    while (ptr != NULL){
        if (id == ptr->nodeData->id){
            binTree_delete_aux(ptr, par);
            break;
        }
        par = ptr;
        if (id > ptr->nodeData->id){
            ptr = ptr->r;
        } else {
            ptr = ptr->l;
        }
    }

    if (ptr == NULL){
        printf("WARNING:  in binTree_delete_by_id:  no node with ID %d found\n", id);
    }

    return;
}

//BINTREE NODE DELETE BY KEY FUNCTION
//non-recursive (using the principle of inorder traversal)
void binTree_delete_by_key(struct _binTree_node *root, char key[]){
    struct _binTree_node *par, *ptr, *tmp; //parent, current pointer, auxiliary
    int cmp = 0;

    ptr = root;
    par = NULL;
    
    while (ptr != NULL){
        if (ptr->l != NULL){ //something's to the left
            tmp = ptr->l; //explore it with tmp
            while (tmp->r != NULL && tmp->r != ptr){
                tmp = tmp->r;
            }
    
            if (tmp->r == ptr){
                tmp->r = NULL;
                if ((cmp = strcmp(ptr->nodeData->key, key)) == 0){
                    binTree_delete_aux(ptr, par);
                    break;
                }
                par = ptr;
                ptr = ptr->r;
            } else {
                tmp->r = ptr;
                par = ptr;
                ptr = ptr->l;
            }
        } else {
            if ((cmp = strcmp(ptr->nodeData->key, key)) == 0){
                binTree_delete_aux(ptr, par);
                break;
            }
            par = ptr;
            ptr = ptr->r;
        }
    }

    //printf(" ptr pointing at [%d] %s\n", ptr->nodeData->id, ptr->nodeData->key);
    //if (par != NULL) {printf(" par pointing at [%d] %s\n", par->nodeData->id, par->nodeData->key);}
    //printf(" root pointing at [%d] %s\n", root->nodeData->id, root->nodeData->key);

    if (ptr == NULL){
        printf("WARNING:  in binTree_delete_by_key:  no node with KEY \"%s\" found\n", key);
    }

    return;
}

//BINTREE NODE DELETE AUXILIARY FUNCTION
//the actual deletor
void binTree_delete_aux(struct _binTree_node *ptr, struct _binTree_node *par){
    struct _binTree_node *succ, *tmp;
    int id = ptr->nodeData->id;

    if (par == NULL){ //deleted node is the root
        succ = NULL; //parent
 
        // Compute the inorder successor
        tmp = ptr->r;
        while (tmp->l != NULL) {
            succ = tmp;
            tmp = tmp->l;
        }
 
        if (succ != NULL){
            succ->l = tmp->r;
        } else {
            ptr->r = tmp->r;
        }
 
        ptr->nodeData->id = tmp->nodeData->id;
        strcpy(ptr->nodeData->key, tmp->nodeData->key);
        
        free(tmp->nodeData);
        free(tmp);
        return;
    }

    tmp = ptr;
    if (tmp->r == NULL){
        ptr = ptr->l;
    } else if (tmp->r->l == NULL){
        ptr = ptr->r;
        ptr->l = tmp->l;
    } else {
        succ = ptr->r;
        while (succ->l->l != NULL){
            succ = succ->l;
        }
        ptr = succ->l;
        succ->l = ptr->r;
        ptr->l = tmp->l;
        ptr->r = tmp->r;
    }

    free(tmp->nodeData);
    free(tmp);

    if (par != NULL){
        if (id < par->nodeData->id){
            par->l = ptr;
        } else {
            par->r = ptr;
        }
    }
}





//BINTREE NODE SEARCH BY ID FUNCTION
//non-recursive
struct _binTree_node* binTree_search_by_id (struct _binTree_node *root, int id){
    while (root != NULL){
        //printf("id is %d\n", id);
        //printf("root->nodeData->id is %d\n", root->nodeData->id);
        if (id < root->nodeData->id){
            root = root->l; /*Move to left child*/
        } else if (id > root->nodeData->id){
            root = root->r;  /*Move to right child */
        } else { /*id found*/
            return root;
        }
    }

    return NULL;
}

//BINTREE NODE SEARCH BY KEY FUNCTION
//non-recursive (basically just inorder traversal)
struct _binTree_node* binTree_search_by_key (struct _binTree_node *root, char key[]){
    struct _binTree_node *tmp; //forward checker
    int cmp = 0;
 
    while (root != NULL){
        if (root->l != NULL){
            tmp = root->l;
            while (tmp->r != NULL && tmp->r != root){
                tmp = tmp->r;
            }
 
            if (tmp->r == root){
                tmp->r = NULL;
                if ((cmp = strcmp(root->nodeData->key, key)) == 0){
                    return root;
                }
                root = root->r;
            } else {
                tmp->r = root;
                root = root->l;
            }
        } else {
            if ((cmp = strcmp(root->nodeData->key, key)) == 0){
                return root;
            }
            root = root->r;
        }
    }

    return NULL;
}

//BINTREE INORDER PRINT FUNCTION
//non-recursive
void binTree_inorder(struct _binTree_node *root){
    struct _binTree_node *tmp; //forward checker
    //if (root != NULL) {printf("INORDER: root is [%d] %s\n", root->nodeData->id, root->nodeData->key);}
 
    while (root != NULL){
        if (root->l != NULL){
            tmp = root->l;
            while (tmp->r != NULL && tmp->r != root){
                tmp = tmp->r;
            }
 
            if (tmp->r == root){
                tmp->r = NULL;
                printf("    [%d] %s\n", root->nodeData->id, root->nodeData->key);
                root = root->r;
            } else {
                tmp->r = root;
                root = root->l;
            }
        } else {
            printf("    [%d] %s\n", root->nodeData->id, root->nodeData->key);
            root = root->r;
        }
    }
}
