
#include "windows/thread_impl.h"
#include <thread>

NAMESPACE_TARO_INFRA_BEGIN

thread::thread( const char* name )
    : _impl( new thread_impl( name ) )
{
    TARO_ASSERT( nullptr != _impl );
}

thread::~thread()
{

}

int32_t thread::start( thread_cb_t const& cb, uint32_t stack_size, thread_priority_e priority )
{
    return _impl->start( cb, stack_size, priority );
}

int32_t thread::stop()
{
    return _impl->stop();
}

uint64_t thread::current_tid()
{
    return ( int64_t )GetCurrentThreadId();
}

void thread::sleep( uint32_t ms )
{
    Sleep( ms );
}

NAMESPACE_TARO_INFRA_END
