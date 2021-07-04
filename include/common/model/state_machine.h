
#pragma once

#include "model/defs.h"
#include "utils/assert.h"
#include "memory/deleter.h"
#include "function/function.h"
#include <map>
#include <mutex>

NAMESPACE_TARO_MODEL_BEGIN

/// 状态
template<typename S, typename... Args>
class state_base;

/// 状态机
template<typename S, typename... Args>
class state_machine;

/// 状态机指针
template<typename S, typename... Args>
using state_base_ptr = std::shared_ptr< state_base<S, Args...> >;

/// 状态机对象
template<typename S, typename... Args>
class state_base
{
public: /// 公共函数

    /*
    * @brief   构造函数
    */
    state_base( S state )
        : _state( state )
    {
        
    }

protected : /// 保护函数定义

    /*
    * @brief      虚析构
    */
    virtual ~state_base()
    {

    }
    
    /*
    * @brief      初始化函数
    */
    virtual void initialize() 
    {

    }

    /*
    * @brief      反初始化函数
    */
    virtual void uninitialize() 
    {

    }

    /*
    * @brief      事件处理函数
    */
    virtual void handle( Args... args ) = 0;

    /*
    * @brief      转换状态
    *
    * @param[in]  next 下一个状态
    */
    void to_next_state( S const& next )
    {
        TARO_ASSERT( _change_cb );
        _change_cb( next );
    }

    /*
    * @brief      获取状态对象
    */
    state_base_ptr<S, Args...> get_state( S const& state )
    {
        TARO_ASSERT( _get_state_cb );
        return _get_state_cb( state );
    }

private: /// 私有类型

    /// 友元类
    friend class state_machine< S, Args... >;

    /// 状态切换函数
    using change_cb_t = func::function<void( S const& )>;

    /// 获取指定状态对象
    using get_state_cb_t = func::function<state_base_ptr<S, Args...>( S const& )>;

private: /// 私有变量

    S              _state;        // 状态
    change_cb_t    _change_cb;    // 状态转换函数
    get_state_cb_t _get_state_cb; // 获取指定状态对象函数
};

/// 状态机对象
template<typename S, typename... Args>
class state_machine
{
public: /// 公共函数定义

    /**
    * @brief      添加状态
    *
    * @param[in]  state      状态值
    * @param[in]  state_base 状态对象
    * @return     bool
    * @retval     true 成功 false 失败
    */
    bool add_state( S const& state, state_base_ptr<S, Args...> const& state_base )
    {
        if( nullptr == state_base )
        {
            return false;
        }

        std::lock_guard<std::mutex> g( _mutex );
        auto it = _state_map.find( state );
        if( it != _state_map.end() )
        {
            return false;
        }

        std::shared_ptr< this_type > this_ptr( this, memory::delete_do_nothing< this_type > );
        state_base->_change_cb = 
            func::function<void( S const& )>( &state_machine::on_state_change, this_ptr );
        state_base->_get_state_cb = 
            func::function<state_base_ptr<S, Args...>( S const& )>( &state_machine::get_state, this_ptr );

        _state_map[state] = state_base;
        return true;
    }

    /**
    * @brief      删除状态
    *
    * @param[in]  state         状态值
    * @return     bool
    * @retval     true 成功 false 失败
    */
    bool remove_state( S const& state )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _state_map.find( state );
        if( it == _state_map.end() )
        {
            return false;
        }
        _state_map.erase( it );
        return true;
    }

    /**
    * @brief      获取当前状态
    *
    * @return     S
    * @retval     当前状态
    */
    S cur_state() const
    {
        std::lock_guard<std::mutex> g( _mutex );
        TARO_ASSERT( _cur_state );
        return _cur_state->_state;
    }

    /**
    * @brief      设置初始状态
    *
    * @param[in]  state         状态值
    * @return     bool
    * @retval     true 成功 false 失败
    */
    void set_init_state( S state )
    {
        std::lock_guard<std::mutex> g( _mutex );

        TARO_ASSERT( nullptr == _cur_state );
        auto it = _state_map.find( state );

        TARO_ASSERT( it != _state_map.end() && it->second != nullptr );
        _cur_state = it->second;

        _cur_state->initialize();
    }

    /**
    * @brief      事件处理函数
    *
    * @param[in]  evt         事件对象
    */
    void handle( Args... args )
    {
        std::lock_guard<std::mutex> g( _mutex );

        TARO_ASSERT( nullptr != _cur_state );

        _cur_state->handle( args... );
    }

private: /// 私有类型定义

    /// 状态容器
    using state_map = std::map< S, state_base_ptr<S, Args...> >;

    /// 当前类的类型
    using this_type = state_machine<S, Args...>;

private: /// 私有函数定义

    /**
    * @brief      切换状态
    *
    * @param[in]  state  事件状态
    */
    void on_state_change( S const& state )
    {
        TARO_ASSERT( nullptr != _cur_state );

        if( _cur_state->_state == state )
        {
            return;
        }

        auto it = _state_map.find( state );
        TARO_ASSERT( it != _state_map.end() && nullptr != it->second );

        _cur_state->uninitialize();
        _cur_state = it->second;
        _cur_state->initialize();
    }

    /**
    * @brief      获取状态对象
    */
    state_base_ptr<S, Args...> get_state( S const& state )
    {
        auto it = _state_map.find( state );
        if( it == _state_map.end() )
        {
            return state_base_ptr<S, Args...>();
        }
        return it->second;
    }

private: /// 私有变量定义

    state_map                  _state_map;   ///< 状态机对象Map
    mutable std::mutex         _mutex;
    state_base_ptr<S, Args...> _cur_state;   ///< 当前状态
};

NAMESPACE_TARO_MODEL_END
