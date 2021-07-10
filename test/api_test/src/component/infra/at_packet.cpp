
#include "infra/inc.h"
#include "gtest/gtest.h"
#include "function/inc.h"
#include <string>
#include <vector>

class infra_packet_test : public testing::Test
{
public: // 公共函数

};

USING_NAMESPACE_TARO

static bool my_extera_st_state = false;

struct my_extra_st
{
    my_extra_st()
        : a( 1 )
        , p( new int )
    {
        my_extera_st_state = false;
    }

    ~my_extra_st()
    {
        if ( nullptr != p )
            delete p;
        p = nullptr;
        my_extera_st_state = true;
    }

    int a;
    int* p;
};

void destruct_func( uint8_t* addr, uint32_t const& )
{
    ( ( my_extra_st* )addr )->~my_extra_st();
}

TEST_F( infra_packet_test, append_test )
{
    const char* str_ptr = "hello";
    auto packet = infra::create_default_packet( 1024 );
    EXPECT_TRUE( packet != nullptr );
    EXPECT_EQ( packet->append( ( uint8_t* )str_ptr, strlen( str_ptr ) + 1 ), errno_ok );
    EXPECT_TRUE( 0 == strcmp( ( char* )packet->buffer(), str_ptr ) );
    EXPECT_EQ( packet->size(), strlen( str_ptr ) + 1 );
}

TEST_F( infra_packet_test, expand_test )
{
    const char* str_ptr = "hello";
    auto packet = infra::create_default_packet();
    EXPECT_TRUE( packet != nullptr );
    EXPECT_EQ( errno_ok, packet->expand(128) );
    strcpy( (char*)packet->buffer(), str_ptr );
    EXPECT_EQ( packet->resize( strlen( str_ptr ) + 1 ), errno_ok );
    EXPECT_TRUE( 0 == strcmp( ( char* )packet->buffer(), str_ptr ) );
    EXPECT_EQ( packet->size(), strlen( str_ptr ) + 1 );
}

TEST_F( infra_packet_test, extra_test )
{
    const char* str_ptr = "hello";
    auto packet = infra::create_default_packet( 1024, 128 );
    EXPECT_TRUE( packet != nullptr );
    EXPECT_EQ( packet->append( ( uint8_t* )str_ptr, strlen( str_ptr ) + 1 ), errno_ok );
    EXPECT_TRUE( 0 == strcmp( ( char* )packet->buffer(), str_ptr ) );
    EXPECT_EQ( packet->size(), strlen( str_ptr ) + 1 );

    EXPECT_EQ( packet->extra_data_size(), 128 );
    EXPECT_TRUE( packet->extra_data() != nullptr );

    EXPECT_EQ( packet->expand_extra_data(256), errno_ok );   
    EXPECT_EQ( packet->extra_data_size(), 256 );
    EXPECT_TRUE( packet->extra_data() != nullptr );
}

TEST_F( infra_packet_test, extra_test_destruct )
{
    const char* str_ptr = "hello";
    auto packet = infra::create_default_packet( 1024, 128, 8, func::make_func( &destruct_func ) );
    EXPECT_TRUE( packet != nullptr );
    EXPECT_EQ( packet->append( ( uint8_t* )str_ptr, strlen( str_ptr ) + 1 ), errno_ok );
    EXPECT_TRUE( 0 == strcmp( ( char* )packet->buffer(), str_ptr ) );
    EXPECT_EQ( packet->size(), strlen( str_ptr ) + 1 );

    EXPECT_EQ( packet->extra_data_size(), 128 );
    EXPECT_TRUE( packet->extra_data() != nullptr );

    new ( packet->extra_data() ) my_extra_st();
    EXPECT_FALSE( my_extera_st_state );
    packet.reset();
    EXPECT_TRUE( my_extera_st_state );
}

TEST_F( infra_packet_test, offset_test )
{
    const char* str_ptr = "hello";
    auto packet = infra::create_default_packet( 1024 );
    EXPECT_TRUE( packet != nullptr );
    EXPECT_EQ( packet->append( ( uint8_t* )str_ptr, strlen( str_ptr ) + 1 ), errno_ok );
    EXPECT_TRUE( 0 == strcmp( ( char* )packet->buffer(), str_ptr ) );
    EXPECT_EQ( packet->size(), strlen( str_ptr ) + 1 );

    packet->set_head_offset( 2 );
    packet->resize( strlen( str_ptr ) - 1 );

    const char* tmp = str_ptr + 2;
    EXPECT_TRUE( 0 == strcmp( ( char* )packet->buffer(), tmp ) );
    EXPECT_EQ( packet->size(), strlen( tmp ) + 1 );
}
