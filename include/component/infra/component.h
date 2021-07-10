
#pragma once

#include "infra/defs.h"
#include <memory>

NAMESPACE_TARO_INFRA_BEGIN

// 组件关键字
struct component_key
{
public: // 公共函数

    /*
    * @brief: 构造函数
    */
    component_key();

    /*
    * @brief: 构造函数
    *
    * @param[in]  iid      interface id
    * @param[in]  cid      class id
    */
    component_key( const char* iid, const char* cid );

    /*
    * @brief: 获取interface id
    */
    const char* iid() const;

    /*
    * @brief: 获取class id
    */
    const char* cid() const;

    /*
    * @brief: 小于比较
    */
    bool operator<( component_key const& r ) const;

private: // 私有变量

    struct impl;
    std::shared_ptr<impl> _impl;
};

class component_factory;
using component_factory_ptr = std::shared_ptr<component_factory>;

// 组件工厂类
class component_factory : public std::enable_shared_from_this< component_factory >
{
public: // 公共函数

    /*
    * @brief: 虚析构
    */
    virtual ~component_factory();

    /*
    * @brief: 获取工厂对象
    *
    * @param[in] key 工厂标识
    * @return 工厂对象指针
    */
    template< class C >
    static std::shared_ptr< C > get_component( const component_key& key )
    {
        auto component_ptr = find( key );
        if ( nullptr == component_ptr )
        {
            return std::shared_ptr< C >();
        }
        return std::dynamic_pointer_cast< C >( component_ptr );
    }

protected: // 保护函数

    /*
    * @brief: 注册工厂
    *
    * @param[in] key      工厂标识
    * @param[in] this_ptr 工厂对象指针
    * @return true 注册成功 false 注册失败
    */
    bool register_factory( const component_key& key, component_factory_ptr const& this_ptr );

    /*
    * @brief: 注销工厂
    *
    * @param[in] key      工厂标识
    * @return true 注销成功 false 注销失败
    */
    bool deregister_factory( const component_key& key );

private: // 私有函数

    /*
    * @brief: 查找工厂对象
    *
    * @param[in] key      工厂标识
    * @return 工厂对象指针
    */
    static component_factory_ptr find( const component_key& key );
};

// 获取对象
template< typename F >
std::shared_ptr<typename F::component_type> get_component( const char* cid, const char* url = nullptr )
{
    auto factory_ptr = component_factory::get_component<F>( F::key( cid ) );
    if( nullptr == factory_ptr )
    {
        return std::shared_ptr<typename F::component_type>();
    }
    return factory_ptr->get( url );
}

NAMESPACE_TARO_INFRA_END
