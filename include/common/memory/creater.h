
#pragma once

#include "memory/deleter.h"
#include <memory>

NAMESPACE_TARO_MEMORY_BEGIN

template<typename T, typename U>
U* create_object()
{
    return dynamic_cast< U* >( new T );
}

// 构造单例智能指针
template<typename T>
std::shared_ptr<T> inst_sptr( T* thiz )
{
    return std::shared_ptr<T>( thiz, delete_do_nothing<T> );
}

// shared_ptr 转成 weak_ptr
template<typename T>
std::weak_ptr<T> shared_wptr( std::shared_ptr<T> const& thiz )
{
    return std::weak_ptr<T>( thiz );
}

NAMESPACE_TARO_MEMORY_END
