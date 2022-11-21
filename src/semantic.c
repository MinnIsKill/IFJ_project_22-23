/**
 * @file semantic.c
 * @author Vojtěch Kališ (xkalis03)
 * @brief semantic analyzer implementation
 *        receives an AST passed by parser as input, walks through it to evaluate semantic
 *        correctness, and if successful, passes the AST to code generator
 *
 * @date of last update:   21st November 2022
**/

// ./compiler <./ast_examples/[test_name].php

///TODO: NULLABLES

#include "semantic.h"

int sem_retcode = SEM_SUCCESS;
symtable_stack symstack;
bintree_node prev_scope_func;
bintree_node curr_scope_func;
bintree_node search;
bintree_node search2;
bintree_node var;


/**===========================================================**\
 *                     AUXILIARY FUNCTIONS                     *
 *                                                             *
 * the functions declared below this header serve primarily to *
 * be used inside the program for the purposes of implementing *
 * simple functional resources, type conversions and printing  *
 * many results/structures for the sake of debugging           *
\**===========================================================**/

//ID gen
static size_t id = 0;
size_t generate_id(){
    return (id++);
}

//(almost)breadth-first print of AST
void AST_dotprint(ast_node* root){
    if (root == NULL){
        return;
    }

    fprintf(stderr,"--------------------START OF AST PRINT--------------------\n");
    AST_dotprint_internal(root);
    fprintf(stderr,"---------------------END OF AST PRINT---------------------\n");
}
void AST_dotprint_internal(ast_node* root){
    ast_node *tmp;
    
    if (root->children_cnt != 0){
        for (size_t i = 0; i < root->children_cnt; i++){
            tmp = root->children[i];
            if (i != root->children_cnt){
                fprintf(stderr,"%lld -> %lld\n",root->id,tmp->id);
            }
        }
        for (size_t k = 0; k < root->children_cnt; k++){
            tmp = root->children[k];
            AST_dotprint_internal(tmp);
        }
    } else {
        fprintf(stderr,"node has no children\n");
    }
}

const char* node_type_tostr(node_type type){
    switch(type){
        case TERM:        return "TERM";        // terminal/leaf node
        case NTERM:       return "NTERM";       // non terminal generic rule, will change
        case EXPR:        return "EXPR";        // expression
        case EXPR_PAR:    return "EXPR_PAR";    // ( expression ) ; expression surrounded with parenthesis 
        case EXPR_FCALL:  return "EXPR_FCALL";  // FID ( expr or expr_list ) ; function call
        case EXPR_LIST:   return "EXPR_LIST";   // expr,expr,... ; list of at least two expressions.
                                                // can only be found under EXPR_FCALL node
        case EXPR_ASSIGN: return "EXPR_ASSIGN"; // ID = EXPR
        case BODY:        return "BODY";
        case FUN_BODY:    return "FUN_BODY";
        case PROG_BODY:   return "PROG_BODY";
        case WHILE_N:     return "WHILE_N";
        case IF_N:        return "IF_N";
        case ELSE_N:      return "ELSE_N";
        case RETURN_N:    return "RETURN_N";
        case RET_TYPE:    return "RET_TYPE";
        case FDEF:        return "FDEF";
        case PARAM:       return "PARAM";
        case PAR_LIST:    return "PAR_LIST";
        default:          return "ERROR";
    }
}

const char* node_subtype_tostr(token_type type){
    //PERSONAL NOTE: I WON'T BE NEEDING MOST OF THESE - WILL CULL ONCE SEMANTIC'S DONE
    switch (type){
    // end of source == EOF
        case EOS:    return "EOS";
    // keywords
        case FUNC:   return "FUNC"; //function
        case IF:     return "IF";
        case ELSE:   return "ELSE";
        case WHILE:  return "WHILE";
        case RETURN: return "RETURN";
    // types
        case ITYPE:  return "ITYPE";  // int
        case FTYPE:  return "FTYPE";  // float
        case STYPE:  return "STYPE";  // string
        case VTYPE:  return "VTYPE";  // void
    // nullable types
        case NITYPE: return "NITYPE"; // ?int
        case NFTYPE: return "NFTYPE"; // ?float
        case NSTYPE: return "NSTYPE"; // ?string
    // type op
        case TYPE:   return "TYPE";   //:
    // consts
        case IVAL:   return "IVAL";   // int
        case FVAL:   return "FVAL";   // float
        case SVAL:   return "SVAL";   // (?:string@)?"(?:[^\$]*)|(?:\x(?:[0-9A-Fa-f][0-9A-Fa-f])|(?:\[\t"n$])|(?:\[0-9][0-9][0-9]))"
        case VVAL:   return "VVAL";   // null
    // identifier
        case ID:     return "ID";     // must start with $
        case FID:    return "FID";    // mus not start with $
    // operators
        case ADD:    return "ADD";    // +
        case SUB:    return "SUB";    // -
        case MUL:    return "MUL";    // *
        case DIV:    return "DIV";    // /
        case STRCAT: return "STRCAT"; // .
        case LT:     return "LT";     // <
        case GT:     return "GT";     // >
        case LTE:    return "LTE";    // <=
        case GTE:    return "GTE";    // >=
        case EQ:     return "EQ";     // ===
        case NEQ:    return "NEQ";    // !==
        case COMMA:  return "COMMA";  // : return "";
    // aux
        case LPAR:   return "LPAR";   // (
        case RPAR:   return "RPAR";   // )
        case ASSIG:  return "ASSIG";  // =
    // scopes
        case LBRC:   return "LBRC";   // {
        case RBRC:   return "RBRC";   // }
        case SEMIC:  return "SEMIC";  // ;
    // other
        case PS_MARK: return "PS_MARK"; // <?php
        case PC_MARK: return "PC_MARK"; // declare(strict_types=1); 
        case PE_MARK: return "PE_MARK"; // ?>: return ""; optional
    // special 
        case NOT_USED: return "NOT USED";
        case INVALID: return "INVALID"; // this token signify error
        case STOP:    return "STOP";    // $ used by operator precedence parser
    // not used

    // error
        default:      return "ERROR";
    }
}

