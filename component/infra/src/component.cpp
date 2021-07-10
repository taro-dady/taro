
#include "infra/error.h"
#include "infra/component.h"
#include "memory/object_pool.h"
#include <map>
#include <string.h>

NAMESPACE_TARO_INFRA_BEGIN

static memory::kobject_pool<component_key, component_factory_ptr> component_container;

struct component_key::impl
{
    std::pair<std::string, std::string> key;
};

component_key::component_key()
    : _impl( new impl )
{
    TARO_ASSERT( nullptr != _impl );
}

component_key::component_key( const char* iid, const char* cid )
    : _impl( new impl )
{
    TARO_ASSERT( nullptr != _impl
              && nullptr != iid 
              && strlen( iid ) > 0 
              && nullptr != cid 
              && strlen( cid ) > 0 );

    _impl->key.first  = iid;
    _impl->key.second = cid;
}

const char* component_key::iid() const
{
    return _impl->key.first.c_str();
}

const char* component_key::cid() const
{
    return _impl->key.second.c_str();
}

bool component_key::operator<( component_key const& r ) const
{
    return _impl->key < r._impl->key;
}

component_factory::~component_factory()
{

}

bool component_factory::register_factory( const component_key& key, component_factory_ptr const& this_ptr )
{
    if ( nullptr == this_ptr )
    {
        set_err_msg( "pointer is invalid" );
        return false;
    }
    return component_container.add( key, this_ptr );
}

bool component_factory::deregister_factory( const component_key& key )
{
    return component_container.remove( key );
}

component_factory_ptr component_factory::find( const component_key& key )
{
    component_factory_ptr factory_ptr;
    ( void )component_container.find( key, factory_ptr );
    return factory_ptr;
}

NAMESPACE_TARO_INFRA_END
