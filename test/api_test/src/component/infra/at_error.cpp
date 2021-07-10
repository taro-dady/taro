
#include "infra/inc.h"
#include "gtest/gtest.h"
#include <string.h>

USING_NAMESPACE_TARO

class error_test : public testing::Test
{
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }
};
 
TEST_F( error_test, set_and_get )
{ 
    infra::set_err_no( errno_memory_out );
    EXPECT_EQ( errno_memory_out, infra::get_err_no() ); 
 
    infra::set_err_msg( "errno num is 0" );
    EXPECT_TRUE( 0 == strcmp( infra::get_err_msg(), "errno num is 0" ) );
}
