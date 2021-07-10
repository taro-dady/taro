
#include "infra/inc.h"
#include "gtest/gtest.h"

USING_NAMESPACE_TARO

#if defined WIN32 || defined WIN64
#define DYN_LIB_NAME "test_dynlib.dll"
#else // WIN32
#define DYN_LIB_NAME "libtest_dynlib.so"
#endif // WIN32

#define FUN_NAME "test_dynlib_add"

class infra_dynlib_test : public testing::Test
{
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }
};

typedef int( *fn_ptr )( int, int );

TEST_F( infra_dynlib_test, normal_test ) 
{   
    infra::dyn_lib dynlib;
    EXPECT_EQ( errno_ok,  dynlib.load( DYN_LIB_NAME ) );

    fn_ptr fn;
    fn = ( fn_ptr )dynlib.func_addr( FUN_NAME ); 
    EXPECT_TRUE( fn != nullptr );
    EXPECT_EQ( 15, fn( 10, 5 ) );

    EXPECT_EQ( errno_ok, dynlib.unload() );
}

TEST_F( infra_dynlib_test, multiple_load )
{    
    infra::dyn_lib dynlib;
    EXPECT_EQ( errno_ok,  dynlib.load( DYN_LIB_NAME ) ); 
    EXPECT_NE( errno_ok,  dynlib.load( DYN_LIB_NAME ) ); 

    fn_ptr fn;
    fn = ( fn_ptr )dynlib.func_addr( FUN_NAME );
    EXPECT_TRUE( fn != nullptr );
    EXPECT_EQ( 15, fn( 10, 5 ) );

    EXPECT_EQ( errno_ok, dynlib.unload() );
    EXPECT_NE( errno_ok, dynlib.unload() );
}

