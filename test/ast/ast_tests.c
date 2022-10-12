#include"./../../src/ast.h"

#include<stdio.h>
#include<stdarg.h>
#include<stddef.h>
#include<setjmp.h>

#include<cmocka.h>

int skip_mock = 0;//dirty hack to make gcov work
// since gcov needs alloc functions or strdup
// i need to block their mocking functionality
// after all unit tests

extern void* __real_malloc(size_t);
void* __wrap_malloc(size_t n)
{
    if(skip_mock == 1) return(__real_malloc(n));
    int m = (int)mock();
    if(m == 0)
    {
        return(NULL);
    }
    else
    {
        return(__real_malloc(n));
    }
}

extern void* __real_realloc(void*,size_t);
void* __wrap_realloc(void* ptr, size_t n)
{
    if(skip_mock == 1) return(__real_realloc(ptr,n));
    int m = (int)mock();
    if(m == 0)
    {
        return(NULL);
    }
    else
    {
        return(__real_realloc(ptr,n));
    }
}

extern char* __real_strdup(char* ptr);
char* __wrap_strdup(char* ptr)
{
    if(skip_mock == 1) return(__real_strdup(ptr));
    int m = (int)mock();
    if(m == 0)
    {
        return(NULL);
    }
    else
    {
        return(__real_strdup(ptr));
    }
}

/**
 *  test if memory for new node is allocated corectly
 *  test if structure members are initialized correctly
 *  test if nodes are deleted corectly
 **/
static void test_node_new(void** state)
{
    (void)state;
    
    //attrib
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* n = NULL;
    n = node_new(TERM,ID,"$abc");
    assert_non_null(n);
    assert_null(n->children);
    assert_int_equal(n->children_cnt,0);
    assert_int_equal(n->type,TERM);
    assert_int_equal(n->sub_type,ID);
    assert_int_equal(n->id,0);
    assert_string_equal(n->attrib,"$abc");
    
    //empty attrib
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* n1 = NULL;
    n1 = node_new(TERM,ADD,"");
    assert_non_null(n1);
    assert_null(n1->children);
    assert_int_equal(n1->children_cnt,0);
    assert_int_equal(n1->type,TERM);
    assert_int_equal(n1->sub_type,ADD);
    assert_int_equal(n1->id,1);
    assert_string_equal(n1->attrib,"");
    
    //attrib NULL
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* n2 = node_new(EXPR,ADD,NULL);
    assert_non_null(n2);
    assert_null(n2->children);
    assert_int_equal(n2->children_cnt,0);
    assert_int_equal(n2->type,EXPR);
    assert_int_equal(n2->sub_type,ADD);
    assert_int_equal(n2->id,2);
    assert_string_equal(n2->attrib,"");
    
    node_delete(&n);
    assert_null(n);

    node_delete(&n1);
    assert_null(n1);
    
    node_delete(&n2);
    assert_null(n2);
}

/**
 *  test what happen if allocations fails
 *  check valgrind!
 **/
static void test_node_new_fails(void** state)
{
    (void)state;
    
    // when everithing faild node should be cleaned
    // automaticly and no leaks should be possible

    // fail alloc 
    will_return(__wrap_malloc,0);
    ast_node* n = NULL;
    n = node_new(TERM,ID,"$foo");
    assert_null(n);
    
    // fail strdup
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,0);
    n = node_new(TERM,FVAL,"3.5");
    assert_null(n);
    
    // fail strdup with null str pointer
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,0);
    n = node_new(TERM,SEMIC,NULL);
    assert_null(n);
    
    // fail strdup with empty str
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,0);
    n = node_new(TERM,EOS,"");
    assert_null(n);
}

static void test_node_delete_invalids(void** state)
{
    ast_node* n = NULL;
    node_delete(&n);
    node_delete(NULL);
}

/**
 * test if node are added corectly to each other
 **/
static void test_node_add(void** state)
{
    (void)state;
    
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* root = NULL;
    root = node_new(EXPR,ADD,"");
    assert_non_null(root);
    // id counter keeps counting in every test, so manual reset
    root->id = 0;
    
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* left = NULL;
    left = node_new(TERM,ID,"$a");
    assert_non_null(left);
    // id counter keeps counting in every test, so manual reset
    left->id = 1;
    
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* right = NULL;
    right = node_new(TERM,IVAL,"1");
    assert_non_null(right);
    // id counter keeps counting in every test, so manual reset
    right->id = 2;
    
    assert_int_equal(root->children_cnt,0);
    assert_null(root->children);
    
    // check if adding node will change parent
    will_return(__wrap_realloc,1);
    assert_true(node_add(root,left));
    assert_int_equal(root->children_cnt,1);
    assert_int_equal(root->id,0);
    assert_int_equal(root->type,EXPR);
    assert_int_equal(root->sub_type,ADD);
    assert_string_equal(root->attrib,"");
    assert_non_null(root->children);

    // check if adding multiple node will change parent
    will_return(__wrap_realloc,1);
    assert_true(node_add(root,right));
    assert_int_equal(root->children_cnt,2);
    assert_int_equal(root->id,0);
    assert_int_equal(root->type,EXPR);
    assert_int_equal(root->sub_type,ADD);
    assert_string_equal(root->attrib,"");
    assert_non_null(root->children);

    //test if first added nodes has changed
    assert_non_null(root->children[0]);
    assert_null(root->children[0]->children);
    assert_int_equal(root->children[0]->children_cnt,0);
    assert_int_equal(root->children[0]->type,TERM);
    assert_int_equal(root->children[0]->sub_type,ID);
    assert_int_equal(root->children[0]->id,1);
    assert_string_equal(root->children[0]->attrib,"$a");
    
    //test if second added nodes has changed
    assert_non_null(root->children[1]);
    assert_null(root->children[1]->children);
    assert_int_equal(root->children[1]->children_cnt,0);
    assert_int_equal(root->children[1]->type,TERM);
    assert_int_equal(root->children[1]->sub_type,IVAL);
    assert_int_equal(root->children[1]->id,2);
    assert_string_equal(root->children[1]->attrib,"1");

    node_delete(&root);
}

