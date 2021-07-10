
#include "infra/inc.h"
#include "gtest/gtest.h"
#include <string>

class infra_thread_test : public testing::Test
{
public: // 公共函数

};

USING_NAMESPACE_TARO

static bool run_flag = false;
static bool rt_flag = false;
static int32_t call_times_array[3] = { 0 };

static void thrd_proc()
{
    printf("thrd_proc\n");
    run_flag = true;
}

static void thrd_proc_low()
{
    while(run_flag)
    {
        ++call_times_array[infra::thread::thread_prio_low];
        //infra::thread::sleep(50);
    }
}

static void thrd_proc_middle()
{
    while(run_flag)
    {
        ++call_times_array[infra::thread::thread_prio_middle];
       // infra::thread::sleep(50);
    }
}

static void thrd_proc_high()
{
    while(run_flag)
    {
        ++call_times_array[infra::thread::thread_prio_high];
        //infra::thread::sleep(50);
    }
}

static void thrd_runtime_test()
{
    while (rt_flag)
    {
        infra::thread::sleep(50);
    }
}

// 正常创建线程测试
TEST_F( infra_thread_test, create_thread_ok ) 
{
    infra::thread thrd( "test1" );
 
    run_flag = false;
    auto ret = thrd.start( func::bind( &thrd_proc ) );
    EXPECT_EQ( ret, errno_ok );

    ret = thrd.stop(); 
    EXPECT_EQ( ret, errno_ok );

    EXPECT_TRUE( run_flag );
}
 
// 创建相同名称线程测试
TEST_F( infra_thread_test, create_thread_failed )
{
    infra::thread thrd( "test1" );
    
    run_flag = false; 
    auto ret = thrd.start( func::bind( &thrd_proc ) );
    EXPECT_EQ( ret, errno_ok );

    // 不能创建相同名称的线程
    infra::thread thrd1( "test1" );
    ret = thrd1.start( func::bind( &thrd_proc ) );
    EXPECT_EQ( ret, errno_multiple );

    ret = thrd1.stop();
    EXPECT_EQ( ret, errno_multiple ); 

    ret = thrd.stop();
    EXPECT_EQ( ret, errno_ok );

    EXPECT_TRUE( run_flag ); 
}

// 获取线程信息测试
TEST_F( infra_thread_test, get_thread_info )
{
    infra::thread thrd( "test1" );
    
    run_flag = false; 
    auto ret = thrd.start( func::bind( &thrd_proc ) );
    EXPECT_EQ( ret, errno_ok );

    infra::thread thrd1( "test2" );
    ret = thrd1.start( func::bind( &thrd_proc ) );
    EXPECT_EQ( ret, errno_ok );

    {
        auto infos = infra::get_all_thread_info();
        EXPECT_TRUE( 2 == infos.size() );
        EXPECT_TRUE( infos[0].name == "test1" );
        EXPECT_TRUE( infos[0].stack_size == 0 );
        EXPECT_TRUE( infos[0].priority == infra::thread::thread_prio_low );
        EXPECT_TRUE( infos[1].name == "test2" );
        EXPECT_TRUE( infos[1].stack_size == 0 );
        EXPECT_TRUE( infos[1].priority == infra::thread::thread_prio_low );
    }
    
    ret = thrd1.stop();
    EXPECT_EQ( ret, errno_ok );

    {
        auto infos = infra::get_all_thread_info();
        EXPECT_TRUE( 1 == infos.size() );
        EXPECT_TRUE( infos[0].name == "test1" );
        EXPECT_TRUE( infos[0].stack_size == 0 );
        EXPECT_TRUE( infos[0].priority == infra::thread::thread_prio_low );
    }

    ret = thrd.stop();
    EXPECT_EQ( ret, errno_ok );

    {
        auto infos = infra::get_all_thread_info();
        EXPECT_TRUE( 0 == infos.size() );
    }

    EXPECT_TRUE( run_flag ); 
}

