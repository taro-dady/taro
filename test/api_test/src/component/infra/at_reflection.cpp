
#include "infra/inc.h"
#include "gtest/gtest.h"
#include <string>

class infra_reflection_test : public testing::Test
{
public: // 公共函数

};

USING_NAMESPACE_TARO

struct reflect_test
{
    int         a;
    std::string b;
    double      c;
};

struct reflect_test1
{
    int         a;
    std::string b;
    double      c;
};
REFLECTION( reflect_test1, a, b, c );

TEST_F( infra_reflection_test, member_serial_test )
{
    std::shared_ptr<infra::variable_reflection_base> a_m 
        = std::make_shared< infra::variable_reflection< reflect_test, decltype( &reflect_test::a ) > >( &reflect_test::a );

    std::shared_ptr<infra::variable_reflection_base> b_m
        = std::make_shared< infra::variable_reflection< reflect_test, decltype( &reflect_test::b ) > >( &reflect_test::b, "" );

    std::shared_ptr<infra::variable_reflection_base> c_m
        = std::make_shared< infra::variable_reflection< reflect_test, decltype( &reflect_test::c ) > >( &reflect_test::c );

    reflect_test test;
    test.a = 100;
    test.b = "hello";
    test.c = -0.1998;

    auto ret = a_m->serialization( ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_TRUE( std::string( "100" ) == ret.get_value() );

    ret = b_m->serialization( ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_TRUE( std::string( "hello" ) == ret.get_value() );

    ret = c_m->serialization( ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_TRUE( std::string( "-0.1998" ) == ret.get_value() );
}

TEST_F( infra_reflection_test, member_deserial_test )
{
    std::shared_ptr<infra::variable_reflection_base> a_m
        = std::make_shared< infra::variable_reflection< reflect_test, decltype( &reflect_test::a ) > >( &reflect_test::a );

    std::shared_ptr<infra::variable_reflection_base> b_m
        = std::make_shared< infra::variable_reflection< reflect_test, decltype( &reflect_test::b ) > >( &reflect_test::b, "" );

    std::shared_ptr<infra::variable_reflection_base> c_m
        = std::make_shared< infra::variable_reflection< reflect_test, decltype( &reflect_test::c ) > >( &reflect_test::c );

    reflect_test test;
    test.a = -1;
    test.b = "";
    test.c = -1;
    auto ret = a_m->deserialization( "2021", ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_EQ( test.a, 2021 );

    ret = b_m->deserialization( "world", ( void* )&test );
    EXPECT_TRUE( ret );
    EXPECT_TRUE( std::string( "world" ) == test.b );

    ret = c_m->deserialization( "-20.98732", ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_DOUBLE_EQ( -20.98732, test.c );
}

TEST_F( infra_reflection_test, class_member_test )
{
    std::shared_ptr<infra::variable_reflection_base> a_m
        = std::make_shared< infra::variable_reflection< reflect_test, decltype( &reflect_test::a ) > >( &reflect_test::a );
    a_m->set_name( "a" );

    std::shared_ptr<infra::variable_reflection_base> b_m
        = std::make_shared< infra::variable_reflection< reflect_test, decltype( &reflect_test::b ) > >( &reflect_test::b, "" );
    a_m->set_name( "b" );

    std::shared_ptr<infra::variable_reflection_base> c_m
        = std::make_shared< infra::variable_reflection< reflect_test, decltype( &reflect_test::c ) > >( &reflect_test::c );
    c_m->set_name( "c" );

    auto& inst = infra::reflection_manager::instance();
    EXPECT_EQ( inst.register_class( "reflect_test", &typeid( reflect_test ) ), errno_ok );
    EXPECT_EQ( inst.register_class_member( "reflect_test", a_m ), errno_ok );
    EXPECT_EQ( inst.register_class_member( "reflect_test", b_m ), errno_ok );
    EXPECT_EQ( inst.register_class_member( "reflect_test", c_m ), errno_ok );
    
    auto find_ret = inst.find_class_name( &typeid( reflect_test ) );
    EXPECT_TRUE( find_ret == "reflect_test" );

    auto reflections = inst.get_class_members( "reflect_test" ); 
    EXPECT_EQ( 3, reflections.size() );
    EXPECT_TRUE( reflections[0] == a_m );
    EXPECT_TRUE( reflections[1] == b_m );
    EXPECT_TRUE( reflections[2] == c_m );
}

TEST_F( infra_reflection_test, macro_test )
{
    auto& inst = infra::reflection_manager::instance();
    auto find_ret = inst.find_class_name( &typeid( reflect_test1 ) ); 
    EXPECT_TRUE( find_ret == "reflect_test1" );

    auto reflections = inst.get_class_members( "reflect_test1" );
    EXPECT_EQ( 3, reflections.size() );

    auto test = reflect_test1{ 2, "23232", -90.01 };
    auto ret = std::dynamic_pointer_cast<infra::variable_reflection_base>( reflections[0] )->serialization( ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_TRUE( std::string( "2" ) == ret.get_value() ); 

    ret = std::dynamic_pointer_cast< infra::variable_reflection_base >( reflections[1] )->serialization( ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_TRUE( std::string( "'23232'" ) == ret.get_value() );

    ret = std::dynamic_pointer_cast< infra::variable_reflection_base >( reflections[2] )->serialization( ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_TRUE( std::string( "-90.01" ) == ret.get_value() );

    auto ret1 = std::dynamic_pointer_cast< infra::variable_reflection_base >( reflections[0] )->deserialization( "200", ( ( void* )&test ) );
    EXPECT_TRUE( ret1 );
    EXPECT_EQ( test.a, 200 );

    ret1 = std::dynamic_pointer_cast< infra::variable_reflection_base >( reflections[1] )->deserialization( "jack is girl", ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_TRUE( std::string( "jack is girl" ) == test.b );

    ret1 = std::dynamic_pointer_cast< infra::variable_reflection_base >( reflections[2] )->deserialization( "-290.08765", ( ( void* )&test ) );
    EXPECT_TRUE( ret );
    EXPECT_DOUBLE_EQ( -290.08765, test.c );
}
