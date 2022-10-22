/** 
 * @brief Parser
 * @file parser.c
 * @author Jan Lutonský, xluton02
 **/
#include"parser.h"

// rule function pointer
// used by some rules that have many alternatives
typedef p_codes (*parser_fnc)(ast_node*,context*);

// function forward declarations
p_codes prog_body(ast_node* root,context* con);
p_codes prog_end(ast_node* root,context* con);
p_codes body(ast_node* root,context* con);
p_codes body_part(ast_node* root,context* con);
p_codes extended_expr(ast_node* root, context* con);
p_codes ret(ast_node* root,context* con);
p_codes ret_cont(ast_node* root,context* con);
p_codes while_n(ast_node* root,context* con);
p_codes if_n(ast_node* root,context* con);
p_codes else_n(ast_node* root,context* con);
p_codes fun_def(ast_node* root,context* con);
p_codes par_list(ast_node* root,context* con);
p_codes ret_type(ast_node* root,context* con);
p_codes type_n(ast_node* root,context* con);


// test if current token is of type t
// and if is then consume it(load next token)
bool consume(token_type t, context* con)
{
    if(con == NULL)
    {
        return(false);
    }
    if(con->token != t)
    {
        infoprint("%s was not consumed",token_str(t));
        return(false);
    }
    infoprint("[%s|%s] was consumed",token_str(t),con->attrib);
    lex_next(con);
    return(true);
}

// test if current token is of type t
bool peek(token_type t, context* con)
{
    if(con == NULL)
    {
        return(false);
    }
    if(con->token != t)
    {
        infoprint("%s was not found",token_str(t));
        return(false);
    }
    infoprint("[%s|%s] was found",token_str(t),con->attrib);
    return(true);
}
/**
 *  this function implements prog rule from grammar
 *  prog -> PS_MARK PC_MARK prog_body prog_end
 **/
p_codes parse(context* con)
{
    infoprint("root");
    if(!consume(PS_MARK,con))
    {
        return(P_SYNTAX_ERROR);
    }

    if(!consume(PC_MARK,con))
    {
        return(P_SYNTAX_ERROR);
    }

    p_codes p = prog_body(con->root,con);
    if( p != P_SUCCESS)
    {
        return(p);
    }

    return(P_SUCCESS);
}

/**  
 *  recursive rule implemented using while cycle
 *
 *   /-----------------------------\
 *  |                               \
 *   \--> prog_body -> body_part ----+
 *                   | fun_def -----/
 *                   | EPS-------------->
 **/                      
p_codes prog_body(ast_node* root, context* con)
{
    infoprint("prog body");
    
    ast_node* node = node_new(PROG_BODY,SEMIC,"body");
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }
    
    //TODO better loop

    bool should_loop = true;
    while(should_loop)
    {
        should_loop = false;
        switch(body_part(node,con))
        {
            case(P_SUCCESS):
                should_loop = true;
                break;

            case(P_CAN_SKIP):
                switch(fun_def(node,con))
                {
                    case(P_CAN_SKIP):
                        break;
                    
                    case(P_SUCCESS):
                        should_loop = true;
                        break;
            
                    case(P_AST_ERROR):
                        node_delete(&node);
                        return(P_AST_ERROR);
                        break;
                    
                    case(P_STACK_ERROR):
                        node_delete(&node);
                        return(P_STACK_ERROR);
                        break;

                    default:
                        node_delete(&node);
                        return(P_SYNTAX_ERROR);
                        break;
                }
                break;

            case(P_AST_ERROR):
                node_delete(&node);
                return(P_AST_ERROR);
                break;

            case(P_STACK_ERROR):
                node_delete(&node);
                return(P_STACK_ERROR);
                break;
                
            default:
                node_delete(&node);
                return(P_SYNTAX_ERROR);
                break;
        }
    }

    if(!node_add(root,node))
    {
        node_delete(&node);
        return(P_AST_ERROR);
    }
    return(P_SUCCESS);
}


/**
 *  prog_end -> EOS
 *            | PE_MARK EOS
 **/
p_codes prog_end(ast_node* root,context* con)
{
    (void)root; // not used
    infoprint("prog end");

    if(consume(EOS,con))
    {
        return(P_SUCCESS);
    }
    
    if(!consume(PE_MARK,con))
    {
        return(P_SYNTAX_ERROR);
    }
    
    if(!consume(EOS,con))
    {
        return(P_SYNTAX_ERROR);
    }
    
    return(P_SUCCESS);
}

/**
 *  recursive rule implemented using while cycle
 *
 *  /-----------------------\
 * |                         |
 *  \--> body -> body_part --+
 *             | EPS ---------------->                 
 **/
