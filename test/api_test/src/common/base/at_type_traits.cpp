
#include "gtest/gtest.h"
#include "base/inc.h"
#include <string.h>
#include <type_traits>
#include <cstdint>

USING_NAMESPACE_TARO

class type_traits_test : public testing::Test
{
};

TEST_F( type_traits_test, string_test )
{
    std::string v;
    EXPECT_TRUE( is_string< decltype( v ) >::value );

    std::string const& v1 = v;
    EXPECT_FALSE( is_string< decltype( v1 ) >::value );
    EXPECT_TRUE( is_string< unwapper_type< decltype( v1 ) >::type >::value );

    std::string& v2 = v;
    EXPECT_FALSE( is_string< decltype( v2 ) >::value );
    EXPECT_TRUE( is_string< unwapper_type< decltype( v2 ) >::type >::value );
}

TEST_F( type_traits_test, const_char_pointer_test )
{
    const char* v = 0;
    EXPECT_TRUE( is_string< decltype( v ) >::value );

    const char*& v1 = v;
    EXPECT_FALSE( is_string< decltype( v1 ) >::value );
    EXPECT_TRUE( is_string< unwapper_type< decltype( v1 ) >::type >::value );

    using tmp_type = unwapper_type< decltype( v1 ) >::type;
    auto ret = std::is_same< tmp_type, const char* >::value;
    EXPECT_TRUE( ret );
}

TEST_F( type_traits_test, char_pointer_test )
{
    char* v = 0;
    EXPECT_TRUE( is_string< decltype( v ) >::value );

    char*& v1 = v;
    EXPECT_FALSE( is_string< decltype( v1 ) >::value );
    EXPECT_TRUE( is_string< unwapper_type< decltype( v1 ) >::type >::value );

    char* const volatile v2 = v;
    EXPECT_FALSE( is_string< decltype( v2 ) >::value );
    EXPECT_TRUE( is_string< unwapper_type< decltype( v2 ) >::type >::value );
}

TEST_F( type_traits_test, char_test )
{
    EXPECT_FALSE( is_string< char >::value );
    EXPECT_FALSE( is_string< uint8_t >::value );
}

TEST_F( type_traits_test, int32_test )
{
    EXPECT_FALSE( is_string< int32_t >::value );
    EXPECT_FALSE( is_string< uint32_t >::value );
}

TEST_F( type_traits_test, int16_test )
{
    EXPECT_FALSE( is_string< int16_t >::value );
    EXPECT_FALSE( is_string< uint16_t >::value );
}

TEST_F( type_traits_test, float_test )
{
    EXPECT_FALSE( is_string< float >::value );
    EXPECT_FALSE( is_string< double >::value );
}
