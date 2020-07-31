
#pragma once

#include "memory/defs.h"

NAMESPACE_TARO_MEMORY_BEGIN

template<typename T>
void delete_do_nothing( T* )
{

}

template<typename T>
void delete_array( T* ptr )
{
    if( nullptr != ptr )
    {
        delete[] ptr;
    }
}

NAMESPACE_TARO_MEMORY_END
