
#include "infra/time.h"
#include "infra/error.h"
#include "utils/defer.h"
#include "timer_manager.h"
#include "memory/creater.h"
#include "system/thread_pool.h"

NAMESPACE_TARO_INFRA_BEGIN

constexpr uint32_t idle_wait_time = 360 * 60 * 1000;

timer_manager::timer_manager()
    : _looping( false )
    , _modified( false )
    , _wakeup_tm( -1 )
{
    
}

timer_manager::~timer_manager()
{
    ( void )stop();
}

int32_t timer_manager::start( uint32_t worker_num )
{
    std::unique_lock< std::mutex > g( _mutex );
    if ( _looping )
    {
        set_err_msg( "already started." );
        return errno_multiple;
    }

    if( 0 == worker_num )
    {
        _thrd_pool.reset();
    }
    else
    {
        _thrd_pool.reset( new timer_thrd_pool_t );
        if( !_thrd_pool->start( worker_num ) )
        {
            _thrd_pool.reset();
            set_err_msg( "thread pool start failed." );
            return errno_general;
        }
    }

    _looping = true;
    _thrd    = std::thread( &timer_manager::thrd_proc, this );
    g.unlock();

    std::this_thread::sleep_for( std::chrono::milliseconds(200) ); // 等待线程完全启动

    return errno_ok;
}

int32_t timer_manager::stop()
{
    std::unique_lock< std::mutex > g( _mutex );
    if ( !_looping )
    {
        set_err_msg( "already stopped." );
        return errno_multiple;
    }

    if ( ( nullptr != _thrd_pool ) && !_thrd_pool->stop() )
    {
        set_err_msg( "stop thread pool failed." );
        return errno_general;
    }

    _thrd_pool.reset();
    _looping  = false;
    _modified = true;
    _cond.notify_one();
    g.unlock();

    _thrd.join();
    return errno_ok;
}

int32_t timer_manager::add_timer( timer_impl_ptr const& timer_ptr )
{
    TARO_ASSERT( nullptr != timer_ptr );

    std::lock_guard< std::mutex > g( _mutex );
    auto iter = std::find_if( std::begin( _container ), std::end( _container ), [&]( timer_impl_ptr const& t )
    {
        return timer_ptr->identity() == t->identity();
    } );

    if ( std::end( _container ) != iter )
    {
        set_err_msg( "already added." );
        return errno_multiple;
    }

    _container.emplace_back( timer_ptr );
    _container.sort( timer_compare );
    modify( _container.front()->get_call_time() );
    return errno_ok;
}

int32_t timer_manager::remove_timer( timer_impl_ptr const& timer_ptr )
{
    TARO_ASSERT( nullptr != timer_ptr );

    std::lock_guard< std::mutex > g( _mutex );
    auto iter = std::find_if( std::begin( _container ), std::end( _container ), [&]( timer_impl_ptr const& t )
    {
        return timer_ptr->identity() == t->identity();
    } );

    if ( std::end( _container ) == iter )
    {
        set_err_msg( "already removed." );
        return errno_not_found;
    }

    _container.erase( iter );
    if( !_container.empty() )
    {
        modify( _container.front()->get_call_time() );
    }
    else
    {
        modify( idle_wait_time );
    }
    return errno_ok;
}

void timer_manager::update_timer()
{
    std::lock_guard< std::mutex > g( _mutex );
    _container.sort( timer_compare );
    modify( _container.front()->get_call_time() );
}

int32_t timer_manager::get_timer_info( std::vector<timer_info>& info ) const
{
    std::lock_guard< std::mutex > g( _mutex );
    for ( auto const& one : _container )
    {
        info.emplace_back( one->get_info() );
    }
    return errno_ok;
}

void timer_manager::thrd_proc()
{
    _wakeup_tm = sys_time::current_ms() + idle_wait_time;

    while ( _looping )
    {
        std::unique_lock< std::mutex > g( _mutex );
        while ( _looping && !_modified )
        {
            int64_t diff = _wakeup_tm - sys_time::current_ms();
            if ( diff > 0 )
            {
                ( void )_cond.wait_until( g, std::chrono::system_clock::now() + std::chrono::milliseconds( diff ) );
                invoke();
                if ( _modified )
                {
                    _modified = false;
                }
            }
            else
            {
                invoke();
            }
        }
    }
}

void timer_manager::modify( int64_t delay )
{
    if( _wakeup_tm != delay )
    {
        _wakeup_tm = delay;
        _modified  = true;
        _cond.notify_one();
    }
}

void timer_manager::invoke()
{
    auto now = sys_time::current_ms();

    auto it = _container.begin();
    while ( it != _container.end() && ( ( *it )->get_call_time() <= now ) )
    {
        if( nullptr != _thrd_pool )
        {
            _thrd_pool->push( ( *it )->identity(), func::bind( &timer_impl::invoke, ( *it ) ) );
        }
        else
        {
            ( *it )->invoke();
        }
        
        if ( !( *it )->update_call_time() )
        {
            it = _container.erase( it );
        }
        else
        {
            ++it;
        }
    }

    if ( !_container.empty() )
    {
        _container.sort( timer_compare );
        modify( _container.front()->get_call_time() );
    }
    else
    {
        modify( sys_time::current_ms() + idle_wait_time );
    }
}

extern int32_t start_timer_manager( uint32_t worker_num )
{
    return timer_manager_inst.start( worker_num );
}

extern int32_t stop_timer_manager()
{
    return timer_manager_inst.stop();
}

extern int32_t get_timer_info( std::vector<timer_info>& info )
{
    return timer_manager_inst.get_timer_info( info );
}

NAMESPACE_TARO_INFRA_END
