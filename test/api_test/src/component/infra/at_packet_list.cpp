
#include "infra/inc.h"
#include "gtest/gtest.h"
#include "function/inc.h"
#include <string>
#include <vector>
#include <algorithm> 

class infra_packetlist_test : public testing::Test
{
public: // 公共函数

};

USING_NAMESPACE_TARO

TEST_F( infra_packetlist_test, append_test )
{
    infra::packet_list pktlist;

    auto packet1 = infra::create_default_packet( 128 );
    packet1->append( (uint8_t*)"hello ", strlen( "hello " ) );

    auto packet2 = infra::create_default_packet( 128 );
    packet2->append( (uint8_t*)"world", strlen( "world" ) );

    EXPECT_EQ( pktlist.append( packet1 ), errno_ok );
    EXPECT_EQ( pktlist.append( packet2 ), errno_ok );

    EXPECT_EQ( pktlist.size(), 11 );
}

TEST_F( infra_packetlist_test, consume_test )
{
    infra::packet_list pktlist;

    auto packet1 = infra::create_default_packet( 128 );
    packet1->append( ( uint8_t* )"hello ", strlen( "hello " ) );

    auto packet2 = infra::create_default_packet( 128 );
    packet2->append( ( uint8_t* )"world", strlen( "world" ) );

    EXPECT_EQ( pktlist.append( packet1 ), errno_ok );
    EXPECT_EQ( pktlist.append( packet2 ), errno_ok );
    EXPECT_EQ( pktlist.size(), 11 );

    EXPECT_NE( pktlist.consume( 12 ), errno_ok );

    EXPECT_NE( pktlist.consume( 6 ), errno_ok );
    EXPECT_EQ( pktlist.size(), 5 );
}

TEST_F( infra_packetlist_test, try_read_test )
{
    infra::packet_list pktlist;

    auto packet1 = infra::create_default_packet( 128 );
    packet1->append( ( uint8_t* )"hello ", strlen( "hello " ) );

    auto packet2 = infra::create_default_packet( 128 );
    packet2->append( ( uint8_t* )"world", strlen( "world" ) );

    EXPECT_EQ( pktlist.append( packet1 ), errno_ok );
    EXPECT_EQ( pktlist.append( packet2 ), errno_ok );
    EXPECT_EQ( pktlist.size(), 11 );

    uint8_t buf[128] = { 0 };
    EXPECT_EQ( pktlist.try_read( buf, 12, 0 ), 11 );
    EXPECT_EQ( pktlist.size(), 11 );
    EXPECT_TRUE( strcmp( (char*)buf, "hello world" ) == 0 );
}

TEST_F( infra_packetlist_test, read_test )
{
    infra::packet_list pktlist;

    auto packet1 = infra::create_default_packet( 128 );
    packet1->append( ( uint8_t* )"hello ", strlen( "hello " ) );

    auto packet2 = infra::create_default_packet( 128 );
    packet2->append( ( uint8_t* )"world", strlen( "world" ) );

    EXPECT_EQ( pktlist.append( packet1 ), errno_ok );
    EXPECT_EQ( pktlist.append( packet2 ), errno_ok );
    EXPECT_EQ( pktlist.size(), 11 );

    auto ret_packet = pktlist.read( 5 );
    EXPECT_TRUE( ret_packet != nullptr ); 
    EXPECT_EQ( pktlist.size(), 6 );
    EXPECT_EQ( ret_packet->size(), 5 );
    EXPECT_TRUE( std::string( ( char* )ret_packet->buffer(), 5 ) == "hello" );
}

TEST_F( infra_packetlist_test, search_test )
{
    infra::packet_list pktlist;

    auto packet1 = infra::create_default_packet( 128 );
    packet1->append( ( uint8_t* )"hello ", strlen( "hello " ) );

    auto packet2 = infra::create_default_packet( 128 );
    packet2->append( ( uint8_t* )"wORrld", strlen( "wORld" ) );

    EXPECT_EQ( pktlist.append( packet1 ), errno_ok );
    EXPECT_EQ( pktlist.append( packet2 ), errno_ok );
    EXPECT_EQ( pktlist.size(), 11 );

    uint32_t pos = 0;
    auto ret = pktlist.search( "wor", 2, pos, []( std::string a, std::string b ) 
    {
        std::transform( a.begin(), a.end(), a.begin(), ::toupper ); // tolower
        std::transform( b.begin(), b.end(), b.begin(), ::toupper );
        return a == b;
    } );
    EXPECT_TRUE( ret );
    EXPECT_EQ( pos, 6 );
}
