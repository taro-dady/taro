
#include "system/data_queue.h"
#include "gtest/gtest.h"
#include <string>

USING_NAMESPACE_TARO

class data_queue_test : public testing::Test
{
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }
};

TEST_F( data_queue_test, push_back )
{
    system::data_queue<int> dq;

    dq.push( 2 );
    EXPECT_EQ( 1, dq.size() );

    auto ret = dq.take();
    EXPECT_EQ( ret, 2 );
    EXPECT_EQ( 0, dq.size() );
}

TEST_F( data_queue_test, push_front )
{
    system::data_queue<int> dq;

    dq.push( 2 );
    dq.push( 3, true );
    EXPECT_EQ( 2, dq.size() );

    auto ret = dq.take();
    EXPECT_EQ( ret, 3 );
    EXPECT_EQ( 1, dq.size() );

    ret = dq.take();
    EXPECT_EQ( ret, 2 );
    EXPECT_EQ( 0, dq.size() );
}

TEST_F( data_queue_test, wait_time_out )
{
    system::data_queue<int> dq;
    int data = -1;
    auto ret = dq.take(data, 1000);
    EXPECT_FALSE( ret );
}

TEST_F( data_queue_test, wait_ok )
{
    system::data_queue<int> dq;
    dq.push( 2 );

    int data = -1;
    auto ret = dq.take(data, 1000);
    EXPECT_TRUE( ret );
    EXPECT_EQ( data, 2 );
}

TEST_F( data_queue_test, pick_ok )
{
    system::data_queue<int> dq;
    dq.push( 2 );
    dq.push( 3 );
    dq.push( 4 );
    dq.push( 5 );

    int data = -1;
    auto ret = dq.pick(data, []( int a ){ return a == 4; }, 1000);
    EXPECT_TRUE( ret );
    EXPECT_EQ( data, 4 );
}

TEST_F( data_queue_test, pick_failed )
{
    system::data_queue<int> dq;
    dq.push( 2 );
    dq.push( 3 );
    dq.push( 4 );
    dq.push( 5 );

    int data = -1;
    auto ret = dq.pick(data, []( int a ){ return a == 6; }, 1000);
    EXPECT_FALSE( ret );
    EXPECT_EQ( data, -1 );
}

TEST_F( data_queue_test, clear )
{
    system::data_queue<int> dq;
    dq.push( 2 );
    EXPECT_EQ( 1, dq.size() );

    dq.clear();
    EXPECT_EQ( 0, dq.size() );
}

