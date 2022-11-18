/**
 * @file dll.h
 * @author Vojtěch Kališ (xkalis03)
 * @brief double-linked list implementation
 *
 * @date of last update:   4th August 2022
 */

#ifndef DLL_H
#define DLL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "dbg.h"

struct dll_data{
    size_t id;
    char type[32]; //var type
    char key[32];  //var name
};
typedef struct dll_data* dll_data;

/* link structure */
struct dll_link{
    struct dll_data* linkData;
    struct dll_link* next; //next node ptr
    struct dll_link* prev; //prev node ptr
};
typedef struct dll_link* dll_link;

struct dll{
    struct dll_link* head;
    struct dll_link* tail;
};
typedef struct dll* dll;


//
bool dll_is_empty (struct dll *list);

//
struct dll* dll_create ();

//
int dll_insert_first (struct dll *list, size_t id, char key[], char type[]);

//
int dll_insert_last (struct dll *list, size_t id, char key[], char type[]);

//
int dll_insert_after (struct dll *list, size_t id_oldlink, size_t id_newlink, char key[], char type[]);

//
int dll_insert_before (struct dll *list, size_t id_oldlink, size_t id_newlink, char key[], char type[]);

//
void dll_delete_first(struct dll *list);

//
void dll_delete_last(struct dll *list);

//
void dll_delete_by_id(struct dll *list, size_t id);

//
struct dll_link* dll_search_by_key(struct dll *list, char key[]);

//
void dll_destroy(struct dll *list);

//
void dll_print_forwards (struct dll *list);

//
void dll_print_backwards (struct dll *list);

#endif