arg_type token_type_to_arg_type(token_type token){
    switch (token){
        case ITYPE: return int_t;    //int
        case FTYPE: return float_t;  //float
        case STYPE: return string_t; //string
        case VTYPE: return void_t;   //void

        case NITYPE: return nint_t;    //?int
        case NFTYPE: return nfloat_t;  //?float
        case NSTYPE: return nstring_t; //?string

        default: return ARG_TYPE_ERROR; //!!!
    }
}

arg_type token_type_to_arg_type_forvals(token_type token){
    switch (token){
        case IVAL: return int_t;    //int value (e.g. '1')
        case FVAL: return float_t;  //float value
        case SVAL: return string_t; //string value
        case VVAL: return void_t;   //void

        default: return ARG_TYPE_ERROR;
    }
}



/**===========================================================**\
 *                 SEMANTIC CHECKING FUNCTIONS                 *
 *                                                             *
 * the functions declared below this header serve the singular *
 * purpose of being called when a corresponding AST node is    *
 * encountered to check semantic correctness of said node      *
\**===========================================================**/

/**
 * @function: semantic_get_expr_type
 * @brief:  function receives(expects) 'EXPR' or 'EXPR_PAR' AST node as input and, through
 *          traversing its subnodes, determines the expression's resulting type (int, float,
 *          string) while also checking compatibility of the expression's values (e.g. if
 *          CONCAT attempt is above two strings, float+int compatibility, etc.)
 * 
 *  @param[in] node: AST node serving as root node for this function's purpose
 *  @param[in] global_symtab: binary tree structure holding function declarations
 *  @return: determined resulting type of the whole expression
 *  @errcodes: 3 - undefined function
 *             5 - attempt at using an undefined variable
 *             7 - type incompatibility in arithmetic, string or relational expressions
 *             8 - general (division by zero, ...)
 *            99 - internal (AST structure in wrong format, ...)
*/
arg_type semantic_get_expr_type(ast_node* node, struct bintree_node* global_symtab){
    if (node->type == EXPR || node->type == EXPR_PAR){ //check if function truly received an 'EXPR' node
        arg_type type_l, type_r;
        if (node->type == EXPR_PAR){ //if node is 'EXPR_PAR', AST makes sure to insert the node with the actual value as its child, so just move into it
            while (node->type == EXPR_PAR){
                node = node->children[0];
            }
        }
        if (node->children_cnt == 0){ //EXPR with no children, has to be a value or variable
            if (node->sub_type == IVAL || node->sub_type == FVAL || node->sub_type == SVAL || node->sub_type == VVAL){ //EXPR is a value
                return token_type_to_arg_type_forvals(node->sub_type);
            } else if (node->sub_type == ID){ //EXPR is a variable
                if ((var = bintree_search_by_key(curr_scope_func->local_symtab, node->attrib)) != NULL){ //is value defined in current scope?
                    //dbgprint("var->node_data->curr_type:  %s", bintree_fnc_arg_type_tostr(var->node_data->curr_type));
                    return var->node_data->curr_type;
                } else {
                    dbgprint("ERROR[5]:  attempt at using an undefined variable detected");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = UNDEF_VAR_ERR;}
                    return ARG_TYPE_ERROR;
                }
            } else {
                dbgprint("ERROR[99]:  unexpected format of 'EXPR' node: unsupported subtype");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                return ARG_TYPE_ERROR;
            }
        } else if (node->children_cnt == 2){ //EXPR with 2 children, has to be a numerical/string operation
            if (node->sub_type == ADD || node->sub_type == SUB || node->sub_type == MUL || node->sub_type == DIV || node->sub_type == STRCAT ||
                node->sub_type == LT || node->sub_type == GT || node->sub_type == LTE || node->sub_type == GTE || node->sub_type == EQ || node->sub_type == NEQ){
                type_l = semantic_get_expr_type(node->children[0], global_symtab); //get type of left node recursively
                type_r = semantic_get_expr_type(node->children[1], global_symtab); //get type of right node recursively
                //dbgprint("type_l: %s", bintree_fnc_arg_type_tostr(type_l));
                //dbgprint("type_r: %s", bintree_fnc_arg_type_tostr(type_r));
                //dbgprint("node->sub_type: %s", node_subtype_tostr(node->sub_type));
                if (node->sub_type == STRCAT){ //first check if STRCAT
                    if (type_l == string_t && type_r == string_t){
                        return string_t;
                    } else {
                        dbgprint("ERROR[7]:  found a type incompatibility error in an expression");
                        if (sem_retcode == SEM_SUCCESS){sem_retcode = INCOMP_TYPES_ERR;}
                        return ARG_TYPE_ERROR;
                    }
                }
                if ((type_l == type_r) && (type_l != string_t && type_r != string_t)){ //if types are equal and aren't strings
                    if (node->sub_type == DIV){ //if dividing, check if right value isn't a zero
                        if ((type_r == int_t && atoll(node->children[1]->attrib) == 0) ||
                            (type_r == float_t && atof(node->children[1]->attrib) == 0.0)){
                                dbgprint("ERROR[8]:  found an attempt at division by zero");
                                if (sem_retcode == SEM_SUCCESS){sem_retcode = SEM_GENERAL_ERR;}
                                return ARG_TYPE_ERROR;
                        }
                    }
                    return type_l; //just return it
                } else if ((type_l == int_t && type_r == float_t) || (type_l == float_t && type_r == int_t)){ //if one type is int and the other float
                    if (node->sub_type == DIV){ //if dividing, check if right value isn't a zero
                        if ((type_r == int_t && atoll(node->children[1]->attrib) == 0) ||
                            (type_r == float_t && atof(node->children[1]->attrib) == 0.0)){
                                dbgprint("ERROR[8]:  found an attempt at division by zero");
                                if (sem_retcode == SEM_SUCCESS){sem_retcode = SEM_GENERAL_ERR;}
                                return ARG_TYPE_ERROR;
                        }
                    }
                    return float_t; //return float    
                } else {
                    dbgprint("ERROR[7]:  found a type incompatibility error in an expression");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = INCOMP_TYPES_ERR;}
                    return ARG_TYPE_ERROR;
                }
            }
        } else {
            dbgprint("ERROR[99]:  unexpected format of 'EXPR' node: children_cnt not 0 or 2");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return ARG_TYPE_ERROR;
        }
    } else if (node->type == EXPR_FCALL){
        semantic_check_fcall(node, global_symtab);
        if (sem_retcode != SEM_SUCCESS){return ARG_TYPE_ERROR;}
        search = bintree_search_by_key(global_symtab, node->attrib);
        return search->node_data->rtype;
    } else {
        dbgprint("ERROR[99]:  'semantic_get_expr_type' received a non-'EXPR' AST node of type: %s", node_type_tostr(node->type));
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return ARG_TYPE_ERROR;
    }
    return ARG_TYPE_ERROR;
}

