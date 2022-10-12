#include"./../../src/ast_stack.h"
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

extern void* __real_calloc(size_t,size_t);
void* __wrap_calloc(size_t n,size_t s)
{
    if(skip_mock == 1) return(__real_calloc(n,s));
    int m = (int)mock();
    if(m == 0)
    {
        return(NULL);
    }
    else
    {
        return(__real_calloc(n,s));
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

/**
 *  test if memory for new ast_stack is allocated corectly
 *  test if structure members are initialized correctly
 *  test if nodes are deleted corectly
 **/
static void test_ast_stack_init(void** state)
{
    (void)state;

    // try init NULL, should not crash
    assert_false(ast_stack_init(NULL,621));
    // just for test coverage and also crash test
    ast_stack_destroy(NULL);

    ast_stack s;
    // test if allocation will allocate minimal size
    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE-1));
    assert_non_null(s.data);
    assert_int_equal(s.size, AST_STACK_MIN_SIZE);
    assert_int_equal(s.index, 0);

    ast_stack_destroy(&s);
    
    // test if allocation will allocate more than minimal size
    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE+1));
    assert_non_null(s.data);
    assert_int_equal(s.size, AST_STACK_MIN_SIZE+1);
    assert_int_equal(s.index, 0);
    
    ast_stack_destroy(&s);
    
    // exact minimal size just in case
    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE));
    assert_non_null(s.data);
    assert_int_equal(s.size, AST_STACK_MIN_SIZE);
    assert_int_equal(s.index, 0);
    
    ast_stack_destroy(&s);
}

/**
 *  test what happen if calloc fails
 **/
static void test_ast_stack_init_fails(void** state)
{
    (void)state;

    ast_stack s;
    // minimal size
    will_return(__wrap_calloc,0);
    assert_false(ast_stack_init(&s,AST_STACK_MIN_SIZE-1));
    assert_null(s.data);
    assert_int_equal(s.size, 0);
    assert_int_equal(s.index, 0);

    ast_stack_destroy(&s);
    
    // more than minimal size
    will_return(__wrap_calloc,0);
    assert_false(ast_stack_init(&s,AST_STACK_MIN_SIZE+1));
    assert_null(s.data);
    assert_int_equal(s.size, 0);
    assert_int_equal(s.index, 0);
    
    ast_stack_destroy(&s);
    
    // exact minimal size just in case
    will_return(__wrap_calloc,0);
    assert_false(ast_stack_init(&s,AST_STACK_MIN_SIZE));
    assert_null(s.data);
    assert_int_equal(s.size, 0);
    assert_int_equal(s.index, 0);
    
    ast_stack_destroy(&s);
}


/**
 *  test push
 **/
static void test_ast_stack_push(void** state)
{
    (void)state;

    ast_stack s;
    // minimal size
    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE));

    ast_node* n = NULL;
    n = node_new(TERM,ADD,"");
    assert_non_null(n);
   
    // crash test, coverage
    assert_false(ast_stack_push(NULL,n));
    assert_false(ast_stack_push(&s,NULL));
    assert_false(ast_stack_push(NULL,NULL));
    
    // test single insertion
    assert_true(ast_stack_push(&s,n));
    assert_int_equal(s.size, AST_STACK_MIN_SIZE);
    assert_int_equal(s.index, 1);
    assert_ptr_equal(n,s.data[0]);

    // !!! to eliminate multiple allocations i will insert one node multiple times
    // !!! DO NOT DO THIS 
    
    // tests repeated insertions
    assert_true(ast_stack_push(&s,n));
    assert_int_equal(s.size, AST_STACK_MIN_SIZE);
    assert_int_equal(s.index, 2);
    assert_ptr_equal(n,s.data[1]);

    // test inserting untli reallocation happen
    will_return(__wrap_realloc,1);
    for(size_t i = 3 ; i < AST_STACK_MIN_SIZE+2 ; ++i)
    {
        assert_true(ast_stack_push(&s,n));
        assert_int_equal(s.index, i);
        assert_ptr_equal(n,s.data[i-1]);
    }
    
    assert_int_equal(s.size, AST_STACK_MIN_SIZE+AST_STACK_CHUNK);

    // DO NOT DO THIS THIS IS SPECIAL DEALOCATION
    node_delete(&n);
    s.size = 0;
    s.index = 0;
    free(s.data);
}

/**
 *  test failing realloc in push
 **/