/**
 * test what happen whe node_add fails to allocate memory
 **/
static void test_node_add_fails(void** state)
{
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* root = NULL;
    root = node_new(EXPR,ADD,"");
    assert_non_null(root);
    // id counter keeps counting in every test, so manual reset
    root->id = 0;

    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* left = NULL;
    left = node_new(TERM,ID,"$a");
    assert_non_null(left);
    // id counter keeps counting in every test, so manual reset
    left->id = 1;
    
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* right = NULL;
    right = node_new(TERM,IVAL,"1");
    assert_non_null(right);
    // id counter keeps counting in every test, so manual reset
    right->id = 2;

    assert_false(node_add(NULL,left));
    
    // test what happen if realloc failed
    will_return(__wrap_realloc,0);
    assert_false(node_add(root,left));
    assert_int_equal(root->children_cnt,0);
    assert_null(root->children);
    
    // add one children for next test
    will_return(__wrap_realloc,1);
    assert_true(node_add(root,left));
    assert_int_equal(root->children_cnt,1);
    assert_non_null(root->children);
    
    // test if failed realloc will not overwtrite
    // already added children
    will_return(__wrap_realloc,0);
    assert_false(node_add(root,right));
    assert_int_equal(root->children_cnt,1);
    assert_non_null(root->children);
    
    // test children was added correctly
    assert_non_null(root->children[0]);
    assert_null(root->children[0]->children);
    assert_int_equal(root->children[0]->children_cnt,0);
    assert_int_equal(root->children[0]->type,TERM);
    assert_int_equal(root->children[0]->sub_type,ID);
    assert_int_equal(root->children[0]->id,1);
    assert_string_equal(root->children[0]->attrib,"$a");
    
    node_delete(&root);

    node_delete(&right); // this node was never added to root
}

/**
 * probably useles test
 * testing if node_type enum is in-sync with node_type_str()
 **/
static void test_node_type_str(void** state)
{
    assert_string_equal(node_type_str(TERM),"terminal");
    assert_string_equal(node_type_str(NTERM),"non-terminal");
    assert_string_equal(node_type_str(EXPR),"expression");
    assert_string_equal(node_type_str(EXPR_PAR),"(expression)");
    assert_string_equal(node_type_str(EXPR_FCALL),"function call");
    assert_string_equal(node_type_str(EXPR_LIST),"expression list");
    assert_string_equal(node_type_str(PROG_BODY),"prog_body");
}

// just run print function to see if they do not segfault
// and to complete code coverage
static void test_node_prints(void** state)
{
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* root = NULL;
    root = node_new(EXPR,ADD,"");
    assert_non_null(root);
    
    will_return(__wrap_malloc,1);
    will_return(__wrap_strdup,1);
    ast_node* left = NULL;
    left = node_new(TERM,ID,"$a");
    assert_non_null(left);

    will_return(__wrap_realloc,1);
    assert_true(node_add(root,left));

    FILE* f = fopen("/dev/null","w");
    assert_non_null(f);
    tree_dot_print(f,root);
    tree_dot_print(f,NULL);
    node_print(f,root);
    node_print(f,NULL);
    fclose(f);

    node_delete(&root);
}

int main(int argc, char** argv)
{
    const struct CMUnitTest tests [] =
    {
        cmocka_unit_test(test_node_new),
        cmocka_unit_test(test_node_new_fails),
        cmocka_unit_test(test_node_delete_invalids),
        cmocka_unit_test(test_node_add),
        cmocka_unit_test(test_node_add_fails),
        cmocka_unit_test(test_node_type_str),
        cmocka_unit_test(test_node_prints),
    };

    if((argc > 1))
    {
        if(argv[1][0] == '-' && argv[1][1] == 'x')
        {
            cmocka_set_message_output(CM_OUTPUT_XML);
        }
    }

    skip_mock = 0;
    size_t n = cmocka_run_group_tests(tests,NULL,NULL);
    skip_mock = 1;
    return(n);
}