/** @function: global_symtab_funcinsert
 *  @brief:  function called when 'FUN_DEF' encountered in AST traversal, creates a binary tree serving 
 *           as the function's declaration and inserts it into 'global_symtable'
 *  WARNING: - function expects the received AST 'FUN_DEF' node to already contain its full structure,
 *             which means all sub-nodes
 *           - non-reusable/portable - this function expects to receive a binary tree (AST) with a very
 *             specific structure declared for the purposes of this project, and thus has little to no
 *             portability in between projects
 * 
 * @param[in] node: 'FUN_DEF' node of AST, serves as root node for this function's purpose
 * @param[in] global_symtab: binary tree structure into which the function declaration's saved
 * @return: pointer back to where hte 'global_symtab' param was initially pointing
*/
struct bintree_node* global_symtab_funcinsert(ast_node* node, struct bintree_node* global_symtab){
    global_symtab = bintree_insert(global_symtab, generate_id(), node->attrib, function);
    if ((search = bintree_search_by_key(global_symtab, node->attrib)) == NULL){
        dbgprint("ERROR[99]:  in 'global_symtab_insert': function 'bintree_insert' failed inserting function into global symtable\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    }
    dll list = dll_create();
    if (list == NULL){
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    }
    search->node_data->args_list = list;
    search = bintree_node_nullifyinfo(search);
    bintree_init(&search->local_symtab);

    if (node->children[0]->type == PAR_LIST){ //!!!assuming parameter list is always the first child
        search->node_data->rtype = token_type_to_arg_type(node->children[1]->sub_type);
        node = node->children[0]; //go to parameter list
        //fill out "search->node_data->arg_names_list" && "search->node_data->arg_type_list"
        for (size_t i = 0; i < node->children_cnt; i++){
            //dbgprint("node->children[i]->type: %s",node_type_tostr(node->children[i]->type));
            if (node->children[i]->type == PARAM){
                arg_type type = token_type_to_arg_type(node->children[i]->sub_type);
                char tmp[32];
                strncpy(tmp, bintree_fnc_arg_type_tostr(type), sizeof(tmp));
                //dbgprint("node->children[i]->attrib: %s",node->children[i]->attrib);
                search->local_symtab = bintree_insert(search->local_symtab, search->node_data->arg_cnt, node->children[i]->attrib, variable); //insert the variable into function's local symtab
                var = bintree_search_by_key(search->local_symtab, node->children[i]->attrib);
                var = bintree_node_nullifyinfo(var);
                var->node_data->init_type = type;
                var->node_data->curr_type = type;
                dll_insert_last(search->node_data->args_list, search->node_data->arg_cnt, node->children[i]->attrib, tmp); //insert arg name and type
                //search->local_symtab->node_data->arg_type_list[search->node_data->arg_cnt] = token_type_to_arg_type(node->children[i]->sub_type); //insert arg type
                search->node_data->vars_cnt++; //increment var count
                search->node_data->arg_cnt++; //increment arg count
            } else {
                dbgprint("ERROR[99]:  in 'global_symtab_insert': found non-param node inserted in parameter list in AST\n       Function was not inserted into global tree => wrong behaviour may follow");
            }
        }
    } else if (node->children[0]->type == RET_TYPE){ //function with no parameters
        search->node_data->vars_cnt = 0;
        search->node_data->arg_cnt = 0;
        search->node_data->rtype = token_type_to_arg_type(node->children[0]->sub_type);
    } else {
        dbgprint("ERROR[99]:  in 'global_symtab_insert': function declaration empty or missing sub-nodes\n       Function was not inserted into global tree => wrong behaviour may follow"); //!!!todo: error code (but this should be syntax error)
    }

    return global_symtab;
}

/** @function: AST_DF_firsttraversal
 *  @brief depth-first traversal of AST ran at start of semantic checking to search for function declarations
 *  NOTE: THIS FUNCTION WAS USED FOR PERSONAL TESTING AND IS UNUSED IN THE FINAL IMPLEMENTATION, AS A DIFFERENT
 *        APPROACH WAS REPLACED IT INSTEAD
 * 
 *  @param[in] AST: AST root node (from context)
 *  @param[in] global_symtab: global symtable (from context)
 *  @errcodes: 3 - attempt at function redefinition
 *  @return: pointer back to where the 'global_symtab' param was initially pointing
**/
struct bintree_node* AST_DF_firsttraversal(ast_node* AST, struct bintree_node* global_symtab){
    if (AST == NULL || sem_retcode != 0){
        return global_symtab;
    }

    switch (AST->type){
    //function definition encountered
        case FDEF:
            if ((search = bintree_search_by_key(global_symtab, AST->attrib)) != NULL){ //check if function was already defined
                dbgprint("ERROR[3]: attempt at function redefinition encountered"); //!!!
                if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
            } else { //if it doesn't exist yet, create new node in symtable
                global_symtab = global_symtab_funcinsert(AST, global_symtab);
            }
            break;
        default: 
            break;
    }

    INFORUN(
        const char* type = node_type_tostr(AST->type);
        const char* sub_type = node_subtype_tostr(AST->sub_type);
        fprintf(stdout,"[attrib]%-10s:  [type]%-10s  [subtype]%-10s\n", AST->attrib, type, sub_type);
    );

    for(size_t i = 0; i < AST->children_cnt; i++){
        if(AST->children[i] != NULL && sem_retcode == 0){
            global_symtab = AST_DF_firsttraversal(AST->children[i], global_symtab);
        }
    }

    return global_symtab;
}



/**===========================================================**\
 *                 SEMANTIC CHECKING FUNCTIONS                 *
 *                                                             *
 * the functions declared below this header serve the singular *
 * purpose of being called when a corresponding AST node is    *
 * encountered to check semantic correctness of said node      *
\**===========================================================**/

/**
 * [ASSIGNMENT CHECKING]
 * 
 * @param[in] node: 'EXPR_ASSIGN' node of AST, serves as root node for this function's purpose
 * @param[in] global_symtab: binary tree structure holding function declarations
 * @errcodes: 3 - undefined function
 *            5 - attempt at using an undefined variable
 *            7 - type incompatibility in arithmetic, string or relational expressions
 *            8 - general (division by zero, ...)
*/
void semantic_check_assign(ast_node* node, struct bintree_node* global_symtab){
    if (node->children_cnt < 2){
        dbgprint("ERROR:  found assignment with no left or right parameters (e.g. '= expr' or '$var ='"); //!!! should be handled by parser
    }
    if (node->children[0]->sub_type != ID){
        dbgprint("ERROR:  found assignment with its left parameter not being a variable"); //!!! should be handled by parser
    }

    //check if everything that's on the right side of the equation can actually be added together
    arg_type type;
    ///NOTE: 'semantic_get_expr_type' already checks for compatibility
    type = semantic_get_expr_type(node->children[1], global_symtab);
    if (sem_retcode != SEM_SUCCESS){
        return;
    }

    //found a legitimate assignment => put variable into symtable if not there yet
    if ((var = bintree_search_by_key(curr_scope_func->local_symtab, node->children[0]->attrib)) == NULL){
        curr_scope_func->local_symtab = bintree_insert(curr_scope_func->local_symtab, curr_scope_func->node_data->vars_cnt++, node->children[0]->attrib, variable);
        var = bintree_search_by_key(curr_scope_func->local_symtab, node->children[0]->attrib);
        var = bintree_node_nullifyinfo(var);
        curr_scope_func->local_symtab->node_data->vars_cnt++; //increment var count
        var->node_data->init_type = type;
        var->node_data->curr_type = type;
    } else {
        var->node_data->curr_type = type;
    }
}

/**
 * [FUNCTION CALL CHECKING]
 * 
 * @param[in] node: 'EXPR_FCALL' node of AST, serves as root node for this function's purpose
 * @param[in] global_symtab: binary tree structure holding function declarations
 * @errcodes: 3 - undefined function
 *            4 - wrong number/types of parameters passed while calling a function
 *            5 - attempt at using an undefined variable
 *            7 - type incompatibility in arithmetic, string or relational expressions
 *            
*/
void semantic_check_fcall(ast_node* node, struct bintree_node* global_symtab){
// [3] check if function defined
    if ((search = bintree_search_by_key(global_symtab, node->attrib)) == NULL){
        dbgprint("ERROR[3]:  found an attempt at calling an undefined function");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
        return;
    }

// [5] if function call contains variable parameters, check if they're defined
// [7] check type compatibility of arithmetic, string or relational expressions inside function call
//  └─> THESE ARE BOTH ALREADY HANDLED BY 'semantic_get_expr_type'

// [4] check number/type of parameters while calling a function

    //number of parameters

    if (node->children_cnt == 0){ //if no parameters passed
        if (search->node_data->arg_cnt != 0){ //check if called function needs any params
            dbgprint("ERROR[4]:  wrong number of parameters passed in function call");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
        }
        return; //nothing to be done for function with no params
    }

    if (node->children[0]->type == EXPR_PAR){ //if expr surrounded by multiple parentheses (for whatever reason)
        while (node->children[0]->type == EXPR_PAR){
            node = node->children[0];
        }
    }

    if (node->children[0]->type == EXPR_LIST){ //if child is EXPR_LIST
        node = node->children[0];
    } else if (node->children[0]->type == EXPR || node->children[0]->type == EXPR_FCALL){ //if child is EXPR or EXPR_FCALL
        if (search->node_data->arg_cnt != 1){
            dbgprint("ERROR[4]:  wrong number of parameters passed in function call");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
            return;
        }
        if (node->children[0]->type == EXPR_FCALL){
            semantic_check_fcall(node->children[0], global_symtab);
            if (sem_retcode != SEM_SUCCESS){return;}
        }
        node = node->children[0];
    } else {//else AST is wrongly formatted
        dbgprint("ERROR[99]: wrong format of AST received: in function call, expected node with type 'EXPR_LIST' but %s received instead", node_type_tostr(node->children[0]->type));
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return;
    }

    if ((node->children_cnt != search->node_data->arg_cnt) && (node->type == EXPR_LIST)){ //if number of parameters in EXPR_LIST doesn't correspond to the number of parameters required by given function
        dbgprint("ERROR[4]:  wrong number of parameters passed in function call");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
        return;
    }

    //types of parameters

    struct dll_link *ptr = search->node_data->args_list->head; //go to start of function's 'args_list' list
    char tmp[32];
    arg_type type;
    ///NOTE: the following loop will only run for 'EXPR_LIST' nodes
    if (node->type == EXPR_LIST){
        for (size_t i = 0; i < search->node_data->arg_cnt; i++){ //repeat until you reach end of function's 'args_list' list
            /**INFORUN(
                fprintf(stderr,"-- node->children[i]->type:    %s\n", node_type_tostr(node->children[i]->type));
                fprintf(stderr,"-- node->children[i]->attrib:  %s\n", node->children[i]->attrib);
                fprintf(stderr,"-- node->children_cnt:         %ld\n", node->children_cnt);
                fprintf(stderr,"-- search->node_data->arg_cnt: %ld\n", search->node_data->arg_cnt);
            );**/
            if (node->children[i]->type == EXPR_FCALL){ //if passed parameter is a function call
                if ((search2 = bintree_search_by_key(global_symtab, node->children[i]->attrib)) == NULL){ //check if called function exists
                    dbgprint("ERROR[3]:  found an attempt at calling an undefined function");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
                    return;
                }
                arg_type type = semantic_get_expr_type(node->children[i], global_symtab); //get its rettype (also has imbedded fcall checking)
                if (sem_retcode != SEM_SUCCESS){return;}
                //dbgprint("bintree_fnc_arg_type_tostr(type):  %s",bintree_fnc_arg_type_tostr(type));
                //dbgprint("ptr->linkData->type:               %s",ptr->linkData->type);
                if (strcmp(bintree_fnc_arg_type_tostr(type), ptr->linkData->type) != 0){ //should also handle nullables? (?int, ?string,...)
                    dbgprint("ERROR[4]:  wrong types of parameters passed in function call");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
                    return;
                }
            } else if (node->children[i]->type == EXPR){
                type = semantic_get_expr_type(node->children[i], global_symtab);
                strncpy(tmp, bintree_fnc_arg_type_tostr(type), sizeof(tmp));
                if (sem_retcode != 0){
                    return;
                }
                //dbgprint("?type: %s", bintree_fnc_arg_type_tostr(type));
                //dbgprint("?tmp:  %s", tmp);

                /**INFORUN(
                    const char* type = node_type_tostr(node->children[i]->type);
                    const char* sub_type = node_subtype_tostr(node->children[i]->sub_type);
                    fprintf(stdout,"![attrib]%-10s:  [type]%-10s  [subtype]%-10s\n", node->children[i]->attrib, type, sub_type);
                );**/
                //dbgprint("bintree_fnc_arg_type_tostr(type):  %s",bintree_fnc_arg_type_tostr(type));
                //dbgprint("ptr->linkData->type:               %s",ptr->linkData->type);
                if (strcmp(tmp, ptr->linkData->type) != 0){ //if passed parameter is 'int', 'float' or 'string' (+ their nullable versions)
                    dbgprint("ERROR[4]:  encountered wrong type of parameter passed in function call");
                    sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;
                    return;
                }
                //dbgprint("?type: %s", bintree_fnc_arg_type_tostr(type));
                //dbgprint("?tmp:  %s", tmp);
            }
            /**INFORUN(
                const char* type = node_type_tostr(node->children[i]->type);
                const char* sub_type = node_subtype_tostr(node->children[i]->sub_type);
                fprintf(stdout,"![attrib]%-10s:  [type]%-10s  [subtype]%-10s\n", node->children[i]->attrib, type, sub_type);
            );**/
            ptr = ptr->next; //move to next parameter in list
        }
    }

    if (node->type == EXPR){ //
        //dbgprint("! tmp: %s",tmp);
        type = semantic_get_expr_type(node, global_symtab);
        if (sem_retcode != SEM_SUCCESS){return;}
        //dbgprint("!! tmp: %s",tmp);
    } else if (node->type == EXPR_LIST){ //check the tail as well
        //dbgprint("- tmp: %s",tmp);
        type = semantic_get_expr_type(node->children[node->children_cnt-1], global_symtab);
        if (sem_retcode != SEM_SUCCESS){return;}
        //dbgprint("-- type: %d",type);
        //dbgprint("-- tmp:  %s",tmp);
    } else if (node->type == EXPR_FCALL) {
        if ((search = bintree_search_by_key(global_symtab, node->attrib)) == NULL){ //check if called function exists
            dbgprint("ERROR[3]:  found an attempt at calling an undefined function");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
            return;
        }
        //arg_type = semantic_get_expr_type();
        arg_type type = semantic_get_expr_type(node, global_symtab);
        if (sem_retcode != SEM_SUCCESS){return;}
        if (search->node_data->rtype != type){ //should also handle nullables? (?int, ?string,...)
            dbgprint("ERROR[4]:  wrong types of parameters passed in function call");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
            return;
        }
    }

}

/**
 * [EXPR CHECKING]
 * 
 * @param[in] node: 'EXPR' node of AST, serves as root node for this function's purpose
 * @param[in] global_symtab: binary tree structure holding function declarations
 * @errcodes: 3 - undefined function
 *            5 - attempt at using an undefined variable
 *            7 - type incompatibility in arithmetic, string or relational expressions
 *            8 - general (division by zero, ...)
 *           99 - internal (AST structure in wrong format, ...)
*/
void semantic_check_expr(ast_node* node, struct bintree_node* global_symtab){
    if (node->type == EXPR){
        semantic_get_expr_type(node, global_symtab);
    }/** else {
        dbgprint("ERROR[99]  'semantic_check_expr' received AST node which wasn't of type 'EXPR'");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
    }**/

    return;
}

/**
 * [RETURN CHECKING]
 * 
 * @param[in] node: 'RETURN' node of AST, serves as root node for this function's purpose
 * @param[in] global_symtab: binary tree structure holding function declarations
 * @errcodes: 4 - wrong return type
 *            5 - attempt at using an undefined variable
 *            6 - missing or excessive expression in return
 *            7 - type incompatibility in arithmetic, string or relational expressions
*/
void semantic_check_return(ast_node* node, struct bintree_node* global_symtab){
    if (node->type == RETURN_N){
        if (node->children_cnt == 0){ //empty return (return;)
            if (curr_scope_func->node_data->rtype != void_t){
                dbgprint("ERROR[6]:  found empty return inside a function not defined with 'void' return type");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_RET_EXPR_ERR;}
            }
        } else if (node->children_cnt == 1){ //return with expression (e.g. [return 1;] or [return $b+foo($b);])
            if (curr_scope_func->node_data->rtype == void_t){
                if (strcmp(curr_scope_func->node_data->key, ":b") != 0){ //if it's not main body
                    dbgprint("ERROR[6]:  found non-empty return inside a function defined with 'void' return type");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_RET_EXPR_ERR;}
                    return;
                }
            } else {
                arg_type type = semantic_get_expr_type(node->children[0], global_symtab); //get type of return expr
                if (sem_retcode != SEM_SUCCESS){return;}
                if (type != curr_scope_func->node_data->rtype){ //if type of expr doesn't match function's defined return type
                    //!!!if return type is int and function defined as returning float, should be compatible (I guess?)
                    if (!(type == int_t && curr_scope_func->node_data->rtype == float_t)){
                        dbgprint("ERROR[4]:  wrong return type encountered");
                        if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
                    } 
                }
            }
        } else {
            dbgprint("ERROR[99]:  received 'RETURN' node in AST with more than 1 child nodes");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        }
    }/** else {
        dbgprint("ERROR[99]  'semantic_check_return' received AST node which wasn't of type 'RETURN_N'");
        sem_retcode = ERR_INTERNAL;
    }**/

