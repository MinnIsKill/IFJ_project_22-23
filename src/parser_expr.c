/** 
 * @brief Expression parser
 * @file parser_expr.c
 * @author Jan Lutonský, xluton02
 **/
#include"parser_expr.h"

// used in precedence table
typedef enum{
    SHIFT,           //  <
    REDUCE,          //  >
    SHIFT_REDUCE,    //  =
    ERROR,           //  X
    ACCEPT           //  A
}tab_op;

// input symbol precedence classes 
typedef enum{
    F,   // function = {FID}
    I,   // ID 
    T,   // term = {IVAL,FVAL,SVAL,NUL}
    CL1, // class1 = {*, \ }
    CL2, // class2 = {+, -,  .}
    CL3, // class3 = {<, >, <=, >=}
    CL4, // class4 = {===, !==}
    LP,  // (
    RP,  // )
    CM,  // ,
    CEQ, // =
    DOL  // $
}tab_index;

// dimmension of precedence table
#define tab_dim 12
// precedence table
tab_op tab[tab_dim][tab_dim]={
//          F       I       T       CL1      CL2      CL3      CL4      LPAR     RPAR           ,        =         $
/* F   */ { ERROR , ERROR , ERROR , ERROR  , ERROR  , ERROR  , ERROR  , SHIFT  , ERROR        , ERROR  , ERROR , ERROR  },
/* I   */ { ERROR , ERROR , ERROR , REDUCE , REDUCE , REDUCE , REDUCE , ERROR  , REDUCE       , REDUCE , SHIFT , REDUCE },
/* T   */ { ERROR , ERROR , ERROR , REDUCE , REDUCE , REDUCE , REDUCE , ERROR  , REDUCE       , REDUCE , ERROR , REDUCE },
/* CL1 */ { SHIFT , SHIFT , SHIFT , REDUCE , REDUCE , REDUCE , REDUCE , SHIFT  , REDUCE       , REDUCE , ERROR , REDUCE },
/* CL2 */ { SHIFT , SHIFT , SHIFT , SHIFT  , REDUCE , REDUCE , REDUCE , SHIFT  , REDUCE       , REDUCE , ERROR , REDUCE },
/* CL3 */ { SHIFT , SHIFT , SHIFT , SHIFT  , SHIFT  , REDUCE , REDUCE , SHIFT  , REDUCE       , REDUCE , ERROR , REDUCE },
/* CL4 */ { SHIFT , SHIFT , SHIFT , SHIFT  , SHIFT  , SHIFT  , REDUCE , SHIFT  , REDUCE       , REDUCE , ERROR , REDUCE },
/* (   */ { SHIFT , SHIFT , SHIFT , SHIFT  , SHIFT  , SHIFT  , SHIFT  , SHIFT  , SHIFT_REDUCE , SHIFT  , ERROR , ERROR  },
/* )   */ { ERROR , ERROR , ERROR , ERROR  , ERROR  , ERROR  , ERROR  , ERROR  , ERROR        , REDUCE , ERROR , ERROR  },
/* ,   */ { SHIFT , SHIFT , SHIFT , SHIFT  , SHIFT  , SHIFT  , SHIFT  , SHIFT  , REDUCE       , REDUCE , ERROR , ERROR  },
/* =   */ { SHIFT , SHIFT , SHIFT , SHIFT  , SHIFT  , SHIFT  , SHIFT  , SHIFT  , REDUCE       , REDUCE , ERROR , REDUCE },
/* $   */ { SHIFT , SHIFT , SHIFT , SHIFT  , SHIFT  , SHIFT  , SHIFT  , SHIFT  , ERROR        , SHIFT  , SHIFT , ACCEPT }
};

