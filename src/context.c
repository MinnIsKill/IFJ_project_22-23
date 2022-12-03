#include"context.h"
#include"lex.h"
#include"assert.h"
#include"error.h"

int context_new(context* c)
{
    assert(c != NULL);

    c->root = node_new(NTERM,PS_MARK,"ROOT");
    if(c->root == NULL)
    {
        return(INTERNAL_ERROR);
    }
    
    if(!ast_stack_init(&(c->expr_stack),64))
    {
        node_delete(&c->root);
        return(INTERNAL_ERROR);
    }

    if(!lex_init(c,stdin))
    {
        ast_stack_destroy(&(c->expr_stack));
        node_delete(&c->root);
        return(LEX_ERROR);
    }
    
    bintree_init(&c->global_symtab);
    
    return(SUCCESS);
}

void context_delete(context* c)
{
    assert(c != NULL);

    node_delete(&(c->root));
    ast_stack_destroy(&(c->expr_stack));
    lex_destroy(c);
    
    bintree_dispose(c->global_symtab);
}
