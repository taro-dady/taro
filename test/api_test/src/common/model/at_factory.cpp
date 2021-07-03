
#include "model/abstract_factory.h"
#include "gtest/gtest.h"
#include <string>

USING_NAMESPACE_TARO

std::string bird_fly;

// 鸟的基类
struct bird
{
    virtual ~bird() = default;
    virtual void fly() = 0;
};

// 定义一个鸟的工厂类
struct bird_factory : public model::abstract_factory<std::string>
{
    virtual ~bird_factory() = default;
    virtual std::shared_ptr<bird> get() const = 0;
};

// 定义鸭子
struct duck : public bird
{
    virtual void fly() override final
    {
        bird_fly = "duck";
    }
};

struct duck_factory : public bird_factory
{
    bool init()
    {
        return register_factory( "duck", shared_from_this() );
    }

    bool uinit()
    {
        return deregister_factory( "duck" );
    }

    virtual std::shared_ptr<bird> get() const override final 
    { 
        return std::make_shared<duck>();
    }
};

// 鸡的类型
struct chicken : public bird
{
    virtual void fly() override final
    {
        bird_fly = "chicken";
    }
};

struct chicken_factory : public bird_factory
{
    bool init()
    {
        return register_factory( "chicken", shared_from_this() );
    }

    bool uinit()
    {
        return deregister_factory( "chicken" );
    }

    virtual std::shared_ptr<bird> get() const override final 
    { 
        return std::make_shared<chicken>();
    }
};

class model_factory_test : public testing::Test
{
    virtual void SetUp() override
    {
        
    }

    virtual void TearDown() override
    {
        
    }
};

TEST_F( model_factory_test, register_ok )
{
    EXPECT_TRUE( std::make_shared<duck_factory>()->init() );
    EXPECT_TRUE( std::make_shared<duck_factory>()->uinit() );
    EXPECT_TRUE( std::make_shared<duck_factory>()->init() );
    EXPECT_TRUE( std::make_shared<duck_factory>()->uinit() );
}

TEST_F( model_factory_test, register_failed )
{
    EXPECT_TRUE( std::make_shared<duck_factory>()->init() );
    EXPECT_FALSE( std::make_shared<duck_factory>()->init() );
    EXPECT_TRUE( std::make_shared<duck_factory>()->uinit() );
    EXPECT_FALSE( std::make_shared<duck_factory>()->uinit() );
}

TEST_F( model_factory_test, get_obj )
{
    // 注册工厂
    std::make_shared<duck_factory>()->init();
    std::make_shared<chicken_factory>()->init();

    auto fac  = duck_factory::get_abstract_factory<bird_factory>( "duck" ); // 获取duck工厂
    auto bird = fac->get(); // 获取duck对象
    bird->fly();
    EXPECT_TRUE( bird_fly == "duck" );
    
    fac  = duck_factory::get_abstract_factory<bird_factory>( "chicken" );
    bird = fac->get();
    bird->fly();
    EXPECT_TRUE( bird_fly == "chicken" );

    // 注销工厂
    std::make_shared<duck_factory>()->uinit();
    std::make_shared<chicken_factory>()->uinit();
}
