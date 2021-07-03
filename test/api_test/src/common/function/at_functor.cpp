
#include "gtest/gtest.h"
#include "function/inc.h"
#include "memory/creater.h"
#include <string.h>

USING_NAMESPACE_TARO

class functor_test : public testing::Test
{
};

static int functor_add( int a, int b )
{
    return a + b;
}

struct func_test_obj
{
    int32_t minus( int32_t i, int32_t j )
    {
        return i - j;
    }
};

TEST_F( functor_test, general_func )
{
    auto fun = func::bind( &functor_add, 2, 20 );
    EXPECT_EQ( fun(), 22 );
}

TEST_F( functor_test, shared_obj_func )
{
    auto fun = func::bind( &func_test_obj::minus, std::make_shared<func_test_obj>(), 20, 100 );
    EXPECT_EQ( fun(), -80 );
}

TEST_F( functor_test, weak_obj_func )
{
    auto shared_obj = std::make_shared<func_test_obj>();
    auto fun = func::bind( &func_test_obj::minus, memory::shared_wptr( shared_obj ), 15, 5 );
    EXPECT_EQ( fun(), 10 );
}

TEST_F( functor_test, weak_obj_func_expired )
{
    auto shared_obj = std::make_shared<func_test_obj>();
    auto fun = func::bind( &func_test_obj::minus, memory::shared_wptr( shared_obj ), 15, 5 );
    shared_obj.reset();

    try
    {
        fun();
    }
    catch( func::exception const& e )
    {
        EXPECT_EQ( e, func::exception_weak_pointer_expired );
    }
}

TEST_F( functor_test, raw_obj_func )
{
    func_test_obj obj;
    auto fun = func::bind( &func_test_obj::minus, memory::inst_sptr( &obj ), -9, -11 );
    EXPECT_EQ( fun(), 2 );
}

TEST_F( functor_test, lambdas_func )
{
    int32_t tmp = 2;
    auto fun = func::bind( [=]( int32_t b ){ return tmp + b; }, 10 );
    EXPECT_EQ( fun(), 12 );
}
