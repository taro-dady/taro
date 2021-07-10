
#include "infra/inc.h"
#include "gtest/gtest.h"
#include <string>

class infra_time_test : public testing::Test
{
public: // 公共函数

};

USING_NAMESPACE_TARO

TEST_F( infra_time_test, time_to_str ) 
{
    infra::sys_time tm1( "2021-06-01 00:00:00" );  
    infra::sys_time tm2( "2021-06-02 00:00:00" );  
    
    std::cout << tm1.to_str() << std::endl;
    std::cout << tm2.to_str() << std::endl;
    EXPECT_TRUE( tm1.to_str() == "2021-06-01 00:00:00" ); 
    EXPECT_TRUE( tm2.to_str() == "2021-06-02 00:00:00" );
}

TEST_F( infra_time_test, compare_test )
{
    infra::sys_time tm1( "2021-06-01 00:00:00" );
    infra::sys_time tm2( "2021-06-02 00:00:00" );
    infra::sys_time tm3( "2021-06-02 00:00:00" );

    EXPECT_TRUE( tm3 > tm1 );
    EXPECT_TRUE( tm3 >= tm1 );
    EXPECT_TRUE( tm3 == tm2 );
    EXPECT_TRUE( tm3 <= tm2 );
    EXPECT_TRUE( tm3 != tm1 );
    EXPECT_TRUE( tm1 < tm2 );
    EXPECT_TRUE( tm1 <= tm3 );
}

TEST_F( infra_time_test, diff_test )
{
    infra::sys_time tm1( "2021-06-01 00:00:00" ); 
    infra::sys_time tm2( "2021-06-02 00:00:00" );

    EXPECT_EQ( tm2.diff( tm1 ), 24 * 60 * 60 ); 
}

TEST_F( infra_time_test, int_construct )
{
    infra::sys_time compare( infra::sys_time::current_sec() );
    infra::sys_time now;
    EXPECT_TRUE( compare.to_str() == now.to_str() );
}