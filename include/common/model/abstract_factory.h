
#pragma once

#include "model/defs.h"
#include "memory/object_pool.h"
#include <memory>

NAMESPACE_TARO_MODEL_BEGIN

template<class K>
class abstract_factory;

template<class K>
using abstract_factory_ptr = std::shared_ptr< abstract_factory<K> >;

// 抽象工厂
template<class K>
class abstract_factory : public std::enable_shared_from_this< abstract_factory<K> >
{
public: // 公共函数

    /*
    * @brief: 虚析构
    */
    virtual ~abstract_factory() {}

    /*
    * @brief: 获取抽象工厂对象
    */
    template<class C>
    static std::shared_ptr<C> get_abstract_factory( K const& key )
    {
        return std::dynamic_pointer_cast< C >( find( key ) );
    }

protected: // 保护类

    /*
    * @brief: 注册工厂
    */
    bool register_factory( const K& key, abstract_factory_ptr<K> const& this_ptr )
    {
        return _factory_pool.add( key, this_ptr );
    }

    /*
    * @brief: 注销工厂
    */
    bool deregister_factory( const K& key )
    {
        return _factory_pool.remove( key );
    }

private: // 私有类型

    // 组件工厂容器
    using factory_map = memory::kobject_pool< K, abstract_factory_ptr<K> >;

private: // 私有函数

    /*
    * @brief: 查询组件工厂
    */
    static abstract_factory_ptr<K> find( K const& key )
    {
        abstract_factory_ptr<K> factory_ptr;
        ( void )_factory_pool.find( key, factory_ptr );
        return factory_ptr;
    }

private: // 私有变量

    static factory_map _factory_pool;
};

template<class K>
typename abstract_factory<K>::factory_map abstract_factory<K>::_factory_pool;

NAMESPACE_TARO_MODEL_END
