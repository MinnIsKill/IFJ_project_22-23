/* 
 *  @brief "Generic" stack of void pointers
 *         implementad using vector
 *  @file g_stack.h
 *  @author Jan Lutonský, xluton02
 */

#include"g_stack.h"

bool g_stack_init(g_stack* s, size_t init_size)
{
    if(s == NULL)
    {
        dbgprint("Can not initialize NULL, nothing happens.");
        return(false);
    }
    if(init_size < G_STACK_MIN_SIZE)
    {
        infoprint("User wants less slots than minimu(%lu), setting number slots to : %u"
                  ,init_size,G_STACK_MIN_SIZE);
        init_size = G_STACK_MIN_SIZE;
    }

    s->index = 0;
    s->data = (void**)calloc(init_size,sizeof(void*));
    if(s->data == NULL)
    {
        s->size = 0;
        dbgprint("Stack failed to allocate memory for datta buffer.");
        return(false);
    }
        
    s->size = init_size;
    return(true);
}

bool g_stack_destroy(g_stack* s)
{
    if(s == NULL)
    {
        // no need to destroy
        return(true);
    }
    if(s->index != 0)
    {
        dbgprint("user is trying to destroy non-empty stack");
        return(false);
    }
    free(s->data);
    s->data = NULL;
    s->size=0;
    return(true);
}

bool g_stack_push(g_stack* s,void* data)
{
    if(s == NULL)
    {
        dbgprint("You passed NULL(s), nothig happens.");
        return(false);
    }
    if(data == NULL)
    {
        dbgprint("You are trying to push NULL onto the stack, you should not need to push NULL onto the stack!");
        return(false);
    }
    if(s->index+1 > s->size)
    {
        //realloc
        void** tmp = (void**)realloc(s->data,(s->size+G_STACK_CHUNK)*sizeof(void*));
        if(tmp == NULL)
        {
            dbgprint("Stack push failed to realloc memory.");
            return(false);
        }
        s->data = tmp;
        s->size += G_STACK_CHUNK;
    }
    s->data[s->index++] = data;
    return(true);
}

void* g_stack_top(g_stack* s)
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


void* g_stack_peel(g_stack* s)
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
    void* ret = s->data[--s->index];
    s->data[s->index] = NULL;
    return(ret);
}

bool g_stack_is_empty(g_stack* s)
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

void* g_stack_dive(g_stack* s, size_t n)
{
    if (s == NULL)
    {
        dbgprint("You passed NULL, nothig happens.");
        return(NULL);
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