p_codes body(ast_node* root, context* con)
{
    infoprint("body");

    ast_node* node = node_new(BODY,SEMIC,"body");
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }

    p_codes rc = P_SUCCESS;
    while(rc == P_SUCCESS)
    {
        rc = body_part(node,con);
    }

    if(rc == P_SUCCESS || rc == P_CAN_SKIP)
    {
        if(!node_add(root,node))
        {
            node_delete(&node);
            return(P_AST_ERROR);
        }
        return(P_SUCCESS);
    }

    node_delete(&node);
    return(rc);
}


/**
 *  body_part -> if_n 
 *             | while_n 
 *             | extended_expr
 *             | ret
 **/
#define fnc_cnt 4 // carefull undefined after function
p_codes body_part(ast_node* root, context* con)
{
    infoprint("body");

    static parser_fnc fnc[fnc_cnt] =
    {
        &if_n,
        &while_n,
        &extended_expr,
        &ret,
    };
   
    for(size_t i = 0; i < fnc_cnt ; ++i)
    {
        // function will atach new node
        switch(fnc[i](root,con))
        {
            case(P_CAN_SKIP):
                continue;
            
            case(P_SUCCESS):
                return(P_SUCCESS);
                break;

            case(P_AST_ERROR):
                return(P_AST_ERROR);
                break;
            
            case(P_STACK_ERROR):
                return(P_STACK_ERROR);
                break;

            default:
                return(P_SYNTAX_ERROR);
                break;
        }
    }
    // all function can be skipped
    return(P_CAN_SKIP);
}
#undef fnc_cnt

bool can_skip_expr(context* con)
{
    if(con == NULL)
    {
        return(false);
    }
    // falltrought filter
    switch(con->token)
    {
        case(FID):
        case(ID):
        case(IVAL):
        case(FVAL):
        case(SVAL):
        case(VVAL):
        case(LPAR):
            return(false);
            break;
        default:
            return(true);
            break;
    }
}

/**
 *  extended_expr -> EXPR SEMIC
 *                 | EXPR_FCALL SEMIC
 *                 | EXPR_PAR SEMIC
 *                 | EXPR_ASSIGN SEMIC
 **/
p_codes extended_expr(ast_node* root, context* con)
{
    if(can_skip_expr(con))
    {
        return(P_CAN_SKIP);
    }
    
    switch(parse_expr(root,con))
    {
        case(EP_EXPR):
        case(EP_EXPR_FCALL):
        case(EP_EXPR_PAR):
        case(EP_EXPR_ASSIGN):
            break;

        case(EP_AST_ERROR):
            return(P_AST_ERROR);
            break;
        
        case(EP_STACK_ERROR):
            return(P_STACK_ERROR);
            break;

        default:
            return(P_SYNTAX_ERROR);
            break;
    }    
    if(!consume(SEMIC,con))
    {
        return(P_SYNTAX_ERROR);
    }

    return(P_SUCCESS);
}



/**
 *  ret -> RETURN ret_cont
 **/
p_codes ret(ast_node* root, context* con)
{
    infoprint("ret");

    if(!consume(RETURN,con))
    {
        return(P_CAN_SKIP);
    }

    ast_node* node = node_new(RETURN_N,RETURN,"");
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }
   
    p_codes p = ret_cont(node,con);
    if( p != P_SUCCESS)
    {
        node_delete(&node);
        return(p);
    }
   
    if(!node_add(root,node))
    {
        node_delete(&node);
        return(P_AST_ERROR);
    }
    return(P_SUCCESS);
}

/**
 *  ret_cont -> EXPR SEMIC
 *            | EXPR_PAR SEMIC
 *            | EXPR_FCALL SEMIC
 *            | SEMIC
 **/
p_codes ret_cont(ast_node* root, context* con)
{
    infoprint("ret cont");

    if(consume(SEMIC,con))
    {
        return(P_SUCCESS);
    }

    switch(parse_expr(root,con))
    {
        case(EP_EXPR):
        case(EP_EXPR_PAR):
        case(EP_EXPR_FCALL):
            break;

        case(EP_AST_ERROR):
            return(P_AST_ERROR);
            break;

        case(EP_STACK_ERROR):
            return(P_STACK_ERROR);
            break;

        default:
            return(P_SYNTAX_ERROR);
            break;
    }
    
    if(!consume(SEMIC,con))
    {
        return(P_SYNTAX_ERROR);
    }
    
    return(P_SUCCESS);
}

