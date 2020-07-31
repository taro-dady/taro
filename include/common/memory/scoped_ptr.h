
#pragma once

#include "memory/detail/destroyer.h"
#include <cstddef>

NAMESPACE_TARO_MEMORY_BEGIN

// 作用与std::unique_ptr相同,解决unique_ptr必须接受完整类，无法用于接口类的问题
template<class T>
class scoped_ptr
{
public: // 公共函数

    /**
    * @brief      构造函数
    */
    scoped_ptr()
        : _p( nullptr )
        , _del( nullptr )
    {

    }

    /**
    * @brief      构造函数
    */
    explicit scoped_ptr( std::nullptr_t )
        : _p( nullptr )
        , _del( nullptr )
    {

    }

    /**
    * @brief      构造函数
    */
    explicit scoped_ptr( T* p )
        : _p( p )
        , _del( new detail::normal_destroyer<T>( p ) )
    {

    }

    /**
    * @brief      构造函数
    */
    template<class D>
    scoped_ptr( T* p, D del )
        : _p( p )
        , _del( new detail::user_destroyer<T, D>( p, del ) )
    {

    }

    /**
    * @brief      析构函数
    */
    ~scoped_ptr()
    {
        if( _del ) 
            _del->release();
        _del = nullptr;
    }

    /**
    * @brief      获取引用
    */
    T& operator*() const
    {
        return *_p;
    }

    /**
    * @brief      获取指针
    */
    T* operator->() const
    {
        return _p;
    }

    /**
    * @brief      重置指针
    */
    void reset( T* p = nullptr )
    {
        scoped_ptr( p ).swap( *this );
    }

    /**
    * @brief      重置指针
    */
    template<class D>
    void reset( T* p, D del )
    {
        scoped_ptr( p, del ).swap( *this );
    }

    /**
    * @brief      有效性判断
    */
    explicit operator bool() const
    {
        return _p != nullptr;
    }

    /**
    * @brief      获取原始指针
    */
    T* get() const
    {
        return _p;
    }

private: // 私有函数

    /**
    * @brief      交换对象
    */
    void swap( scoped_ptr<T>& r )
    {
        T*                 ptr = _p;
        detail::destroyer* del = _del;

        _p     = r._p;
        _del   = r._del;
        r._p   = ptr;
        r._del = del;
    }

    // 禁止拷贝构造
    scoped_ptr( scoped_ptr const& ) = delete;
    scoped_ptr& operator=( scoped_ptr const& ) = delete;

private: // 私有变量

    T*                 _p;
    detail::destroyer* _del;
};

NAMESPACE_TARO_MEMORY_END

template<class T, class U>
bool operator==( taro::memory::scoped_ptr<T> const& l, taro::memory::scoped_ptr<U> const& r )
{
    return l.get() == r.get();
}

template<class T, class U>
bool operator!=( taro::memory::scoped_ptr<T> const& l, taro::memory::scoped_ptr<U> const& r )
{
    return l.get() != r.get();
}

template<class T>
bool operator==( taro::memory::scoped_ptr<T> const& l, std::nullptr_t )
{
    return l.get() == nullptr;
}

template<class T>
bool operator==( std::nullptr_t, taro::memory::scoped_ptr<T> const& r )
{
    return r.get() == nullptr;
}

template<class T>
bool operator!=( taro::memory::scoped_ptr<T> const& l, std::nullptr_t )
{
    return nullptr != l.get();
}

template<class T>
bool operator!=( std::nullptr_t, taro::memory::scoped_ptr<T> const& r )
{
    return nullptr != r.get();
}
