
#pragma once

#include "infra/defs.h"
#include "base/type_traits.h"
#include "memory/scoped_ptr.h"
#include "serialization/string.h"
#include <memory>
#include <vector>
#include <typeinfo>
#include <stdlib.h>
#include <string.h>

NAMESPACE_TARO_INFRA_BEGIN

// 反射对象
class reflection
{
public: // 公共类型

    // 类型
    enum reflection_type
    {
        ref_type_variable, // 变量
        ref_type_function, // 函数
    };

public: // 公共函数

    /*
    * @brief  虚析构
    */
    virtual ~reflection() {}

    /*
    * @brief  获取反射类型
    */
    virtual reflection_type get_ref_type() const = 0;

    /*
    * @brief  获取名称
    */
    virtual const char* get_name() const = 0;

    /*
    * @brief  获取类型
    */
    virtual const std::type_info* get_type() const = 0;
};

using reflection_ptr = std::shared_ptr<reflection>;

// 变量反射基类
class variable_reflection_base : public reflection
{
public: // 公共函数

    /*
    * @brief  虚析构
    */
    virtual ~variable_reflection_base() {}

    /*
    * @brief  设置名称
    */
    virtual void set_name( const char* name ) = 0;

    /*
    * @brief  获取反射类型
    */
    virtual reflection_type get_ref_type() const override final
    {
        return ref_type_variable;
    }

    /*
    * @brief  将成员变量转换为字符串
    *
    * @param[in] class_obj_ptr 类对象指针
    * @return    成员变量序列化结果
    */
    virtual result< std::string > serialization( void* class_obj_ptr ) const = 0;

    /*
    * @brief  将字符串转换为成员变量的值
    *
    * @param[in] text 成员对象
    * @param[in] class_obj_ptr 类对象指针
    * @return    成员变量序列化结果
    */
    virtual ret_val deserialization( const char* text, void* class_obj_ptr ) const = 0;
};

using variable_reflection_base_ptr = std::shared_ptr<variable_reflection_base>;

// 函数反射
template< class F >
class function_reflection : public reflection
{
public: // 公共函数

    /*
    * @brief  构造函数
    *
    * @param[in] name 函数名称
    * @param[in] f    函数对象
    */
    function_reflection( const char* name, F const& f )
        : _func( f )
        , _name( name )
    {
        
    }

    /*
    * @brief  获取反射类型
    */
    virtual reflection_type get_ref_type() const override final
    {
        return ref_type_function;
    }

    /*
    * @brief  获取名称
    */
    virtual const char* get_name() const override final
    {
        return _name.c_str();
    }

    /*
    * @brief  获取类型
    */
    virtual const std::type_info* get_type() const override final
    {
        return &typeid( F );
    }
    
    /*
    * @brief  获取函数
    */
    F get_func() const
    {
        return _func;
    }

private: // 私有变量

    F           _func;
    std::string _name;
};

// 变量反射
template< class T, class M >
class variable_reflection : public variable_reflection_base
{
public: // 公共函数

    /*
    * @brief  构造函数
    *
    * @param[in] member      对象变量指针
    * @param[in] name        变量名称
    * @param[in] str_wrapper 变量转成字符串时的外包字符 如转为数据库的字符串需要在头尾增加单引号 'jack'
    */
    variable_reflection( M const& member, const char* str_wrapper = "'" )
        : _ptr( member )
        , _str_wrapper( str_wrapper )
    {

    }

    /*
    * @brief  设置名称
    */
    virtual void set_name( const char* name ) override final
    {
        _name = name;
    }

    /*
    * @brief  获取名称
    */
    virtual const char* get_name() const override final
    {
        return _name.c_str();
    }

    /*
    * @brief  获取类型
    */
    virtual const std::type_info* get_type() const override final
    {
        T* obj = nullptr;
        return &typeid( decltype( obj->*_ptr ) );
    }

    /*
    * @brief  将变量转换为字符串
    */
    virtual result< std::string > serialization( void* class_obj_ptr ) const override
    {
        result< std::string > ret( "" );

        T* obj = static_cast< T* >( class_obj_ptr );
        if( nullptr == obj )
        {
            ret.set_errno( errno_invalid_param );
            return ret;
        }

        auto cast_ret = cast_to_str< typename unwapper_type< decltype( obj->*_ptr ) >::type >( std::move( obj->*_ptr ) );
        ret.set_value( cast_ret );
        return ret;
    }

    /*
    * @brief  将字符串转换为变量的值
    */
    virtual ret_val deserialization( const char* text, void* class_obj_ptr ) const override
    {
        ret_val ret;

        T* obj = static_cast< T* >( class_obj_ptr );
        if( nullptr == obj || nullptr == text || 0 == strlen( text ) )
        {
            ret.set_errno( errno_invalid_param );
            return ret;
        }

        obj->*_ptr = serial::str_to_value< typename unwapper_type< decltype( obj->*_ptr ) >::type >( text );
        return ret;
    }

private: // 私有函数

    /*
    * @brief  值转换为字符串，如果变量为字符型则增加外包字符
    */
    template< class A >
    typename std::enable_if< is_string<A>::value, std::string >::type
        cast_to_str( A&& value ) const
    {
        std::string str = value;
        if( nullptr != _str_wrapper )
        {
            std::string w = _str_wrapper;
            str = w + str + w;
        }
        return str;
    }

    /*
    * @brief  值转换为字符串
    */
    template< class A >
    typename std::enable_if< !is_string<A>::value, std::string >::type
        cast_to_str( A&& value ) const
    {
        return serial::type_to_str< A >( std::forward<A>( value ) );
    }

private: // 私有变量

    M           _ptr;          // 变量指针
    std::string _name;         // 变量名称
    const char* _str_wrapper;  // 转换为字符串的外包字符
};

// 反射管理类
class reflection_manager
{
public: // 公共函数

    /*
    * @brief 获取单例
    */
    static reflection_manager& instance();

    /*
    * @brief 注册类对象
    *
    * @param[in] class_name 类名称
    * @param[in] info       类型描述
    */
    int32_t register_class( const char* class_name, const std::type_info* info );

    /*
    * @brief 查询类名称
    *
    * @param[in]  info    类型描述
    * @return     类名称
    */
    std::string find_class_name( const std::type_info* info ) const;

    /*
    * @brief 注册类成员
    *
    * @param[in] class_name     类名称
    * @param[in] reflection_ptr 反射对象
    */
    int32_t register_class_member( const char* class_name, reflection_ptr const& reflection_ptr );

    /*
    * @brief 获取类成员名称
    *
    * @param[in]  class_name 类名称
    * @return     该类所有的反射对象
    */
    std::vector<reflection_ptr> get_class_members( const char* class_name ) const;

private: // 私有函数

    /*
    * @brief 构造函数
    */
    reflection_manager();

    /*
    * @brief 析构函数
    */
    ~reflection_manager();

private: // 私有变量

    struct impl;
    memory::scoped_ptr<impl> _impl;
};

NAMESPACE_TARO_INFRA_END
