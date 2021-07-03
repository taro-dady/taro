
#include "model/observers.h"
#include "gtest/gtest.h"
#include <string>
#include <time.h>
#include <thread>

USING_NAMESPACE_TARO

class model_observers_test : public testing::Test
{
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }
};

static int observers_test_val = 0;
static int64_t general_tm = -1, obj_tm = -2;

void observer_test1( int a )
{
    observers_test_val += a;
    general_tm = ::time( NULL );
    std::this_thread::sleep_for( std::chrono::milliseconds(1500) );
}

struct observer_obj
{
    void test( int b )
    {
        observers_test_val += b;
        obj_tm = ::time( NULL );
        std::this_thread::sleep_for( std::chrono::milliseconds(1500) );
    }
};

TEST_F( model_observers_test, back_ok )
{
    model::observers< int > ob;

    observers_test_val = 0;
    EXPECT_TRUE( ob.push_back( func::make_func( &observer_test1 ) ) );
    EXPECT_TRUE( ob.push_back( func::make_func( &observer_obj::test, std::make_shared<observer_obj>() ) ) );
    ob(3);

    EXPECT_EQ( 6, observers_test_val );
    EXPECT_TRUE( obj_tm > general_tm ); // 先注册的先调用
}

TEST_F( model_observers_test, front_ok )
{
    model::observers< int > ob;

    observers_test_val = 0;
    EXPECT_TRUE( ob.push_back( func::make_func( &observer_test1 ) ) );
    EXPECT_TRUE( ob.push_front( func::make_func( &observer_obj::test, std::make_shared<observer_obj>() ) ) );
    ob(3);

    EXPECT_EQ( 6, observers_test_val );
    EXPECT_TRUE( general_tm > obj_tm ); // 后注册的先调用
}

TEST_F( model_observers_test, push_failed )
{
    model::observers< int > ob;

    observers_test_val = 0;
    EXPECT_TRUE( ob.push_back( func::make_func( &observer_test1 ) ) );
    EXPECT_FALSE( ob.push_back( func::make_func( &observer_test1 ) ) );
    ob(3);

    EXPECT_EQ( 3, observers_test_val );
}

TEST_F( model_observers_test, remove_ok )
{
    model::observers< int > ob;

    observers_test_val = 0;
    auto obj_func = func::make_func( &observer_obj::test, std::make_shared<observer_obj>() );
    EXPECT_TRUE( ob.push_back( func::make_func( &observer_test1 ) ) );
    EXPECT_TRUE( ob.push_front( obj_func ) );

    EXPECT_TRUE( ob.remove( func::make_func( &observer_test1 ) ) );

    ob(3);
    EXPECT_EQ( 3, observers_test_val );

    observers_test_val = 0;
    EXPECT_TRUE( ob.remove( obj_func ) );
    ob(3);
    EXPECT_EQ( 0, observers_test_val );
}

TEST_F( model_observers_test, remove_failed )
{
    model::observers< int > ob;

    observers_test_val = 0;
    auto obj_func = func::make_func( &observer_obj::test, std::make_shared<observer_obj>() );
    EXPECT_TRUE( ob.push_back( func::make_func( &observer_test1 ) ) );
    EXPECT_TRUE( ob.push_front( obj_func ) );

    EXPECT_TRUE( ob.remove( func::make_func( &observer_test1 ) ) );

    ob(3);
    EXPECT_EQ( 3, observers_test_val );

    observers_test_val = 0;
    EXPECT_TRUE( ob.remove( obj_func ) );
    ob(3);
    EXPECT_EQ( 0, observers_test_val );

    EXPECT_FALSE( ob.remove( func::make_func( &observer_test1 ) ) );
    EXPECT_FALSE( ob.remove( obj_func ) );
}


TEST_F( model_observers_test, exist_test )
{
    model::observers< int > ob;

    EXPECT_TRUE( ob.push_back( func::make_func( &observer_test1 ) ) );

    EXPECT_TRUE( ob.existed( func::make_func( &observer_test1 ) ) );

    EXPECT_TRUE( ob.remove( func::make_func( &observer_test1 ) ) );
    EXPECT_FALSE( ob.existed( func::make_func( &observer_test1 ) ) );
}
