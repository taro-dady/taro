
#pragma once

#include "system/defs.h"
#include <list>
#include <mutex>
#include <thread>
#include <algorithm>
#include <condition_variable>

NAMESPACE_TARO_SYSTEM_BEGIN

template<class D>
class data_queue
{
public: /// 公共函数定义

    /*
    * @brief: 添加数据
    *
    * @param[in] data   被添加的数据
    * @param[in] front  是否被添加到头部 true表示添加到头部 false添加到尾部
    */
    void push( D const& data, bool front = false )
    {
        std::lock_guard<std::mutex> g( _mutex );
        if( front )
            _queue.emplace_front( data );
        else
            _queue.emplace_back( data );
        _cond.notify_one();
    }

    /*
    * @brief:  从队列头部获取数据
    *
    * @return  获取的数据
    */
    D take()
    {
        std::unique_lock<std::mutex> g( _mutex );
        _cond.wait( g, [&]() { return !_queue.empty(); } );

        D ret = _queue.front();
        _queue.pop_front();
        return ret;
    }

    /*
    * @brief:    从队列头部获取数据，带有等待超时时间
    *
    * @param[out] data  获取的数据
    * @param[in]  ms    等待超时的时间
    * @return  true 获取成功  false 获取失败
    */
    bool take( D& data, uint32_t ms = 2000 )
    {
        std::unique_lock<std::mutex> g( _mutex );
        if( !_cond.wait_for( g, 
                             std::chrono::milliseconds( ms ), 
                             [&]() { return !_queue.empty(); } ) )
        {
            return false;
        }

        data = _queue.front();
        _queue.pop_front();
        return true;
    }

    /*
    * @brief:    从队列中挑选指定的数据
    *
    * @param[out] data    获取的数据
    * @param[in]  matcher 用于匹配数据的对象
    * @param[in]  ms      等待超时的时间
    * @return  获取的数据
    */
    template<class Matcher>
    bool pick( D& data, Matcher matcher, int32_t ms = 2000 )
    {
        auto cond = [&]()
        {
            auto it = std::find_if( std::begin( _queue ), std::end( _queue ), [&]( D const& one )
            {
                return matcher( one );
            } );

            if ( it == std::end( _queue ) )
            {
                return false;
            }

            data = *it;
            _queue.erase( it );
            return true;
        };

        std::unique_lock<std::mutex> g( _mutex );
        return _cond.wait_for( g, 
                             std::chrono::milliseconds( ms ), 
                             cond );
    }

    /*
    * @brief:  获取队列大小 
    */
    uint32_t size() const
    {
        std::unique_lock<std::mutex> g( _mutex );
        return ( uint32_t )_queue.size();
    }

    /*
    * @brief:  清空队列
    */
    void clear()
    {
        std::unique_lock<std::mutex> g( _mutex );
        _queue.clear();
    }

private: /// 私有变量

    std::list<D>            _queue;
    mutable std::mutex      _mutex;
    std::condition_variable _cond;
};

NAMESPACE_TARO_SYSTEM_END
