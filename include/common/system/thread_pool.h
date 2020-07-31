
#pragma once

#include "utils/defer.h"
#include "utils/assert.h"
#include "system/data_queue.h"
#include <map>
#include <memory>
#include <atomic>
#include <thread>
#include <vector>

NAMESPACE_TARO_SYSTEM_BEGIN

template<class A>
class thread_lite : public std::enable_shared_from_this< thread_lite<A> >
{
public: // 公共函数定义

    /*
    * @brief: 构造函数
    */
    thread_lite()
        : _looping( false )
        , _cnt( 0 )
    {

    }

    /*
    * @brief: 启动线程
    */
    bool start()
    {
        std::lock_guard<std::mutex> g( _mutex );
        if( _looping )
        {
            return false;
        }

        _looping = true;
        _thrd = std::move( std::thread( &thread_lite::thrd_proc, thread_lite<A>::shared_from_this() ) );
        return true;
    }

    /*
    * @brief: 停止线程
    */
    bool stop()
    {
        std::lock_guard<std::mutex> g( _mutex );
        if( !_looping )
        {
            return false;
        }

        _looping = false;
        _dq.push( A() ); // wakeup dataqueue
        _thrd.join();
        return true;
    }

    /*
    * @brief: 向队列中投递数据
    */
    void push( A const& a )
    {
        ++_cnt;
        _dq.push( a );
    }

    /*
    * @brief: 获取待处理数据大小
    */
    uint32_t size() const
    {
        return _cnt;
    }

private: // 私有函数

    /*
    * @brief: 线程处理函数
    */
    void thrd_proc()
    {
        while( _looping )
        {
            A data = _dq.take();
            if( !data )
            {
                --_cnt;
                continue;
            }
            data();
            --_cnt;
        }
    }

private: // 私有变量

    bool                  _looping;
    std::mutex            _mutex;
    std::thread           _thrd;
    data_queue<A>         _dq;
    std::atomic<uint32_t> _cnt;
    
};

// 平衡线程负载的策略
class default_policy
{
public: // 公共函数声明

    /*
    * @brief: 挑选合适的线程
    */
    template<class A>
    static int32_t pick( A const& thrd_pool )
    {
        uint32_t size = thrd_pool[0]->size();
        if ( 0 == size )
        {
            return 0;
        }

        uint32_t idx = 0;
        for( uint32_t i = 1; i < thrd_pool.size(); ++i )
        {
            if( size > thrd_pool[i]->size() ) // 选择负载小的
            {
                size = thrd_pool[i]->size();
                if( 0 == size )
                {
                    return i;
                }
                idx  = i;
            }
        }
        return idx;
    }
};

// 带有key值的线程池，用来保证同一个key值的数据被顺序处理
template<class K, class A>
class kthread_pool
{
public: // 公共类型

    // 清除map表标识
    struct clear_tag{};

public: // 公共函数

    /*
    * @brief: 构造函数
    */
    kthread_pool()
        : _started( false )
    {
        
    }

    /*
    * @brief: 启动线程池
    */
    bool start( uint32_t thrd_no = 4 )
    {
        std::lock_guard<std::mutex> g( _mutex );
        if( _started )
        {
            return false;
        }

        utils::defer rc( [&]()
        {
            for( auto& thrd : _thrd_pool )
            {
                if( thrd ) ( void )thrd->stop();
            }
            _thrd_pool.clear();
        } );

        for( uint32_t i = 0; i < thrd_no; ++i )
        {
            auto thrd_lite = thrd_type( new thread_lite<functor> );
            if( !thrd_lite->start() )
            {
                return false;
            }
            _thrd_pool.emplace_back( thrd_lite );
        }

        rc = false;
        _started = true;
        return true;
    }

    /*
    * @brief: 停止线程池
    */
    bool stop()
    {
        std::lock_guard<std::mutex> g( _mutex );
        if( !_started )
        {
            return false;
        }

        for( auto& thrd : _thrd_pool )
        {
            TARO_ASSERT( thrd != nullptr );
            ( void )thrd->stop();
        }
        _thrd_pool.clear();
        _started = false;
        return true;
    }

    /*
    * @brief: 添加处理对象
    */
    template< class P = default_policy >
    void push( K const& key, A const& data )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _keys.find( key );
        if( it != _keys.end() )
        {
            auto shared_key = it->second.first.lock();
            if( shared_key != nullptr ) // 说明该key的数据正在被处理，则放到该处理线程中去
            {
                uint32_t thrd_idx = it->second.second;
                TARO_ASSERT( ( thrd_idx < ( uint32_t )_thrd_pool.size() ) && ( _thrd_pool[thrd_idx] != nullptr ) );
                _thrd_pool[thrd_idx]->push( functor{data, shared_key} );
                return;
            }
            _keys.erase( it ); // 说明该key的值已经失效
        }

        // 根据策略挑选适合的线程
        uint32_t idx = P::pick( _thrd_pool );

        auto shared_key = key_shared( new bool );
        _keys[key] = std::make_pair( shared_key, idx );