/**
 *  while_n -> WHILE EXPR_PAR LBRC body RBRC
 **/
p_codes while_n(ast_node* root, context* con)
{   
    infoprint("while");

    if(!consume(WHILE,con))
    {
        return(P_CAN_SKIP);
    }

    ast_node* node = node_new(WHILE_N,WHILE,"");
    if(node == NULL) 
    {
        return(P_AST_ERROR);
    }

    switch(parse_expr(node,con))
    {
        case(EP_EXPR_PAR):
            break;
        
        case(EP_AST_ERROR):
            node_delete(&node);
            return(P_AST_ERROR);
            break;

        case(EP_STACK_ERROR):
            node_delete(&node);
            return(P_STACK_ERROR);
            break;

        default:
            node_delete(&node);
            return(P_SYNTAX_ERROR);
            break;
    }
    
    if(!consume(LBRC,con))
    {
        node_delete(&node);
        return(P_SYNTAX_ERROR);
    }
    

    p_codes p = body(node,con);
    if( p != P_SUCCESS)
    {
        node_delete(&node);
        return(p);
    }
    
    if(!consume(RBRC,con))
    {
        node_delete(&node);
        return(P_SYNTAX_ERROR);
    }
   
    if(!node_add(root,node))
    {
        return(P_AST_ERROR);
    }
    return(P_SUCCESS);
}

/**
 *  if_n -> IF EXPR_PAR LBRC body RBRC else_n
 **/
p_codes if_n(ast_node* root, context* con)
{
    infoprint("if");

    if(!consume(IF,con))
    {
        return(P_CAN_SKIP);
    }

    ast_node* node = node_new(IF_N,IF,"");
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }
    
    switch(parse_expr(node,con))
    {
        case(EP_EXPR_PAR):
            break;

        case(EP_AST_ERROR):
            node_delete(&node);
            return(P_AST_ERROR);
            break;
        
        case(EP_STACK_ERROR):
            node_delete(&node);
            return(P_STACK_ERROR);
            break;

        default:
            node_delete(&node);
            return(P_SYNTAX_ERROR);
    }
    
    if(!consume(LBRC,con))
    {
        node_delete(&node);
        return(P_SYNTAX_ERROR);
    }
    
    p_codes p = body(node,con);
    if( p != P_SUCCESS)
    {
        node_delete(&node);
        return(p);
    }
    
    if(!consume(RBRC,con))
    {
        node_delete(&node);
        return(P_SYNTAX_ERROR);
    }

    switch(else_n(node,con))
    {
        case(P_SUCCESS):
        case(P_CAN_SKIP):
            break;

        case(P_AST_ERROR):
            node_delete(&node);
            return(P_AST_ERROR);
            break;
        
        case(P_STACK_ERROR):
            node_delete(&node);
            return(P_STACK_ERROR);
            break;

        default:
            node_delete(&node);
            return(P_SYNTAX_ERROR);
            break;
    }

    if(!node_add(root,node))
    {
        return(P_AST_ERROR);
    }

    return(P_SUCCESS);
}

/**
 *  else_n -> ELSE LBRC body RBRC
 *          | EPS
 **/
p_codes else_n(ast_node* root, context* con)
{
    infoprint("else");

    if(!consume(ELSE,con))
    {
        return(P_CAN_SKIP);
    }
    
    if(!consume(LBRC,con))
    {
        return(P_SYNTAX_ERROR);
    }
    
    p_codes p = body(root,con);
    if( p != P_SUCCESS)
    {
        return(p);
    }
    
    if(!consume(RBRC,con))
    {
        return(P_SYNTAX_ERROR);
    }

    return(P_SUCCESS);
}

/**
 *  fun_def -> FUNC FID LPAR par_list RPAR TYPE ret_type LBRC fun_body RBRC
 */
p_codes fun_def(ast_node* root, context* con)
{
    infoprint("function def");

    if(!consume(FUNC,con))
    {
        return(P_CAN_SKIP);
    }
    
    if(!peek(FID,con))
    {
        return(P_SYNTAX_ERROR);
    }
        
    ast_node* node = node_new(FDEF,FID,con->attrib);
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }
    lex_next(con);
    
    if(!consume(LPAR,con))
    {
        node_delete(&node);
        return(P_SYNTAX_ERROR);
    }

    p_codes p = par_list(node,con);
    switch( p )
    {
        case(P_SUCCESS):
        case(P_CAN_SKIP):
            break;

        default:
            node_delete(&node);
            return(p);
            break;
    }

    if(!consume(RPAR,con))
    {
        node_delete(&node);
        return(P_SYNTAX_ERROR);
    }

    if(!consume(TYPE,con))
    {
        node_delete(&node);
        return(P_SYNTAX_ERROR);
    }

    p = ret_type(node,con);
    if( p != P_SUCCESS)
    {
        node_delete(&node);
        return(p);
    }


    if(!consume(LBRC,con))
    {
        node_delete(&node);
        return(P_SYNTAX_ERROR);
    }

    p = body(node,con);
    if( p != P_SUCCESS)
    {
        node_delete(&node);
        return(p);
    }

    if(!consume(RBRC,con))
    {
        node_delete(&node);
        return(P_SYNTAX_ERROR);
    }

    if(!node_add(root,node))
    {
        return(P_AST_ERROR);
    }
    return(P_SUCCESS);
}