    return;
}

/**
 * [CONDITIONALS (if/while) CHECKING]
 * 
 * @param[in] node: 'IF_N' or 'WHILE_N' node of AST, serves as root node for this function's purpose
 * @param[in] global_symtab: binary tree structure holding function declarations
 * @errcodes: 7 - type incompatibility in arithmetic, string or relational expressions
*/
void semantic_check_conditionals(ast_node* node, struct bintree_node* global_symtab){
    while (node->type == EXPR_PAR){ //get rid of EXPR_PAR nodes
        node = node->children[0];
    }
    if (node->children_cnt == 1){
        semantic_get_expr_type(node->children[0], global_symtab); //this is here basically just because it already has fun and var def checks
        if (sem_retcode != SEM_SUCCESS){return;}
    } else if (node->children_cnt == 2){
        arg_type type_l, type_r;
        ///NOTE: 'semantic_get_expr_type' already checks for compatibility
        type_l = semantic_get_expr_type(node->children[0], global_symtab);
        if (sem_retcode != SEM_SUCCESS){return;}
        type_r = semantic_get_expr_type(node->children[1], global_symtab);
        if (sem_retcode != SEM_SUCCESS){return;}

        if (!((type_l == type_r) || (type_l == int_t && type_r == float_t) || (type_l == float_t && type_r == int_t))){
            dbgprint("ERROR[7]:  found type incompatibility in a(n) %s conditional", node_type_tostr(node->type));
        }
    }
    return;
}



