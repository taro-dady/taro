
#include "timer_impl.h"
#include "infra/time.h"
#include "infra/error.h"
#include "utils/assert.h"
#include "timer_manager.h"
#include <atomic>
#include <thread>

NAMESPACE_TARO_INFRA_BEGIN

static std::atomic<uint32_t> glo_timer_seq( 0 );

timer_impl::timer_impl( const char* name )
    : _started( false )
    , _call_tm( -1 )
    , _identity( ++glo_timer_seq )
    , _name( name )
    , _running( false )
    , _run_tp( -1 )
{
    TARO_ASSERT( !_name.empty() );
}

timer_impl::~timer_impl()
{
    ( void )stop();
}

uint32_t timer_impl::identity() const
{
    return _identity;
}

int32_t timer_impl::start( uint32_t delay, uint32_t period, timer::timer_cb_t cb )
{
    if ( !cb )
    {
        set_err_msg( "call back is invalid." );
        return errno_invalid_param;
    }

    std::unique_lock< std::mutex > g( _mutex );
    if ( _started )
    {
        set_err_msg( "already start." );
        return errno_multiple;
    }

    _period   = period;
    _started  = true;
    _timer_cb = cb;
    _call_tm  = sys_time::current_ms() + delay;
    g.unlock();
    
    timer_manager_inst.add_timer( shared_from_this() );
    return errno_ok;
}

int32_t timer_impl::stop( bool wait )
{
    std::unique_lock< std::mutex > g( _mutex );
    if ( !_started )
    {
        set_err_msg( "already stop." );
        return errno_multiple;
    }

    if ( wait )
    {
        while ( _running )
        {
            std::this_thread::sleep_for( std::chrono::milliseconds( 10 ) );
        }
    }
    
    _started  = false;
    _call_tm  = -1;
    _timer_cb = timer::timer_cb_t();
    g.unlock();

    timer_manager_inst.remove_timer( shared_from_this() );
    return errno_ok;
}

int32_t timer_impl::modify( uint32_t period )
{
    if ( 0 == period )
    {
        set_err_msg( "period invalid." );
        return errno_invalid_param;
    }

    std::unique_lock< std::mutex > g( _mutex );
    _period  = period;
    _call_tm = sys_time::current_ms() + period;
    g.unlock();

    timer_manager_inst.update_timer();
    return errno_ok;
}

bool timer_impl::is_running() const
{
    return _running;
}

void timer_impl::invoke()
{
    std::unique_lock< std::mutex > g( _mutex );
    auto tmp = _timer_cb;
    g.unlock();

    if ( !tmp )
    {
        return;
    }

    _run_tp.store( sys_time::current_ms() );
    _running.store( true );
    tmp();
    _run_tp.store( -1 );
    _running.store( false );
}

bool timer_impl::update_call_time()
{
    std::unique_lock< std::mutex > g( _mutex );
    if ( 0 == _period )
    {
        return false;
    }
    _call_tm = sys_time::current_ms() + _period;
    return true;
}

int64_t timer_impl::get_call_time() const
{
    std::unique_lock< std::mutex > g( _mutex );
    return _call_tm;
}

timer_info timer_impl::get_info() const
{
    timer_info info;
    info.name    = _name;
    info.cost    = -1;
    info.running = _running;
    if ( _running )
    {
        info.cost = sys_time::current_ms() - _run_tp;
    }
    return info;
}

NAMESPACE_TARO_INFRA_END