static void test_ast_stack_push_fails(void** state)
{
    (void)state;

    ast_stack s;
    // minimal size
    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE));

    ast_node* n = NULL;
    n = node_new(TERM,ADD,"");
    assert_non_null(n);
    
    assert_int_equal(s.size, AST_STACK_MIN_SIZE);
   
    // test inserting untli reallocation happen
    for(size_t i = 1 ; i < AST_STACK_MIN_SIZE+1 ; ++i)
    {
        assert_true(ast_stack_push(&s,n));
        assert_int_equal(s.size, AST_STACK_MIN_SIZE);
        assert_int_equal(s.index, i);
        assert_ptr_equal(n,s.data[i-1]);
    }
    
    will_return(__wrap_realloc,0);
    assert_false(ast_stack_push(&s,n));
    assert_int_equal(s.size, AST_STACK_MIN_SIZE);
    assert_int_equal(s.index, AST_STACK_MIN_SIZE);
    

    // DO NOT DO THIS THIS IS SPECIAL DEALOCATION
    node_delete(&n);
    s.size = 0;
    s.index = 0;
    free(s.data);
}

/**
 *  test that top works
 **/
static void test_ast_stack_top(void** state)
{
    (void)state;

    ast_stack s;
    ast_node* top = NULL;

    // minimal size
    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE));

    ast_node* n = NULL;
    n = node_new(TERM,ADD,"");
    assert_non_null(n);
    
    ast_node* n1 = NULL;
    n1 = node_new(EXPR,STRCAT,".");
    assert_non_null(n1);
    
    // top NULL
    top = NULL;
    top = ast_stack_top(NULL);
    assert_null(top);

    // top empty stack
    top = NULL;
    top = ast_stack_top(&s);
    assert_null(top);

    // top after single push
    assert_true(ast_stack_push(&s,n));
    top = NULL;
    top = ast_stack_top(&s);
    assert_non_null(top);

    // compare n and top
    assert_int_equal(top->type,n->type);
    assert_int_equal(top->sub_type,n->sub_type);
    assert_int_equal(top->id,n->id);
    assert_string_equal(top->attrib,n->attrib);
    assert_ptr_equal(top->children,n->children);
    assert_int_equal(top->children_cnt,n->children_cnt);


    // second push and top
    assert_true(ast_stack_push(&s,n1));
    top = NULL;
    top = ast_stack_top(&s);
    assert_non_null(top);
    
    // compare n1 and top
    assert_int_equal(top->type,n1->type);
    assert_int_equal(top->sub_type,n1->sub_type);
    assert_int_equal(top->id,n1->id);
    assert_string_equal(top->attrib,n1->attrib);
    assert_ptr_equal(top->children,n1->children);
    assert_int_equal(top->children_cnt,n1->children_cnt);

    ast_stack_destroy(&s);
}

/**
 *  test stack reset
 **/
static void test_ast_stack_reset(void** state)
{
    (void)state;
    ast_stack s;

    // minimal size
    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE));

    for(size_t i = 0 ; i < 6; ++i)
    {
        ast_node* n = NULL;
        n = node_new(TERM,IVAL,"1");
        assert_non_null(n);
        assert_true(ast_stack_push(&s,n));
    }
    assert_int_equal(s.size,AST_STACK_MIN_SIZE);
    assert_int_equal(s.index,6);

    //crash test
    ast_stack_reset(NULL);

    ast_stack_reset(&s);
    assert_int_equal(s.size,AST_STACK_MIN_SIZE);
    assert_int_equal(s.index,0);

    ast_stack_destroy(&s);
}

/**
 *  test stack pop
 **/
static void test_ast_stack_pop(void** state)
{
    (void)state;
    ast_stack s;

    // pop-ing from null, crash test
    assert_false(ast_stack_pop(NULL));

    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE));

    // pop-ing from empty stack, crash test
    assert_false(ast_stack_pop(&s));

    // try push pop
    ast_node* n = NULL;
    n = node_new(TERM,IVAL,"1");
    assert_true(ast_stack_push(&s,n));

    assert_int_equal(s.index,1);
    assert_true(ast_stack_pop(&s));
    assert_int_equal(s.index,0);
    
    // try push 2 nodes and pop one 
    // if top will match the first pushed node
    n = NULL;
    n = node_new(TERM,IVAL,"1");
    assert_true(ast_stack_push(&s,n));
    
    n = NULL;
    n = node_new(TERM,FVAL,"2.0");
    assert_true(ast_stack_push(&s,n));
    
    assert_true(ast_stack_pop(&s));

    // compare top with first pushed node
    n = ast_stack_top(&s);
    assert_non_null(n);
    assert_int_equal(n->type,TERM);
    assert_int_equal(n->sub_type,IVAL);
    assert_string_equal(n->attrib,"1");

    ast_stack_destroy(&s);
}

