/**
 * @brief Stack implemented using vector
 * @file ast_stack.h
 * @author Jan Lutonský, xluton02
 *
 * @warning push only root nodes to stack
 * @warning do not use g_stack function directly
 *
 * typed wrapper for g_stack
 * @see g_stack.h
 **/
#ifndef INCLUDE_AST_STACK_H
#define INCLUDE_AST_STACK_H

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

#include"symtable.h"
#include"g_stack.h"
#include"dbg.h"

typedef struct{
    // private do not acces this
    g_stack stack; 
}symtable_stack;

/*
 * @see g_stack.h
 */
bool symtab_stack_init(symtable_stack* s, size_t init_size);

/*
 * @brief Free memmory allocated by ast_stack
 *        it will also free every node in ast_stack.
 *        passing null does nothing
 * @param s ast_stack that will be freed
 */
void symtab_stack_destroy(symtable_stack* s);

/*
 * @brief Empty the stack
 *        it will also free every node in ast_stack
 *        but wont free children buffer.
 *        passing null does nothing
 * @param s ast_stack that will be freed
 */
void symtab_stack_reset(symtable_stack* s);

/*
 * @see g_stack.h
 */
bool symtab_stack_push(symtable_stack* s,struct bintree_node* node);

/*
 * @see g_stack.h
 */
struct bintree_node* symtable_stack_top(symtable_stack* s);


/**
 * @brief Destroy element on top of ast_stack
 * @warrning do not use this function on empty
 *           ast_stack, checked by assert()
 * @param[in] s input ast_stack
 * @return false if you passed NULL or if stack is empty
 *         true otherwise
 */
bool symtable_stack_pop(symtable_stack* s);

/**
 * @see g_stack.h
 */
struct bintree_node* symtable_stack_peel(symtable_stack* s);

/**
 * @see g_stack.h
 **/
bool symtable_stack_is_empty(symtable_stack* s);


/*
 * @see g_stack.h
 */
struct bintree_node* ast_stack_dive(symtable_stack* s, size_t n);

#endif
