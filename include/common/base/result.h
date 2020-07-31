
#pragma once

#include "base/base.h"
#include "base/error_num.h"
#include <string>

NAMESPACE_TARO_BEGIN

template<typename T>
struct result
{
public: // 公共函数

    explicit result( T const& default_value )
        : _value( default_value )
        , _errno( errno_ok )
    {

    }

    void set_value( T const& t )
    {
        _value = t;
        _errno = errno_ok;
    }

    T& get_value()
    {
        return _value;
    }

    void set_errno( int32_t err_no )
    {
        _errno = err_no;
    }

    int32_t get_errno() const
    {
        return _errno;
    }

    operator bool() const
    {
        return _errno == errno_ok;
    }

    void set_err_msg( const char* err )
    {
        _errmsg = err;
    }

    const char* get_err_msg() const
    {
        return _errmsg.c_str();
    }

private: // 私有变量

    T           _value;  // 返回值
    int32_t     _errno;  // 错误码
    std::string _errmsg; // 错误消息
};

template<>
struct result<void>
{
public: // 公共函数

    result()
        : _errno( errno_ok )
    {

    }

    void set_errno( int32_t err_no )
    {
        _errno = err_no;
    }

    int32_t get_errno() const
    {
        return _errno;
    }

    operator bool() const
    {
        return _errno == errno_ok;
    }

    void set_err_msg( const char* err )
    {
        _errmsg = err;
    }

    const char* get_err_msg() const
    {
        return _errmsg.c_str();
    }

private: // 私有变量

    int32_t     _errno;  // 错误码
    std::string _errmsg; // 错误消息
};

using ret_val = result<void>;

NAMESPACE_TARO_END
