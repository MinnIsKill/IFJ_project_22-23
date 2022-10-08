/**
 * @file vector.c
 * @author Vojtěch Kališ (xkalis03)
 * @brief vector implementation
 */

#include "vector.h"

//initializes vector
void vector_init(struct _vector *vect){
    vect->children_cnt = 0; //total amount of stuff inserted to _vector
    vect->capacity = INIT_VECT_CAP;
    vect->next = malloc(vect->capacity * sizeof(void*)); //void, for now (change 'sizeof(void*)' if changing pointer in vector.h)
}

//doubles the vector's current size
void vector_upsize(struct _vector *vect){
    vect->capacity = vect->capacity * 2;
    void **next;
    if ((next = realloc(vect->next, vect->capacity * sizeof(void*))) == NULL){
        fprintf(stderr,"ERROR:  in vector_upsize: realloc failed\n");
    } else {
        vect->next = next;
    }
}

//halves the vector's current size
void vector_downsize(struct _vector *vect){
    vect->capacity = vect->capacity / 2;
    void **next = realloc(vect->next, vect->capacity * sizeof(void*)); //void, for now (change 'sizeof(void*)' if changing pointer in vector.h)
    vect->next = next;
}

//appends new item to end of vector
void vector_append(struct _vector *vect, void *item){ //void, for now (change 'void *item' if changing pointer in vector.h)
    if (vect->capacity == vect->children_cnt){
        vector_upsize(vect);
    }
    vect->next[vect->children_cnt++] = item;
}

//overwrites an item at 'index' with a new one
void vector_set(struct _vector *vect, size_t index, void *item){ //void, for now (change 'void *item' if changing pointer in vector.h)
    if (index >= vect->children_cnt){ //size_t will already always be >= 0, so there's no need to check that condition
        fprintf(stderr,"ERROR: found a 'vector_set' call with an out-of-bounds argument.\n       Function call was skipped (nothing was done).\n");
    } else {
        vect->next[index] = item;
    }
}

//returns item at 'index' from vector
void *vector_get(struct _vector *vect, size_t index){
    if (index >= vect->children_cnt){ //size_t will already always be >= 0, so there's no need to check that condition
        fprintf(stderr,"ERROR: found a 'vector_get' call with an out-of-bounds argument.\n       Function call was skipped (nothing was done).\n");
        return NULL;
    } else {
        return vect->next[index];
    }
}

//deletes item at 'index' from vector
void vector_delete(struct _vector *vect, size_t index){
    if (index >= vect->children_cnt){ //size_t will already always be >= 0, so there's no need to check that condition
        fprintf(stderr,"ERROR: found a 'vector_delete' call with an out-of-bounds argument.\n       Function call was skipped (nothing was done).\n");
        return;
    }

    //move every following item one by one to the left
    for (size_t i = index; i < vect->children_cnt - 1; i++){
        vect->next[i] = vect->next[i + 1];
    }
    vect->next[vect->children_cnt - 1] = NULL;
    vect->children_cnt--;

    if (vect->children_cnt > 0 && vect->children_cnt == vect->capacity / 4)
        vector_downsize(vect);
}

//frees whole vector
void vector_free(struct _vector *vect){
    free(vect->next);
}

//prints vector contents item by item
void vector_print(struct _vector *vect){
    for (size_t i = 0; i < vect->children_cnt; i++){
        printf("item #%ld:  [%s]\n", i, (char*) vector_get(vect, i));
    }   
}