        TARO_ASSERT( idx < ( uint32_t )_thrd_pool.size() && _thrd_pool[idx] != nullptr );
        _thrd_pool[idx]->push( functor{ data, shared_key } );
    }

    /*
    * @brief: 添加处理对象同时清空map表中无效key
    */
    template< class P = default_policy >
    void push( K const& key, A const& data, clear_tag )
    {
        bool found = false;

        std::lock_guard<std::mutex> g( _mutex );
        auto it = _keys.begin();
        while( it != _keys.end() )
        {
            key_shared shared_key = it->second.first.lock();
            if( nullptr == shared_key ) // key值失效则删除
            {
                it = _keys.erase( it );
                continue;
            }

            if( it->first == key ) // 找到相同key的线程,则添加至线程队列末尾
            {
                found         = true;
                auto thrd_idx = it->second.second;
                TARO_ASSERT( ( thrd_idx < ( int32_t )_thrd_pool.size() ) && ( _thrd_pool[thrd_idx] != nullptr ) );
                _thrd_pool[thrd_idx]->push( functor{ data, shared_key } );
            }
            ++it;
        }

        if( found )
        {
            return;
        }

        // 根据策略挑选适合的线程
        auto idx = P::pick( _thrd_pool );

        auto shared_key = key_shared( new bool );
        _keys[key] = std::make_pair( shared_key, idx );

        TARO_ASSERT( idx < ( int32_t )_thrd_pool.size() && _thrd_pool[idx] != nullptr );
        _thrd_pool[idx]->push( functor{ data, shared_key } );
    }

    /*
    * @brief: 重置线程,必须在已经停止的情况下才能重置，重置后要重新start
    */
    bool reset( uint32_t thrd_no = 4 )
    {
        std::lock_guard<std::mutex> g( _mutex );
        if( _started )
        {
            return false;
        }

        _thrd_pool.clear();
        for( uint32_t i = 0; i < thrd_no; ++i )
        {
            _thrd_pool.emplace_back( thrd_type( new thread_lite<functor> ) );
        }
        return true;
    }

private: // 私有类型

    using key_weak   = std::weak_ptr< bool >;
    using key_shared = std::shared_ptr< bool >;
    using map_type   = std::map< K, std::pair<key_weak, int32_t> >; //< pair first表示key是否存在，second 表示在哪个线程中

    // 函数子
    struct functor
    {
        void operator()()
        {
            _functor();
        }

        operator bool() const
        {
            return ( bool )_functor;
        }

        A          _functor;
        key_shared _key;
    };
    using thrd_type  = std::shared_ptr< thread_lite<functor> >;

private: // 私有变量

    bool                   _started;   // 线程池是否已启动的标识
    map_type               _keys;      // 记录key的map，注意该map在key过多的情况下可能比较大，需要在push时选择clear_tag进行清理
    std::mutex             _mutex;     // 用于锁启动/停止标识，以及_keys
    std::vector<thrd_type> _thrd_pool; // 线程对象容器
};

// 线程池
template<class A>
class thread_pool
{
public: // 公共函数

    /*
    * @brief: 构造函数
    */
    thread_pool( uint32_t thrd_no = 4 )
        : _started( false )
    {
        for( uint32_t i = 0; i < thrd_no; ++i )
        {
            _thrd_pool.emplace_back( thrd_type( new thread_lite<A> ) );
        }
    }

    /*
    * @brief: 启动线程池
    */
    bool start()
    {
        std::lock_guard<std::mutex> g( _mutex );
        if( _started )
        {
            return false;
        }

        std::list<thrd_type> rclist;
        utils::defer rc( [&]()
        {
            for( auto& thrd : rclist )
            {
                if( thrd ) ( void )thrd->stop();
            }
        } );

        for( auto& thrd : _thrd_pool )
        {
            TARO_ASSERT( thrd != nullptr );

            if( thrd->start() )
            {
                rclist.emplace_back( thrd );
            }
            else
            {
                return false;
            }
        }

        rc = false;
        _started = true;
        return true;
    }

    /*
    * @brief: 停止线程池
    */
    bool stop()
    {
        std::lock_guard<std::mutex> g( _mutex );
        if( !_started )
        {
            return false;
        }

        for( auto& thrd : _thrd_pool )
        {
            TARO_ASSERT( thrd != nullptr );
            ( void )thrd->stop();
        }

        _started = false;
        return true;
    }

    /*
    * @brief: 添加处理对象
    */
    template< class P = default_policy >
    void push( A const& data )
    {
        std::lock_guard<std::mutex> g( _mutex );

        // 根据策略挑选适合的线程
        int32_t idx = P::pick( _thrd_pool );

        TARO_ASSERT( idx < ( int32_t )_thrd_pool.size() && _thrd_pool[idx] != nullptr );
        _thrd_pool[idx]->push( data );
    }

    /*
    * @brief: 重置线程,必须在已经停止的情况下才能重置，重置后要重新start
    */
    bool reset( uint32_t thrd_no = 4 )
    {
        std::lock_guard<std::mutex> g( _mutex );
        if( _started )
        {
            return false;
        }

        _thrd_pool.clear();
        for( uint32_t i = 0; i < thrd_no; ++i )
        {
            _thrd_pool.emplace_back( thrd_type( new thread_lite<A> ) );
        }
        return true;
    }

private: // 私有类型

    using thrd_type = std::shared_ptr< thread_lite<A> >;

private: // 私有变量

    bool                   _started;   // 线程池是否已启动的标识
    std::mutex             _mutex;     // 用于锁启动/停止标识
    std::vector<thrd_type> _thrd_pool; // 线程对象容器
};

NAMESPACE_TARO_SYSTEM_END
