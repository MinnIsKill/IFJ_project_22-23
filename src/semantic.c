/**
 * @file semantic.c
 * @author Vojtěch Kališ (xkalis03)
 * @brief semantic analyzer implementation
 *        receives an AST passed by parser as input, walks through it to evaluate semantic
 *        correctness, and if successful, passes the AST to code generator
 *
 * @date of last update:   18th October 2022
**/

// ./compiler <./ast_examples/prog.php

#include "semantic.h"

int sem_retcode = SEM_SUCCESS;
bintree_node prog_body_fnc; //prog_body
bintree_node search;
bintree_node var;
char curr_scope_func[64];

//ID gen
static size_t id = 0;
size_t generate_id(){
    return (id++);
}

//(almost)breadth-first print of AST
void AST_dotprint(FILE* f, ast_node* root){
    if (root == NULL){
        return;
    }

    fprintf(f,"--------------------START OF AST PRINT--------------------\n");
    AST_dotprint_internal(f, root);
    fprintf(f,"---------------------END OF AST PRINT---------------------\n");
}
void AST_dotprint_internal(FILE* f, ast_node* root){
    ast_node *tmp;
    
    if (root->children_cnt != 0){
        for (size_t i = 0; i < root->children_cnt; i++){
            tmp = root->children[i];
            if (i != root->children_cnt){
                fprintf(f,"%lld -> %lld\n",root->id,tmp->id);
            }
        }
        for (size_t k = 0; k < root->children_cnt; k++){
            tmp = root->children[k];
            AST_dotprint_internal(f, tmp);
        }
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
        case INVALID: return "INVALID"; // this token signify error
        case STOP:    return "STOP";    // $ used by operator precedence parser
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

/**
 * 
*/
struct bintree_node* global_symtab_funcinsert(ast_node* node, struct bintree_node* global_symtab){
    global_symtab = bintree_insert(global_symtab, generate_id(), node->attrib, function);
    search = bintree_search_by_key(global_symtab, node->attrib);
    dll list = dll_create();
    search->node_data->args_list = list;
    search = bintree_node_nullifyinfo(search);
    bintree_init(&search->local_symtab);

    if (node->children[0]->type == PAR_LIST){ //!!!assuming parameter list is always the first child
        search->node_data->rtype = token_type_to_arg_type(node->children[1]->sub_type);
        node = node->children[0]; //go to parameter list
        //fill out "search->node_data->arg_names_list" && "search->node_data->arg_type_list"
        for (size_t i = 0; i < node->children_cnt; i++){
            if (node->children[i]->type == PARAM){
                arg_type type = token_type_to_arg_type(node->children[i]->sub_type);
                //bintree_fnc_arg_type_tostr();
                char tmp[32];
                strcpy(tmp, bintree_fnc_arg_type_tostr(type));
                search->local_symtab = bintree_insert(search->local_symtab, search->node_data->arg_cnt, node->children[i]->attrib, variable); //insert the variable into function's local symtab
                var = bintree_search_by_key(search->local_symtab, node->children[i]->attrib);
                var = bintree_node_nullifyinfo(var);
                var->node_data->type = type;
                dll_insert_last(search->node_data->args_list, search->node_data->arg_cnt, node->children[i]->attrib, tmp); //insert arg name and type
                //search->local_symtab->node_data->arg_type_list[search->node_data->arg_cnt] = token_type_to_arg_type(node->children[i]->sub_type); //insert arg type
                search->node_data->arg_cnt++; //increment arg count
            } else {
                dbgprint("ERROR:  found non-param node inserted in parameter list in AST\n        (param won't be inserted into function's local symtab)");
            }
        }
    } else if (node->children[0]->type == RET_TYPE){ //function with no parameters
        search->node_data->arg_cnt = 0;
        search->node_data->rtype = token_type_to_arg_type(node->children[0]->sub_type);
    } else {
        dbgprint("ERROR: function declaration HAS TO contain return type specification\n       (for now, only this message is diplayed)"); //!!!todo: error code (but this should be syntax error)
    }

    return global_symtab;
}

/**
 * @brief: depth-first traversal of AST ran at start of semantic checking to search for function declarations
 * @errcodes: 3 - attempt at function redefinition
*/
struct bintree_node* AST_DF_firsttraversal(ast_node* AST, struct bintree_node* global_symtab){
    if (AST == NULL || sem_retcode != 0){
        return global_symtab;
    }

    switch (AST->type){
    //function definition encountered
        case FDEF:
            if ((search = bintree_search_by_key(global_symtab, AST->attrib)) != NULL){ //check if function was already defined
                dbgprint("ERROR[3]: attempt at function redefinition encountered"); //!!!
                sem_retcode = FUNC_DEF_ERR;
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
        fprintf(stdout,"checking token with  type:\"%s\"\n              and subtype:\"%s\"\n", type, sub_type);
    );

    for(size_t i = 0; i < AST->children_cnt; i++){
        if(AST->children[i] != NULL && sem_retcode == 0){
            global_symtab = AST_DF_firsttraversal(AST->children[i], global_symtab);
        }
    }

    return global_symtab;
}





/**
 * RETURN CHECKING
 * @errcodes: 4 - wrong return type
 *            5 - attempt at using an undefined variable
 *            6 - missing or excessive expression in return
 *            7 - type incompatibility in arithmetic, string or relational expressions
 *            
*/
/**void semantic_check_return(ast_node* node, struct bintree_node* global_symtab){

    return;
}**/

/**
 * FUNCTION CALL CHECKING
 * @errcodes: 3 - undefined function
 *            4 - wrong number/type of parameters passed while calling a function
 *            5 - attempt at using an undefined variable
 *            7 - type incompatibility in arithmetic, string or relational expressions
 *            
*/
int semantic_check_fcall(ast_node* node, struct bintree_node* global_symtab){
    if ((search = bintree_search_by_key(global_symtab, node->attrib)) == NULL){
        dbgprint("ERROR[3]:  found an attempt at calling an undefined function");
        return FUNC_DEF_ERR;
    }

    return SEM_SUCCESS;
}

/**
 * ASSIGNMENT CHECKING
 * @errcodes: 5 - attempt at using an undefined variable
 *            7 - type incompatibility in arithmetic, string or relational expressions
 *            8 - general (division by zero, ...)
*/
int semantic_check_assign(ast_node* node, struct bintree_node* global_symtab){
    if (node->children_cnt < 2){
        dbgprint("ERROR:  found assignment with no left or right parameters (e.g. '= expr' or '$var ='"); //!!! should be handled by parser
    }
    if (node->children[0]->sub_type != ID){
        dbgprint("ERROR:  found assignment with its left parameter not being a variable"); //!!! should be handled by parser
    }
    if (bintree_search_by_key(global_symtab, node->children[0]->attrib) == NULL){

    }
    return SEM_SUCCESS;
}

//depth-first traversal of AST ran as second (and main) traversal
void AST_DF_traversal(ast_node* AST, struct bintree_node* global_symtab){
    if (AST == NULL || sem_retcode != 0){
        return;
    }

    INFORUN(
        const char* type = node_type_tostr(AST->type);
        const char* sub_type = node_subtype_tostr(AST->sub_type);
        fprintf(stdout,"checking token with  type:\"%s\"\n              and subtype:\"%s\"\n", type, sub_type);
    );

    switch (AST->type){
    //assignment encountered
        case EXPR_ASSIGN:
            sem_retcode = semantic_check_assign(AST, global_symtab);
            break;
        case EXPR_FCALL:
            sem_retcode = semantic_check_fcall(AST, global_symtab);
            break;
        default: 
            break;
    }

    for(size_t i = 0; i < AST->children_cnt; i++){
        if(AST->children[i] != NULL && sem_retcode == 0){
            AST_DF_traversal(AST->children[i], global_symtab);
        }
    }
}

/**
 * SEMANTIC START
*/
int semantic(context* cont){
    INFORUN(
        fprintf(stdout,"\n  __  _               ___ ___  _  __ \n");
        fprintf(stdout," (_  |_ |\\/|  /\\  |\\ | |   |  /  (_  \n");
        fprintf(stdout," __) |_ |  | /--\\ | \\| |  _|_ \\_ __) \n\n");
    );
    bintree_init(&cont->global_symtab);
    cont->global_symtab = bintree_insert(cont->global_symtab, 0, "prog_body", prog_b);
    if ((prog_body_fnc = bintree_search_by_key_withprogb(cont->global_symtab, "prog_body")) == NULL){
        dbgprint("ERROR:  failed to insert 'prog_body_fnc' (aka main) node into global symtable");
        return ERR_INTERNAL;
    }
    bintree_node_nullifyinfo(prog_body_fnc);
    bintree_init(&prog_body_fnc->local_symtab);

//forward function declarations into global symtable
    prog_body_fnc->local_symtab = AST_DF_firsttraversal(cont->root, prog_body_fnc->local_symtab);
    if (sem_retcode != 0){ return sem_retcode; } //if error occured during first traversal (afaik, only error #3 can occur here)
//DEBUG: print out global symtable
    INFORUN(
        fprintf(stdout,"--------------------------------------------------\n");
        bintree_inorder_fullprint(cont->global_symtab, false);
        fprintf(stdout,"--------------------------------------------------\n");
    );
//main AST traversal
    AST_DF_traversal(cont->root, cont->global_symtab);
    if (sem_retcode != 0){ return sem_retcode; } //if error occured

    return sem_retcode;
}