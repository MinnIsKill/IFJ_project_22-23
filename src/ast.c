/**
 * @brief Abstract syntax tree datastructure
 * @file ast.c
 * @author Jan Lutonský, xluton02
 **/

#include"ast.h"

// private function, generating
// "unique" ids for tree nodes
unsigned long long get_id()
{
    static unsigned long long id = 0;
    return(id++);
}

ast_node* node_new(node_type type,token_type sub_type, char* attrib)
{
    //TODO maybe ranage check type
    //TODO maybe ranage check sub_type
    
    // alloc self
    ast_node* new_node = (ast_node*)malloc(sizeof(ast_node));
    if(new_node == NULL) 
    {
        dbgprint("Failed to allocate memory for node.");
        return(NULL);
    }
    
    // alloc copy of attrib
    new_node->attrib = strdup((attrib != NULL)?(attrib):(""));
    if(new_node->attrib == NULL)
    {
        dbgprint("Failed to allocate string buffer for node, freeing node.");
        free(new_node);
        return(NULL);
    }
    
    new_node->id = get_id();
   
    new_node->type = type;
    new_node->sub_type = sub_type;
    
    new_node->children_cnt = 0;
    new_node->children = NULL;
    
    return(new_node);
}

void node_delete(ast_node** n)
{
    if(n == NULL)
    {
        return;
    }
    ast_node* dn = *n;
    if(dn == NULL) 
    {
        return;
    }

    // free children
    for(size_t i = 0; i < dn->children_cnt; ++i)
    {
        node_delete(&(dn->children[i]));
    }

    //free selfe
    free(dn->attrib);
    free(dn->children);
    free(dn);

    // NULL root, just in case
    *n = NULL;
}

bool node_add(ast_node* dst,ast_node* src)
{
    if(dst == NULL)
    {
        dbgprint("Parent node is NULL.");
        return(false);
    }
    size_t n = dst->children_cnt;
    ast_node** tmp = (ast_node**)realloc(dst->children,(n+1)*sizeof(ast_node*));
    
    if(tmp == NULL)
    {
        dbgprint("Failed to reallocate memory for node.")
        return(false);
    }
    
    tmp[n] = src;
    dst->children = tmp;
    dst->children_cnt++;
    return(true);
}


//========== DEBUG AND PRINT FUNCTIONS ==========


const char* node_type_str(node_type t)
{
    // TODO maybe range check of t
    
    // !!! KEEP IN SYNC WITH node_type !!!
    static const char* s[]=
    {
    "terminal",
    "non-terminal",
    "expression",
    "(expression)",
    "function call",
    "expression list",
    "body",
    "fun_body",
    "prog_body",
    "assignment",
    "while",
    "if",
    "else",
    "return",
    "return type",
    "type",
    "function definition",
    "parametr list",
    };
    return(s[t]);
}

void node_print(FILE* f,ast_node* n)
{
    if(n == NULL)
    {
        return;
    }
    fprintf(f,"[%s|%s|%s]",node_type_str(n->type),token_str(n->sub_type),n->attrib);
}

// private function
// real recursive inmplementation of tree_dot_print()
void _tree_dot_print(FILE* f,ast_node* n)
{
    // this function was 
    if(n == NULL)
    {
        return;
    }
    fprintf(f,"%llu[shape=record;label=\"{{%s|%s}",n->id,node_type_str(n->type),token_str(n->sub_type));
    if(n->attrib[0] != '\0')
    {
        fprintf(f,"|%s",n->attrib);
    }
    fprintf(f,"}\"];\n");

    for(size_t i = 0; i < n->children_cnt; ++i)
    {
        if(n->children[i] != NULL)
        {
            fprintf(f,"%llu->%llu;\n",n->id,n->children[i]->id);
            _tree_dot_print(f,n->children[i]);
        }
    }
}

// just wraper for _tree_dot_print()
void tree_dot_print(FILE* f,ast_node* n)
{
    fprintf(f,"digraph{\n");
    _tree_dot_print(f,n);
    fprintf(f,"}\n");
}

