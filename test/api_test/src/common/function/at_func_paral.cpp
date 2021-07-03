
#include "gtest/gtest.h"
#include "function/inc.h"
#include "memory/creater.h"
#include "system/thread_pool.h"
#include <string.h>
#include <sstream>

class func_paral_test : public testing::Test
{
};

inline unsigned long long get_thrd_id()
{
    std::ostringstream oss;
    oss << std::this_thread::get_id();
    std::string stid = oss.str();
    return std::stoull(stid);
}

USING_NAMESPACE_TARO

TEST_F( func_paral_test, normal )
{
    int c[4] = { 101, 102, 103, 104 };
    int result[4] = { 0 };
    unsigned long long tid[4];

    func::parallel_functor f;

    // 准备map函数，将数组C分别计算，将结果放入result中
    for ( auto i = 0; i < 4; ++i )
    {
        f.add( [=, &result, &tid]() 
            {
                std::this_thread::sleep_for( std::chrono::milliseconds(100) );
                result[i] = c[i] + 10 * i;
                tid[i] = get_thrd_id();
            }
        );
    }

    // 4个线程的线程池，确保每个函数都能安排到不同的线程
    system::thread_pool< func::functor< void > > thrd_pool(4);

    // 异步运行该并行函数
    EXPECT_TRUE( thrd_pool.start() );
    f.async_call( thrd_pool );
    f.wait();
    EXPECT_TRUE( thrd_pool.stop() );

    EXPECT_EQ( result[0], 101 );
    EXPECT_EQ( result[1], 112 );
    EXPECT_EQ( result[2], 123 );
    EXPECT_EQ( result[3], 134 );

    // 确认每个线程ID不相同
    std::set<unsigned long long > tid_set;
    for ( int i = 0; i < 4; ++i )
        EXPECT_TRUE( tid_set.insert( tid[i] ).second );
}
