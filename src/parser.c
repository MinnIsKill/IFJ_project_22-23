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
// P_PARAM_ERROR ---> P_ERROR
// P_CAN_SKIP    ---> P_SYNTAX_ERROR / continue
// P_LEX_ERROR   ---> P_LEX_ERROR
// P_SUCCESS     ---> P_SUCCESS
p_codes consume(token_type t, context* con)
{
    if(con == NULL)
    {
        return(P_PARAM_ERROR);
    }
    if(con->token != t)
    {
        infoprint("%s was not consumed",token_str(t));
        return(P_CAN_SKIP);
    }
    infoprint("[%s|%s] was consumed",token_str(t),con->attrib.buffer);
    if(!lex_next(con,stdin))
    {
        return(P_LEX_ERROR);
    }
    return(P_SUCCESS);
}

// test if current token is of type t
// P_PARAM_ERROR ---> P_ERROR
// P_CAN_SKIP    ---> P_SYNTAX_ERROR / continue
// P_SUCCESS     ---> P_SUCCESS
p_codes peek(token_type t, context* con)
{
    if(con == NULL)
    {
        return(P_PARAM_ERROR);
    }
    if(con->token != t)
    {
        infoprint("%s was not found",token_str(t));
        return(P_CAN_SKIP);
    }
    infoprint("[%s|%s] was found",token_str(t),con->attrib.buffer);
    return(P_SUCCESS);
}
/**
 *  this function implements prog rule from grammar
 *  prog -> PS_MARK PC_MARK prog_body prog_end
 **/