// 获取线程信息测试，线程运行态测试
TEST_F( infra_thread_test, get_thread_info_running )
{
    infra::thread thrd( "test1" );
    
    rt_flag = true; 
    auto ret = thrd.start( func::bind( &thrd_runtime_test ) );
    EXPECT_EQ( ret, errno_ok );

    infra::thread::sleep(200);
    {
        auto infos = infra::get_all_thread_info();
        EXPECT_TRUE( 1 == infos.size() );
        EXPECT_TRUE( infos[0].name == "test1" );
        EXPECT_TRUE( infos[0].is_running );
        EXPECT_TRUE( infos[0].stack_size == 0 );
        EXPECT_TRUE( infos[0].priority == infra::thread::thread_prio_low );
    }

    rt_flag = false;
    infra::thread::sleep(1000);
    {
        auto infos = infra::get_all_thread_info();
        EXPECT_TRUE( 1 == infos.size() );
        EXPECT_TRUE( infos[0].name == "test1" );
        EXPECT_FALSE( infos[0].is_running );
        EXPECT_TRUE( infos[0].stack_size == 0 );
        EXPECT_TRUE( infos[0].priority == infra::thread::thread_prio_low );
    }

    ret = thrd.stop();
    EXPECT_EQ( ret, errno_ok );

    {
        auto infos = infra::get_all_thread_info();
        EXPECT_TRUE( 0 == infos.size() );
    }

    EXPECT_TRUE( run_flag ); 
}

// 线程优先级测试
TEST_F( infra_thread_test, thread_priority )
{ 
    infra::thread thrd( "test1" );
    infra::thread thrd1( "test2" );
    infra::thread thrd2( "test3" );
    
    run_flag = true;
    memset( call_times_array, 0, sizeof( call_times_array ) );

    int32_t ret;
    
    ret = thrd.start( func::bind( &thrd_proc_low ) );
    EXPECT_EQ( ret, errno_ok ); 

    ret = thrd1.start( func::bind( &thrd_proc_middle ), 0, infra::thread::thread_prio_middle );
    EXPECT_EQ( ret, errno_ok );

    ret = thrd2.start( func::bind( &thrd_proc_high ), 0, infra::thread::thread_prio_high );
    EXPECT_EQ( ret, errno_ok );

    infra::thread::sleep( 1000 );
    run_flag = false;

    ret = thrd.stop();
    EXPECT_EQ( ret, errno_ok ); 
 
    ret = thrd1.stop();
    EXPECT_EQ( ret, errno_ok ); 
 
    ret = thrd2.stop();
    EXPECT_EQ( ret, errno_ok );

    printf( "high %d middle %d low %d\n", call_times_array[2], call_times_array[1], call_times_array[0] ); 

    EXPECT_TRUE( call_times_array[infra::thread::thread_prio_high] > call_times_array[infra::thread::thread_prio_middle] );
    EXPECT_TRUE( call_times_array[infra::thread::thread_prio_high] > call_times_array[infra::thread::thread_prio_low] );
}

// 线程优先级测试
TEST_F( infra_thread_test, thread_priority_mid )
{ 
    infra::thread thrd( "test1" );
    infra::thread thrd1( "test2" );
    
    run_flag = true;
    memset( call_times_array, 0, sizeof( call_times_array ) );

    int32_t ret;
    
    ret = thrd.start( func::bind( &thrd_proc_low ) );
    EXPECT_EQ( ret, errno_ok ); 

    ret = thrd1.start( func::bind( &thrd_proc_middle ), 0, infra::thread::thread_prio_middle );
    EXPECT_EQ( ret, errno_ok );

    infra::thread::sleep( 1000 );
    run_flag = false;

    ret = thrd.stop();
    EXPECT_EQ( ret, errno_ok ); 
 
    ret = thrd1.stop();
    EXPECT_EQ( ret, errno_ok ); 

    printf( "high %d middle %d low %d\n", call_times_array[2], call_times_array[1], call_times_array[0] ); 

    EXPECT_TRUE( call_times_array[infra::thread::thread_prio_middle] > call_times_array[infra::thread::thread_prio_low] );
}
