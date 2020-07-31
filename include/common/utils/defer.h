
#pragma once

#include <functional>
#include "utils/defs.h"

NAMESPACE_TARO_UTILS_BEGIN

// RAII机制对象
template<class F>
struct resource_guard
{
public: // 公共函数

    resource_guard( F const& f )
        : _f( f )
        , _call( true )
    {

    }

    ~resource_guard()
    {
        if( _call ) 
        {
            _f();
        }
    }

    resource_guard& operator=( bool call )
    {
        _call = call;
        return *this;
    }

PRIVATE:// 私有变量定义

    F    _f;    // 回收函数
    bool _call; // 是否调用
};

using defer = resource_guard< std::function<void()> >;

NAMESPACE_TARO_UTILS_END