// inner function
// input classification function
tab_index token_to_tab_index(token_type t)
{
    switch(t)
    {
        case(FID):
            return(F);
            break;
        
        case(ID):
            return(I);
            break;

        case(IVAL):
        case(FVAL):
        case(SVAL):
        case(VVAL):
            return(T);
            break;

        case(MUL):
        case(DIV):
            return(CL1);
            break;

        case(ADD):
        case(SUB):
        case(STRCAT):
            return(CL2);
            break;

        case(LT):
        case(GT):
        case(LTE):
        case(GTE):
            return(CL3);
            break;

        case(EQ):
        case(NEQ):
            return(CL4);
            break;

        case(LPAR):
            return(LP);
            break;

        case(RPAR):
            return(RP);
            break;

        case(COMMA):
            return(CM);
            break;
        
        case(ASSIG):
            return(CEQ);
            break;

        default:
            return(DOL);
            break;
    }
}

// inner function
// for debug printing
// TODO maybe range check
const char* tab_op_to_str(tab_op o)
{
    static const char* c[] =
    {
    "shift",           //  <
    "reduce",          //  >
    "shift&reduce",    //  =
    "error",           //  X
    "accept"           //  A
    };
    return(c[o]);
}

//===classification function used by reduce_FSM====
// inner functions
// !!! be carefull !!! some functions
// take node_type and other token_typ as parameter
bool is_exprs(node_type t)
{
    return(t >= EXPR && t <= EXPR_FCALL);
}

bool is_exprs_plus(node_type t)
{
    return(t >= EXPR && t <= EXPR_LIST);
}

bool is_terms(token_type t)
{
   return(t >= IVAL && t <= ID); 
}

bool is_binop(token_type t)
{
   return(t >= ADD && t <= NEQ); 
}
//=================================================


/**
 *  inner function
 *  diagram can be found in ./doc/parser/expression
 *  this funnction is called when redduction happen
 *
 *  TODO separate state function into inline functions
 **/
