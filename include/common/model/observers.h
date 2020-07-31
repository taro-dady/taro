
#pragma once

#include "model/defs.h"
#include "function/function.h"
#include <list>
#include <mutex>
#include <algorithm>

NAMESPACE_TARO_MODEL_BEGIN

template<class ...Args>
class observers
{
public: // 公共类型

    // 观察者类型
    using observer_type = func::function<void( Args... )>;

public: // 公共函数定义

    /*
    * @brief: 后部添加观察者
    */
    bool push_back( observer_type const& f )
    {
        if( !f )
        {
            return false;
        }

        std::lock_guard<std::mutex> g( _lock );
        if( existed( f, bool() ) )
        {
            return false;
        }
        _list.emplace_back( f );
        return true;
    }

    /*
    * @brief: 前部添加观察者
    */
    bool push_front( observer_type const& f )
    {
        if( !f )
        {
            return false;
        }

        std::lock_guard<std::mutex> g( _lock );
        if( existed( f, bool() ) )
        {
            return false;
        }
        _list.emplace_front( f );
        return true;
    }

    /*
    * @brief: 删除观察者
    */
    bool remove( observer_type const& f )
    {
        if( !f )
        {
            return false;
        }

        std::lock_guard<std::mutex> g( _lock );
        auto it = std::find( std::begin( _list ), std::end( _list ), f );
        if( it != std::end( _list ) )
        {
            _list.erase( it );
            return true;
        }
        return false;
    }

    /*
    * @brief: 判断观察者是否存在
    */
    bool existed( observer_type const& f ) const
    {
        std::lock_guard<std::mutex> g( _lock );
        return existed( f, bool() );
    }

    /*
    * @brief: 通知所有观察者
    */
    void operator()( Args... args )
    {
        std::list<observer_type> temp;
        {
            std::lock_guard<std::mutex> g( _lock );
            temp = _list;
        }

        std::list<observer_type> remove_list;
        for( auto& f : temp )
        {
            try
            {
                f( args... );
            }
            catch( func::exception const& e )
            {
                // 观察者已失效需要被删除
                if( func::exception_weak_pointer_expired == e )
                {
                    remove_list.emplace_back( f );
                }
            }
        }

        if( !remove_list.empty() )
        {
            std::lock_guard<std::mutex> g( _lock );
            for( auto& f : remove_list )
            {
                auto it = std::find( std::begin( _list ), std::end( _list ), f );
                if( it != std::end( _list ) )
                {
                    _list.erase( it );
                }
            }
        }
    }

private: // 私有函数

    /*
    * @brief: 判断函数对象是否存在
    */
    bool existed( observer_type const& f, bool )
    {
        return std::find( std::begin( _list ), std::end( _list ), f ) != std::end( _list );
    }

private: // 私有变量

    mutable std::mutex       _lock;
    std::list<observer_type> _list;
};

NAMESPACE_TARO_MODEL_END
