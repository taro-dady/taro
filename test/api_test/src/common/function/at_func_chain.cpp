
#include "gtest/gtest.h"
#include "function/inc.h"
#include "memory/creater.h"
#include "system/thread_pool.h"
#include <string.h>

class func_chain_test : public testing::Test
{
};

USING_NAMESPACE_TARO

TEST_F( func_chain_test, normal_sync )
{
    func::functor_chain chain;

    int32_t base = 100;
    int32_t result = -1;

    // 同步调用
    chain.then(
        [&]()
        {
            // 返回值为下一个函数的输入，类型要与下一个函数的输入一致
            return (double)( base - 80 );
        }
    ).then(
        [&]( double a )
        {
            // 返回值为下一个函数的输入，类型要与下一个函数的输入一致
            return (int)( a * 10 );
        }
    ).then(
        [&]( int a )
        {
            result = a + 2;
        }
    )();

    EXPECT_EQ( result, 202 );
}

TEST_F( func_chain_test, normal_async )
{
    func::functor_chain chain;

    int32_t base = 100;
    int32_t result = -1;

    // 异步调用
    chain.then(
        [&]()
        {
            // 返回值为下一个函数的输入，类型要与下一个函数的输入一致
            return (double)(base - 80);
        }
    ).then(
        [&]( double a )
        {
            // 返回值为下一个函数的输入，类型要与下一个函数的输入一致
            return (int)(a * 10);
        }
    ).then(
        [&]( int a )
        {
            result = a + 2;
        }
    );
    
    system::thread_pool< func::functor< void > > thrd_pool;
    EXPECT_TRUE( thrd_pool.start() );
    chain.async_call( thrd_pool );
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );
    EXPECT_TRUE( thrd_pool.stop() );

    EXPECT_EQ( result, 202 );
}
