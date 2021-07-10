
#include "infra/time.h"
#include "infra/error.h"
#include "utils/assert.h"
#include <ctime>
#include <chrono>
#include <sstream>
#include <string.h>


#if defined _WIN32 || defined _WIN64
extern char *strptime( const char *buf, const char *fmt, struct tm *tm );
#endif

NAMESPACE_TARO_INFRA_BEGIN

struct sys_time::impl
{
    time_t _time;
};

sys_time::sys_time()
    : _impl( new impl )
{
    _impl->_time = ::time( nullptr );
}

sys_time::sys_time( sys_time const& r )
    : _impl( new impl )
{
    _impl->_time = r._impl->_time;
}

sys_time::sys_time( int64_t t )
    : _impl( new impl )
{
    _impl->_time = t;
}

sys_time::sys_time( const char* date, const char* format )
    : _impl( new impl )
{
    TARO_ASSERT( nullptr != date
              && strlen( date ) > 0
              && nullptr != format
              && strlen( format ) > 0 );

    struct tm tm_tmp;
    strptime( date, format, &tm_tmp );

#if defined _WIN32 || defined _WIN64
    tm_tmp.tm_year += 70;
#endif
    tm_tmp.tm_isdst = -1;
    
    _impl->_time = ::mktime( &tm_tmp );
}

sys_time& sys_time::operator=( sys_time const& r )
{
    if ( this != &r )
    {
        _impl->_time = r._impl->_time;
    }
    return *this;
}

std::string sys_time::to_str( const char* format ) const
{
    std::string ret;

    if ( nullptr == format || 0 == strlen( format ) )
    {
        set_err_no( errno_invalid_param );
        return ret;
    }

    char tstr[128] = { 0 };
    tm* gmt = localtime( &_impl->_time );
    strftime( tstr, sizeof( tstr ), format, gmt );
    if ( strlen( tstr ) == 0 )
    {
        set_err_no( errno_general );
        return ret;
    }
    return tstr;
}

int64_t sys_time::diff( sys_time const& t )
{
    return ( int64_t )( _impl->_time - t._impl->_time );
}

bool sys_time::operator==( sys_time const& t ) const
{
    return _impl->_time == t._impl->_time;
}

bool sys_time::operator!=( sys_time const& t ) const
{
    return _impl->_time != t._impl->_time;
}

bool sys_time::operator<( sys_time const& t ) const
{
    return _impl->_time < t._impl->_time;
}

bool sys_time::operator<=( sys_time const& t ) const
{
    return _impl->_time <= t._impl->_time;
}

bool sys_time::operator>( sys_time const& t ) const
{
    return _impl->_time > t._impl->_time;
}

bool sys_time::operator>=( sys_time const& t ) const
{
    return _impl->_time >= t._impl->_time;
}

sys_time& sys_time::operator+=( sys_time const& t )
{
    _impl->_time += t._impl->_time;
    return *this;
}

sys_time& sys_time::operator+( sys_time const& t )
{
    _impl->_time += t._impl->_time;
    return *this;
}

sys_time& sys_time::operator-=( sys_time const& t )
{
    _impl->_time -= t._impl->_time;
    return *this;
}

sys_time& sys_time::operator-( sys_time const& t )
{
    _impl->_time -= t._impl->_time;
    return *this;
}

int64_t sys_time::current_ms()
{
    auto duration = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::system_clock::now().time_since_epoch() );
    return ( time_t )duration.count();
}

int64_t sys_time::current_sec()
{
    auto duration = std::chrono::duration_cast< std::chrono::seconds >( std::chrono::system_clock::now().time_since_epoch() );
    return ( time_t )duration.count();
}

NAMESPACE_TARO_INFRA_END
