
#pragma once

#include "utils/assert.h"
#include "function/defs.h"
#include <memory>

NAMESPACE_TARO_FUNCTION_BEGIN

/// 函数类型
enum function_type
{
    general,
    shared_member,
    weak_member,
};

// 函数基类
template<class R, class... Args>
class function_base
{
public:  // 公共函数

    /*
    * @brief: 构造函数
    */
    function_base( function_type type )
        : _type( type )
    {

    }

    /*
    * @brief: 获取函数类型
    */
    function_type type() const
    {
        return _type;
    }

    /*
    * @brief: 函数调用
    */
    virtual R operator()( Args... ) = 0;

    /*
    * @brief: 等于比较
    */
    virtual bool equal( function_base* ) const = 0;

    /*
    * @brief: 有效性判断
    */
    virtual bool valid() const = 0;

    /*
    * @brief: 虚析构
    */
    virtual ~function_base(){}

private: /// 私有变量

    function_type _type;
};

template<class R, class... Args>
class general_function : public function_base<R, Args...>
{
public: // 公共函数

    /*
    * @brief: 构造函数
    */
    explicit general_function( R( *f )( Args... ) )
        : function_base<R, Args...>( general )
        , _f( f )
    {
        TARO_ASSERT( _f != nullptr );
    }

    /*
    * @brief: 函数调用
    */
    virtual R operator()( Args... args ) final
    {
        return _f( args... );
    }

    /*
    * @brief: 等于比较
    */
    virtual bool equal( function_base<R, Args...>* f ) const final
    {
        auto ptr = dynamic_cast< general_function* >( f );
        return _f == ptr->_f;
    }

    /*
    * @brief: 有效性判断
    */
    virtual bool valid() const final
    {
        return _f != nullptr;
    }

private: // 私有变量定义

    R( *_f )( Args... );
};

template<class T, class R, class... Args>
class shared_member_function : public function_base<R, Args...>
{
public: // 公共函数

    /*
    * @brief: 构造函数
    */
    shared_member_function( R( T::*f )( Args... ), std::shared_ptr<T> const& obj )
        : function_base<R, Args...>( shared_member )
        , _f( f )
        , _obj( obj )
    {
        TARO_ASSERT( _f != nullptr && obj != nullptr );
    }

    /*
    * @brief: 函数调用
    */
    virtual R operator()( Args... args ) final
    {
        return ( ( *_obj ).*_f )( args... );
    }

    /*
    * @brief: 等于比较
    */
    virtual bool equal( function_base<R, Args...>* f ) const final
    {
        auto ptr = dynamic_cast< shared_member_function* >( f );
        return ( _f == ptr->_f ) && ( _obj == ptr->_obj );
    }

    /*
    * @brief: 有效性判断
    */
    virtual bool valid() const final
    {
        return ( _f != nullptr ) && ( _obj != nullptr );
    }

private: // 私有变量定义
    
    R( T::*_f )( Args... );
    std::shared_ptr<T> _obj;
};

template<class T, class R, class... Args>
class weak_member_function : public function_base<R, Args...>
{
public: // 公共函数

    /*
    * @brief: 构造函数
    */
    weak_member_function( R( T::*f )( Args... ), std::weak_ptr<T> const& obj )
        : function_base<R, Args...>( weak_member )
        , _raw( nullptr )
        , _f( f )
        , _obj( obj )
    {
        auto temp = obj.lock();
        TARO_ASSERT( _f != nullptr && temp != nullptr );
        _raw = temp.get();
    }

    /*
    * @brief: 函数调用
    */
    virtual R operator()( Args... args ) final
    {
        auto temp = _obj.lock();
        if( temp == nullptr )
        {
            throw exception_weak_pointer_expired;
        }
        return ( ( *temp ).*_f )( args... );
    }

    /*
    * @brief: 等于比较
    */
    virtual bool equal( function_base<R, Args...>* f ) const final
    {
        auto ptr = dynamic_cast< weak_member_function* >( f );
        return ( _f == ptr->_f ) && ( _raw == ptr->_raw );
    }

    /*
    * @brief: 有效性判断
    */
    virtual bool valid() const final
    {
        return ( _f != nullptr ) && ( !_obj.expired() );
    }

private: // 私有变量定义

    T* _raw;
    R( T::*_f )( Args... );
    std::weak_ptr<T> _obj;
};

template<class T>
class function{};

template<class R, class... Args>
class function<R( Args... )>
{
public: /// 公共函数

    function()
    {

    }

    explicit function( R( *f )( Args... ) )
        : _base( new general_function<R, Args...>( f ) )
    {
        TARO_ASSERT( _base != nullptr );
    }

    template<class T>
    explicit function( R( T::*f )( Args... ), std::shared_ptr<T> const& obj )
        : _base( new  shared_member_function<T, R, Args...>( f, obj ) )
    {
        TARO_ASSERT( _base != nullptr );
    }

    template<class T>
    explicit function( R( T::*f )( Args... ), std::weak_ptr<T> const& obj )
        : _base( new  weak_member_function<T, R, Args...>( f, obj ) )
    {
        TARO_ASSERT( _base != nullptr );
    }

    /*
    * @brief: 函数调用
    */
    R operator()( Args... args )
    {
        if( _base == nullptr )
        {
            throw exception_function_invalid;
        }
        return ( *_base )( args... );
    }

    bool operator==( function const& r ) const
    {
        if( this == &r )
        {
            return true;
        }

        if( ( _base == nullptr ) && ( r._base == nullptr ) )
        {
            return true;
        }

        if( ( ( _base == nullptr ) && ( r._base != nullptr ) ) 
         || ( ( _base != nullptr ) && ( r._base == nullptr ) ) )
        {
            return false;
        }

        if( _base->type() != r._base->type() )
        {
            return false;
        }

        return _base->equal( r._base.get() );
    }

    bool operator!=( function const& r ) const
    {
        return !( *this == r );
    }

    operator bool() const
    {
        return _base != nullptr && _base->valid();
    }

private: // 私有变量

    std::shared_ptr< function_base<R, Args...> > _base;
};

NAMESPACE_TARO_FUNCTION_END
