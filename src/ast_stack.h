/*
 * @brief Stack implemented using vector
 * @file ast_stack.h
 * @author Jan Lutonský, xluton02
 *
 * @warning push only root nodes to stack
 * @warning do not use g_stack function directly
 *
 * typed wrapper for g_stack
 * @see g_stack.h
 */
#ifndef INCLUDE_AST_STACK_H
#define INCLUDE_AST_STACK_H

#include<stdlib.h>
#include<stdio.h>
#include<stdbool.h>

#include"ast.h"
#include"g_stack.h"
#include"dbg.h"

typedef struct{
    // private do not acces this
    g_stack stack; 
}ast_stack;

/*
 * @see g_stack.h
 */
bool ast_stack_init(ast_stack* s, size_t init_size);

/*
 * @brief Free memmory allocated by ast_stack
 *        it will also free every node in ast_stack.
 *        passing null does nothing
 * @param s ast_stack that will be freed
 */
void ast_stack_destroy(ast_stack* s);

/*
 * @brief Empty the stack
 *        it will also free every node in ast_stack
 *        but wont free children buffer.
 *        passing null does nothing
 * @param s ast_stack that will be freed
 */
void ast_stack_reset(ast_stack* s);

/*
 * @see g_stack.h
 */
bool ast_stack_push(ast_stack* s,ast_node* node);

/*
 * @see g_stack.h
 */
ast_node* ast_stack_top(ast_stack* s);


/*
 * @brief Destroy element on top of ast_stack
 * @warrning do not use this function on empty
 *           ast_stack, checked by assert()
 * @param[in] s input ast_stack
 * @return false if you passed NULL or if stack is empty
 *         true otherwise
 */
bool ast_stack_pop(ast_stack* s);

/*
 * @see g_stack.h
 */
ast_node* ast_stack_peel(ast_stack* s);

/*
 * @see g_stack.h
 */
bool ast_stack_is_empty(ast_stack* s);


/*
 * @see g_stack.h
 */
ast_node* ast_stack_dive(ast_stack* s, size_t n);

#endif
