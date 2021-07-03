
#include "gtest/gtest.h"
#include "base/inc.h"
#include <string.h>

USING_NAMESPACE_TARO

class result_test : public testing::Test
{
};

TEST_F( result_test, set_err_result )
{
    result<int> res( 2 );
    res.set_errno( errno_unaccess );
    res.set_err_msg( "unaccess" );
    EXPECT_FALSE( res );
    EXPECT_EQ( res.get_errno(), errno_unaccess );
    EXPECT_EQ( res.get_value(), 2 );
    EXPECT_TRUE( strcmp( res.get_err_msg(), "unaccess") == 0 );
}

TEST_F( result_test, set_ok_result )
{
    result<int> res( 2 );
    res.set_errno( errno_ok );
    EXPECT_TRUE( res );
    EXPECT_EQ( res.get_errno(), errno_ok );
    EXPECT_EQ( res.get_value(), 2 );
}

TEST_F( result_test, set_other_value )
{
    result<int> res( 2 );
    res.set_errno( errno_ok );
    EXPECT_TRUE( res );
    EXPECT_EQ( res.get_errno(), errno_ok );
    EXPECT_EQ( res.get_value(), 2 );

    res.set_value(10);
    EXPECT_EQ( res.get_value(), 10 );
}