/**===========================================================**\
 *                SEMANTICS MAIN LOOP TRAVERSAL                *
\**===========================================================**/
/**
 * @function: AST_DF_traversal
 * @brief main, depth-first AST traversal
 * 
 * @param[in] AST: AST root node (from context)
 * @param[in] global_symtab: global symtable (from context)
**/
void AST_DF_traversal(ast_node* AST, struct bintree_node* global_symtab){
    if (AST == NULL || sem_retcode != SEM_SUCCESS){
        return;
    }

    INFORUN(
        const char* type = node_type_tostr(AST->type);
        const char* sub_type = node_subtype_tostr(AST->sub_type);
        fprintf(stdout,"[attrib]%-10s:  [type]%-10s  [subtype]%-10s\n", AST->attrib, type, sub_type);
    );

    switch (AST->type){
    //encountered assignment
        case EXPR_ASSIGN:
            semantic_check_assign(AST, global_symtab);
            break;
    //encountered function call
        case EXPR_FCALL:
            semantic_check_fcall(AST, global_symtab);
            break;
    //encountered an expression
        case EXPR:
            semantic_check_expr(AST, global_symtab);
            break;
    //encountered an expression
        case RETURN_N:
            semantic_check_return(AST, global_symtab);
            break;
    //encountered function definition (switches current scope to this function)
        case FDEF:
            curr_scope_func = bintree_search_by_key(global_symtab, AST->attrib);
            break;
        case IF_N:
            if (AST->children_cnt < 2){
                dbgprint("ERROR[99]:  received IF_N AST node with only one child");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                break;
            }
            //checks ONLY the relational statement
            semantic_check_conditionals(AST->children[0], global_symtab);
            if (sem_retcode != SEM_SUCCESS){break;}
            //now check body
            AST_DF_traversal(AST->children[1], global_symtab);
            if (sem_retcode != SEM_SUCCESS){break;}
            //second body = else, check that as well if it exists
            if (AST->children_cnt == 3){
                AST_DF_traversal(AST->children[2], global_symtab);
            }
            break;
        case WHILE_N:
            if (AST->children_cnt < 2){
                dbgprint("ERROR[99]:  received IF_N AST node with only one child");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                break;
            }
            while (AST->children[0]->type == EXPR_PAR){ //get rid of EXPR_PAR nodes
                AST = AST->children[0];
            }
            //checks ONLY the relational statement
            semantic_check_conditionals(AST->children[0], global_symtab);
            if (sem_retcode != SEM_SUCCESS){break;}
            //now check body
            AST_DF_traversal(AST->children[1], global_symtab);
            break;
        default: 
            break;
    }

    for(size_t i = 0; i < AST->children_cnt; i++){
        if(AST->children[i] != NULL && sem_retcode == 0){
            AST_DF_traversal(AST->children[i], global_symtab);
            if (AST->children[i]->type == RETURN_N){
                //destroy everything after RETURN_N
                INFORUN(
                    fprintf(stderr, "trimming: ");
                    node_print(stderr, AST);
                    fprintf(stderr, "  after RETURN_N received\n");
                );
                size_t deleted = 0;
                for (size_t j = i; j < AST->children_cnt; j++){
                    node_delete(&AST->children[j]);
                    deleted++;
                }
                AST->children_cnt -= deleted;
                //then return;
                return;
            }
        }
    }
    if (AST->type == FDEF){ //move current scope back to main
        curr_scope_func = bintree_search_by_key(global_symtab, ":b");
    }
}

