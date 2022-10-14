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

//bintree INIT FUNCTION
void bintree_init(bintree_node *root){
    if (root != NULL){
        *root = NULL;
    }
}

//bintree DISPOSE FUNCTION
//recursive
void bintree_dispose(struct bintree_node *root){
    //printf("DISPOSE: root pointing at [%d] %s\n", root->nodeData->id, root->nodeData->key);
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
    //printf("DELETING: [%d] %s\n", root->nodeData->id, root->nodeData->key);
    free (root->nodeData);
    free (root);
}

//bintree NODE INSERT FUNCTION
//non-recursive
struct bintree_node* bintree_insert(struct bintree_node *root, int id, char key[]){
    struct bintree_node *tmp, *par, *ptr;

    ptr = root;
    par = NULL;

    while (ptr != NULL){
        par = ptr;
        if (id < ptr->nodeData->id){
            ptr = ptr->l;
        } else if (id > ptr->nodeData->id ){
            ptr = ptr->r;
        } else {
            fprintf(stderr,"ERROR:  in bintree_insert:  trying to insert a node with an already existing ID\n");
            return root;
        }
    }

    tmp = (struct bintree_node *)malloc(sizeof(struct bintree_node));
    bintree_data data = (bintree_data)malloc(sizeof(struct bintree_data));
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

//bintree NODE DELETE BY ID FUNCTION
//non-recursive
struct bintree_node* bintree_delete_by_id(struct bintree_node *root, int id){
    struct bintree_node *par; //parent node ptr
    struct bintree_node *ptr;

    ptr = root;
    par = NULL;

    while (ptr != NULL){
        //printf("\n? scanning: [%d]\n\n",ptr->nodeData->id);
        if (id == ptr->nodeData->id){
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
                free(ptr->nodeData);
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
                free(ptr->nodeData);
                free(ptr);
            } else {
                ;
            }
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
        printf("WARNING:  in bintree_delete_by_id:  no node with ID %d found\n", id);
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
                if ((cmp = strcmp(ptr->nodeData->key, key)) == 0){
                    bintree_delete_by_id(root, ptr->nodeData->id);
                    break;
                }
                ptr = ptr->r;
            } else {
                tmp->r = ptr;
                ptr = ptr->l;
            }
        } else {
            if ((cmp = strcmp(ptr->nodeData->key, key)) == 0){
                bintree_delete_by_id(root, ptr->nodeData->id);
                break;
            }
            ptr = ptr->r;
        }
    }

    //printf(" ptr pointing at [%d] %s\n", ptr->nodeData->id, ptr->nodeData->key);
    //if (par != NULL) {printf(" par pointing at [%d] %s\n", par->nodeData->id, par->nodeData->key);}
    //printf(" root pointing at [%d] %s\n", root->nodeData->id, root->nodeData->key);

    if (ptr == NULL){
        printf("WARNING:  in bintree_delete_by_key:  no node with KEY \"%s\" found\n", key);
    }

    return root;
}





//bintree NODE SEARCH BY ID FUNCTION
//non-recursive
struct bintree_node* bintree_search_by_id (struct bintree_node *root, int id){
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

//bintree NODE SEARCH BY KEY FUNCTION
//non-recursive (basically just inorder traversal)
struct bintree_node* bintree_search_by_key (struct bintree_node *root, char key[]){
    struct bintree_node *tmp; //forward checker
    struct bintree_node *found = NULL;
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
            if ((cmp = strcmp(root->nodeData->key, key)) == 0){
                if (found == NULL){
                    found = root;
                }
            }
            root = root->r;
        }
    }

    return found;
}

//bintree INORDER PRINT FUNCTION
//non-recursive
void bintree_inorder(struct bintree_node *root){
    struct bintree_node *tmp; //forward checker
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

//bintree INORDER PRINT FUNCTION WHICH SAVES OUTPUT INTO PASSED STRING
//non-recursive
void bintree_inorder_saveoutput(struct bintree_node *root, char output[]){
    struct bintree_node *tmp; //forward checker
    bool firstflag = false;
    //if (root != NULL) {printf("INORDER: root is [%d] %s\n", root->nodeData->id, root->nodeData->key);}
 
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
                snprintf(tmp, sizeof(tmp), "[%d]:%s",root->nodeData->id, root->nodeData->key);
                strcat(output, tmp);
                root = root->r;
            } else {
                tmp->r = root;
                root = root->l;
            }
        } else {
            char tmp[128];
            snprintf(tmp, sizeof(tmp), "[%d]:%s",root->nodeData->id, root->nodeData->key);
            strcat(output, tmp);
            root = root->r;
        }
    }
}