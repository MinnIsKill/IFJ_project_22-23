/**
 * @brief Stack implemented using vector
 * @file symtable_stack.c
 * @author Jan Lutonský, xluton02
 **/

#include"symtable_stack.h"

bool symtable_stack_init(symtable_stack* s, size_t init_size)
{
    return(g_stack_init((g_stack*)s,init_size));
}

void symtable_stack_destroy(symtable_stack* s)
{
    if(s == NULL)
    {
        // no need to destroy
        return;
    }

    // empty stack before deleting it
    while(((g_stack*)s)->index > 0)
    {
        bintree_dispose((struct bintree_node*)&(((g_stack*)s)->data[--((g_stack*)s)->index]));
    }
    g_stack_destroy((g_stack*)s);
}

void symtable_stack_reset(symtable_stack* s)
{
    if(s == NULL)
    {
        // no need to reset
        return;
    }

    while(((g_stack*)s)->index > 0)
    {
        bintree_dispose((struct bintree_node*)&(((g_stack*)s)->data[--((g_stack*)s)->index]));
    }
}


bool symtable_stack_push(symtable_stack* s,struct bintree_node* node)
{
    return(g_stack_push(((g_stack*)s),node));
}

struct bintree_node* symtable_stack_top(symtable_stack* s)
{
    return(g_stack_top((g_stack*)s));
}

bool symtable_stack_pop(symtable_stack* s)
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
    bintree_dispose((struct bintree_node*)&(((g_stack*)s)->data[--((g_stack*)s)->index]));
    ((g_stack*)s)->data[((g_stack*)s)->index] = NULL;
    return(true);
}

struct bintree_node* symtable_stack_peel(symtable_stack* s)
{
    return(g_stack_peel((g_stack*)s));
}

bool symtable_stack_is_empty(symtable_stack* s)
{
    return(g_stack_is_empty((g_stack*)s));
}

struct bintree_node* symtable_stack_dive(symtable_stack* s, size_t n)
{
    return(g_stack_dive((g_stack*)s,n));
}

