/**
 * @file dll.c
 * @author Vojtěch Kališ (xkalis03)
 * @brief double-linked list implementation
 *
 * @date of last update:   4th August 2022
 * 
 * UPDATE NOTES: 01-08-2022:  - basic foundation laid out, lazy afternoon = not much work done
 *               02-08-2022:  - insertions done, testing them next
 *               04-08-2022:  - minor fixes to previous functions done, deletes done, tests incoming
 */

/**
 * TODO: - die
 */

#include "dll.h"

//
bool dll_is_empty(struct _dll *list){
    if (list == NULL){
        printf("ERROR:   DISCOVERED AN ATTEMPT AT ACCESSING A LIST THAT DOESN'T EXIST\n");
        exit(1);
    } else {
        return (list->head == NULL) ? true : false;
    }
}

//create double-linked list
struct _dll* dll_create (){
    struct _dll *list = NULL;
    list = (struct _dll *)malloc(sizeof(struct _dll));

    struct _dll_link *head = NULL;
    struct _dll_link *tail = NULL;

    list->head = head;
    list->tail = tail;

    return list;
}

//
void dll_insert_first(struct _dll *list, int id, char key[]){
    struct _dll_link *new;
    new = (struct _dll_link *)malloc(sizeof(struct _dll_link));
    _dll_data data = (_dll_data)malloc(sizeof(struct _dll_data));
    new->linkData = data;
    new->linkData->id = id;
    //printf("new->linkData->id = %d\n", new->linkData->id);
    strcpy(new->linkData->key, key);
    //printf("new->linkData->key = %s\n", new->linkData->key);
    new->prev = NULL;
    new->next = NULL;
	
    if (dll_is_empty(list) == true){ //if list is empty
        //make it the first and last link
        list->head = new;
        list->tail = new;
    } else {
        //point old first link to the new one
        list->head->prev = new;
        //point new first link to the old one
        new->next = list->head;
        //set new link as head
        list->head = new;
    }
}

//
void dll_insert_last(struct _dll *list, int id, char key[]){
    struct _dll_link *new;
    new = (struct _dll_link *)malloc(sizeof(struct _dll_link));
    _dll_data data = (_dll_data)malloc(sizeof(struct _dll_data));
    new->linkData = data;
    new->linkData->id = id;
    //printf("new->linkData->id = %d\n", new->linkData->id);
    strcpy(new->linkData->key, key);
    //printf("new->linkData->key = %s\n", new->linkData->key);
    new->prev = NULL;
    new->next = NULL;
	
    if (dll_is_empty(list) == true){ //if list is empty
        //make it the first and last link
        list->head = new;
        list->tail = new;
    } else {
        //point old last link to the new one
        list->tail->next = new;
        //point new last link to the old one
        new->prev = list->tail;
        //set new link as tail
        list->tail = new;
    }
}

//
void dll_insert_after(struct _dll *list, int id_searchedlink, int id_newlink, char key[]){
    struct _dll_link *new;
    new = (struct _dll_link *)malloc(sizeof(struct _dll_link));
    _dll_data data = (_dll_data)malloc(sizeof(struct _dll_data));
    new->linkData = data;
    new->linkData->id = id_newlink;
    //printf("new->linkData->id = %d\n", new->linkData->id);
    strcpy(new->linkData->key, key);
    //printf("new->linkData->key = %s\n", new->linkData->key);
    new->prev = NULL;
    new->next = NULL;

    if (dll_is_empty(list) == true){ //if list is empty
        list->head = new;
        list->tail = new;
    } else { //if list is not empty
        struct _dll_link *curr = list->head;

        //navigate through list
        while (curr->linkData->id != id_searchedlink){
            if (curr->next == NULL){  //if we reached last link
                printf("ERROR:   link  '[%d] %s'  not inserted because link with key [%d] not found\n", id_newlink, key, id_searchedlink);
                return;
            } else {           
                //move to next link
                curr = curr->next;
            }
        }

        if (curr == list->tail){
            //point old last link to the new one
            list->tail->next = new;
            //point new last link to the old one
            new->prev = list->tail;
            //set new link as tail
            list->tail = new;
        } else {
            new->next = curr->next;
            curr->next->prev = new;
            new->prev = curr;
            curr->next = new;
        }
    }

    return;
}