bool can_par_list_skip(context* con)
{
    if(con == NULL)
    {
        return(false);
    }
    // falltrough filter
    switch(con->token)
    {
        case(STYPE):
        case(ITYPE):
        case(FTYPE):
        case(NSTYPE):
        case(NITYPE):
        case(NFTYPE):
            return(false);
            break;

        default:
            return(true);
            break;
    }
}

/**                                         
 *  this rule is combination of rules par_list and par_list_cont
 *  recursive rule implemented using while cycle
 *
 *  /-----------------------------------------------\
 * |                                                 |
 *  \--> par_list -> type_n ID -> COMMA par_list ---/
 *                 | EPS-\      | EPS-------------------->
 *                        \----------------->      
 **/
p_codes par_list(ast_node* root, context* con)
{
    infoprint("par list");

    if(can_par_list_skip(con))
    {
        return(P_CAN_SKIP);
    }

    ast_node* node = node_new(PAR_LIST,TYPE,"");
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }

    bool should_loop = true;
    while(should_loop)
    {
        p_codes p = type_n(node,con);
        if(p != P_SUCCESS)
        {
            node_delete(&node);
            return(p);
        }

        if(!peek(ID,con))
        {
            node_delete(&node);
            return(P_SYNTAX_ERROR);
        }

        ast_node* id = node_new(TERM,ID,con->attrib);
        if(id == NULL)
        {
            node_delete(&node);
            return(P_AST_ERROR);
        }
        lex_next(con);
    
        if(!node_add(node,id))
        {
            node_delete(&id);
            node_delete(&node);
            return(P_AST_ERROR);
        }
        
        if(!consume(COMMA,con))
        {
            should_loop = false;
        }
    }

    if(!node_add(root,node))
    {
        node_delete(&node);
        return(P_AST_ERROR);
    }
    return(P_SUCCESS);
}

/**
 *  ret_type -> type_n
 *            | VTYPE
 **/
p_codes ret_type(ast_node* root, context* con)
{
    infoprint("ret type");

    ast_node* node = node_new(RET_TYPE,ADD,"");
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }
    
    switch(type_n(node,con))
    {
        case(P_SUCCESS):
            if(!node_add(root,node))
            {
                node_delete(&node);
                return(P_AST_ERROR);
            }
            return(P_SUCCESS);
            break;

        case(P_AST_ERROR):
            node_delete(&node);
            return(P_AST_ERROR);
            break;

        case(P_STACK_ERROR):
            node_delete(&node);
            return(P_STACK_ERROR);
            break;

        default:
            break;
    }

    node_delete(&node);

    if(consume(VTYPE,con))
    {
        ast_node* node = node_new(RET_TYPE,VTYPE,"");
        if(node == NULL)
        {
            return(P_AST_ERROR);
        }
        if(!node_add(root,node))
        {
            node_delete(&node);
            return(P_AST_ERROR);
        }
        return(P_SUCCESS);
    }
    
    return(P_SYNTAX_ERROR);
}

/**
 *  type_n -> STYPE
 *          | ITYPE
 *          | FTYPE
 *          | NSTYPE
 *          | NITYPE
 *          | NFTYPE
 **/
p_codes type_n(ast_node* root, context* con)
{
    infoprint("type");
    
    ast_node* node;
    // fall trought filter
    switch(con->token)
    {
        case(STYPE):
        case(ITYPE):
        case(FTYPE):
        case(NSTYPE):
        case(NITYPE):
        case(NFTYPE):
            node = node_new(TYPE_N,con->token,"");
            if(node == NULL)
            {
                return(P_AST_ERROR);
            }
            if(!node_add(root,node))
            {
                node_delete(&node);
                return(P_AST_ERROR);
            }
            lex_next(con);
            break;
        default:
            return(P_SYNTAX_ERROR);

    }
    return(P_SUCCESS);
}

