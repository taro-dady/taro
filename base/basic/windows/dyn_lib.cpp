
#include "basic/dyn_lib.h"
#include <string>
#include <windows.h>

NAMESPACE_TARO_BASIC_BEGIN

struct dyn_lib::impl
{
    HMODULE     _hdl;  // 动态库句柄
    std::string _name; // 动态库名称
};

dyn_lib::dyn_lib()
    : _impl( new impl )
{
    _impl->_hdl = nullptr;
}

ret_val dyn_lib::load( const char* name )
{
    ret_val ret;

    if( ( nullptr == name ) || ( 0 == strlen( name ) ) )
    {
        ret.set_errno( errno_invalid_param );
        return ret;
    }

    _impl->_hdl = LoadLibrary( name );
    if( nullptr == _impl->_hdl )
    {
        ret.set_errno( errno_general );
        return ret;
    }

    _impl->_name = name;
    return ret;
}

ret_val dyn_lib::unload()
{
    ret_val ret;

    if( nullptr == _impl->_hdl )
    {
        ret.set_errno( errno_unaccess );
        return ret;
    }

    if( !FreeLibrary( _impl->_hdl ) )
    {
        ret.set_errno( errno_general );
        return ret;
    }

    _impl->_hdl  = nullptr;
    _impl->_name = "";
    return ret;
}

result<void*> dyn_lib::func_addr( const char* func_name )
{
    result<void*> ret( nullptr );

    if( ( nullptr == func_name ) || ( 0 == strlen( func_name ) ) )
    {
        ret.set_errno( errno_invalid_param );
        return ret;
    }

    if( nullptr == _impl->_hdl )
    {
        ret.set_errno( errno_unaccess );
        return ret;
    }

    void* addr = ( void * )GetProcAddress( _impl->_hdl, func_name );
    if( nullptr == addr )
    {
        ret.set_errno( errno_general );
        return ret;
    }

    ret.set_value( addr );
    return ret;
}

NAMESPACE_TARO_BASIC_END