//
void dll_insert_before(struct _dll *list, int id_searchedlink, int id_newlink, char key[]){
    struct _dll_link *new;
    new = (struct _dll_link *)malloc(sizeof(struct _dll_link));
    _dll_data data = (_dll_data)malloc(sizeof(struct _dll_data));
    new->linkData = data;
    new->linkData->id = id_newlink;
    //printf("new->linkData->id = %d\n", new->linkData->id);
    strcpy(new->linkData->key, key);
    //printf("new->linkData->key = %s\n", new->linkData->key);
    new->prev = NULL;
    new->next = NULL;

    if (dll_is_empty(list) == true){ //if list is empty
        list->head = new;
        list->tail = new;
    } else { //if list is not empty
        struct _dll_link *curr = list->head;

        //navigate through list
        while (curr->linkData->id != id_searchedlink){
            if (curr->next == NULL){  //if we reached last link
                printf("ERROR:   link  '[%d] %s'  not inserted because link with key [%d] not found\n", id_newlink, key, id_searchedlink);
                return;
            } else {           
                //move to next link
                curr = curr->next;
            }
        }

        if (curr == list->head){
            //point old first link to the new one
            list->head->prev = new;
            //point new first link to the old one
            new->next = list->head;
            //set new link as head
            list->head = new;
        } else {
            new->prev = curr->prev;
            curr->prev->next = new;
            new->next = curr;
            curr->prev = new;
        }
    }

    return;
}

//
void dll_delete_first(struct _dll *list){
    if (dll_is_empty(list) == true){ //if list is empty
        printf("ERROR:   discovered an attempt at link deletion in an empty list\n");
        return;
    } else {
        struct _dll_link* second;
        second = list->head->next;

        //remove pointer from second link to head
        second->prev = NULL;
        //free head
        free(list->head->linkData);
        free(list->head);
        //set second link as head
        list->head = second;
    }
}

//
void dll_delete_last(struct _dll *list){
    if (dll_is_empty(list) == true){ //if list is empty
        printf("ERROR:   discovered an attempt at link deletion in an empty list\n");
        return;
    } else {
        struct _dll_link* second_to_last;
        second_to_last = list->tail->prev;

        //remove pointer from second-to-last link to tail
        second_to_last->next = NULL;
        //free tail
        free(list->tail->linkData);
        free(list->tail);
        //set second-to-last link as tail
        list->tail = second_to_last;
    }
}

//
void dll_delete_by_id(struct _dll *list, int id){
    if (dll_is_empty(list) == true){ //if list is empty
        printf("ERROR:   discovered an attempt at link deletion in an empty list %d\n", id);
        return;
    } else {
        struct _dll_link *curr = list->head;

        //navigate through list
        while (curr->linkData->id != id){
            if (curr->next == NULL){  //if we reached last link
                printf("ERROR:   link with key [%d] not deleted because it wasn't found\n", id);
                return;
            } else {           
                //move to next link
                curr = curr->next;
            }
        }

        if (curr == list->head){
            struct _dll_link* second;
            second = list->head->next;

            //remove pointer from second link to head
            second->prev = NULL;
            //free head
            free(list->head->linkData);
            free(list->head);
            //set second link as head
            list->head = second;
        } else if (curr == list->tail){
            struct _dll_link* second_to_last;
            second_to_last = list->tail->prev;

            //remove pointer from second-to-last link to tail
            second_to_last->next = NULL;
            //free tail
            free(list->tail->linkData);
            free(list->tail);
            //set second-to-last link as tail
            list->tail = second_to_last;
        } else {
            struct _dll_link* prev_link;
            struct _dll_link* next_link;

            prev_link = curr->prev;
            next_link = curr->next;

            prev_link->next = next_link;
            next_link->prev = prev_link;

            free(curr->linkData);
            free(curr);
        }
    }
}

//
void dll_destroy(struct _dll *list){
    struct _dll_link *next; //pointer to next link
    struct _dll_link *curr; //pointer to currently deleted link

    curr = list->head;
    
    if (dll_is_empty(list)){
        return;
    } else {
        while (curr != NULL){
            next = curr->next;
            free(curr->linkData);
            free(curr);
            curr = next;
        }
    }

    free(list);
}

//
void dll_print_forwards(struct _dll *list){
    if (dll_is_empty(list) == false){
        struct _dll_link *ptr = list->head;

        while (ptr != list->tail){
            printf("    [%d] %s\n", ptr->linkData->id, ptr->linkData->key);
            ptr = ptr->next;
        }

        //print the tail as well
        printf("    [%d] %s\n", ptr->linkData->id, ptr->linkData->key);
    }
}

//
void dll_print_backwards(struct _dll *list){
    if (dll_is_empty(list) == false){
        struct _dll_link *ptr = list->tail;

        while (ptr != list->head){
            printf("    [%d] %s\n", ptr->linkData->id, ptr->linkData->key);
            ptr = ptr->prev;
        }

        //print the head as well
        printf("    [%d] %s\n", ptr->linkData->id, ptr->linkData->key);
    }
}