
#include "timer_impl.h"
#include "utils/assert.h"

NAMESPACE_TARO_INFRA_BEGIN

timer::timer( const char* name )
    : _impl( std::make_shared< timer_impl >( name ) )
{
    TARO_ASSERT( nullptr != _impl );
}

timer::~timer()
{

}

int32_t timer::start( uint32_t delay, uint32_t period, timer_cb_t cb )
{
    return _impl->start( delay, period, cb );
}

int32_t timer::stop( bool wait )
{
    return _impl->stop( wait );
}

int32_t timer::modify( uint32_t period )
{
    return _impl->modify( period );
}

bool timer::is_running() const
{
    return _impl->is_running();
}

NAMESPACE_TARO_INFRA_END
