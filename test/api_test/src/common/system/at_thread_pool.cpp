
#include "system/thread_pool.h"
#include "function/inc.h"
#include "gtest/gtest.h"
#include <string>
#include <set>

USING_NAMESPACE_TARO

class thrd_pool_test : public testing::Test
{
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }
};

inline unsigned long long get_thrd_id()
{
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    std::string stid = oss.str();
    return std::stoull(stid);
}

static int32_t thrd_pool_test_value[4];
static unsigned long long thrd_pool_tid[4];


void thrd_pool_test_func1( int a )
{
    thrd_pool_test_value[0] = a;
    thrd_pool_tid[0] = get_thrd_id();
}

void thrd_pool_test_func2( int a )
{
    thrd_pool_test_value[1] = a;
    thrd_pool_tid[1] = get_thrd_id();
}

void thrd_pool_test_func3( int a )
{
    thrd_pool_test_value[2] = a;
    thrd_pool_tid[2] = get_thrd_id();
}

void thrd_pool_test_func4( int a )
{
    thrd_pool_test_value[3] = a;
    thrd_pool_tid[3] = get_thrd_id();
}

TEST_F( thrd_pool_test, start_stop_ok )
{
    system::kthread_pool< int, func::functor<void> > thrdpool;

    EXPECT_TRUE( thrdpool.start() );
    EXPECT_TRUE( thrdpool.stop() );
}

TEST_F( thrd_pool_test, start_stop_multiple )
{
    system::kthread_pool< int, func::functor<void> > thrdpool;

    EXPECT_TRUE( thrdpool.start() );
    EXPECT_FALSE( thrdpool.start() );
    EXPECT_TRUE( thrdpool.stop() );
    EXPECT_FALSE( thrdpool.stop() );
}

TEST_F( thrd_pool_test, kthrd_handle_data )
{
    system::kthread_pool< int, func::functor<void> > thrdpool;

    EXPECT_TRUE( thrdpool.start() );

    memset( thrd_pool_test_value, -1, sizeof( thrd_pool_test_value ) );
    memset( thrd_pool_tid, 0, sizeof( thrd_pool_tid ) );
    thrdpool.push( 1, func::bind( &thrd_pool_test_func1, 11 ) );
    thrdpool.push( 2, func::bind( &thrd_pool_test_func2, 12 ) );
    thrdpool.push( 3, func::bind( &thrd_pool_test_func3, 13 ) );
    thrdpool.push( 4, func::bind( &thrd_pool_test_func4, 14 ) );
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );
    EXPECT_EQ( thrd_pool_test_value[0], 11 );
    EXPECT_EQ( thrd_pool_test_value[1], 12 );
    EXPECT_EQ( thrd_pool_test_value[2], 13 );
    EXPECT_EQ( thrd_pool_test_value[3], 14 );

    // 确认每个线程ID不相同
    std::set<unsigned long long > tid_set;
    for ( int i = 0; i < 4; ++i )
        EXPECT_TRUE( tid_set.insert( thrd_pool_tid[i] ).second );

    EXPECT_TRUE( thrdpool.stop() );
}

TEST_F( thrd_pool_test, kthrd_handle_data_same_key )
{
    system::kthread_pool< int, func::functor<void> > thrdpool;

    EXPECT_TRUE( thrdpool.start() );

    memset( thrd_pool_test_value, -1, sizeof( thrd_pool_test_value ) );
    memset( thrd_pool_tid, 0, sizeof( thrd_pool_tid ) );
    thrdpool.push( 1, func::bind( &thrd_pool_test_func1, 11 ) );
    thrdpool.push( 2, func::bind( &thrd_pool_test_func2, 12 ) );
    thrdpool.push( 1, func::bind( &thrd_pool_test_func3, 13 ) );
    thrdpool.push( 2, func::bind( &thrd_pool_test_func4, 14 ) );
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );
    EXPECT_EQ( thrd_pool_test_value[0], 11 );
    EXPECT_EQ( thrd_pool_test_value[1], 12 );
    EXPECT_EQ( thrd_pool_test_value[2], 13 );
    EXPECT_EQ( thrd_pool_test_value[3], 14 );

    // 确认2个线程被使用
    std::set<unsigned long long > tid_set;
    for ( int i = 0; i < 4; ++i )
    {
        if ( 0 != thrd_pool_tid[i] )
        {
            tid_set.insert( thrd_pool_tid[i] );
        }
    }
    EXPECT_EQ( 2, tid_set.size() );

    EXPECT_TRUE( thrdpool.stop() );
}

TEST_F( thrd_pool_test, thrd_handle_data )
{
    system::thread_pool< func::functor<void> > thrdpool;

    EXPECT_TRUE( thrdpool.start() );

    memset( thrd_pool_test_value, -1, sizeof( thrd_pool_test_value ) );
    memset( thrd_pool_tid, 0, sizeof( thrd_pool_tid ) );
    thrdpool.push( func::bind( &thrd_pool_test_func1, 11 ) );
    thrdpool.push( func::bind( &thrd_pool_test_func2, 12 ) );
    thrdpool.push( func::bind( &thrd_pool_test_func3, 13 ) );
    thrdpool.push( func::bind( &thrd_pool_test_func4, 14 ) );
    std::this_thread::sleep_for( std::chrono::milliseconds(500) );
    EXPECT_EQ( thrd_pool_test_value[0], 11 );
    EXPECT_EQ( thrd_pool_test_value[1], 12 );
    EXPECT_EQ( thrd_pool_test_value[2], 13 );
    EXPECT_EQ( thrd_pool_test_value[3], 14 );

    // 确认每个线程ID不相同
    std::set<unsigned long long > tid_set;
    for ( int i = 0; i < 4; ++i )
        EXPECT_TRUE( tid_set.insert( thrd_pool_tid[i] ).second );

    EXPECT_TRUE( thrdpool.stop() );
}
