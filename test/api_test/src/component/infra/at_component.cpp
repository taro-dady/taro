
#include "infra/inc.h"
#include "gtest/gtest.h"

USING_NAMESPACE_TARO

namespace {

    // 鸟的基类
    struct bird
    {
        virtual ~bird() = default;
        virtual const char* fly() const = 0;
    };

    // 定义一个鸟的工厂类
    struct bird_factory : public infra::component_factory
    {
    public:

        using component_type = bird;

        virtual ~bird_factory() = default;
        virtual std::shared_ptr<bird> get( const char* url = nullptr ) const = 0;

        static infra::component_key key( const char* cid )
        {
            return infra::component_key( iid(), cid );
        }

    protected:

        static const char* iid()
        {
            return "bird";
        }
    };

    // 定义鸭子
    struct duck : public bird
    {
        virtual const char* fly() const override final
        {
            return "duck";
        }
    };

    struct duck_factory : public bird_factory
    {
        bool init()
        {
            return register_factory( bird_factory::key( "duck" ), shared_from_this() );
        }

        bool uinit()
        {
            return deregister_factory( bird_factory::key( "duck" ) );
        }

        virtual std::shared_ptr<bird> get( const char* url ) const override final
        {
            return std::make_shared<duck>();
        }
    };

    // 鸡的类型
    struct chicken : public bird
    {
        virtual const char* fly() const override final
        {
            return "chicken";
        }
    };

    struct chicken_factory : public bird_factory
    {
        bool init()
        {
            return register_factory( bird_factory::key( "chicken" ), shared_from_this() );
        }

        bool uinit()
        {
            return deregister_factory( bird_factory::key( "chicken" ) );
        }

        virtual std::shared_ptr<bird> get( const char* url ) const override final
        {
            return std::make_shared<chicken>();
        }
    };


    class infra_component_test : public testing::Test
    {
        virtual void SetUp() override
        {
        }

        virtual void TearDown() override
        {
        }
    };

    TEST_F( infra_component_test, register_ok )
    {
        EXPECT_TRUE( std::make_shared<duck_factory>()->init() );
        EXPECT_TRUE( std::make_shared<duck_factory>()->uinit() );
        EXPECT_TRUE( std::make_shared<duck_factory>()->init() );
        EXPECT_TRUE( std::make_shared<duck_factory>()->uinit() );
    }

    TEST_F( infra_component_test, register_failed )
    {
        EXPECT_TRUE( std::make_shared<duck_factory>()->init() );
        EXPECT_FALSE( std::make_shared<duck_factory>()->init() );
        EXPECT_TRUE( std::make_shared<duck_factory>()->uinit() );
        EXPECT_FALSE( std::make_shared<duck_factory>()->uinit() );
    }

    TEST_F( infra_component_test, get_obj )
    {
        // 注册工厂
        std::make_shared<duck_factory>()->init();
        std::make_shared<chicken_factory>()->init();

        auto bird_ptr = infra::get_component< bird_factory >( "duck" );
        EXPECT_TRUE( std::string( "duck" ) == bird_ptr->fly() );

        bird_ptr = infra::get_component< bird_factory >( "chicken" );
        EXPECT_TRUE( std::string( "chicken" ) == bird_ptr->fly() ); 

        // 注销工厂
        std::make_shared<duck_factory>()->uinit();
        std::make_shared<chicken_factory>()->uinit();
    }
} 