ep_codes reduce_FSM(ast_stack* s)
{
    if(s == NULL)
    {
        dbgprint("received null pointer(s)");
        return(EP_UNKNOWN_ERROR);
    }
    enum{
        ERROR,
        START,
        STATE1,
        STATE2,
        STATE3,
        STATE4,
        STATE5,
        STATE6,
        STATE7,
        STATE8,
        EXPRF2,
        EXPRF,  // embedded in START
        LIST,   // embedded in STATE6
        FCALL,
        ASSIGN,
        PAR,
    }state = START;
    
    // there could be probably array
    ast_node* left = NULL;
    ast_node* middle = NULL;
    ast_node* right = NULL;

    while(true)
    {
        switch(state)
        {
            case(ERROR):
                infoprint("state : ERROR");
                dbgprint("Reduction failed.");
                node_delete(&left);
                node_delete(&middle);
                node_delete(&right);
                return(EP_SYNTAX_ERROR);

            case(START):
                infoprint("state : START");
                
                right = ast_stack_top(s);
                if(right == NULL)
                {
                    dbgprint("stack error.");
                    state = ERROR;
                    break;
                }
                if(is_terms(right->sub_type) && right->type == TERM)
                {
                    // embedded expr3 state
                    infoprint("state : EXPRF");
                    right->type = EXPR;
                    return(EP_SUCCESS);
                }
                else if(right->sub_type == RPAR && right->type == TERM)
                {
                    ast_stack_pop(s);
                    state = STATE1;
                }
                else if(is_exprs(right->type))
                {
                    right = ast_stack_peel(s);
                    if(right == NULL)
                    {
                        dbgprint("stack error - Should not happen.");
                        state = ERROR;
                        break;
                    }
                    state = STATE5;
                }
                else
                {
                    right = NULL;
                    state = ERROR;
                }
                break;

            case(STATE1):
                infoprint("state : STATE1");
                right = ast_stack_peel(s);
                if(right == NULL)
                {
                    dbgprint("stack error.");
                    state = ERROR;
                    break;
                }
                if(right->sub_type == LPAR && right->type == TERM)
                {
                    node_delete(&right);
                    state = STATE2;
                }
                else if(is_exprs(right->type))
                {
                   state = STATE3; 
                }
                else if(right->type == EXPR_LIST)
                {
                    state = STATE4;
                }
                else
                {
                    state = ERROR;
                }
                break;

            case(STATE2):
                infoprint("state : STATE2");
                middle = ast_stack_top(s);
                if(middle == NULL)
                {
                    dbgprint("stack error.")
                    state = ERROR;
                    break;
                }
                if(middle->sub_type == FID && middle->type == TERM)
                {
                    state = FCALL;
                }
                else
                {
                    // middle is still on the
                    // stack so we let stack destroy
                    // function to handle memory dealocation
                    // instead of error state
                    middle = NULL;
                    state = ERROR; 
                }
                break;

            case(STATE3):
                infoprint("state : STATE3");
                middle = ast_stack_peel(s); 
                if(middle == NULL)
                {
                    dbgprint("stack error.");
                    state = ERROR;
                    break;
                }
                if(middle->sub_type == LPAR && middle->type == TERM)
                {
                    node_delete(&middle);
                    state = PAR;
                }
                else
                {
                    state = ERROR;
                }
                break;

            case(STATE4):
                infoprint("state : STATE4");
                middle = ast_stack_peel(s);
                if(middle == NULL)
                {
                    dbgprint("stack error.");
                    state = ERROR;
                    break;
                }
                if(middle->sub_type == LPAR && middle->type == TERM)
                {
                    node_delete(&middle);
                    state = STATE2;
                }
                else
                {
                    state = ERROR;
                }
                break;

            case(STATE5):
                infoprint("state : STATE4");
                middle = ast_stack_peel(s);
                if(middle == NULL)
                {
                    dbgprint("stack error.");
                    state = ERROR;
                    break;
                }
                if(is_binop(middle->sub_type))
                {
                    state = STATE7;
                }
                else if(middle->sub_type == COMMA && middle->type == TERM)
                {
                    state = STATE6;
                }
                else if(middle->sub_type == ASSIG && middle->type == TERM)
                {
                    state = STATE8;
                }
                else
                {
                    state = ERROR;
                }
                break;

            case(STATE6):
                infoprint("state : STATE6");
                left = ast_stack_peel(s);
                if(left == NULL)
                {
                    dbgprint("stack error.");
                    state = ERROR;
                    break;
                }
                if(is_exprs_plus(left->type))
                {
                    // embedded LIST state
                    infoprint("state : LIST");
                    if(left->type == EXPR_LIST)
                    {
                        // left node is alreadt list,
                        // just append right and delete middle
                        node_delete(&middle);
                        if(!node_add(left,right))
                        {
                            dbgprint("ast error.");
                            state = ERROR;
                            break;
                        }
                        middle = left; // so it can be pushed onto stack
                                       // with next function call
                    }
                    else
                    {
                        // we need to create new list
                        middle->type = EXPR_LIST;
                        if(!node_add(middle,left))
                        {
                            dbgprint("ast error.");
                            state = ERROR;
                            break;
                        }
                        
                        if(!node_add(middle,right))
                        {
                            // left is already attached to middle
                            // so we must not delete it 
                            // in the error state
                            left = NULL;
                            dbgprint("ast error.");
                            state = ERROR;
                            break;
                        
                        }
                        
                    }
                        
                    if(!ast_stack_push(s,middle))
                    {
                        // both left and right are attached to middle
                        // so we must not free them in error state
                        left = NULL;
                        right = NULL;
                        dbgprint("stack error.");
                        state = ERROR;
                        break;
                    }
                    return(EP_SUCCESS);
                }
                else
                {
                    state = ERROR;
                }
                break;

            case(STATE7):
                infoprint("state : STATE7");
                left = ast_stack_peel(s);
                if(left == NULL)
                {
                    dbgprint("stack error.");
                    state = ERROR;
                    break;
                }
                if(is_exprs(left->type))
                {
                    state = EXPRF2;
                }
                else
                {
                    state = ERROR;
                }
                break;

            case(STATE8):
                infoprint("state : STATE8");
                left = ast_stack_peel(s);
                if(left == NULL)
                {
                    dbgprint("stack error.");
                    state = ERROR;
                    break;
                }
                if(left->sub_type == ID && left->type == TERM)
                {
                    state = ASSIGN;
                }
                else
                {
                    state = ERROR;
                }
                break;
            
            case(ASSIGN):
                infoprint("state : assign");
                middle->type = EXPR_ASSIGN;
                if(!node_add(middle,left))
                {
                    dbgprint("ast errror.");
                    state = ERROR;
                    break;
                }
                
                if(!node_add(middle,right))
                {
                    // left is already attached
                    // to middle so error state
                    // must not free it
                    left = NULL;
                    dbgprint("ast errror.");
                    state = ERROR;
                    break;
                }
                if(!ast_stack_push(s,middle))
                {
                    // left and righ are already attached
                    // to middle so error state
                    // must not free them
                    left = NULL;
                    right = NULL;
                    dbgprint("stack errror.");
                    state = ERROR;
                    break;
                }
                return(EP_SUCCESS);
                break;

            case(EXPRF2):
                infoprint("state : EXPR1");
                middle->type = EXPR;
                if(!node_add(middle,left))
                {
                    dbgprint("ast errror.");
                    state = ERROR;
                    break;
                }
                
                if(!node_add(middle,right))
                {
                    // left is already attached
                    // to middle so error state
                    // must not free it
                    left = NULL;
                    dbgprint("ast errror.");
                    state = ERROR;
                    break;
                }
                if(!ast_stack_push(s,middle))
                {
                    // left and righ are already attached
                    // to middle so error state
                    // must not free them
                    left = NULL;
                    right = NULL;
                    dbgprint("stack errror.");
                    state = ERROR;
                    break;
                }
                return(EP_SUCCESS);
                break; //just in case

            case(FCALL):
                infoprint("state : FCALL");
                if(right != NULL)
                {
                    if(!node_add(middle,right))
                    {
                        dbgprint("ast error.");
                        state = ERROR;
                        break;
                    }
                }
                middle->type = EXPR_FCALL;
                return(EP_SUCCESS);
                break; // just in case
            
            case(PAR):
                infoprint("state : PAR");
                middle = ast_stack_top(s);
                if(middle == NULL)
                {
                    dbgprint("stack error.");
                    state = ERROR;
                    break;
                }
                if(middle->sub_type == FID && middle->type == TERM)
                {
                    state = FCALL;
                }
                else
                {
                    if(right->type == EXPR_PAR)
                    {
                        if(!ast_stack_push(s,right))
                        {
                            dbgprint("stack_error");
                            state = ERROR;
                            break;
                        }
                        return(EP_SUCCESS);
                    }
                    //if(right->type == EXPR_FCALL)
                    //{
                        ast_node* new_node = node_new(EXPR_PAR,NOT_USED,"()");
                        if(new_node == NULL)
                        {
                            dbgprint("ast_error");
                            state = ERROR;
                            break;
                        }
                        if(!node_add(new_node,right))
                        {
                            dbgprint("ast_error");
                            state = ERROR;
                            break;
                        }
                        right = new_node;
                    //}
                    //else
                    //{
                    //    right->type = EXPR_PAR;
                    //}

                    if(!ast_stack_push(s,right))
                    {
                        dbgprint("stack_error");
                        state = ERROR;
                        break;
                    }
                    return(EP_SUCCESS);
                }
                break;

            case(LIST): // this states are embedded in other states
            case(EXPRF):
            default:
                state = ERROR;
                break;
        }
    }
    return(EP_SYNTAX_ERROR);
}

