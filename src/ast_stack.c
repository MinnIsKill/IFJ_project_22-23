/**
 * @brief Stack implemented using vector
 * @file ast_stack.h
 * @author Jan Lutonský, xluton02
 **/

#include"ast_stack.h"

bool ast_stack_init(ast_stack* s, size_t init_size)
{
    if(s == NULL)
    {
        dbgprint("Can not initialize NULL, nothing happens.");
        return(false);
    }
    if(init_size < AST_STACK_MIN_SIZE)
    {
        init_size = AST_STACK_MIN_SIZE;
    }

    s->index = 0;
    s->data = (ast_node**)calloc(init_size,sizeof(ast_node*));
    if(s->data == NULL)
    {
        s->size = 0;
        dbgprint("Stack failed to allocate memory for datta buffer.");
        return(false);
    }
        
    s->size = init_size;
    return(true);
}

void ast_stack_destroy(ast_stack* s)
{
    if(s == NULL)
    {
        // no need to destroy
        return;
    }

    while(s->index > 0)
    {
        node_delete(&(s->data[--s->index]));
    }
    free(s->data);
    s->size=0;
    //index is set to 0 by while cycle
}

void ast_stack_reset(ast_stack* s)
{
    if(s == NULL)
    {
        // no need to reset
        return;
    }

    while(s->index > 0)
    {
        node_delete(&(s->data[--s->index]));
    }
}


bool ast_stack_push(ast_stack* s,ast_node* node)
{
    if(s == NULL)
    {
        dbgprint("You passed NULL, nothig happens.");
        return(false);
    }
    if(node == NULL)
    {
        dbgprint("You are trying to push NULL onto the stack, you should not need to push NULL onto the stack!");
        return(false);
    }
    if(s->index+1 > s->size)
    {
        //realloc
        ast_node** tmp = (ast_node**)realloc(s->data,(s->size+AST_STACK_CHUNK)*sizeof(ast_node*));
        if(tmp == NULL)
        {
            dbgprint("Stack push failed to realloc memory.");
            return(false);
        }
        s->data = tmp;
        s->size += AST_STACK_CHUNK;
    }
    s->data[s->index++] = node;
    return(true);
}

ast_node* ast_stack_top(ast_stack* s)
{
    if (s == NULL)
    {
        dbgprint("You passed NULL, nothig happens.");
        return(NULL);
    }

    if(s->index == 0)
    {
        dbgprint("You are trying to top empty stack.");
        return(NULL);
    }

    return(s->data[s->index-1]);
}

bool ast_stack_pop(ast_stack* s)
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
    node_delete(&(s->data[--s->index]));
    s->data[s->index] = NULL;
    return(true);
}

ast_node* ast_stack_peel(ast_stack* s)
{
    if (s == NULL)
    {
        dbgprint("You passed NULL, nothig happens.");
        return(NULL);
    }

    if(s->index == 0)
    {
        dbgprint("Stack can't peel from empty ast_stack");
        return(NULL);
    }
    ast_node* ret = s->data[--s->index];
    s->data[s->index] = NULL;
    return(ret);
}

bool ast_stack_is_empty(ast_stack* s)
{
    if (s == NULL)
    {
        dbgprint("You passed NULL, nothig happens.");
        // return false because we cant tell is stack is empty 
        // because we do not have stack
        return(false);
    }
    return(s->index == 0);
}

ast_node* ast_stack_dive(ast_stack* s, size_t n)
{
    if (s == NULL)
    {
        dbgprint("You passed NULL, nothig happens.");
        return(false);
    }
    if(s->index == 0)
    {
        dbgprint("You are trying to dive into empty stack.");
        return(NULL);
    }
    if(n < s->index)
    {
        return(s->data[s->index - n - 1]); 
    }
    else
    {
        dbgprint("You are trying to dive undet the stack.");
        return(NULL);
    }
}

