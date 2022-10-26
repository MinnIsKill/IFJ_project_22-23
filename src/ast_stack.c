/**
 * @brief Stack implemented using vector
 * @file ast_stack.c
 * @author Jan Lutonský, xluton02
 **/

#include"ast_stack.h"

bool ast_stack_init(ast_stack* s, size_t init_size)
{
    return(g_stack_init((g_stack*)s,init_size));
}

void ast_stack_destroy(ast_stack* s)
{
    if(s == NULL)
    {
        // no need to destroy
        return;
    }

    // empty stack before deleting it
    while(((g_stack*)s)->index > 0)
    {
        node_delete((ast_node**)&(((g_stack*)s)->data[--((g_stack*)s)->index]));
    }
    g_stack_destroy((g_stack*)s);
}

void ast_stack_reset(ast_stack* s)
{
    if(s == NULL)
    {
        // no need to reset
        return;
    }

    while(((g_stack*)s)->index > 0)
    {
        node_delete((ast_node**)&(((g_stack*)s)->data[--((g_stack*)s)->index]));
    }
}


bool ast_stack_push(ast_stack* s,ast_node* node)
{
    return(g_stack_push(((g_stack*)s),node));
}

ast_node* ast_stack_top(ast_stack* s)
{
    return(g_stack_top((g_stack*)s));
}

bool ast_stack_pop(ast_stack* s)
{
    if (s == NULL)
    {
        dbgprint("You passed NULL, nothig happens.");
        return(false);
    }
    if(((g_stack*)s)->index == 0)
    {
        dbgprint("You are trying pop empty stack.");
        return(false);
    }
    node_delete((ast_node**)&(((g_stack*)s)->data[--((g_stack*)s)->index]));
    ((g_stack*)s)->data[((g_stack*)s)->index] = NULL;
    return(true);
}

ast_node* ast_stack_peel(ast_stack* s)
{
    return(g_stack_peel((g_stack*)s));
}

bool ast_stack_is_empty(ast_stack* s)
{
    return(g_stack_is_empty((g_stack*)s));
}

ast_node* ast_stack_dive(ast_stack* s, size_t n)
{
    return(g_stack_dive((g_stack*)s,n));
}

