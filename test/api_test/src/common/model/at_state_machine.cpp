
#include "model/state_machine.h"
#include "memory/tuple_param.h"
#include "gtest/gtest.h"
#include <functional>

USING_NAMESPACE_TARO

// 状态定义
enum state_e
{
    state_idle,
    state_busy,
    state_stop,
};

// 事件定义
using evt_param_ptr = memory::type_param_base_ptr<int32_t>;

/*事件类型*/
constexpr static const int32_t TEST_EVENT1 = 1;
constexpr static const int32_t TEST_EVENT2 = 2;
constexpr static const int32_t TEST_EVENT3 = 3;

/*事件参数*/
using event_param_1 = memory::int_tuple_param<TEST_EVENT1, int32_t>;
using event_param_2 = memory::int_tuple_param<TEST_EVENT2, std::string, double>;
using event_param_3 = memory::int_tuple_param<TEST_EVENT3, std::function<int()> >;

// 闲置状态
struct idle_state : public model::state_base<state_e, evt_param_ptr>
{
    idle_state()
        : model::state_base<state_e, evt_param_ptr>( state_idle )
        , init( false )
        , uinit( false )
        , value( -1 )
    {

    }

    virtual void initialize() override
    {
        // 切换到该状态前的处理
        init = true;
    }

    virtual void uninitialize() override
    {
        // 将该状态切换出去做的处理
        uinit = true;
    }

    virtual void handle( evt_param_ptr arg ) override
    {
        if ( arg->type() == TEST_EVENT1 )
        {
            auto& evt = memory::to_param_ref<event_param_1>( arg );
            value = evt.get<0>(); // 获取第一个参数
            to_next_state( state_busy );
        }
    }

    bool init;
    bool uinit;
    int32_t value;
};

// 忙碌状态
struct busy_state : public model::state_base<state_e, evt_param_ptr>
{
    busy_state()
        : model::state_base<state_e, evt_param_ptr>( state_busy )
        , init( false )
        , uinit( false )
    {

    }

    virtual void initialize() override
    {
        // 切换到该状态前的处理
        init = true;
    }

    virtual void uninitialize() override
    {
        // 将该状态切换出去做的处理
        uinit = true;
    }

    virtual void handle( evt_param_ptr arg ) override
    {
        if ( arg->type() == TEST_EVENT2 )
        {
            auto& evt = memory::to_param_ref<event_param_2>( arg );
            value  = evt.get<0>(); // 获取第一个参数
            value1 = evt.get<1>(); // 获取第二个参数
            to_next_state( state_stop );
        }
    }

    bool init;
    bool uinit;
    std::string value;
    double      value1;
};

// 停止状态
struct stop_state : public model::state_base<state_e, evt_param_ptr>
{
    stop_state()
        : model::state_base<state_e, evt_param_ptr>( state_stop )
        , init( false )
        , uinit( false )
    {
        
    }

    virtual void initialize() override
    {
        // 切换到该状态前的处理
        init = true;
    }

    virtual void uninitialize() override
    {
        // 将该状态切换出去做的处理
        uinit = true;
    }

    virtual void handle( evt_param_ptr arg ) override
    {
        if ( arg->type() == TEST_EVENT3 )
        {
            auto& evt = memory::to_param_ref<event_param_3>( arg );
            func1 = evt.get<0>();
            to_next_state( state_idle );
        }
    }

    bool init;
    bool uinit;
    std::function<int()> func1;
};


class state_machine_test : public testing::Test
{
    virtual void SetUp() override
    {
    }

    virtual void TearDown() override
    {
    }
};

TEST_F( state_machine_test, ok )
{
    model::state_machine< state_e, evt_param_ptr > state_mach;

    // 添加状态
    auto idle_st = std::make_shared<idle_state>();
    auto busy_st = std::make_shared<busy_state>();
    auto stop_st = std::make_shared<stop_state>();

    state_mach.add_state( state_idle, idle_st );
    state_mach.add_state( state_busy, busy_st );
    state_mach.add_state( state_stop, stop_st );

    // 设置初始状态
    state_mach.set_init_state(state_idle);
    EXPECT_TRUE( idle_st->init );
    EXPECT_EQ( state_mach.cur_state(), state_idle );

    // 状态切换
    state_mach.handle( memory::make_param<event_param_1>( 12 ) );
    EXPECT_TRUE( idle_st->uinit );
    EXPECT_TRUE( busy_st->init );
    EXPECT_EQ( 12, idle_st->value );
    EXPECT_EQ( state_mach.cur_state(), state_busy );

    state_mach.handle( memory::make_param<event_param_2>( "hello", -100.99 ) );
    EXPECT_TRUE( busy_st->uinit );
    EXPECT_TRUE( stop_st->init );
    EXPECT_DOUBLE_EQ( -100.99, busy_st->value1 );
    EXPECT_TRUE( busy_st->value == std::string("hello") );
    EXPECT_EQ( state_mach.cur_state(), state_stop );

    idle_st->init = false;
    state_mach.handle( memory::make_param<event_param_3>( std::bind( [](){ return -100; } ) ) );
    EXPECT_TRUE( stop_st->uinit );
    EXPECT_TRUE( idle_st->init );
    EXPECT_EQ( -100, stop_st->func1() );
    EXPECT_EQ( state_mach.cur_state(), state_idle );
}
