/**
 * @file _vector.h
 * @author Vojtěch Kališ (xkalis03)
 * @brief _vector implementation
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define INIT_VECT_CAP 8; //init _vector capacity

struct _vector{
    size_t children_cnt;
    size_t capacity;
    //struct _DATA_IDK *next;
    void **next;
};
typedef struct _vector _vector;

//initializes vector
void vector_init(struct _vector*);

//doubles the vector's current size
void vector_upsize(struct _vector*);
//halves the vector's current size
void vector_downsize(struct _vector*);

//appends new item to end of vector
void vector_append(struct _vector*, void*);

//overwrites an item at 'index' with a new one
void vector_set(struct _vector*, size_t, void*);

//returns item at 'index' from vector
void *vector_get(struct _vector*, size_t);

//deletes item at 'index' from vector
void vector_delete(struct _vector*, size_t);

//frees whole vector
void vector_free(struct _vector*);

//prints vector contents item by item
void vector_print(struct _vector*);

#endif