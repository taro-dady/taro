
#include "gtest/gtest.h"

int main( int argc, char** argv )
{
    testing::InitGoogleTest( &argc, argv );
    //testing::GTEST_FLAG( filter ) = "infra_time_test.*";   
    auto ret = RUN_ALL_TESTS();  
    return ret;
}
