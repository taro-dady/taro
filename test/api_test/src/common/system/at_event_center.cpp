
#include "system/event_center.h"
#include "gtest/gtest.h"
#include <string>

USING_NAMESPACE_TARO

class event_center_test : public testing::Test
{
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }
};

static int evt_center_1 = -1;
static int evt_center_2 = -1;

void on_evt_1( int a )
{
    evt_center_1 = a;
}

void on_evt_2( int a )
{
    evt_center_2 = a;
}

TEST_F( event_center_test, mutiple_add )
{
    system::event_center<std::string, int> evt_center;

    EXPECT_TRUE( evt_center.add_event( "test1", func::make_func( &on_evt_1 ) ) );
    EXPECT_FALSE( evt_center.add_event( "test1", func::make_func( &on_evt_1 ) ) );
}

TEST_F( event_center_test, mutiple_remove )
{
    system::event_center<std::string, int> evt_center;

    EXPECT_TRUE( evt_center.add_event( "test1", func::make_func( &on_evt_1 ) ) );
    EXPECT_TRUE( evt_center.remove_event( "test1", func::make_func( &on_evt_1 ) ) );
    EXPECT_FALSE( evt_center.remove_event( "test1", func::make_func( &on_evt_1 ) ) );
}

TEST_F( event_center_test, diff_key )
{
    system::event_center<std::string, int> evt_center;

    EXPECT_TRUE( evt_center.add_event( "test1", func::make_func( &on_evt_1 ) ) );
    EXPECT_TRUE( evt_center.add_event( "test2", func::make_func( &on_evt_2 ) ) );

    evt_center_1 = -1;
    evt_center_2 = -1;
    evt_center.signal( "test1", 3 );
    evt_center.signal( "test2", -110 );

    EXPECT_EQ( evt_center_1, 3 );
    EXPECT_EQ( evt_center_2, -110 );
}

TEST_F( event_center_test, same_key )
{
    system::event_center<std::string, int> evt_center;

    EXPECT_TRUE( evt_center.add_event( "test1", func::make_func( &on_evt_1 ) ) );
    EXPECT_TRUE( evt_center.add_event( "test1", func::make_func( &on_evt_2 ) ) );

    evt_center_1 = -1;
    evt_center_2 = -1;
    evt_center.signal( "test1", 3 );

    EXPECT_EQ( evt_center_1, 3 );
    EXPECT_EQ( evt_center_2, 3 );
}
