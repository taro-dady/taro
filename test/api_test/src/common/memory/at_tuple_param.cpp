
#include "gtest/gtest.h"
#include "memory/tuple_param.h"
#include <string>
#include <memory>

class tuple_param_test : public testing::Test
{
public: // 公共函数

};

USING_NAMESPACE_TARO

TEST_F(tuple_param_test, tuple_param)
{
	using test_type = memory::tuple_param< std::string, int >;
	auto param = memory::make_param<test_type>("test", 2);
	EXPECT_EQ(2, param->get<1>());
	EXPECT_TRUE( param->get<0>() == "test" );
}

TEST_F(tuple_param_test, type_tuple_param)
{
	using test_type = memory::type_tuple_param< std::string, int >;
	auto param = memory::make_param<test_type>( "test", 2 );
	EXPECT_EQ( 2, param->get<0>() );
}

TEST_F(tuple_param_test, int_tuple_param)
{
	using test_type = memory::int_tuple_param< 2, int >;
	auto param = memory::make_param<test_type>(20);
	EXPECT_EQ(20, param->get<0>());

	auto& ref = memory::to_param_ref<test_type>( std::dynamic_pointer_cast<  memory::type_param_base<int32_t> >( param ) );
	EXPECT_EQ(20, ref.get<0>());
}
