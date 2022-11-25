/**
 * @file semantic.c
 * @author Vojtěch Kališ (xkalis03)
 * @brief semantic analyzer implementation
 *        receives an AST passed by parser as input, walks through it to evaluate semantic
 *        correctness, and if successful, passes the AST to code generator
 *
 * @date of last update:   24th November 2022
**/

// ./compiler <./ast_examples/[test_name].php

///TODO: NULLABLES
//           - v rettype
//           - v fundef
//           - v funcall

#include "semantic.h"

int sem_retcode = SEM_SUCCESS;
bool return_encountered = false;
symtable_stack symstack;
bintree_node prev_scope_func;
bintree_node curr_scope_func;
bintree_node search;
bintree_node search2;
bintree_node search3;
bintree_node search4;
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

        case CONVERT_TYPE: return "CONVERT_TYPE";
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
        case IDIV:   return "IDIV";   // int /
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

bool is_it_predef_func_call(ast_node* node){
    if (((strcmp(node->attrib,"reads") == 0)) || ((strcmp(node->attrib,"readi") == 0))  || ((strcmp(node->attrib,"readf") == 0)) ||
        ((strcmp(node->attrib,"write") == 0)) || ((strcmp(node->attrib,"strlen") == 0)) || ((strcmp(node->attrib,"substring") == 0)) ||
        ((strcmp(node->attrib,"ord") == 0))   || ((strcmp(node->attrib,"chr") == 0))){
        return true;
    } else {
        return false;
    }
}