p_codes parse(context* con)
{
    infoprint("root");
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    switch(consume(PS_MARK,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }

    switch(consume(PC_MARK,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }

    p_codes p = prog_body(con->root,con);
    if( p != P_SUCCESS)
    {
        return(p);
    }
    
    p = prog_end(con->root,con);
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
#define fnc_cnt 2
p_codes prog_body(ast_node* root, context* con)
{
    infoprint("prog body");
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }
    
    ast_node* node = node_new(PROG_BODY,NOT_USED,"");
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }
    
    static parser_fnc fnc[fnc_cnt] =
    {
        &body_part,
        &fun_def,
    };
   
    bool should_loop = true;
    while(should_loop)
    {
        should_loop = false;
        for(size_t i = 0; (i < fnc_cnt) && (!should_loop); ++i)
        {
            // function will atach new node
            p_codes p = fnc[i](node,con);
            switch(p)
            {
                case(P_CAN_SKIP):
                    continue;

                case(P_SUCCESS):
                    should_loop = true;
                    break;

                default:
                    node_delete(&node);
                    return(p);
                    break;
            }
        }
    }

    if(!node_add(root,node))
    {
        node_delete(&node);
        return(P_AST_ERROR);
    }
    return(P_SUCCESS);
}
#undef fnc_cnt

/**
 *  prog_end -> EOS
 *            | PE_MARK EOS
 **/
p_codes prog_end(ast_node* root,context* con)
{
    infoprint("prog end");
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    switch(consume(EOS,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            break;

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            return(P_SUCCESS);
    }
    
    switch(consume(PE_MARK,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }
    
    switch(consume(EOS,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            return(P_SUCCESS);
    }
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
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    ast_node* node = node_new(BODY,NOT_USED,"");
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
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

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
        p_codes p = fnc[i](root,con);
        switch(p)
        {
            case(P_CAN_SKIP):
                continue;
            
            case(P_SUCCESS):
                return(P_SUCCESS);
                break;

            default:
                return(p);
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
    infoprint("extended_expr");
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }
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
        
        case(EP_LEX_ERROR):
            return(P_LEX_ERROR);    

        default:
            return(P_SYNTAX_ERROR);
            break;
    }    

    switch(consume(SEMIC,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            return(P_SUCCESS);
    }
}



/**
 *  ret -> RETURN ret_cont
 **/
p_codes ret(ast_node* root, context* con)
{
    infoprint("ret");
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    switch(consume(RETURN,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_CAN_SKIP);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
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
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    switch(consume(SEMIC,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            break;

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
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
        
        case(EP_LEX_ERROR):
            return(P_LEX_ERROR);
            break;

        default:
            return(P_SYNTAX_ERROR);
            break;
    }
    
    switch(consume(SEMIC,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            return(P_SUCCESS);
    }
}

/**
 *  while_n -> WHILE EXPR_PAR LBRC body RBRC
 **/
p_codes while_n(ast_node* root, context* con)
{   
    infoprint("while");
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    switch(consume(WHILE,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_CAN_SKIP);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
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
        
        case(EP_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);
            break;

        default:
            node_delete(&node);
            return(P_SYNTAX_ERROR);
            break;
    }
    
    switch(consume(LBRC,con))
    {
        default:
            node_delete(&node);
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }
    

    p_codes p = body(node,con);
    if( p != P_SUCCESS)
    {
        node_delete(&node);
        return(p);
    }
    
    switch(consume(RBRC,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
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
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    switch(consume(IF,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_CAN_SKIP);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
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
        
        case(EP_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);
            break;
        
        case(EP_STACK_ERROR):
            node_delete(&node);
            return(P_STACK_ERROR);
            break;

        default:
            node_delete(&node);
            return(P_SYNTAX_ERROR);
    }
    
    switch(consume(LBRC,con))
    {
        default:
            node_delete(&node);
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }
    
    p_codes p = body(node,con);
    if( p != P_SUCCESS)
    {
        node_delete(&node);
        return(p);
    }
    
    switch(consume(RBRC,con))
    {
        default:
            node_delete(&node);
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }

    p = else_n(node,con);
    switch(p)
    {
        case(P_SUCCESS):
        case(P_CAN_SKIP):
            break;

        default:
            node_delete(&node);
            return(p);
            break;
    }

    if(!node_add(root,node))
    {
        node_delete(&node);
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
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    switch(consume(ELSE,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_CAN_SKIP);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }
    
    switch(consume(LBRC,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }
    
    p_codes p = body(root,con);
    if( p != P_SUCCESS)
    {
        return(p);
    }
    
    switch(consume(RBRC,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            return(P_SUCCESS);
    }
}

/**
 *  fun_def -> FUNC FID LPAR par_list RPAR TYPE ret_type LBRC fun_body RBRC
 */
p_codes fun_def(ast_node* root, context* con)
{
    infoprint("function def");
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    switch(consume(FUNC,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_CAN_SKIP);

        case(P_LEX_ERROR):
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }
    
    switch(peek(FID,con))
    {
        default:
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            return(P_SYNTAX_ERROR);

        case(P_SUCCESS):
            break;
    }
    
    ast_node* node = node_new(FDEF,FID,con->attrib.buffer);
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }

    if(!lex_next(con,stdin))
    {
        node_delete(&node);
        return(P_LEX_ERROR);
    }
    
    switch(consume(LPAR,con))
    {
        default:
            node_delete(&node);
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
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

    switch(consume(RPAR,con))
    {
        default:
            node_delete(&node);
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }

    switch(consume(TYPE,con))
    {
        default:
            node_delete(&node);
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }

    p = ret_type(node,con);
    if( p != P_SUCCESS)
    {
        node_delete(&node);
        return(p);
    }


    switch(consume(LBRC,con))
    {
        default:
            node_delete(&node);
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }

    p = body(node,con);
    if( p != P_SUCCESS)
    {
        node_delete(&node);
        return(p);
    }

    switch(consume(RBRC,con))
    {
        default:
            node_delete(&node);
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            break;
    }

    if(!node_add(root,node))
    {
        node_delete(&node);
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
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

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
        ast_node* par = node_new(PARAM,INVALID,"");
        if(par == NULL)
        {
            return(P_AST_ERROR);
        }
        p_codes p = type_n(par,con);
        if(p != P_SUCCESS)
        {
            node_delete(&par);
            node_delete(&node);
            return(p);
        }

        switch(peek(ID,con))
        {
            default:
                node_delete(&par);
                node_delete(&node);
                return(P_ERROR);

            case(P_CAN_SKIP):
                node_delete(&par);
                node_delete(&node);
                return(P_SYNTAX_ERROR);

            case(P_LEX_ERROR):
                node_delete(&par);
                node_delete(&node);
                return(P_LEX_ERROR);

            case(P_SUCCESS):
                break;
        }
        
        free(par->attrib);
        char* tmp = strdup(con->attrib.buffer);
        if(tmp == NULL)
        {
            node_delete(&par);
            node_delete(&node);
            return(P_AST_ERROR);
        }
        par->attrib = tmp;

        //TODO CHECK

        if(!lex_next(con,stdin))
        {
            node_delete(&par);
            node_delete(&node);
            return(P_LEX_ERROR);
        }
    
        if(!node_add(node,par))
        {
            node_delete(&par);
            node_delete(&node);
            return(P_AST_ERROR);
        }
        
        switch(consume(COMMA,con))
        {
            default:
                return(P_ERROR);

            case(P_CAN_SKIP):
                should_loop = false;
                break;

            case(P_SUCCESS):
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
 *  ret_type -> type_n
 *            | VTYPE
 **/
p_codes ret_type(ast_node* root, context* con)
{
    infoprint("ret type");
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }

    ast_node* node = node_new(RET_TYPE,INVALID,"");
    if(node == NULL)
    {
        return(P_AST_ERROR);
    }
    
    p_codes p = type_n(node,con);
    switch(p)
    {
        case(P_SUCCESS):
            if(!node_add(root,node))
            {
                node_delete(&node);
                return(P_AST_ERROR);
            }
            return(P_SUCCESS);
            break;

        // syntax error here means that 
        // type_n was not matched
        // so we still need to test if 
        // input is vtype
        case(P_SYNTAX_ERROR):
            break;

        // all other errors are stil errors
        default:
            node_delete(&node);
            return(p);
            break;
    }


    switch(consume(VTYPE,con))
    {
        default:
            node_delete(&node);
            return(P_ERROR);
        
        case(P_CAN_SKIP):
            node_delete(&node);
            return(P_SYNTAX_ERROR);

        case(P_LEX_ERROR):
            node_delete(&node);
            return(P_LEX_ERROR);

        case(P_SUCCESS):
            node->sub_type = VTYPE;
            if(!node_add(root,node))
            {
                node_delete(&node);
                return(P_AST_ERROR);
            }
            return(P_SUCCESS);
    }
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
    if(root == NULL)
    {
        dbgprint("received null pointer(root)");
        return(P_PARAM_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con)");
        return(P_PARAM_ERROR);
    }
    
    // fall trought filter
    switch(con->token)
    {
        case(STYPE):
        case(ITYPE):
        case(FTYPE):
        case(NSTYPE):
        case(NITYPE):
        case(NFTYPE):
            root->sub_type = con->token;
            if(!lex_next(con,stdin))
            {
                return(P_LEX_ERROR);
            }
            return(P_SUCCESS);
            break;
        default:
            return(P_SYNTAX_ERROR);
            break;  
    }
}

// inspired by xlakis03
const char* p_codes_str(const p_codes p)
{
    switch(p)
    {
        case(P_SUCCESS)      : return("succes"); 
        case(P_SYNTAX_ERROR) : return("syntax error"); 
        case(P_AST_ERROR)    : return("ast error"); 
        case(P_STACK_ERROR)  : return("stack error"); 
        case(P_PARAM_ERROR)  : return("invalid parameter"); 
        case(P_LEX_ERROR)    : return("lex error"); 
        case(P_ERROR)        : return("generic error"); 
        default              : return("~NOT MATCHED~");
    }
}


