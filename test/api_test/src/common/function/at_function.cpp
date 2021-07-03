
#include "gtest/gtest.h"
#include "function/inc.h"
#include "memory/creater.h"
#include <string.h>

USING_NAMESPACE_TARO

class function_test : public testing::Test
{
};

static int32_t count = 0;

void function_test1()
{
    ++count;
}

struct func_test_obj
{
    void test_arg_1( int32_t i )
    {
        count = i;
    }
};

TEST_F( function_test, general_func )
{
    auto fun = func::make_func( &function_test1 );
    count = 0;
    fun();
    EXPECT_EQ( count, 1 );
}

TEST_F( function_test, shared_obj_func )
{
    auto fun = func::make_func( &func_test_obj::test_arg_1, std::make_shared<func_test_obj>() );
    count = 0;
    fun( 3 );
    EXPECT_EQ( count, 3 );
}

TEST_F( function_test, weak_obj_func )
{
    auto obj_ptr = std::make_shared<func_test_obj>();
    auto fun = func::make_func( &func_test_obj::test_arg_1, std::weak_ptr<func_test_obj>( obj_ptr ) );
    count = 0;
    fun( 5 );
    EXPECT_EQ( count, 5 );
}

TEST_F( function_test, weak_obj_func_exception )
{
    auto obj_ptr = std::make_shared<func_test_obj>();
    auto fun = func::make_func( &func_test_obj::test_arg_1, memory::shared_wptr( obj_ptr ) );
    count = 0;
    obj_ptr.reset();
    try
    {
        fun( 5 );
    }
    catch( func::exception const& e )
    {
        EXPECT_EQ( e, func::exception_weak_pointer_expired );
    }
}

TEST_F( function_test, raw_obj_func )
{
    func_test_obj obj;
    auto fun = func::make_func( &func_test_obj::test_arg_1, memory::inst_sptr( &obj ) );
    count = 0;
    fun( 8 );
    EXPECT_EQ( count, 8 );
}
