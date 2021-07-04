
#pragma once

#include "memory/defs.h"
#include "utils/assert.h"
#include <map>
#include <list>
#include <mutex>
#include <vector>
#include <memory>
#include <algorithm>

NAMESPACE_TARO_MEMORY_BEGIN

template<class O>
class object_pool
{
public:  // 公共函数

    /*
    * @brief: 添加对象
    */
    bool add( O const& obj )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = std::find( std::begin( _list ), std::end( _list ), obj );
        if( it != std::end( _list ) )
        {
            return false;
        }

        _list.emplace_back( obj );
        return true;
    }

    /*
    * @brief: 添加对象
    */
    template<typename M>
    bool add( O const& obj, M const& matcher )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = std::find( std::begin( _list ), std::end( _list ), matcher );
        if( it != std::end( _list ) )
        {
            return false;
        }

        _list.emplace_back( obj );
        return true;
    }

    /*
    * @brief: 删除对象
    */
    bool remove( O const& obj )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = std::find( std::begin( _list ), std::end( _list ), obj );
        if( it == std::end( _list ) )
        {
            return false;
        }

        _list.erase( it );
        return true;
    }

    /*
    * @brief: 删除匹配对象
    */
    template<class M>
    bool remove_mather( M const& m )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = std::find_if( std::begin( _list ), std::end( _list ), m );
        if( it == std::end( _list ) )
        {
            return false;
        }

        _list.erase( it );
        return true;
    }

    /*
    * @brief: 获取所有对象
    */
    std::list<O> get_all()
    {
        std::lock_guard<std::mutex> g( _mutex );
        return _list;
    }

    /*
    * @brief: 清除所有对象
    */
    void clear()
    {
        std::lock_guard<std::mutex> g( _mutex );
        _list.clear();
    }

private: // 私有变量

    std::mutex   _mutex;
    std::list<O> _list;
};

// 带有关键字的对象存储池
template<class K, class O>
class kobject_pool
{
public:  // 公共函数

    /*
    * @brief: 添加对象
    */
    bool add( K const& key, O const& obj )
    {
        std::lock_guard<std::mutex> g( _mutex );
        return _pool.insert( std::make_pair( key, obj ) ).second;
    }

    /*
    * @brief: 删除对象
    */
    bool remove( K const& key )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _pool.find( key );
        if( it != _pool.end() )
        {
            _pool.erase( it );
            return true;
        }
        return false;
    }

    /*
    * @brief: 删除匹配对象
    */
    template<class M>
    bool remove_mather( M const& m )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = std::find_if( std::begin( _pool ), std::end( _pool ), m );
        if( it == std::end( _pool ) )
        {
            return false;
        }

        _pool.erase( it );
        return true;
    }

    /*
    * @brief: 删除所有匹配对象
    */
    template<class M>
    void remove_all_mather( M const& m )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = std::begin( _pool );
        while( it != std::end( _pool ) )
        {
            if( m( *it ) )
            {
                it = _pool.erase( it );
            }
            else
            {
                ++it;
            }
        }
    }

    /*
    * @brief: 查找对象
    */
    O find( K const& key )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _pool.find( key );
        if( it == _pool.end() )
        {
            return O();
        }
        return it->second;
    }

    /*
    * @brief: 查找对象
    */
    bool find( K const& key, O& obj )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _pool.find( key );
        if( it != _pool.end() )
        {
            obj = it->second;
            return true;
        }
        return false;
    }

    /*
    * @brief: 查找对象
    */
    template<class M>
    bool find_matcher( M const& matcher, O& obj )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = std::find_if( std::begin( _pool ), std::end( _pool ), matcher );
        if( it == std::end( _pool ) )
        {
            return false;
        }
        obj = it->second;
        return true;
    }

    /*
    * @brief: 获取所有对象
    */
    void list_all( std::vector<O>& all )
    {
        std::lock_guard<std::mutex> g( _mutex );
        std::for_each( std::begin( _pool ), std::end( _pool ), [&]( std::pair<K, O> const& item )
        {
            all.emplace_back( item.second );
        } );
    }

    /*
    * @brief: 清除所有对象
    */
    void clear()
    {
        std::lock_guard<std::mutex> g( _mutex );
        _pool.clear();
    }

    /*
    * @brief: 遍历对象
    */
    template<class F>
    void scan( F const& f )
    {
        std::unique_lock<std::mutex> g( _mutex );
        auto tmp = _pool;
        g.unlock();
        
        for( auto& it : tmp )
        {
            f( it );
        }
    }

private: // 私有变量定义

    std::mutex     _mutex;
    std::map<K, O> _pool;
};

// 带有关键字的共享指针对象存储池
template<class K, class O>
class shared_kobject_pool
{
public:  // 公共函数

    /*
    * @brief: 查找对象，若没找到则创建
    */
    void find_and_create( K const& key, std::shared_ptr<O>& ptr )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _pool.find( key );
        if( it == _pool.end() )
        {
            ptr = std::shared_ptr<O>( new O );
            _pool[key] = ptr;
        }
        else
        {
            ptr = it->second;
        }
    }

    /*
    * @brief: 添加对象
    */
    bool add( K const& key, std::shared_ptr<O> const& ptr )
    {
        std::lock_guard<std::mutex> g( _mutex );
        return _pool.insert( std::make_pair( key, ptr ) ).second;
    }

    /*
    * @brief: 删除对象
    */
    bool remove( K const& key )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _pool.find( key );
        if( it != _pool.end() )
        {
            _pool.erase( it );
            return true;
        }
        return false;
    }

    /*
    * @brief: 查找对象
    */
    bool find( K const& key, std::shared_ptr<O>& ptr )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _pool.find( key );
        if( it != _pool.end() )
        {
            ptr = it->second;
            return true;
        }
        return false;
    }

private: // 私有类型定义

    using value_type = std::shared_ptr<O>;
    using map_type   = std::map<K, value_type>;

private: // 私有变量定义

    map_type   _pool;
    std::mutex _mutex;
};

// 带有关键字的对象存储池
template<class K, class O>
class kobject_list_pool
{
public:  // 公共函数

    /*
    * @brief: 添加对象
    */
    bool add( K const& key, O const& obj )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _pool.find( key );
        if( it != _pool.end() )
        {
            auto iter = std::find( std::begin( it->second ), std::end( it->second ), obj );
            if( iter != std::end( it->second ) )
            {
                return false;
            }
            it->second.emplace_back( obj );
            return true;
        }
        _pool[key].emplace_back( obj );
        return true;
    }

    /*
    * @brief: 删除对象
    */
    bool remove( K const& key, O const& obj )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _pool.find( key );
        if( it == _pool.end() )
        {
            return false;
        }
        
        auto iter = std::find( std::begin( it->second ), std::end( it->second ), obj );
        if( iter == std::end( it->second ) )
        {
            return false;
        }
        it->second.erase( iter );
        return true;
    }

    /*
    * @brief: 获取
    */
    std::list<O> find( K const& key )
    {
        std::lock_guard<std::mutex> g( _mutex );
        auto it = _pool.find( key );
        if( it == _pool.end() )
        {
            return std::list<O>();
        }
        return it->second;
    }

    /*
    * @brief: 清除所有对象
    */
    void clear()
    {
        std::lock_guard<std::mutex> g( _mutex );
        _pool.clear();
    }

private: // 私有类型定义

    using map_type = std::map< K, std::list<O> >;

private: // 私有变量

    map_type   _pool;
    std::mutex _mutex;
};

NAMESPACE_TARO_MEMORY_END
