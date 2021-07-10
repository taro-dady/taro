
#include "infra/error.h"
#include "utils/defer.h"
#include "thread_manager.h"
#include "linux/thread_impl.h"
#include "memory/object_pool.h"
#include <string.h>

static void* thread_proc( void *arg ) 
{
    USING_NAMESPACE_TARO_INFRA

    TARO_ASSERT( nullptr != arg );
    ( reinterpret_cast< thread_impl* >( arg ) )->run_thrd_cb();

    return nullptr;
}

NAMESPACE_TARO_INFRA_BEGIN

thread_impl::thread_impl( const char* name )
    : _stack_bytes( 0 )
    , _name( name )
    , _start( false )
    , _priority( thread::thread_prio_low )
{
    TARO_ASSERT( nullptr != name && 0 != strlen( name ) );
}

thread_impl::~thread_impl()
{
    ( void )stop();
}

int32_t thread_impl::start( thread::thread_cb_t const& cb, uint32_t stack_size, thread::thread_priority_e priority )
{
    if ( !cb )
    {
        set_err_msg( "param invalid." );
        return errno_invalid_param;
    }

    bool expr = false;
    if ( !_start.compare_exchange_weak( expr, true ) )
    {
        set_err_msg( "thread started." );
        return errno_multiple;
    }

    auto& inst = model::singleton< thrd_manger >::instance();
    utils::defer rc( [&]() 
    { 
        _start = false; 
        ( void )inst.remove_thrd( _name.c_str() );
    } );
    
    if ( !inst.add_thrd( thread_info_t{ _name, false, stack_size, -1, priority } ) )
    {
        set_err_msg( "thread registered." );
        return errno_multiple;
    }

    pthread_attr_t attr;
    if ( 0 != pthread_attr_init( &attr) )
    {
        set_err_msg( "get thread attribute failed." );
        return errno_set_param;
    }
    
    if ( _stack_bytes > 0 )
    {
        constexpr const uint32_t MIN_STACK_SIZE = 1024 * 4;
        if ( _stack_bytes < MIN_STACK_SIZE )
        {
            _stack_bytes = MIN_STACK_SIZE;
        }

        if ( 0 != pthread_attr_setstacksize( &attr, _stack_bytes ) )
        {
            set_err_msg( "set thread stack size failed." );
            return errno_set_param;
        }
    }

    _priority = priority;
    if ( thread::thread_prio_low != _priority )
    {
        struct sched_param sp;
        memset( ( void* )&sp, 0, sizeof( sp ) );
        if ( thread::thread_prio_middle == _priority )
        {
            sp.sched_priority = ( sched_get_priority_max( SCHED_RR ) + sched_get_priority_min( SCHED_RR ) ) / 2;
        }
        else if ( thread::thread_prio_high == _priority )
        {
            sp.sched_priority = sched_get_priority_max( SCHED_RR );
        }

        if ( 0 != pthread_attr_setinheritsched( &attr, PTHREAD_EXPLICIT_SCHED ) )
        {
            set_err_msg( "set thread inherit sched failed." );
            return errno_set_param;
        }

        if ( 0 != pthread_attr_setschedpolicy( &attr, SCHED_RR ) )
        {
            set_err_msg( "set thread sched policy failed." );
            return errno_set_param;
        }

        if ( 0 != pthread_attr_setschedparam( &attr, &sp ) )
		{
			set_err_msg( "set thread sched param failed." );
			return errno_set_param;
		}
    }
    else
	{
		if ( 0 != pthread_attr_setschedpolicy( &attr, SCHED_OTHER ) )
		{
			set_err_msg("set default thread policy schedule failed.\n");
			return errno_general;
		}
	}
    
    _cb = cb;
    auto ret = pthread_create( &_thrd, &attr, thread_proc, ( void* )this );
    pthread_attr_destroy( &attr );
    if ( ret == -1 )
    {
        set_err_msg("create thread failed.\n");
        return errno_general;
    }

    rc = false;
    return errno_ok;
}

int32_t thread_impl::stop()
{
    bool expr = true;
    if ( !_start.compare_exchange_weak( expr, false ) )
    {
        return errno_multiple;
    }

    pthread_join( _thrd, 0 );
    ( void )model::singleton< thrd_manger >::instance().remove_thrd( _name.c_str() );
    return errno_ok;
}

void thread_impl::run_thrd_cb()
{
    auto& inst = model::singleton< thrd_manger >::instance();
    inst.update_status( _name.c_str(), true );
    
    if ( _cb )
    {
        _cb();
    }
    
    inst.update_status( _name.c_str(), false );
}

NAMESPACE_TARO_INFRA_END