// inner function
// searches the stack for first nonterminal
ast_node* find_term(ast_stack* stk)
{
    if(stk == NULL)
    {
        dbgprint("received null pointer(stk)");
        return(NULL);
    }
    if(ast_stack_is_empty(stk))
    {
        dbgprint("stack is empty, shold not happen");
        return(NULL);
    }

    // TODO implement getter for index
    // TODO use dive here instead of direct acces to data
    for(size_t i = stk->stack.index-1; i+1 > 0 ; --i)
    {
        ast_node* node = stk->stack.data[i];
        if( node->type == TERM)
        {
            return(node);
        }
    }
    return(NULL);
}

// main function called by parser @see parser.c @see parser.h
ep_codes parse_expr(ast_node* root, context* con)
{
    if(root == NULL)
    {
        dbgprint("received null pointer(root).");
        return(EP_UNKNOWN_ERROR);
    }
    if(con == NULL)
    {
        dbgprint("received null pointer(con).");
        return(EP_UNKNOWN_ERROR);
    }

    // addressing the stack for easier use
    ast_stack* stk = &(con->expr_stack);
    // resseting pushing $
    ast_stack_reset(stk);
    ast_node* init_node = node_new(TERM,STOP,"");
    if(init_node == NULL) 
    {
        return(EP_AST_ERROR);
    }

    if(!ast_stack_push(stk,init_node))
    {
        return(EP_STACK_ERROR);
    }

    bool run = true;
    ep_codes ret = EP_UNKNOWN_ERROR;

    while(run)
    {
        ast_node* top = find_term(stk);
        ast_node* shift_node = NULL;
        // this should happen only when ast_stack fails
        if(top == NULL)
        {
            return(EP_STACK_ERROR);
        }

        ep_codes tmp;

        tab_index y = token_to_tab_index(top->sub_type);
        tab_index x = token_to_tab_index(con->token);
        
        INFORUN
        (
            fprintf(stderr,"--==--==--==--==--==\n");
            fprintf(stderr,"stack : ");node_print(stderr,top);fputc('\n',stderr);
            fprintf(stderr,"input : ");fprintf(stderr,"[ %s | %s ]",token_str(con->token),con->attrib.buffer);fputc('\n',stderr);
            fprintf(stderr,"tab[%u][%u] = %s\n",y,x,tab_op_to_str(tab[y][x]));
            fprintf(stderr,"++==++==++==++==++==\n");
        );
        //char* new_attrib;
        switch(tab[y][x])
        {
            default:
                dbgprint("Precedence table invalid index");
                ret = EP_UNKNOWN_ERROR;
                run = false;
                break;
            
            case(ERROR):
                dbgprint("EP syntax error according to precedence table.\n");
                ret = EP_SYNTAX_ERROR;
                run = false;
                break;

            case(SHIFT):
                if((shift_node = node_new(TERM,con->token,con->attrib.buffer)) == NULL)
                {
                    return(EP_AST_ERROR);
                }
                if(!ast_stack_push(stk,shift_node))
                {
                    node_delete(&shift_node);
                    return(EP_STACK_ERROR);
                }
                lex_next(con,stdin);
                break;

            case(REDUCE):
                tmp = reduce_FSM(stk);
                if(tmp != EP_SUCCESS) 
                {
                    return(tmp);
                }
                break;

            case(SHIFT_REDUCE):
                //shift
                if((shift_node = node_new(TERM,con->token,con->attrib.buffer)) == NULL)
                {
                    return(EP_AST_ERROR);
                }
                if(!ast_stack_push(stk,shift_node))
                {
                    node_delete(&shift_node);
                    return(EP_STACK_ERROR);
                }
                lex_next(con,stdin);

                // reduce
                tmp = reduce_FSM(stk);
                if(tmp != EP_SUCCESS)
                {
                    return(tmp);
                }
                break;

            case(ACCEPT):
                shift_node = ast_stack_peel(stk);
                if(shift_node == NULL)
                {
                    // ther must be node on the ast_stacke at the end of parse, atleast $
                    infoprint("return ACCEPT BUT STACK IS EMPTY");
                    return(EP_STACK_ERROR);
                }
 
                switch(shift_node->type)
                {
                    default:
                        infoprint("return SYNTAX ERROR, invalid node on top of stack");
                        node_delete(&shift_node);
                        return(EP_SYNTAX_ERROR);
                        break;
                    
                    case(EXPR):
                        infoprint("return EXPR");
                        ret = EP_EXPR;
                        break;

                    case(EXPR_PAR):
                        infoprint("return EXPR_PAR");
                        ret = EP_EXPR_PAR;
                        break;

                    case(EXPR_FCALL):
                        infoprint("return EXPR_FCALL");
                        ret = EP_EXPR_FCALL;
                        break;

                    case(EXPR_ASSIGN):
                        infoprint("return EXPR_ASSIGN");
                        ret = EP_EXPR_ASSIGN;
                        break;
                }
                
                // try add the node to sack
                if(!node_add(root,shift_node))
                {
                    node_delete(&shift_node);
                    return(EP_STACK_ERROR);
                }
                return(ret);
                break;
        }
    }
    return(ret);
}
