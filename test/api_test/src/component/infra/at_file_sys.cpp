
#include "infra/inc.h"
#include "gtest/gtest.h"
#include <string.h>
#include <fstream>

USING_NAMESPACE_TARO

class file_sys_test : public testing::Test
{
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }
};

TEST_F( file_sys_test, dir_create_remove )
{
    const char* dir_name = "file_sys_test";
    auto& inst = infra::file_sys::instance();
    EXPECT_EQ( inst.create_dir( dir_name ), errno_ok );
    EXPECT_EQ( inst.is_dir_exist( dir_name ), errno_ok );
    EXPECT_EQ( inst.remove_dir( dir_name ), errno_ok );
    EXPECT_NE( inst.is_dir_exist( dir_name ), errno_ok );
}

TEST_F( file_sys_test, copy_dir )
{
    const char* dir_name = "file_sys_test";
    auto& inst = infra::file_sys::instance();
    EXPECT_EQ( inst.create_dir( dir_name ), errno_ok );
    EXPECT_EQ( inst.is_dir_exist( dir_name ), errno_ok );

    std::string file = dir_name;
    file += "/mytestt.txt";
    std::ofstream of( file );
    EXPECT_TRUE( of );
    std::string context = "hello 1, hello 2, hello 3, hello 4";
    of << context;
    of.close();
    EXPECT_EQ( inst.is_file_exist( file.c_str() ), errno_ok );

    const char* dst_dir = "file_sys_test1";
    EXPECT_EQ( inst.copy_dir( dir_name, dst_dir ), errno_ok );
    EXPECT_EQ( inst.is_dir_exist( dst_dir ), errno_ok );

    file = dst_dir;
    file += "/mytestt.txt";
    EXPECT_EQ( inst.is_file_exist( file.c_str() ), errno_ok );

    EXPECT_EQ( inst.remove_dir( dir_name ), errno_ok );
    EXPECT_NE( inst.is_dir_exist( dir_name ), errno_ok );

    EXPECT_EQ( inst.remove_dir( dst_dir ), errno_ok );
    EXPECT_NE( inst.is_dir_exist( dst_dir ), errno_ok );
}

TEST_F( file_sys_test, file_test )
{
    auto& inst = infra::file_sys::instance();

    std::string file;
    file += "mytestt.txt";
    std::ofstream of( file );
    EXPECT_TRUE( of );
    std::string context = "hello 1, hello 2, hello 3, hello 4";
    of << context;
    of.close();
    EXPECT_EQ( inst.is_file_exist( file.c_str() ), errno_ok );

    uint64_t bytes = 0;
    EXPECT_EQ( inst.get_file_size( file.c_str(), bytes ), errno_ok );
    EXPECT_TRUE( bytes > 0 );

    std::string newfile = "mytest11.txt";
    EXPECT_EQ( inst.copy_file( file.c_str(), newfile.c_str() ), errno_ok );
    bytes = 0;
    EXPECT_EQ( inst.get_file_size( newfile.c_str(), bytes ), errno_ok );
    EXPECT_TRUE( bytes > 0 );

    EXPECT_EQ( inst.remove_file( file.c_str() ), errno_ok );
    EXPECT_NE( inst.is_file_exist( file.c_str() ), errno_ok );

    EXPECT_EQ( inst.remove_file( newfile.c_str() ), errno_ok );
    EXPECT_NE( inst.is_file_exist( newfile.c_str() ), errno_ok );
}

TEST_F( file_sys_test, get_dir_info )
{
    const char* dir_name = "file_sys_test";
    auto& inst = infra::file_sys::instance();
    EXPECT_EQ( inst.create_dir( dir_name ), errno_ok );
    EXPECT_EQ( inst.is_dir_exist( dir_name ), errno_ok );

    std::string file = dir_name;
    file += "/mytestt.txt";
    std::ofstream of( file );
    EXPECT_TRUE( of );
    std::string context = "hello 1, hello 2, hello 3, hello 4";
    of << context;
    of.close();
    EXPECT_EQ( inst.is_file_exist( file.c_str() ), errno_ok );

    std::string sub = dir_name;
    sub += "/sub_dir";
    EXPECT_EQ( inst.create_dir( sub.c_str() ), errno_ok );
    EXPECT_EQ( inst.is_dir_exist( sub.c_str() ), errno_ok );

    auto nodes = inst.get_nodes( dir_name );
    EXPECT_EQ( 2, nodes.size() );
    
    for ( auto& one : nodes )
    {
        if ( one.type == infra::file_sys::node_type_file )
        {
            EXPECT_TRUE( one.name == "mytestt.txt" );
        }
        else
        {
            EXPECT_TRUE( one.name == "sub_dir" );
        }
    }

    EXPECT_EQ( inst.remove_dir( dir_name ), errno_ok );
    EXPECT_NE( inst.is_dir_exist( dir_name ), errno_ok );
}
