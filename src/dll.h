/**
 * @file dll.h
 * @author Vojtěch Kališ (xkalis03)
 * @brief double-linked list implementation
 *
 * @date of last update:   4th August 2022
 */

#ifndef BINTREE_H
#define BINTREE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

struct _dll_data{
    int id;
    char key[32];
};
typedef struct _dll_data* _dll_data;

/* link structure */
struct _dll_link{
    struct _dll_data* linkData;
    struct _dll_link* next; //next node ptr
    struct _dll_link* prev; //prev node ptr
};
typedef struct _dll_link* _dll_link;

struct _dll{
    struct _dll_link* head;
    struct _dll_link* tail;
};
typedef struct _dll* _dll;


//
bool dll_is_empty (struct _dll *list);

//
struct _dll* dll_create ();

//
void dll_insert_first (struct _dll *list, int id, char key[]);

//
void dll_insert_last (struct _dll *list, int id, char key[]);

//
void dll_insert_after (struct _dll *list, int id_oldlink, int id_newlink, char key[]);

//
void dll_insert_before (struct _dll *list, int id_oldlink, int id_newlink, char key[]);

//
void dll_delete_first(struct _dll *list);

//
void dll_delete_last(struct _dll *list);

//
void dll_delete_by_id(struct _dll *list, int id);

//
void dll_destroy(struct _dll *list);

//
void dll_print_forwards (struct _dll *list);

//
void dll_print_backwards (struct _dll *list);

#endif