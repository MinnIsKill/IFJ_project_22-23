/*
 * @brief "Generic" stack of void pointers
 *        implementad using vector
 * @file g_stack.h
 * @author Jan Lutonský, xluton02
 *
 * @warning before using any function
 *           stack must be first initialized
 *           with ast_stack_init()
 *
 * @warning stack must be freed using stack_destroy()
 *
 * compile with -DDEBUG to get additional debugging
 * messages.
 *
 * TODO make all of these functions inline, since
 * they won't be used on theyr own but rather in 
 * concrete implementation for one specific type
 */
#ifndef INCLUDE_G_STACK_H
#define INCLUDE_G_STACK_H

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

#include"dbg.h"

// default 24
#define G_STACK_CHUNK 24
// must be in range <1,size_t-max>
// default 8
#define G_STACK_MIN_SIZE 8 

typedef struct{
    size_t size;     // amount of allocated slots
    size_t index;    // index of next free slot
    void** data;     // array of slots(pointers to something)
}g_stack;

/**
 * @brief Initialize stack with init_size slots
 * @param[in] s stack that will be initialized
 * @param[in] init_size amount of slots that will be initialized
 * @return false if you pass NULL or if memory allocation failed
 * @return true if stack was initialize properrly
 **/
bool g_stack_init(g_stack* s, size_t init_size);

/**
 * @brief Free memmory allocated by ast_stack
 * @warning you need to empty stack yourselfe
 *          using g_stack_peel() 0in loop, because stack
 *          doesnt know or care how to free data
 *          that your pointer points to
 * @param s stack that will be freed
 * @see examples below for emptying the stack
 **/
bool g_stack_destroy(g_stack* s);

/**
 * @brief Push data on top of stack
 * @param[in] s stack on which data will be pushed
 * @param[in] data data that will be pushed
 * @return false if memory reallocation fails
 *         or if (s = NULL || data = NULL)
 * @return true if data was pushed
 **/
bool g_stack_push(g_stack* s,void* data);

/**
 * @brief Return element element from top of stack
 * @note this function is equvivalent to g_stack_dive(...,0);
 * @param[in] s input stack
 * @return element on top of stack or NULL
 *         if stack is empty or you passed NULL
 */
void* g_stack_top(g_stack* s);


/**
 * @brief Destroy element on top of ast_stack
 * @warrning do not use this function on empty
 *           ast_stack, checked by assert()
 * @param[in] s input ast_stack
 * @return false if you passed NULL or if stack is empty
 *         true otherwise
 */
bool g_stack_pop(g_stack* s);

/**
 * @brief Take top element out of ast_stack and return it
 * @param[in] s input stack
 * @return element that **was** on top of ast_stack
 *         or null if you passed NULL or if stack is empty
 */
void* g_stack_peel(g_stack* s);

/**
 * @brief Test if stack is empty
 * @param[in] s input stack
 * @return true if ast_stack is empty
 * @return false if ast_stack is not 
 *         empty or you passed NULL
 **/
bool g_stack_is_empty(g_stack* s);

/**
 * @brief Return (n-1)-th element from top of ast_stack
 * @param[in] n index of element from top of the ast_stack
 *              that will be returned
 * @return element of ast_stack
 * @return NULL if n is out of range or if stack
 *         is empty or if you pass NULL
 **/
void* g_stack_dive(g_stack* s, size_t n);

// ======== example functions ======

/* empty stack == reset
while(s->index > 0)
{
    <your free function>(&(s->data[--s->index]));
}
*/


/* pop
bool <yoru name>_stack_pop(<your type>_stack* s)
{
    if (s == NULL)
    {
        dbgprint("You passed NULL, nothig happens.");
        return(false);
    }
    if(s->index == 0)
    {
        dbgprint("You are trying pop empty stack.");
        return(false);
    }
    <your free function>(&(s->data[--s->index]));
    s->data[s->index] = NULL;
    return(true);
}
*/
#endif