struct bintree_node* create_predef_funcs(struct bintree_node* global_symtab){
//reads() : ?string
    global_symtab = bintree_insert(global_symtab, generate_id(), "reads", function); //main program body
    //check if inserted and set curr scope to main body
    if ((search = bintree_search_by_key(global_symtab, "reads")) == NULL){
        fprintf(stderr,"ERROR[99]:  failed to find 'reads' predefined function's node in global symtable\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    } else {
        dll list = dll_create();
        if (list == NULL){
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return global_symtab;
        }
        search->node_data->args_list = list;
        bintree_node_nullifyinfo(search);
        bintree_init(&search->local_symtab);

        search->node_data->rtype = nstring_t;
    }

//readi() : ?int
    global_symtab = bintree_insert(global_symtab, generate_id(), "readi", function); //main program body
    //check if inserted and set curr scope to main body
    if ((search = bintree_search_by_key(global_symtab, "readi")) == NULL){
        fprintf(stderr,"ERROR[99]:  failed to find 'readi' predefined function's node in global symtable\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    } else {
        dll list = dll_create();
        if (list == NULL){
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return global_symtab;
        }
        search->node_data->args_list = list;
        bintree_node_nullifyinfo(search);
        bintree_init(&search->local_symtab);

        search->node_data->rtype = nint_t;
    }

//readf() : ?float
    global_symtab = bintree_insert(global_symtab, generate_id(), "readf", function); //main program body
    //check if inserted and set curr scope to main body
    if ((search = bintree_search_by_key(global_symtab, "readf")) == NULL){
        fprintf(stderr,"ERROR[99]:  failed to find 'readf' predefined function's node in global symtable\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    } else {
        dll list = dll_create();
        if (list == NULL){
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return global_symtab;
        }
        search->node_data->args_list = list;
        bintree_node_nullifyinfo(search);
        bintree_init(&search->local_symtab);
        
        search->node_data->rtype = nfloat_t;
    }

//write (term1, term2, ..., term(n)) : void
    global_symtab = bintree_insert(global_symtab, generate_id(), "write", function); //main program body
    //check if inserted and set curr scope to main body
    if ((search = bintree_search_by_key(global_symtab, "write")) == NULL){
        fprintf(stderr,"ERROR[99]:  failed to find 'write' predefined function's node in global symtable\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    } else {
        dll list = dll_create();
        if (list == NULL){
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return global_symtab;
        }
        search->node_data->args_list = list;
        bintree_node_nullifyinfo(search);
        bintree_init(&search->local_symtab);

        search->node_data->rtype = int_t;

        search->node_data->variadic_func = true;
    }


//function strlen(string $s) : int
    global_symtab = bintree_insert(global_symtab, generate_id(), "strlen", function); //main program body
    //check if inserted and set curr scope to main body
    if ((search = bintree_search_by_key(global_symtab, "strlen")) == NULL){
        fprintf(stderr,"ERROR[99]:  failed to find 'strlen' predefined function's node in global symtable\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    } else {
        dll list = dll_create();
        if (list == NULL){
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return global_symtab;
        }
        search->node_data->args_list = list;
        bintree_node_nullifyinfo(search);
        bintree_init(&search->local_symtab);

        search->node_data->rtype = int_t;

        //insert 'string $s'
        search->local_symtab = bintree_insert(search->local_symtab, 0, "s", variable); //insert the variable into function's local symtab
        var = bintree_search_by_key(search->local_symtab, "s");
        var = bintree_node_nullifyinfo(var);
        var->node_data->init_type = string_t;
        var->node_data->curr_type = string_t;
        dll_insert_last(search->node_data->args_list, 0, "s", "string"); //insert arg name and type

        search->node_data->vars_cnt = 1; //set var count
        search->node_data->arg_cnt = 1; //set arg count
    }

//function substring(string $s, int $i, int $j) : ?string
    global_symtab = bintree_insert(global_symtab, generate_id(), "substring", function); //main program body
    //check if inserted and set curr scope to main body
    if ((search = bintree_search_by_key(global_symtab, "substring")) == NULL){
        fprintf(stderr,"ERROR[99]:  failed to find 'substring' predefined function's node in global symtable\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    } else {
        dll list = dll_create();
        if (list == NULL){
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return global_symtab;
        }
        search->node_data->args_list = list;
        bintree_node_nullifyinfo(search);
        bintree_init(&search->local_symtab);

        search->node_data->rtype = nstring_t;

        //insert 'string $s'
        search->local_symtab = bintree_insert(search->local_symtab, 0, "s", variable); //insert the variable into function's local symtab
        var = bintree_search_by_key(search->local_symtab, "s");
        var = bintree_node_nullifyinfo(var);
        var->node_data->init_type = string_t;
        var->node_data->curr_type = string_t;
        dll_insert_last(search->node_data->args_list, 0, "s", "string"); //insert arg name and type
        //insert 'int $i'
        search->local_symtab = bintree_insert(search->local_symtab, 1, "i", variable); //insert the variable into function's local symtab
        var = bintree_search_by_key(search->local_symtab, "i");
        var = bintree_node_nullifyinfo(var);
        var->node_data->init_type = int_t;
        var->node_data->curr_type = int_t;
        dll_insert_last(search->node_data->args_list, 1, "i", "int"); //insert arg name and type
        //insert 'int $j'
        search->local_symtab = bintree_insert(search->local_symtab, 2, "j", variable); //insert the variable into function's local symtab
        var = bintree_search_by_key(search->local_symtab, "j");
        var = bintree_node_nullifyinfo(var);
        var->node_data->init_type = int_t;
        var->node_data->curr_type = int_t;
        dll_insert_last(search->node_data->args_list, 2, "j", "int"); //insert arg name and type

        search->node_data->vars_cnt = 3; //set var count
        search->node_data->arg_cnt = 3; //set arg count
    }

//ord(string $c) : int
    global_symtab = bintree_insert(global_symtab, generate_id(), "ord", function); //main program body
    //check if inserted and set curr scope to main body
    if ((search = bintree_search_by_key(global_symtab, "ord")) == NULL){
        fprintf(stderr,"ERROR[99]:  failed to find 'ord' predefined function's node in global symtable\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    } else {
        dll list = dll_create();
        if (list == NULL){
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return global_symtab;
        }
        search->node_data->args_list = list;
        bintree_node_nullifyinfo(search);
        bintree_init(&search->local_symtab);

        search->node_data->rtype = int_t;

        //insert 'string $c'
        search->local_symtab = bintree_insert(search->local_symtab, 0, "c", variable); //insert the variable into function's local symtab
        var = bintree_search_by_key(search->local_symtab, "c");
        var = bintree_node_nullifyinfo(var);
        var->node_data->init_type = string_t;
        var->node_data->curr_type = string_t;
        dll_insert_last(search->node_data->args_list, 0, "c", "string"); //insert arg name and type

        search->node_data->vars_cnt = 1; //set var count
        search->node_data->arg_cnt = 1; //set arg count
    }

//chr(int $i) : string
    global_symtab = bintree_insert(global_symtab, generate_id(), "chr", function); //main program body
    //check if inserted and set curr scope to main body
    if ((search = bintree_search_by_key(global_symtab, "chr")) == NULL){
        fprintf(stderr,"ERROR[99]:  failed to find 'chr' predefined function's node in global symtable\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return global_symtab;
    } else {
        dll list = dll_create();
        if (list == NULL){
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return global_symtab;
        }
        search->node_data->args_list = list;
        bintree_node_nullifyinfo(search);
        bintree_init(&search->local_symtab);

        search->node_data->rtype = string_t;

        //insert 'int $i'
        search->local_symtab = bintree_insert(search->local_symtab, 0, "i", variable); //insert the variable into function's local symtab
        var = bintree_search_by_key(search->local_symtab, "i");
        var = bintree_node_nullifyinfo(var);
        var->node_data->init_type = int_t;
        var->node_data->curr_type = int_t;
        dll_insert_last(search->node_data->args_list, 0, "i", "int"); //insert arg name and type

        search->node_data->vars_cnt = 1; //set var count
        search->node_data->arg_cnt = 1; //set arg count
    }

    return global_symtab;
}



/**===========================================================**\
 *                 SEMANTIC IMPORTANT FUNCTIONS                *
 *                                                             *
 * the functions declared below this header serve to handle    *
 * the most important tasks, such as to determine expression   *
 * resulting type, inserting function into global symtable or  *
 * inserting conversion node into AST (to simplify codegen's   *
 * work)                                                       *
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
    bool already_converted = false;
    if (node->type == CONVERT_TYPE){
        node = node->children[0];
    }
    //dbgprint("node->type:  %s", node_type_tostr(node->type));
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
                    fprintf(stderr,"ERROR[5]:  attempt at using an undefined variable detected\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = UNDEF_VAR_ERR;}
                    return ARG_TYPE_ERROR;
                }
            } else {
                fprintf(stderr,"ERROR[99]:  unexpected format of 'EXPR' node: unsupported subtype\n");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                return ARG_TYPE_ERROR;
            }
        } else if (node->children_cnt == 2){ //EXPR with 2 children, has to be a numerical/string operation
            if (node->sub_type == ADD || node->sub_type == SUB || node->sub_type == MUL || node->sub_type == DIV || node->sub_type == IDIV || node->sub_type == STRCAT ||
                node->sub_type == LT || node->sub_type == GT || node->sub_type == LTE || node->sub_type == GTE || node->sub_type == EQ || node->sub_type == NEQ){
                type_l = semantic_get_expr_type(node->children[0], global_symtab); //get type of left node recursively
                type_r = semantic_get_expr_type(node->children[1], global_symtab); //get type of right node recursively

                if (node->children[0]->type == CONVERT_TYPE || node->children[1]->type == CONVERT_TYPE){
                    already_converted = true;
                }

                ///TODO: AST CONVERT_TYPE nodes insertion

                //dbgprint("type_l: %s", bintree_fnc_arg_type_tostr(type_l));
                //dbgprint("type_r: %s", bintree_fnc_arg_type_tostr(type_r));
                //dbgprint("node->sub_type: %s", node_subtype_tostr(node->sub_type));
                //string concatenation
                if (node->sub_type == STRCAT){ //first check if STRCAT
                    if ((type_l == string_t || type_l == nstring_t || type_l == void_t) && (type_r == string_t || type_r == nstring_t || type_r == void_t)){
                        if (type_l == void_t || type_r == void_t){
                            if (already_converted == false){
                                handle_conversions(node, type_l, type_r);
                            }
                        }
                        return string_t;
                    } else {
                        fprintf(stderr,"ERROR[7]:  found a type incompatibility error in an expression\n");
                        if (sem_retcode == SEM_SUCCESS){sem_retcode = INCOMP_TYPES_ERR;}
                        return ARG_TYPE_ERROR;
                    }
                //int/float compatibility + division by zero
                } else if (((type_l == int_t || type_l == nint_t) && (type_r == float_t || type_r == nfloat_t)) || 
                           ((type_l == float_t || type_l == nfloat_t) && (type_r == int_t || type_r == nint_t)) ||
                           ((type_l == int_t || type_l == nint_t) && (type_r == int_t || type_r == nint_t)) ||
                           ((type_l == float_t || type_l == nfloat_t) && (type_r == float_t || type_r == nfloat_t))){ //if one type is int and the other float
                    if (node->sub_type == DIV || node->sub_type == IDIV){ //if dividing, check if right value isn't a zero
                        if (((type_r == int_t || type_r == nint_t) && (atoll(node->children[1]->attrib) == 0)) ||
                            ((type_r == float_t || type_r == nfloat_t) && (atof(node->children[1]->attrib) == 0.0))){
                            if (node->children[1]->sub_type != ID){
                                fprintf(stderr,"ERROR[8]:  found an attempt at division by zero\n");
                                if (sem_retcode == SEM_SUCCESS){sem_retcode = SEM_GENERAL_ERR;}
                                return ARG_TYPE_ERROR;
                            } else {
                                if ((var = bintree_search_by_key(curr_scope_func->local_symtab, node->children[1]->attrib)) != NULL){ //already checked elsewhere anyway
                                    if (var->node_data->is_zero == true){
                                        fprintf(stderr,"ERROR[8]:  found an attempt at division by zero\n");
                                        if (sem_retcode == SEM_SUCCESS){sem_retcode = SEM_GENERAL_ERR;}
                                        return ARG_TYPE_ERROR;
                                    }
                                }
                            }
                        }
                    } //it's not DIV or division by zero, but still, both operands must be of type float
                    if (already_converted == false){
                        handle_conversions(node, type_l, type_r);
                    }
                    if ((type_l == int_t || type_l == nint_t) && (type_r == int_t || type_r == nint_t)){
                        return int_t; //return int
                    } else {
                        return float_t; //return float
                    }
                //conversions (except strings)
                } else if ((type_l == void_t || type_r == void_t) && ((type_l != string_t && type_l != nstring_t) && (type_r != string_t && type_r != nstring_t))){
                    if (type_l == void_t){
                        if (already_converted == false){
                            handle_conversions(node, type_l, type_r);
                        }
                        return type_r;
                    } else {
                        if (already_converted == false){
                            handle_conversions(node, type_l, type_r);
                        }
                        return type_l;
                    }
                //conditionals (need string conversions as well)
                } else if (node->sub_type == LT || node->sub_type == GT || node->sub_type == LTE || node->sub_type == GTE || node->sub_type == EQ || node->sub_type == NEQ){
                    if ((type_l == string_t || type_l == nstring_t || type_l == void_t) && (type_r == string_t || type_r == nstring_t || type_r == void_t)){
                        if (type_l == void_t || type_r == void_t){
                            if (already_converted == false){
                                handle_conversions(node, type_l, type_r);
                            }
                        }
                        return string_t;
                    } else {
                        if (node->sub_type != EQ && node->sub_type != NEQ){
                            fprintf(stderr,"ERROR[7]:  found a type incompatibility error in an expression\n");
                            if (sem_retcode == SEM_SUCCESS){sem_retcode = INCOMP_TYPES_ERR;}
                            return ARG_TYPE_ERROR;
                        }
                    }
                } else {
                    fprintf(stderr,"ERROR[7]:  found a type incompatibility error in an expression\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = INCOMP_TYPES_ERR;}
                    return ARG_TYPE_ERROR;
                }
            }
        } else {
            fprintf(stderr,"ERROR[99]:  unexpected format of 'EXPR' node: children_cnt not 0 or 2\n");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
            return ARG_TYPE_ERROR;
        }
    } else if (node->type == EXPR_FCALL){
        if ((search3 = bintree_search_by_key(global_symtab, node->attrib)) == NULL){
            fprintf(stderr,"ERROR[3]:  found an attempt at calling an undefined function\n");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
            return ARG_TYPE_ERROR;
        }
        return search3->node_data->rtype;
    } else if (node->type == CONVERT_TYPE) {
        ;
    } else {
        fprintf(stderr,"ERROR[99]:  'semantic_get_expr_type' received a non-'EXPR' AST node of type: %s\n", node_type_tostr(node->type));
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
        fprintf(stderr,"ERROR[99]:  in 'global_symtab_insert': function 'bintree_insert' failed inserting function into global symtable\n");
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
                fprintf(stderr,"ERROR[99]:  in 'global_symtab_insert': found non-param node inserted in parameter list in AST\n       Function was not inserted into global tree => wrong behaviour may follow\n");
            }
        }
    } else if (node->children[0]->type == RET_TYPE){ //function with no parameters
        search->node_data->vars_cnt = 0;
        search->node_data->arg_cnt = 0;
        search->node_data->rtype = token_type_to_arg_type(node->children[0]->sub_type);
    } else {
        fprintf(stderr,"ERROR[99]:  in 'global_symtab_insert': function declaration empty or missing sub-nodes\n       Function was not inserted into global tree => wrong behaviour may follow\n"); //!!!todo: error code (but this should be syntax error)
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
                fprintf(stderr,"ERROR[3]: attempt at function redefinition encountered\n"); //!!!
                if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
                return global_symtab;
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

/**
 * AST CONVERSION NODE INSERTION FOR ARITHMETICS AND STRINGS
*/
void handle_conversions(ast_node* parent, arg_type type_l, arg_type type_r){
    if (parent->sub_type == EQ || parent->sub_type == NEQ){ //=== and !== don't have converts
        return;
    }
    if (parent->children_cnt == 1){
        if (parent->children[0]->type == CONVERT_TYPE){
            return; //already was converted
        }
    } else if (parent->children_cnt == 2){
        if ((parent->children[0]->type == CONVERT_TYPE) || (parent->children[1]->type == CONVERT_TYPE)){
            return; //already was converted
        }
    }
    //dbgprint("type_l:  %s",bintree_fnc_arg_type_tostr(type_l));
    //dbgprint("type_r:  %s",bintree_fnc_arg_type_tostr(type_r));
    //if division, both operands have to be converted to float (unless they're both int)
    if (parent->sub_type == DIV || parent->sub_type == IDIV){
        //dbgprint("DIV");
        if ((type_l == int_t || type_l == nint_t) && (type_r == int_t || type_r == nint_t)){
            //dbgprint("IDIV found");
            parent->sub_type = IDIV;
            return;
        }
        if ((type_l != float_t) && (type_l != nfloat_t)){
            ast_node* node_l;
            if (type_l == int_t || type_l == nint_t){
                node_l = node_new(CONVERT_TYPE, INT_TO_FLOAT, NULL);
            } else if (type_l == void_t){
                node_l = node_new(CONVERT_TYPE, NULL_TO_FLOAT, NULL);
            }
            if (node_insert_betwene(parent, node_l, parent->children[0]) == false){
                fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                return; 
            }
        }

        if ((type_r != float_t) && (type_r != nfloat_t)){
            ast_node* node_r;
            if (type_r == int_t || type_r == nint_t){
                node_r = node_new(CONVERT_TYPE, INT_TO_FLOAT, NULL);
            } else if (type_r == void_t){
                node_r = node_new(CONVERT_TYPE, NULL_TO_FLOAT, NULL);
            }
            if (node_insert_betwene(parent, node_r, parent->children[1]) == false){
                fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                return; 
            }
        }
    //if string concatenation, both operands have to be converted to string
    } else if (parent->sub_type == STRCAT){ //anything other than void is STRNUM extension
        //dbgprint("STRCAT");
        if (type_l == void_t){
            ast_node* node_l = node_new(CONVERT_TYPE, NULL_TO_STR, NULL);
            if (node_insert_betwene(parent, node_l, parent->children[0]) == false){
                fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                return; 
            }
        }

        if (type_r == void_t){
            ast_node* node_r = node_new(CONVERT_TYPE, NULL_TO_STR, NULL);
            if (node_insert_betwene(parent, node_r, parent->children[1]) == false){
                fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                return; 
            }
        }
    //if other arithmetic operation, both types have to be of the same type (higher type has priority, float > int > void)
    } else {
        //dbgprint("ARITHMETIC / RELATIONAL");
        if (type_l == void_t){
            if (parent->sub_type == GT || parent->sub_type == LT){ //don't put NULL conversions into '<' or '>'
                return;
            }
            ast_node* node_l;
            if (type_r == int_t || type_r == nint_t){
                node_l = node_new(CONVERT_TYPE, NULL_TO_INT, NULL);
                if (node_insert_betwene(parent, node_l, parent->children[0]) == false){
                    fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                    return; 
                }
            } else if (type_r == float_t || type_r == nfloat_t){
                node_l = node_new(CONVERT_TYPE, NULL_TO_FLOAT, NULL);
                if (node_insert_betwene(parent, node_l, parent->children[0]) == false){
                    fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                    return; 
                }
            } else if (type_r == string_t){
                node_l = node_new(CONVERT_TYPE, NULL_TO_STR, NULL);
                if (node_insert_betwene(parent, node_l, parent->children[0]) == false){
                    fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                    return; 
                }
            }
        } else if (type_l == int_t || type_l == nint_t){
            ast_node* node_l;
            if (type_r == float_t || type_r == nfloat_t){
                node_l = node_new(CONVERT_TYPE, INT_TO_FLOAT, NULL);
                if (node_insert_betwene(parent, node_l, parent->children[0]) == false){
                    fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                    return; 
                }
            }
        }

        if (type_r == void_t){
            if (parent->sub_type == GT || parent->sub_type == LT){ //don't put NULL conversions into '<' or '>'
                return;
            }
            ast_node* node_r;
            if (type_l == int_t || type_l == nint_t){
                node_r = node_new(CONVERT_TYPE, NULL_TO_INT, NULL);
                if (node_insert_betwene(parent, node_r, parent->children[1]) == false){
                    fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                    return; 
                }
            } else if (type_l == float_t || type_l == nfloat_t){
                node_r = node_new(CONVERT_TYPE, NULL_TO_FLOAT, NULL);
                if (node_insert_betwene(parent, node_r, parent->children[1]) == false){
                    fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                    return; 
                }
            } else if (type_l == string_t){
                node_r = node_new(CONVERT_TYPE, NULL_TO_STR, NULL);
                if (node_insert_betwene(parent, node_r, parent->children[1]) == false){
                    fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                    return; 
                }
            }
        } else if (type_r == int_t || type_r == nint_t){
            ast_node* node_r;
            if (type_l == float_t || type_l == nfloat_t){
                node_r = node_new(CONVERT_TYPE, INT_TO_FLOAT, NULL);
                if (node_insert_betwene(parent, node_r, parent->children[1]) == false){
                    fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                    return; 
                }
            }
        }
    }
}

/**
 * AST CONVERSION NODE INSERTION FOR CONDITIONALS
*/
void handle_conversions_conditionals(ast_node* parent, ast_node* child, struct bintree_node* global_symtab){
    arg_type type = semantic_get_expr_type(child, global_symtab);
    if (sem_retcode != SEM_SUCCESS){return;}

    dbgprint("type: %s", bintree_fnc_arg_type_tostr(type));

    ast_node* node;
    if (type == int_t || type == nint_t){
        node = node_new(CONVERT_TYPE, INT_TO_BOOL, NULL);
    } else if (type == float_t || type == nfloat_t){
        node = node_new(CONVERT_TYPE, FLOAT_TO_BOOL, NULL);
    } else if (type == string_t || type == nstring_t){
        node = node_new(CONVERT_TYPE, STR_TO_BOOL, NULL);
    } else if (type == void_t){
        node = node_new(CONVERT_TYPE, NULL_TO_BOOL, NULL);
    } else {
        dbgprint("reeeeeeeeee");
    }

    if (node_insert_betwene(parent, node, parent->children[0]) == false){
        fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return; 
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
        fprintf(stderr,"ERROR:  found assignment with no left or right parameters (e.g. '= expr' or '$var ='\n"); //!!! should be handled by parser
    }
    if (node->children[0]->sub_type != ID){
        fprintf(stderr,"ERROR:  found assignment with its left parameter not being a variable\n"); //!!! should be handled by parser
    }

    //check if everything that's on the right side of the equation can actually be added together
    arg_type type;
    ///NOTE: 'semantic_get_expr_type' already checks for compatibility
    type = semantic_get_expr_type(node->children[1], global_symtab);
    //val = semantic_get_expr_val(node->children[1], global_symtab);
    /**if (type == void_t){ //NULL assignment
        fprintf(stderr,"ERROR[7]:  found NULL assignment");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = INCOMP_TYPES_ERR;}
    }**/
    if (sem_retcode != SEM_SUCCESS){return;}

    //found a legitimate assignment => put variable into symtable if not there yet
    if ((var = bintree_search_by_key(curr_scope_func->local_symtab, node->children[0]->attrib)) == NULL){
        curr_scope_func->local_symtab = bintree_insert(curr_scope_func->local_symtab, curr_scope_func->node_data->vars_cnt++, node->children[0]->attrib, variable);
        var = bintree_search_by_key(curr_scope_func->local_symtab, node->children[0]->attrib);
        var = bintree_node_nullifyinfo(var);
        curr_scope_func->local_symtab->node_data->vars_cnt++; //increment var count
        var->node_data->init_type = type;
        var->node_data->curr_type = type;
        //dbgprint("node->children[1]->attrib:  %s", node->children[1]->attrib);
        //dbgprint("type:                       %s", bintree_fnc_arg_type_tostr(type));
        //char* tmp;
        //float f = strtof(node->children[1]->attrib, &tmp);
        //dbgprint("f:  %.1f", f);
        if (((type == int_t || type == nint_t) && (strcmp(node->children[1]->attrib, "0") == 0)) ||
            ((type == float_t || type == nfloat_t) && ((strcmp(node->children[1]->attrib, "0.0") == 0) || (strcmp(node->children[1]->attrib, "0.00") == 0)))){
            //dbgprint("zero");
            var->node_data->is_zero = true;
        } else {
            //dbgprint("not zero");
            var->node_data->is_zero = false;
        }
    } else {
        //CHECK CONVERSION COMPATIBILITY
        //dbgprint("var->node_data->curr_type: %s", bintree_fnc_arg_type_tostr(var->node_data->curr_type));
        //dbgprint("type:                      %s", bintree_fnc_arg_type_tostr(type));
        /**if (!(var->node_data->curr_type == type || //same types
           (var->node_data->curr_type == void_t) || //var is type void
           ((var->node_data->curr_type == int_t || var->node_data->curr_type == nint_t) && (type == int_t || type == nint_t || type == float_t || type == nfloat_t || type == void_t)) ||          //?int = int/void
           ((var->node_data->curr_type == float_t || var->node_data->curr_type == nfloat_t) && (type == int_t || type == nint_t || type == float_t || type == nfloat_t || type == void_t)) ||    //?float = float/void
           ((var->node_data->curr_type == string_t || var->node_data->curr_type == nstring_t) && (type == string_t || type == nstring_t || type == void_t)))){ //?string = string/void 
            fprintf(stderr,"ERROR[7]:  type incompatibility in assignment");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = INCOMP_TYPES_ERR;}
            return;
        } else {**/
            if (type != void_t){
                var->node_data->curr_type = type;
                //dbgprint("node->children[1]->attrib:  %s", node->children[1]->attrib);
                if ((type == int_t || type == nint_t) && (strcmp(node->children[1]->attrib, "0") == 0)){
                    var->node_data->is_zero = true;
                } else {
                    var->node_data->is_zero = false;
                }
            } else if (node->children[1]->sub_type == VVAL){
                var->node_data->curr_type = void_t;
                var->node_data->is_zero = false;
                /**ast_node* node_r;
                if (var->node_data->curr_type == int_t || var->node_data->curr_type == nint_t){
                    node_r = node_new(CONVERT_TYPE, NULL_TO_INT, NULL);
                } else if (var->node_data->curr_type == float_t || var->node_data->curr_type == nfloat_t){
                    node_r = node_new(CONVERT_TYPE, NULL_TO_FLOAT, NULL);
                } else if (var->node_data->curr_type == string_t || var->node_data->curr_type == nstring_t){
                    node_r = node_new(CONVERT_TYPE, NULL_TO_STR, NULL);
                } else { //should never happen but just in case
                    return;
                }
                if (node_insert_betwene(node, node_r, node->children[1]) == false){
                    fprintf(stderr,"ERROR[99]:  'node_insert_between' failed to insert conversion node");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                    return; 
                }**/
            }
        //}
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
        fprintf(stderr,"ERROR[3]:  found an attempt at calling an undefined function\n");
        if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
        return;
    }

// [5] if function call contains variable parameters, check if they're defined
// [7] check type compatibility of arithmetic, string or relational expressions inside function call
//  └─> THESE ARE BOTH ALREADY HANDLED BY 'semantic_get_expr_type'

// [4] check number/type of parameters while calling a function

    //number of parameters

    if (search->node_data->variadic_func == true){ //if function is variadic, no reason to check number/types of parameters
        return;
    }

    if (node->children_cnt == 0){ //if no parameters passed
        if (search->node_data->arg_cnt != 0){ //check if called function needs any params
            fprintf(stderr,"ERROR[4]:  wrong number of parameters passed in function call\n");
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
            fprintf(stderr,"ERROR[4]:  wrong number of parameters passed in function call\n");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
            return;
        }
    } else {//else AST is wrongly formatted
        fprintf(stderr,"ERROR[99]: wrong format of AST received: in function call, expected node with type 'EXPR_LIST' but %s received instead\n", node_type_tostr(node->children[0]->type));
        if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        return;
    }

    if ((node->children_cnt != search->node_data->arg_cnt) && (node->type == EXPR_LIST)){ //if number of parameters in EXPR_LIST doesn't correspond to the number of parameters required by given function
        if (search->node_data->variadic_func == false){
            fprintf(stderr,"ERROR[4]:  wrong number of parameters passed in function call\n");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
            return;
        }
    }

    //types of parameters

    struct dll_link *ptr = search->node_data->args_list->head; //go to start of function's 'args_list' list
    char tmp[32];
    arg_type type;
    ///NOTE: the following loop will only run for 'EXPR_LIST' nodes
    if (node->type == EXPR_LIST || node->type == EXPR || node->type == EXPR_FCALL){
        for (size_t i = 0; i < search->node_data->arg_cnt; i++){ //repeat until you reach end of function's 'args_list' list
            if (node->children[i]->type == EXPR_PAR){
                ast_node* node2;
                node2 = node->children[i];
                while (node2->children[0]->type == EXPR_PAR){
                    node2 = node2->children[0];
                }

                if (node2->children[0]->type == EXPR_FCALL){ //if passed parameter is a function call
                    dbgprint("9.9");
                    if ((search2 = bintree_search_by_key(global_symtab, node2->children[0]->attrib)) == NULL){ //check if called function exists
                        fprintf(stderr,"ERROR[3]:  found an attempt at calling an undefined function\n");
                        if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
                        return;
                    }
                    arg_type type = semantic_get_expr_type(node2->children[0], global_symtab); //get its rettype (also has imbedded fcall checking)
                    if (sem_retcode != SEM_SUCCESS){return;}
                    //dbgprint("bintree_fnc_arg_type_tostr(type):  %s",bintree_fnc_arg_type_tostr(type));
                    //dbgprint("ptr->linkData->type:               %s",ptr->linkData->type);
                    if (strcmp(bintree_fnc_arg_type_tostr(type), ptr->linkData->type) != 0){ //should also handle nullables? (?int, ?string,...)
                        fprintf(stderr,"ERROR[4]:  wrong types of parameters passed in function call\n");
                        if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
                        return;
                    }
                } else if (node2->children[0]->type == EXPR){
                    type = semantic_get_expr_type(node2->children[0], global_symtab);
                    strncpy(tmp, bintree_fnc_arg_type_tostr(type), sizeof(tmp));
                    if (sem_retcode != 0){return;}
                    //dbgprint("?type: %s", bintree_fnc_arg_type_tostr(type));
                    //dbgprint("?tmp:  %s", tmp);

                    /**INFORUN(
                        const char* type = node_type_tostr(node->children[i]->type);
                        const char* sub_type = node_subtype_tostr(node->children[i]->sub_type);
                        fprintf(stdout,"![attrib]%-10s:  [type]%-10s  [subtype]%-10s\n", node->children[i]->attrib, type, sub_type);
                    );**/
                    //dbgprint("bintree_fnc_arg_type_tostr(type):  %s",bintree_fnc_arg_type_tostr(type));
                    //dbgprint("tmp:                               %s",tmp);
                    //dbgprint("ptr->linkData->type:               %s",ptr->linkData->type);
                    if (strcmp(tmp, ptr->linkData->type) != 0){ //if passed parameter is 'int', 'float' or 'string' (+ their nullable versions)
                        if (!(((strcmp(tmp, "void") == 0) && (strcmp(ptr->linkData->type, "?int") == 0 ||
                                                            strcmp(ptr->linkData->type, "?float") == 0 ||
                                                            strcmp(ptr->linkData->type, "?string") == 0)) ||
                            (((strcmp(tmp, "int") == 0) || (strcmp(tmp, "?int") == 0)) && ((strcmp(ptr->linkData->type, "int") == 0) || (strcmp(ptr->linkData->type, "?int") == 0))) ||
                            (((strcmp(tmp, "float") == 0) || (strcmp(tmp, "?float") == 0)) && ((strcmp(ptr->linkData->type, "float") == 0) || (strcmp(ptr->linkData->type, "?float") == 0))) ||
                            (((strcmp(tmp, "string") == 0) || (strcmp(tmp, "?string") == 0)) && ((strcmp(ptr->linkData->type, "string") == 0) || (strcmp(ptr->linkData->type, "?string") == 0))))){
                            fprintf(stderr,"ERROR[4]:  encountered wrong type of parameter passed in function call\n");
                            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
                            return;
                        }
                    }
                    //dbgprint("?type: %s", bintree_fnc_arg_type_tostr(type));
                    //dbgprint("?tmp:  %s", tmp);
                }
            }
            /**INFORUN(
                fprintf(stderr,"-- node->children[i]->type:    %s\n", node_type_tostr(node->children[i]->type));
                fprintf(stderr,"-- node->children[i]->attrib:  %s\n", node->children[i]->attrib);
                fprintf(stderr,"-- node->children_cnt:         %ld\n", node->children_cnt);
                fprintf(stderr,"-- search->node_data->arg_cnt: %ld\n", search->node_data->arg_cnt);
            );**/
            if (node->children[i]->type == EXPR_FCALL){ //if passed parameter is a function call
                if ((search2 = bintree_search_by_key(global_symtab, node->children[i]->attrib)) == NULL){ //check if called function exists
                    fprintf(stderr,"ERROR[3]:  found an attempt at calling an undefined function\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
                    return;
                }
                arg_type type = semantic_get_expr_type(node->children[i], global_symtab); //get its rettype (also has imbedded fcall checking)
                if (sem_retcode != SEM_SUCCESS){return;}
                //dbgprint("bintree_fnc_arg_type_tostr(type):  %s",bintree_fnc_arg_type_tostr(type));
                //dbgprint("ptr->linkData->type:               %s",ptr->linkData->type);
                if (strcmp(bintree_fnc_arg_type_tostr(type), ptr->linkData->type) != 0){ //should also handle nullables? (?int, ?string,...)
                    fprintf(stderr,"ERROR[4]:  wrong types of parameters passed in function call\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
                    return;
                }
            } else if (node->children[i]->type == EXPR){
                type = semantic_get_expr_type(node->children[i], global_symtab);
                strncpy(tmp, bintree_fnc_arg_type_tostr(type), sizeof(tmp));
                if (sem_retcode != 0){return;}
                //dbgprint("?type: %s", bintree_fnc_arg_type_tostr(type));
                //dbgprint("?tmp:  %s", tmp);

                /**INFORUN(
                    const char* type = node_type_tostr(node->children[i]->type);
                    const char* sub_type = node_subtype_tostr(node->children[i]->sub_type);
                    fprintf(stdout,"![attrib]%-10s:  [type]%-10s  [subtype]%-10s\n", node->children[i]->attrib, type, sub_type);
                );**/
                //dbgprint("bintree_fnc_arg_type_tostr(type):  %s",bintree_fnc_arg_type_tostr(type));
                //dbgprint("tmp:                               %s",tmp);
                //dbgprint("ptr->linkData->type:               %s",ptr->linkData->type);
                if (strcmp(tmp, ptr->linkData->type) != 0){ //if passed parameter is 'int', 'float' or 'string' (+ their nullable versions)
                    if (!(((strcmp(tmp, "void") == 0) && (strcmp(ptr->linkData->type, "?int") == 0 ||
                                                        strcmp(ptr->linkData->type, "?float") == 0 ||
                                                        strcmp(ptr->linkData->type, "?string") == 0)) ||
                        (((strcmp(tmp, "int") == 0) || (strcmp(tmp, "?int") == 0)) && ((strcmp(ptr->linkData->type, "int") == 0) || (strcmp(ptr->linkData->type, "?int") == 0))) ||
                        (((strcmp(tmp, "float") == 0) || (strcmp(tmp, "?float") == 0)) && ((strcmp(ptr->linkData->type, "float") == 0) || (strcmp(ptr->linkData->type, "?float") == 0))) ||
                        (((strcmp(tmp, "string") == 0) || (strcmp(tmp, "?string") == 0)) && ((strcmp(ptr->linkData->type, "string") == 0) || (strcmp(ptr->linkData->type, "?string") == 0))))){
                        fprintf(stderr,"ERROR[4]:  encountered wrong type of parameter passed in function call\n");
                        if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
                        return;
                    }
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
        type = semantic_get_expr_type(node, global_symtab);
        strncpy(tmp, bintree_fnc_arg_type_tostr(type), sizeof(tmp));
        if (sem_retcode != SEM_SUCCESS){return;}
        ptr = search->node_data->args_list->tail; //go to end of function's 'args_list' list
        if (strcmp(tmp, ptr->linkData->type) != 0){
            if (!(((strcmp(tmp, "void") == 0) && (strcmp(ptr->linkData->type, "?int") == 0 ||
                                                  strcmp(ptr->linkData->type, "?float") == 0 ||
                                                  strcmp(ptr->linkData->type, "?string") == 0)) ||
                  (((strcmp(tmp, "int") == 0) || (strcmp(tmp, "?int") == 0)) && ((strcmp(ptr->linkData->type, "int") == 0) || (strcmp(ptr->linkData->type, "?int") == 0))) ||
                  (((strcmp(tmp, "float") == 0) || (strcmp(tmp, "?float") == 0)) && ((strcmp(ptr->linkData->type, "float") == 0) || (strcmp(ptr->linkData->type, "?float") == 0))) ||
                  (((strcmp(tmp, "string") == 0) || (strcmp(tmp, "?string") == 0)) && ((strcmp(ptr->linkData->type, "string") == 0) || (strcmp(ptr->linkData->type, "?string") == 0))))){
                fprintf(stderr,"ERROR[4]:  encountered wrong type of parameter passed in function call\n");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
                return;
            }
        }
    } else if (node->type == EXPR_LIST){ //check the tail as well
        type = semantic_get_expr_type(node->children[node->children_cnt-1], global_symtab);
        strncpy(tmp, bintree_fnc_arg_type_tostr(type), sizeof(tmp));
        if (sem_retcode != SEM_SUCCESS){return;}
        ptr = search->node_data->args_list->tail; //go to end of function's 'args_list' list
        /**dbgprint("node->children[node->children_cnt-1]-attrib: %s", node->children[node->children_cnt-1]->attrib);
        dbgprint("node->children[node->children_cnt-1]->type:  %s", node_subtype_tostr(node->children[node->children_cnt-1]->sub_type));
        dbgprint("tmp:                  %s", tmp);
        dbgprint("ptr->linkData->type:  %s", ptr->linkData->type);
        dbgprint("arg_list:             ");
        dll_print_forwards(search->node_data->args_list);
        fprintf(stdout,"\n");**/

        if (strcmp(tmp, ptr->linkData->type) != 0){
            if (!(((strcmp(tmp, "void") == 0) && (strcmp(ptr->linkData->type, "?int") == 0 ||
                                                  strcmp(ptr->linkData->type, "?float") == 0 ||
                                                  strcmp(ptr->linkData->type, "?string") == 0)) ||
                  (((strcmp(tmp, "int") == 0) || (strcmp(tmp, "?int") == 0)) && ((strcmp(ptr->linkData->type, "int") == 0) || (strcmp(ptr->linkData->type, "?int") == 0))) ||
                  (((strcmp(tmp, "float") == 0) || (strcmp(tmp, "?float") == 0)) && ((strcmp(ptr->linkData->type, "float") == 0) || (strcmp(ptr->linkData->type, "?float") == 0))) ||
                  (((strcmp(tmp, "string") == 0) || (strcmp(tmp, "?string") == 0)) && ((strcmp(ptr->linkData->type, "string") == 0) || (strcmp(ptr->linkData->type, "?string") == 0))))){
                fprintf(stderr,"ERROR[4]:  encountered wrong type of parameter passed in function call %s\n",search->node_data->key);
                if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
                return;
            }
        }
    } else if (node->type == EXPR_FCALL) {
        if ((search2 = bintree_search_by_key(global_symtab, node->attrib)) == NULL){ //check if called function exists
            fprintf(stderr,"ERROR[3]:  found an attempt at calling an undefined function\n");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_DEF_ERR;}
            return;
        }
        //arg_type = semantic_get_expr_type();
        arg_type type = semantic_get_expr_type(node, global_symtab);
        if (sem_retcode != SEM_SUCCESS){return;}
        dbgprint("!!!!!!!!!!!!!!!!!!!!!!!!")
        if (search2->node_data->rtype != type){ //should also handle nullables? (?int, ?string,...)
            fprintf(stderr,"ERROR[4]:  wrong types of parameters passed in function call\n");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
            return;
        }
        return;
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
        fprintf(stderr,"ERROR[99]  'semantic_check_expr' received AST node which wasn't of type 'EXPR'");
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
                fprintf(stderr,"ERROR[6]:  found empty return inside a function not defined with 'void' return type\n");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_RET_EXPR_ERR;}
            }
        } else if (node->children_cnt == 1){ //return with expression (e.g. [return 1;] or [return $b+foo($b);])
            if (curr_scope_func->node_data->rtype == void_t){
                if ((strcmp(curr_scope_func->node_data->key, ":b") != 0) && (node->children[0]->sub_type != VVAL)){ //if it's not main body and NULL
                    fprintf(stderr,"ERROR[6]:  found non-empty return inside a function defined with 'void' return type\n");
                    if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_RET_EXPR_ERR;}
                    return;
                }
            } else {
                arg_type type = semantic_get_expr_type(node->children[0], global_symtab); //get type of return expr
                if (sem_retcode != SEM_SUCCESS){return;}
                if (type != curr_scope_func->node_data->rtype){ //if type of expr doesn't match function's defined return type
                    //!!!if return type is int and function defined as returning float, should be compatible (I guess?)
                    if (!((type == void_t && (curr_scope_func->node_data->rtype == nfloat_t || 
                                              curr_scope_func->node_data->rtype == nint_t || 
                                              curr_scope_func->node_data->rtype == nstring_t ||
                                              curr_scope_func->node_data->rtype == void_t)) ||    //void + ?float,?int,?string,void
                          (type == int_t && curr_scope_func->node_data->rtype == nint_t) ||       //int + ?int
                          (type == float_t && curr_scope_func->node_data->rtype == nfloat_t) ||   //float + ?float
                          (type == string_t && curr_scope_func->node_data->rtype == nstring_t))){ //string + ?string
                        fprintf(stderr,"ERROR[4]:  wrong return type encountered\n");
                        if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_CALL_OR_RETTYPE_ERR;}
                    } 
                }
            }
        } else {
            fprintf(stderr,"ERROR[99]:  received 'RETURN' node in AST with more than 1 child nodes\n");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
        }
    }/** else {
        fprintf(stderr,"ERROR[99]  'semantic_check_return' received AST node which wasn't of type 'RETURN_N'");
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
    ast_node* parent = node;
    node = node->children[0]; //dive into relational statement
    while (node->type == EXPR_PAR){ //get rid of EXPR_PAR nodes
        node = node->children[0];
    }
    if (node->children_cnt == 0){
        semantic_get_expr_type(node, global_symtab); //this is here basically just because it already has fun and var def checks
        if (sem_retcode != SEM_SUCCESS){return;}
        handle_conversions_conditionals(parent, node, global_symtab);
    } else if (node->children_cnt == 2){
        arg_type type_l, type_r;
        ///NOTE: 'semantic_get_expr_type' already checks for compatibility
        type_l = semantic_get_expr_type(node->children[0], global_symtab);
        if (sem_retcode != SEM_SUCCESS){return;}
        type_r = semantic_get_expr_type(node->children[1], global_symtab);
        if (sem_retcode != SEM_SUCCESS){return;}

        //dbgprint("type_l:  %s",bintree_fnc_arg_type_tostr(type_l));
        //dbgprint("type_r:  %s",bintree_fnc_arg_type_tostr(type_r));

        if (!((type_l == type_r) || //same
              ((type_l == int_t || type_l == nint_t) && (type_r == float_t || type_r == nfloat_t)) ||     //int and float
              ((type_l == float_t || type_l == nfloat_t) && (type_r == int_t || type_r == nint_t)) ||     //float and int
              ((type_l == int_t || type_l == nint_t) && (type_r == int_t || type_r == nint_t)) ||         //int/?int and int/?int
              ((type_l == float_t || type_l == nfloat_t) && (type_r == float_t || type_r == nfloat_t)) || //float/?float and float/?float
              ((type_l == string_t || type_l == nstring_t) && (type_r == string_t || type_r == nstring_t)) || //string/?string and string/?string
              (type_l == void_t || type_r == void_t))){ //void
            if (node->sub_type != EQ && node->sub_type != NEQ){
                fprintf(stderr,"ERROR[7]:  found type incompatibility in a(n) %s conditional\n", node_type_tostr(node->type));
                if (sem_retcode == SEM_SUCCESS){sem_retcode = INCOMP_TYPES_ERR;}
                return;
            }
        } else {
            if (node->sub_type == ADD || node->sub_type == SUB || node->sub_type == MUL || node->sub_type == DIV || node->sub_type == IDIV || node->sub_type == STRCAT){
                handle_conversions_conditionals(parent, node, global_symtab);
            } else {
                handle_conversions(node, type_l, type_r);
            }
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
                fprintf(stderr,"ERROR[99]:  received IF_N AST node with only one child\n");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                break;
            }
            //checks ONLY the relational statement
            semantic_check_conditionals(AST, global_symtab);
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
                fprintf(stderr,"ERROR[99]:  received IF_N AST node with only one child\n");
                if (sem_retcode == SEM_SUCCESS){sem_retcode = ERR_INTERNAL;}
                break;
            }
            //checks ONLY the relational statement
            semantic_check_conditionals(AST, global_symtab);
            if (sem_retcode != SEM_SUCCESS){break;}
            //now check body
            AST_DF_traversal(AST->children[1], global_symtab);
            break;
        default: 
            break;
    }

    for(size_t i = 0; i < AST->children_cnt; i++){
        if(AST->children[i] != NULL && sem_retcode == SEM_SUCCESS){
            AST_DF_traversal(AST->children[i], global_symtab);
            if (AST->children[i]->type == RETURN_N){
                if (return_encountered == false){
                    return_encountered = true;
                }
                //destroy everything after RETURN_N
                INFORUN(
                    fprintf(stderr, "trimming: ");
                    node_print(stderr, AST);
                    fprintf(stderr, "  after RETURN_N received\n");
                );
                for (size_t j = i+1; j < AST->children_cnt;){
                    if (AST->children[j] != NULL && AST->children[j]->type != FDEF){
                        node_remove_child(AST, AST->children[j]);
                    } else {
                        ++j;
                    }
                }
            }
        }
    }
    if (AST->type == FDEF){ //move current scope back to main
        search4 = bintree_search_by_key(global_symtab, AST->attrib); //at this point, this has been checked enough times
        if (search4->node_data->rtype != void_t && return_encountered == false){
            fprintf(stderr,"ERROR[6]:  missing return in a function returning non-void\n");
            if (sem_retcode == SEM_SUCCESS){sem_retcode = FUNC_RET_EXPR_ERR;}
            return;
        }
        return_encountered = false;

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
//create predefined funcs
    create_predef_funcs(cont->global_symtab);
//check if inserted and set curr scope to main body
    if ((curr_scope_func = bintree_search_by_key(cont->global_symtab, ":b")) == NULL){
        fprintf(stderr,"ERROR[99]:  failed to find ':b' (aka main) node in global symtable\n");
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
    if (sem_retcode != SEM_SUCCESS){ return sem_retcode; } //if error occured during first traversal (afaik, only error #3 can occur here)
//DEBUG: print out main body symtable and global symtable
    INFORUN(
        fprintf(stdout,"--------------------------------------------------\n");
        bintree_inorder_fullprint(cont->global_symtab, false);
        fprintf(stdout,"--------------------------------------------------\n");
    );
//main AST traversal
    AST_DF_traversal(cont->root, cont->global_symtab);
    if (sem_retcode != SEM_SUCCESS){ return sem_retcode; } //if error occured

    INFORUN(
        fprintf(stdout,"--------------------------------------------------\n");
        bintree_inorder_currvarsonly(curr_scope_func->local_symtab);
        fprintf(stdout,"--------------------------------------------------\n");
    );

    return sem_retcode;
}