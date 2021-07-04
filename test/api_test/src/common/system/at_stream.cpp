
#include "system/stream.h"
#include "gtest/gtest.h"
#include <string>
#include <thread>

USING_NAMESPACE_TARO

class stream_test : public testing::Test
{
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }
};

TEST_F( stream_test, out_str_invalid )
{
    system::outstream<int, double> out(1);
    EXPECT_FALSE( out );
}

TEST_F( stream_test, async_snd_recv )
{
    system::instream<int, double> in(1);
    system::outstream<int, double> out(1);

    EXPECT_TRUE( out );
    EXPECT_TRUE( in );

    auto thrd = std::thread( [&]()
    { 
        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
        out << 2.0; 
    } );

    double d = -1;
    in >> d;
    EXPECT_DOUBLE_EQ( d, 2.0 );
    thrd.join();
}

TEST_F( stream_test, instream_timeout_0 )
{
    system::instream<int, double> in(1, 1000); // 超时时间1000ms
    EXPECT_TRUE( in );

    double d = -1;
    in >> d;
    EXPECT_DOUBLE_EQ( d, -1 );
}

TEST_F( stream_test, instream_timeout_1 )
{
    system::instream<int, double> in(1); 
    EXPECT_TRUE( in );
    in.set_timeout(1000); // 超时时间1000ms

    double d = -1;
    in >> d;
    EXPECT_DOUBLE_EQ( d, -1 );
}

TEST_F( stream_test, instream_pick_ok )
{
    system::instream<int, double> in(1); 
    system::outstream<int, double> out(1);
    EXPECT_TRUE( out );
    EXPECT_TRUE( in );

    auto thrd = std::thread( [&]()
    { 
        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
        out << 2.0; 
        out << 2.1;
        out << 2.2;
        out << 2.3;
    } );

    // d 输出数据 lambda比较函数，2000等待时间
    double d = -1;
    auto ret = in(d, []( double a ){ return std::fabs( a - 2.2 ) < 0.00001; }, 2000 );
    EXPECT_DOUBLE_EQ( d, 2.2 );
    EXPECT_TRUE( ret );
    thrd.join();
}

TEST_F( stream_test, instream_pick_failed )
{
    system::instream<int, double> in(1); 
    system::outstream<int, double> out(1);
    EXPECT_TRUE( out );
    EXPECT_TRUE( in );

    auto thrd = std::thread( [&]()
    { 
        std::this_thread::sleep_for( std::chrono::milliseconds(100) );
        out << 2.0; 
        out << 2.1;
        out << 2.2;
        out << 2.3;
    } );

    // d 输出数据 lambda比较函数，2000等待时间
    double d = -1;
    auto ret = in(d, []( double a ){ return std::fabs( a - 3.2 ) < 0.00001; }, 2000 );
    EXPECT_FALSE( ret );
    thrd.join();
}
