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

bool node_insert_betwene(ast_node* parent, ast_node* node, ast_node* child)
{
    // can not operate on NULL
    if(parent == NULL || node == NULL || child == NULL)
    {
        infoprint("parent || node || child is NULL");
        return(false);
    }
    // parent has no child
    if(parent->children == NULL || parent->children == 0)
    {
        infoprint("parent has no children.");
        return(false);
    }

    // find child and its slot  
    ast_node** ptr = parent->children;
    for(size_t i = 0; i < parent->children_cnt ; ++i)
    {
        if(ptr[i] == child)
        {
            // adding child to node
            // and then insert node into child's slot
            if(!node_add(node,child))
            {
                dbgprint("Insertion failed, because node_add.");
                return(false);
            }

            ptr[i] = node;
            return(true);
        }
    }
    
    dbgprint("Input child is not child of input parent.");
    return(false);
}

bool node_remove_child(ast_node* parent, ast_node* child)
{
    // can not operate on NULL
    if(parent == NULL || child == NULL)
    {
        infoprint("parent || child is NULL");
        return(false);
    }
    // parent has no child
    if(parent->children == NULL || parent->children_cnt == 0)
    {
        infoprint("parent has no children.");
        return(false);
    }

    // find correct node
    size_t i = 0;
    bool found = false;
    for(; i < parent->children_cnt ; ++i )
    {
        if(parent->children[i] == child)
        {
            infoprint("child was found");
            found = true;
            break;
        }
    }

    // if node was not found return
    if(!found)
    {
        infoprint("child was NOT found");
        return false;
    }

    // if node was found delete it and decremente children cnt
    node_delete(&(parent->children[i]));
    parent->children_cnt--;

    // now just move the whole array by one left
    // sice i decremented children count i sould never read past
    // children array
    for(; i < parent->children_cnt; ++i)
    {
        parent->children[i] = parent->children[i+1];
    }

    // just in case set last spot to NULL
    // because the array is not shrinked
    // and the slot may be avalible
    parent->children[parent->children_cnt] = NULL;
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
    "parenthesis",
    "function call",
    "expression list",
    "assignment",
    "body",
    "fun_body",
    "prog_body",
    "while",
    "if",
    "else",
    "return",
    "return type",
    "function definition",
    "parameter",
    "parametr list",
    "type conversion"
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

void print_escape(FILE* f, const char* source)
{
    char c;
    for(;(c = *source) != '\0'; ++source)
    {
        switch(c)
        {
            default:
                fputc(c,f);
                break;
            
            case('"'):
                fputc('\\',f);
                fputc(c,f);
                break;

            case('<'):
                fputc('\\',f);
                fputc(c,f);
                break;

            case('>'):
                fputc('\\',f);
                fputc(c,f);
                break;
        } 
    }
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
    fprintf(f,"%llu[shape=record;label=\"{{",n->id);
    print_escape(f,node_type_str(n->type));
    fprintf(f,"[%lu]|",n->children_cnt);
    print_escape(f,token_str(n->sub_type));
    fprintf(f,"}");
    if(n->attrib[0] != '\0')
    {
        fprintf(f,"|");
        print_escape(f,n->attrib);
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

