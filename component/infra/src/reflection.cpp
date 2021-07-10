
#include "infra/error.h"
#include "infra/reflection.h"
#include <map>
#include <mutex>
#include <algorithm>

NAMESPACE_TARO_INFRA_BEGIN

using value_t = std::pair< std::string, std::vector<reflection_ptr> >;

struct reflection_manager::impl
{
    std::mutex                                 lock;
    std::map< const std::type_info*, value_t > ref_objs;
};

reflection_manager& reflection_manager::instance()
{
    static reflection_manager inst;
    return inst;
}

reflection_manager::reflection_manager()
    : _impl( new impl )
{
    
}

reflection_manager::~reflection_manager()
{

}

int32_t reflection_manager::register_class( const char* class_name, const std::type_info* info )
{
    if( nullptr == class_name || 0 == strlen( class_name ) || nullptr == info )
    {
        return errno_invalid_param;
    }

    if( nullptr == _impl )
    {
        return errno_unaccess;
    }

    std::lock_guard<std::mutex> g( _impl->lock );
    if( !_impl->ref_objs.insert( std::make_pair( info,
        std::make_pair( class_name, std::vector<reflection_ptr>() ) ) ).second )
    {
        return errno_general;
    }
    return errno_ok;
}

std::string reflection_manager::find_class_name( const std::type_info* info ) const
{
    if( nullptr == info )
    {
        set_err_no( errno_invalid_param );
        return "";
    }

    if( nullptr == _impl )
    {
        set_err_no( errno_unaccess );
        return "";
    }

    std::string ret;
    std::lock_guard<std::mutex> g( _impl->lock );
    auto it = _impl->ref_objs.find( info );
    if( it == _impl->ref_objs.end() )
    {
        set_err_no( errno_general );
    }
    else
    {
        ret = it->second.first;
    }
    return ret;
}

int32_t reflection_manager::register_class_member( const char* class_name, reflection_ptr const& ref )
{
    if( nullptr == class_name || 0 == strlen( class_name ) || nullptr == ref )
    {
        return errno_invalid_param;
    }

    if( nullptr == _impl )
    {
        return errno_unaccess;
    }

    std::lock_guard<std::mutex> g( _impl->lock );
    auto it = std::find_if( std::begin( _impl->ref_objs ), std::end( _impl->ref_objs ), [&]( std::pair< const std::type_info*, value_t > const& item ) 
    {
        return item.second.first == class_name;
    } );
    
    if( std::end( _impl->ref_objs ) == it )
    {
        return errno_general;
    }

    auto iter = std::find_if( std::begin( it->second.second ), std::end( it->second.second ), [&]( reflection_ptr const& ptr ) 
    {
        return std::string( ref->get_name() ) == ptr->get_name();
    } );

    if( std::end( it->second.second ) != iter )
    {
        return errno_general;
    }
    it->second.second.emplace_back( ref );
    return errno_ok;
}

std::vector<reflection_ptr> reflection_manager::get_class_members( const char* class_name ) const
{
    std::vector<reflection_ptr> ret;

    if( nullptr == class_name || 0 == strlen( class_name ) )
    {
        set_err_no( errno_invalid_param );
        return ret;
    }

    if( nullptr == _impl )
    {
        set_err_no( errno_unaccess );
        return ret;
    }

    std::lock_guard<std::mutex> g( _impl->lock );
    auto it = std::find_if( std::begin( _impl->ref_objs ), std::end( _impl->ref_objs ), [&]( std::pair< const std::type_info*, value_t > const& item )
    {
        return item.second.first == class_name;
    } );

    if( std::end( _impl->ref_objs ) == it )
    {
        set_err_no( errno_general );
        return ret;
    }
    ret = it->second.second;
    return ret;
}

NAMESPACE_TARO_INFRA_END