/**===========================================================**\
 *                       SEMANTICS START                       *
\**===========================================================**/
int semantic(context* cont){
    INFORUN(
        fprintf(stdout,"\n  __  _               ___ ___  _  __ \n");
        fprintf(stdout," (_  |_ |\\/|  /\\  |\\ | |   |  /  (_  \n");
        fprintf(stdout," __) |_ |  | /--\\ | \\| |  _|_ \\_ __) \n\n");
    );
//initialize global symtable
    //bintree_init(&cont->global_symtab);
//create main body symtable node
    cont->global_symtab = bintree_insert(cont->global_symtab, generate_id(), ":b", function); //main program body
//check if inserted and set curr scope to main body
    if ((curr_scope_func = bintree_search_by_key(cont->global_symtab, ":b")) == NULL){
        dbgprint("ERROR:  failed to find ':b' (aka main) node in global symtable");
        return ERR_INTERNAL;
    }
    dll list = dll_create();
    if (list == NULL){
        return ERR_INTERNAL;
    }
    curr_scope_func->node_data->args_list = list;
    bintree_node_nullifyinfo(curr_scope_func);
    curr_scope_func->node_data->rtype = void_t;
    bintree_init(&curr_scope_func->local_symtab);

//load forward function declarations into global symtable
    cont->global_symtab = AST_DF_firsttraversal(cont->root, cont->global_symtab);
//DEBUG: print out main body symtable and global symtable
    INFORUN(
        fprintf(stdout,"--------------------------------------------------\n");
        bintree_inorder_fullprint(cont->global_symtab, false);
        fprintf(stdout,"--------------------------------------------------\n");
    );
    if (sem_retcode != SEM_SUCCESS){ return sem_retcode; } //if error occured during first traversal (afaik, only error #3 can occur here)
//main AST traversal
    AST_DF_traversal(cont->root, cont->global_symtab);
    if (sem_retcode != SEM_SUCCESS){ return sem_retcode; } //if error occured

    return sem_retcode;
}