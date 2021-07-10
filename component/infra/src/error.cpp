
#include "infra/error.h"
#include <string.h>

#define ERROR_MSG_MAX_BYTES 512

#if defined WIN32 || defined WIN64
    #include <windows.h>
    static __declspec( thread ) int32_t t_err_no = 0;
    static __declspec( thread ) char t_err_msg[ERROR_MSG_MAX_BYTES] = { 0 };
#else 
    static __thread int32_t t_err_no = 0;
    static __thread char t_err_msg[ERROR_MSG_MAX_BYTES] = { 0 };
#endif

NAMESPACE_TARO_INFRA_BEGIN

void set_err_no( int32_t err_no )
{
    t_err_no = err_no;
}

int32_t get_err_no( void )
{
    return t_err_no;
}

void set_err_msg( const char* msg )
{
    if( nullptr == msg || strlen( msg ) == 0 )
    {
        return;
    }
    strncpy( t_err_msg, msg, sizeof( t_err_msg ) );
}

const char* get_err_msg( void )
{
    if ( strlen( t_err_msg ) == 0 )
    {
        return nullptr;
    }
    return t_err_msg;
}

NAMESPACE_TARO_INFRA_END
