
#include "gtest/gtest.h"

int main( int argc, char** argv )
{
    testing::InitGoogleTest( &argc, argv );
    //testing::GTEST_FLAG( filter ) = "net_udp_group_test.*";
    auto ret = RUN_ALL_TESTS();
    return ret;
}
