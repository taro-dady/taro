
#include "gtest/gtest.h"
#include "memory/any.h"
#include <string>

class any_test : public testing::Test  
{
public: // 公共函数

};

USING_NAMESPACE_TARO
USING_NAMESPACE_TARO_MEMORY

struct any_test_t
{
    any_test_t( double v )
        : a( v )
    {}

    double a;
};

TEST_F( any_test, set_get_int_ok )
{
    any a;
    a = 2;
    EXPECT_EQ( &a.type(), &typeid( int32_t ) );
    EXPECT_EQ( 2, a.cast<int32_t>() );
}

TEST_F( any_test, transfer_int_ok )
{
    any a;
    a = 2;
    auto b = a;
    EXPECT_EQ( &b.type(), &typeid( int32_t ) );
    EXPECT_EQ( 2, b.cast<int32_t>() );
}

TEST_F( any_test, set_get_string_ok )
{
    any a;
    a = std::string( "hello" );
    EXPECT_EQ( &a.type(), &typeid( std::string ) );
    EXPECT_EQ( std::string( "hello" ), a.cast<std::string>() );
}

TEST_F( any_test, transfer_string_ok )
{
    any a;
    a = std::string( "hello" );
    auto b = a;
    EXPECT_EQ( &b.type(), &typeid( std::string ) );
    EXPECT_EQ( std::string( "hello" ), b.cast<std::string>() );
}

TEST_F( any_test, set_get_struct_ok )
{
    any a;
    a = any_test_t( 2.29991 );
    EXPECT_EQ( &a.type(), &typeid( any_test_t ) );
    EXPECT_FLOAT_EQ( 2.29991, a.cast<any_test_t>().a );
}

TEST_F( any_test, transfer_struct_ok )
{
    any a;
    a = any_test_t( 2.29991 );
    auto b = a;
    EXPECT_EQ( &a.type(), &typeid( any_test_t ) );
    EXPECT_FLOAT_EQ( 2.29991, a.cast<any_test_t>().a );
}
