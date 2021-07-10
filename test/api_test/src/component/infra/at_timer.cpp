
#include "infra/inc.h"
#include "gtest/gtest.h"
#include <string>

class infra_timer_test : public testing::Test
{
public: // 公共函数

};

USING_NAMESPACE_TARO

// 正常测试
TEST_F( infra_timer_test, timer_manager_ok )
{
    auto ret = infra::start_timer_manager();
    EXPECT_EQ( ret, errno_ok );

    ret = infra::stop_timer_manager();
    EXPECT_EQ( ret, errno_ok );
}

// 多次启动停止
TEST_F( infra_timer_test, timer_manager_failed )
{
    auto ret = infra::start_timer_manager();
    EXPECT_EQ( ret, errno_ok );

    ret = infra::start_timer_manager();
    EXPECT_EQ( ret, errno_multiple );

    ret = infra::stop_timer_manager();
    EXPECT_EQ( ret, errno_ok );

    ret = infra::stop_timer_manager();
    EXPECT_EQ( ret, errno_multiple );
}

// 正常定时器启动测试
TEST_F( infra_timer_test, timer_ok )
{
    auto ret = infra::start_timer_manager();
    EXPECT_EQ( ret, errno_ok );

    int32_t count = 0;
    infra::timer tm( "test1" );

    // 500延时，1000ms时间间隔
    ret = tm.start( 500, 1000, func::bind( [&](){ ++count; } ) );
    EXPECT_EQ( ret, errno_ok );

    infra::thread::sleep( 2000 );

    ret = tm.stop();
    EXPECT_EQ( ret, errno_ok );

    printf( "count %d\n", count );
    EXPECT_TRUE( count > 0 );

    ret = infra::stop_timer_manager();
    EXPECT_EQ( ret, errno_ok );
}

// 定时器定时精度测试
TEST_F( infra_timer_test, timer_single_accurate )
{
    auto ret = infra::start_timer_manager();
    EXPECT_EQ( ret, errno_ok );

    infra::timer tm( "test1" );

    // 500延时，1000ms时间间隔
    int64_t last_time = infra::sys_time::current_ms(), first_call = 0, interval = -1;
    ret = tm.start( 500, 1000, func::bind( [&]()
    {
        if ( 0 == first_call )
        {
            first_call = infra::sys_time::current_ms() - last_time;
            last_time = infra::sys_time::current_ms();
        }
        else
        {
            interval = infra::sys_time::current_ms() - last_time;
            last_time = infra::sys_time::current_ms();
        }
    } ) );
    EXPECT_EQ( ret, errno_ok );

    infra::thread::sleep( 2000 );

    ret = tm.stop();
    EXPECT_EQ( ret, errno_ok );

    printf( "first %d interval %d\n", (int32_t)first_call, (int32_t)interval );
    EXPECT_TRUE( std::abs( first_call - 500 ) < 5 );
    EXPECT_TRUE( std::abs( interval - 1000 ) < 5 );

    ret = infra::stop_timer_manager();
    EXPECT_EQ( ret, errno_ok );
}

// 定时器定时精度测试
TEST_F( infra_timer_test, timer_multiple_accurate )
{
    auto ret = infra::start_timer_manager();
    EXPECT_EQ( ret, errno_ok );

    infra::timer tm1( "test1" );
    infra::timer tm2( "test2" ); 

    // 500延时，1000ms时间间隔
    int64_t last_time = infra::sys_time::current_ms(), first_call = -1, interval = -1;
    ret = tm1.start( 500, 1000, func::bind( [&]()
    {
        if ( -1 == first_call )
        {
            first_call = infra::sys_time::current_ms() - last_time;
            last_time = infra::sys_time::current_ms();
        }
        else
        {
            interval = infra::sys_time::current_ms() - last_time;
            last_time = infra::sys_time::current_ms(); 
        }
    } ) );
    EXPECT_EQ( ret, errno_ok ); 

    int64_t last_time1 = infra::sys_time::current_ms(), first_call1 = -1, interval1 = -1;
    ret = tm2.start( 0, 2000, func::bind( [&]() 
    {
        if ( -1 == first_call1 )
        {
            first_call1 = infra::sys_time::current_ms() - last_time1; 
            last_time1  = infra::sys_time::current_ms();
        }
        else
        {
            interval1  = infra::sys_time::current_ms() - last_time1;
            last_time1 = infra::sys_time::current_ms(); 
        }
    } ) );
    EXPECT_EQ( ret, errno_ok );

    infra::thread::sleep( 5000 );

    ret = tm1.stop();
    EXPECT_EQ( ret, errno_ok );

    ret = tm2.stop();
    EXPECT_EQ( ret, errno_ok );

    constexpr int32_t std_value = 5;

    printf( "first %d interval %d\n", (int32_t)first_call, (int32_t)interval );
    EXPECT_TRUE( std::abs( first_call - 500 ) < std_value );
    EXPECT_TRUE( std::abs( interval - 1000 ) < std_value );

    printf( "first1 %d interval1 %d\n", (int32_t)first_call1, (int32_t)interval1 );
    EXPECT_TRUE( std::abs( first_call1 - 0 ) < std_value );
    EXPECT_TRUE( std::abs( interval1 - 2000 ) < std_value );
    ret = infra::stop_timer_manager();
    EXPECT_EQ( ret, errno_ok );
}
