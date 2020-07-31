
#pragma once

#include "model/singleton.h"
#include "system/data_queue.h"
#include "memory/object_pool.h"
#include <string>
#include <iterator>

NAMESPACE_TARO_SYSTEM_BEGIN

template<class K, class D>
class instream
{
public: // 公共函数定义

    /*
    * @brief: 构造函数
    */
    explicit instream( K const& key, uint32_t timeout = 0 )
        : _key( key )
        , _timeout( timeout )
    {
        dq_kobject_pool_inst::instance().find_and_create( key, _dq );
    }

    /*
    * @brief: 析构函数
    */
    ~instream()
    {
        dq_kobject_pool_inst::instance().remove( _key );
    }

    /*
    * @brief: 获取数据
    */
    instream& operator>>( D& data )
    {
        if( 0 == _timeout )
        {
            data = _dq->take();
            return *this;
        }

        ( void )_dq->take( data, ( int32_t )_timeout );
        return *this;
    }

    /*
    * @brief: 获取指定数据
    */
    template<class M>
    bool operator()( D& data, M matcher, int32_t timeout = 2000 )
    {
        return _dq->pick( data, matcher, timeout );
    }

    /*
    * @brief: 有效性判断
    */
    operator bool() const
    {
        return _dq != nullptr;
    }

    /*
    * @brief: 设置超时时间
    */
    void set_timeout( uint32_t ms )
    {
        _timeout = ms;
    }

private: // 私有类型定义

    using dq_type              = data_queue<D>;
    using dq_kobject_pool      = memory::shared_kobject_pool< K, dq_type >;
    using dq_kobject_pool_inst = model::singleton<dq_kobject_pool>;

private: // 私有变量声明

    K                        _key;
    uint32_t                 _timeout;
    std::shared_ptr<dq_type> _dq;
};

template<class K, class D>
class outstream
{
public: // 公共函数定义

    /*
    * @brief: 构造函数
    */
    explicit outstream( K const& key )
        : _valid( false )
        , _timeout( 0 )
    {
        std::shared_ptr<dq_type> value;
        if( dq_kobject_pool_inst::instance().find( key, value ) )
        {
            _dq    = value;
            _valid = true;
        }
    }

    /*
    * @brief: 发送数据
    *
    * @return 对象引用
    */
    outstream& operator<<( D const& data )
    {
        auto temp = _dq.lock();
        if( nullptr != temp )
        {
            temp->push( data );
        }
        return *this;
    }

    /*
    * @brief: 有效性判断
    */
    operator bool() const
    {
        return _valid;
    }

private: // 私有类型定义

    using dq_type              = data_queue<D>;
    using dq_kobject_pool      = memory::shared_kobject_pool< K, dq_type >;
    using dq_kobject_pool_inst = model::singleton<dq_kobject_pool>;

private: // 私有变量声明

    bool                   _valid;
    uint32_t               _timeout;
    std::weak_ptr<dq_type> _dq;
};

NAMESPACE_TARO_SYSTEM_END