/**
 *  test stack peel
 **/
static void test_ast_stack_peel(void** state)
{
    (void)state;
    ast_stack s;

    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE));
    
    // crash tests
    assert_null(ast_stack_peel(NULL));
    // test empty stack
    assert_null(ast_stack_peel(&s));

    // push and peel one node
    ast_node* n = NULL;
    n = node_new(TERM,IVAL,"1");
    assert_non_null(n);
    assert_true(ast_stack_push(&s,n));

    assert_int_equal(s.index,1);
    ast_node* peel = NULL;
    peel = ast_stack_peel(&s);
    assert_non_null(peel);
    assert_int_equal(s.index,0);

    // compare pushed and peeled nodes
    assert_ptr_equal(n,peel);
    assert_int_equal(n->type,peel->type);
    assert_int_equal(n->sub_type,peel->sub_type);
    assert_int_equal(n->id,peel->id);
    assert_string_equal(n->attrib,peel->attrib);
    
    // test two pushes and one peel
    assert_true(ast_stack_push(&s,n));
    n = node_new(TERM,IVAL,"2");
    assert_non_null(n);
    assert_true(ast_stack_push(&s,n));

    assert_int_equal(s.index,2);
    peel = NULL;
    peel = ast_stack_peel(&s);
    assert_non_null(peel);
    assert_int_equal(s.index,1);
    
    assert_ptr_equal(n,peel);
    assert_int_equal(n->type,peel->type);
    assert_int_equal(n->sub_type,peel->sub_type);
    assert_int_equal(n->id,peel->id);
    assert_string_equal(n->attrib,peel->attrib);

    node_delete(&peel);
    ast_stack_destroy(&s);
}

/**
 *  test stack_is_empty
 **/
static void test_ast_stack_is_empty(void** state)
{
    (void)state;
    assert_false(ast_stack_is_empty(NULL));
    ast_stack s;

    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE));
    
    // freshly initialized stack must be empty
    assert_true(ast_stack_is_empty(&s));
    
    // one node on stack should make stack non-empty
    ast_node* n = NULL;
    n = node_new(TERM,IVAL,"1");
    assert_non_null(n);
    assert_true(ast_stack_push(&s,n));
    assert_false(ast_stack_is_empty(&s));

    // and it should be empty after pop
    assert_true(ast_stack_pop(&s));
    assert_true(ast_stack_is_empty(&s));

    ast_stack_destroy(&s);
}

/**
 *  test stack_is_empty
 **/
static void test_ast_stack_dive(void** state)
{
    (void)state;
    assert_null(ast_stack_dive(NULL,911));
    ast_stack s;

    will_return(__wrap_calloc,1);
    assert_true(ast_stack_init(&s,AST_STACK_MIN_SIZE));
    
    assert_null(ast_stack_dive(&s,42));
    
    // push one dive unde stack
    ast_node* n = NULL;
    n = node_new(TERM,IVAL,"1");
    assert_non_null(n);
    assert_true(ast_stack_push(&s,n));
    
    assert_null(ast_stack_dive(&s,1));
    
    // dive(0) and should be the same as top
    ast_node* dive = NULL;
    ast_node* top = NULL;
    dive = ast_stack_dive(&s,0);
    assert_non_null(dive);
    top = ast_stack_top(&s);
    assert_non_null(top);
    assert_ptr_equal(dive,top);

    // push second element and dive under top
    n = node_new(EXPR,ADD,"");
    assert_non_null(n);
    assert_true(ast_stack_push(&s,n));
    
    dive = ast_stack_dive(&s,1);
    assert_non_null(dive);
    assert_int_equal(dive->type,TERM);
    assert_int_equal(dive->sub_type,IVAL);
    assert_string_equal(dive->attrib,"1");
    
    // just in calse
    assert_null(ast_stack_dive(&s,2));

    ast_stack_destroy(&s);
}

int main(int argc, char** argv)
{
    const struct CMUnitTest tests [] =
    {
        cmocka_unit_test(test_ast_stack_init),
        cmocka_unit_test(test_ast_stack_init_fails),
        cmocka_unit_test(test_ast_stack_push),
        cmocka_unit_test(test_ast_stack_push_fails),
        cmocka_unit_test(test_ast_stack_top),
        cmocka_unit_test(test_ast_stack_reset),
        cmocka_unit_test(test_ast_stack_pop),
        cmocka_unit_test(test_ast_stack_peel),
        cmocka_unit_test(test_ast_stack_is_empty),
        cmocka_unit_test(test_ast_stack_dive),
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
