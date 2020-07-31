
#pragma once

#include "memory/deleter.h"
#include <memory>

NAMESPACE_TARO_MEMORY_BEGIN

template<typename T, typename U>
U* create_object()
{
    return dynamic_cast< U* >( new T );
}

template<typename T>
std::shared_ptr<T> inst_sptr( T* thiz )
{
    return std::shared_ptr<T>( thiz, delete_do_nothing<T> );
}

NAMESPACE_TARO_MEMORY_END
