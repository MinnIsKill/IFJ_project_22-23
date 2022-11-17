#include"context.h"
#include"fake_lex.h"

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
    // TODO init symtable
    // TODO HANDLE LEX ERORR
    lex_init(c);
    return(true);
}

void context_delete(context* c)
{
    assert(c != NULL);

    lex_destroy();
    node_delete(&(c->root));
    ast_stack_destroy(&(c->expr_stack));
    
    // TODO clean symtable
    // the free should be part of lex_clean
    free(c->attrib);
}
