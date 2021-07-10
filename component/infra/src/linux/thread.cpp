
#include "linux/thread_impl.h"
#include <thread>
#include <unistd.h>
#include <sys/syscall.h>

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
    return ( int64_t )syscall( SYS_gettid );
}

void thread::sleep( uint32_t ms )
{
    if ( 0 == ms )
    {
        pthread_yield();
        return;
    }

    struct timespec t;
	t.tv_sec  = ms / 1000;
	t.tv_nsec = ( (ms % 1000) * 1000000 ) ; // 0 to 999 999 999
	while( nanosleep( &t, &t ) != 0 );
}

NAMESPACE_TARO_INFRA_END
