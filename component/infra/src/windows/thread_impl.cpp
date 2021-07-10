
#include "infra/error.h"
#include "utils/defer.h"
#include "thread_manager.h"
#include "memory/object_pool.h"
#include "windows/thread_impl.h"

static unsigned _stdcall thread_proc( void* arg )
{
    USING_NAMESPACE_TARO_INFRA

    TARO_ASSERT( nullptr != arg );
    ( reinterpret_cast< thread_impl* >( arg ) )->run_thrd_cb();

    return 0;
}

NAMESPACE_TARO_INFRA_BEGIN

thread_impl::thread_impl( const char* name )
    : _stack_bytes( 0 )
    , _name( name )
    , _exit( false )
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

    int32_t prior     = 0;
    int32_t min_prior = THREAD_PRIORITY_IDLE;
    int32_t max_prior = THREAD_PRIORITY_TIME_CRITICAL;

    //设置优先级
    _priority    = priority;
    _stack_bytes = stack_size;
    if ( _priority == thread::thread_prio_high )
    {
        prior = THREAD_PRIORITY_TIME_CRITICAL;
    }
    else
    {
        constexpr int32_t thread_prior_top    = 1;
        constexpr int32_t thread_prior_mid    = 64;
        constexpr int32_t thread_prior_bottom = 127;

        int32_t real_prior = ( _priority == thread::thread_prio_middle ) ? thread_prior_mid : thread_prior_bottom;
        prior = max_prior - ( real_prior - thread_prior_top )*( max_prior - min_prior ) / ( thread_prior_bottom - thread_prior_top );
        if ( prior <= THREAD_PRIORITY_IDLE )
        {
            prior = THREAD_PRIORITY_IDLE;
        }
        else if ( prior <= THREAD_PRIORITY_LOWEST )
        {
            prior = THREAD_PRIORITY_LOWEST;
        }
        else if ( prior <= THREAD_PRIORITY_BELOW_NORMAL )
        {
            prior = THREAD_PRIORITY_BELOW_NORMAL;
        }
        else if ( prior <= THREAD_PRIORITY_NORMAL )
        {
            prior = THREAD_PRIORITY_NORMAL;
        }
        else if ( prior >= THREAD_PRIORITY_TIME_CRITICAL )
        {
            prior = THREAD_PRIORITY_TIME_CRITICAL;
        }
        else if ( prior >= THREAD_PRIORITY_HIGHEST )
        {
            prior = THREAD_PRIORITY_HIGHEST;
        }
        else if ( prior >= THREAD_PRIORITY_ABOVE_NORMAL )
        {
            prior = THREAD_PRIORITY_ABOVE_NORMAL;
        }
        else if ( prior >= THREAD_PRIORITY_NORMAL )
        {
            prior = THREAD_PRIORITY_NORMAL;
        }
        else
        {
            return errno_invalid_param;
        }
    }

    unsigned int id;
    _cb   = cb;
    _thrd = _beginthreadex( NULL, _stack_bytes, thread_proc, ( void* )this, 0, &id );
    if ( NULL == _thrd )
    {
        return errno_general;
    }

    if ( !SetThreadPriority( ( HANDLE )_thrd, prior ) )
    {
        return errno_set_param;
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

    while ( !_exit )
    {
        std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
    }
    
    WaitForSingleObject( ( HANDLE )_thrd, INFINITE );
    ( void )model::singleton< thrd_manger >::instance().remove_thrd( _name.c_str() );
    return errno_ok;
}

void thread_impl::run_thrd_cb()
{
    _exit.store( false );

    auto& inst = model::singleton< thrd_manger >::instance();
    inst.update_status( _name.c_str(), true );

    if ( _cb )
    {
        _cb();
    }

    inst.update_status( _name.c_str(), false );

    _exit.store( true );
}

NAMESPACE_TARO_INFRA_END
