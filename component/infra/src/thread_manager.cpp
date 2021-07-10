
#include "thread_manager.h"
#include <string.h>
#include <algorithm>

NAMESPACE_TARO_INFRA_BEGIN

bool thrd_manger::add_thrd( thread_info_t const& lite )
{
    std::lock_guard< std::mutex > g( _mutex );
    auto it = std::find_if( std::begin( _lites ), std::end( _lites ), [&]( thread_info_t const& one )
    {
        return one.name == lite.name;
    } );

    if ( it != std::end( _lites ) )
    {
        return false;
    }

    _lites.emplace_back( lite );
    return true;
}


bool thrd_manger::remove_thrd( const char* name )
{
    TARO_ASSERT( nullptr != name && 0 != strlen( name ) );

    std::lock_guard< std::mutex > g( _mutex );
    auto it = std::find_if( std::begin( _lites ), std::end( _lites ), [&]( thread_info_t const& one )
    {
        return one.name == name;
    } );

    if ( it == std::end( _lites ) )
    {
        return false;
    }

    _lites.erase( it );
    return true;
}

void thrd_manger::update_status( const char* name, bool running )
{
    std::lock_guard< std::mutex > g( _mutex );
    auto it = std::find_if( std::begin( _lites ), std::end( _lites ), [&]( thread_info_t const& one )
    {
        return one.name == name;
    } );

    if ( it == std::end( _lites ) )
    {
        return;
    }

    if ( running )
    {
        it->is_running = true;
        it->run_time = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::steady_clock::now().time_since_epoch() ).count();
    }
    else
    {
        it->is_running = false;
        it->run_time = -1;
    }
}

std::vector< thread_info_t > thrd_manger::get_thrd_info() const
{
    std::vector< thread_info_t > ret;

    std::unique_lock< std::mutex > g( _mutex );
    auto tmp_list = _lites;
    g.unlock();

    for ( auto one : tmp_list )
    {
        if ( one.is_running )
        {
            auto now = std::chrono::duration_cast< std::chrono::milliseconds >( std::chrono::steady_clock::now().time_since_epoch() ).count();
            one.run_time = now - one.run_time;
        }
        ret.emplace_back( one );
    }

    return ret;
}

std::vector< thread_info_t > get_all_thread_info()
{
    return model::singleton< thrd_manger >::instance().get_thrd_info();
}

NAMESPACE_TARO_INFRA_END
