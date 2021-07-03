
#pragma once

#include "function/creater.h"
#include "function/functor_chain.h"
#include <list>
#include <mutex>
#include <condition_variable>

NAMESPACE_TARO_FUNCTION_BEGIN

// 并行函数，提供对数据分片处理的函数框架
class parallel_functor
{   
public: // 公共函数

    /*
    * @brief 构造函数
    */
    parallel_functor()
        : _size( 0 )
    {

    }

    /*
    * @brief 添加函数
    */
    template<class F>
    parallel_functor& add( F f )
    {
        auto ptr = chain_functor_creater<F, function_traits<decltype( f )>::arg_num == 0>::create( f );
        _p.emplace_back( ptr );
        return *this;
    }

    /*
    * @brief 异步调用
    */
    template<typename P>
    void async_call( P& thrd_pool )
    {
        _size = ( uint32_t )_p.size();
        for ( auto const& one : _p )
        {
            thrd_pool.push( 
                func::bind(
                [=]() {
                    chain_arg_ptr in, out;
                    one->invoke( in, out );
                    
                    std::unique_lock<std::mutex> g( _mutex );
                    if ( --_size == 0 )
                    {
                        _cond.notify_one();
                    }
                }
            ) );
        }
    }

    /*
    * @brief 等待调用结束
    */
    void wait()
    {
        std::unique_lock<std::mutex> g( _mutex );
        _cond.wait( g, [&]() { return _size == 0; } );
    }

private: // 私有变量

    uint32_t                _size;
    std::mutex              _mutex;
    std::condition_variable _cond;
    std::vector< std::shared_ptr<chain_functor_base> > _p;
};

NAMESPACE_TARO_FUNCTION_END
