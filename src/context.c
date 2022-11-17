#include"context.h"
#include"lex.h"
#include"assert.h"

bool context_new(context* c)
{
    assert(c != NULL);

    c->root = node_new(NTERM,PS_MARK,"ROOT");
    if(c->root == NULL)
    {
        return(false);
    }
    
    if(!ast_stack_init(&(c->expr_stack),64))
    {
        node_delete(&c->root);
        return(false);
    }

    //if(!char_buffer_init(&(c->attrib)))
    //{
    //    return(false); 
    //}

    // TODO init symtable
    // TODO HANDLE LEX ERORR
    lex_init(c,stdin);
    return(true);
}

void context_delete(context* c)
{
    assert(c != NULL);

    node_delete(&(c->root));
    ast_stack_destroy(&(c->expr_stack));
    lex_destroy(c);
    
    // TODO clean symtable

    //char_buffer_destroy(&c->attrib);
}
