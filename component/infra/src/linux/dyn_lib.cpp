﻿
#include "infra/error.h"
#include "infra/dyn_lib.h"
#include <string>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

NAMESPACE_TARO_INFRA_BEGIN

struct dyn_lib::impl
{
    void*       hdl;
    std::string name;
};

dyn_lib::dyn_lib()
    : _impl( new impl )
{
    _impl->hdl = nullptr;
}

dyn_lib::~dyn_lib()
{
    ( void )unload();
}

int32_t dyn_lib::load( const char* name )
{
    if ( nullptr == name || strlen( name ) == 0 )
    {
        return errno_invalid_param;
    }

    if ( _impl->hdl != nullptr )
    {
        return errno_multiple;
    }

    _impl->hdl = dlopen( name, RTLD_NOW | RTLD_GLOBAL );
    if ( nullptr == _impl->hdl )
    {
        return errno_unaccess;
    }
    _impl->name = name;
    return errno_ok;
}

int32_t dyn_lib::unload()
{
    if ( nullptr == _impl->hdl )
    {
        return errno_invalid_res;
    }

    if ( 0 == dlclose( _impl->hdl ) )
    {
        _impl->hdl  = nullptr;
        _impl->name = "";
        return errno_ok;
    }
    return errno_general;
}

void* dyn_lib::func_addr( const char* func_name )
{
    if ( nullptr == func_name || strlen( func_name ) == 0 )
    {
        set_err_no( errno_invalid_param );
        return nullptr;
    }

    if ( _impl->hdl == nullptr )
    {
        set_err_no( errno_invalid_res );
        return nullptr;
    }

    void* addr = ( void * )dlsym( _impl->hdl, func_name );
    if ( nullptr == addr )
    {
        set_err_no( errno_general );
        return nullptr;
    }
    return addr;
}

NAMESPACE_TARO_INFRA_END
