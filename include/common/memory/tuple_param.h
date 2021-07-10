
#pragma once

#include "memory/defs.h"
#include "utils/assert.h"
#include <tuple>
#include <memory>

NAMESPACE_TARO_MEMORY_BEGIN

/*
* @brief 参数基类
*/
struct param_base
{
public: // 公共函数

    /*
    * @brief 析构函数
    */
    virtual ~param_base() = default;
};

using param_base_ptr = std::shared_ptr< param_base >;

/*
* @brief 带有类型的参数基类
*/
template<typename ...Args>
struct tuple_param : public param_base
{
public: // 公共函数

    /*
    * @brief 构造函数
    */
	tuple_param( Args... args )
        : _param( std::make_tuple( args... ) )
    {

    }

    /*
    * @brief 获取参数
    */
    template<uint32_t I>
    typename std::tuple_element<I, std::tuple<Args...> >::type& get()
    {
        return std::get<I>( _param );
    }

private: // 私有变量

    std::tuple<Args...> _param;
};

/*
* @brief 带有类型的参数基类
*/
template<class T>
struct type_param_base
{
public: // 公共函数

    /*
    * @brief 构造函数
    */
    explicit type_param_base( T const& t )
        : _type( t )
    {

    }

    /*
    * @brief 析构函数
    */
    virtual ~type_param_base() = default;

    /*
    * @brief 获取类型
    */
    T type() const
    {
        return _type;
    }

private: // 私有变量

    T _type;  // 参数类型
};

template<class T>
using type_param_base_ptr = std::shared_ptr< type_param_base<T> >;

/*
* @brief 基于tuple的参数类型
*/
template<typename T, typename ...Args>
struct type_tuple_param : public type_param_base<T>
{
public: // 公共函数

    /*
    * @brief 构造函数
    */
    type_tuple_param( T const& t, Args... args )
        : type_param_base<T>( t )
        , _param( std::make_tuple( args... ) )
    {

    }

    /*
    * @brief 析构函数
    */
    virtual ~type_tuple_param() = default;

    /*
    * @brief 获取参数
    */
    template<uint32_t I>
    typename std::tuple_element<I, std::tuple<Args...> >::type& get()
    {
        return std::get<I>( _param );
    }

private: // 私有变量

    std::tuple<Args...> _param;
};

template< typename T, typename... Args >
std::shared_ptr<T> make_param( Args&&... args )
{
    return std::make_shared< T >( std::forward<Args>( args )... );
}

template< typename T, typename U >
T& to_param_ref( U const& ptr )
{
    return *( std::dynamic_pointer_cast< T >( ptr ) );
}

// 特化关键字类型为int
template<int32_t N, typename... Args>
struct int_tuple_param : public memory::type_tuple_param<int32_t, Args...>
{
    int_tuple_param( Args&&... args )
        : memory::type_tuple_param<int32_t, Args...>( N, std::forward<Args>( args )... )
    {

    }
};

NAMESPACE_TARO_MEMORY_END
