
#include "utils/assert.h"
#include "packet_list_impl.h"

NAMESPACE_TARO_BASIC_BEGIN

packet_list::packet_list()
    : _impl( new impl )
{

}

packet_list::~packet_list()
{

}

int32_t packet_list::append( memory::packet_ptr const& p )
{
    if( p == nullptr || p->size() == 0 )
    {
        return errno_invalid_param;
    }

    TARO_ASSERT( _impl != nullptr );
    _impl->append( p );
    return errno_ok;
}

int32_t packet_list::size() const
{
    TARO_ASSERT( _impl != nullptr );
    return _impl->size();
}

int32_t packet_list::try_read( uint8_t* buf, uint32_t size, uint32_t offset )
{
    if( buf == nullptr || 0 == size )
    {
        return errno_invalid_param;
    }

    TARO_ASSERT( _impl != nullptr );
    return _impl->try_read( buf, size, offset );
}

memory::packet_ptr packet_list::read( uint32_t size )
{
    if( 0 == size )
    {
        return memory::packet_ptr();
    }

    TARO_ASSERT( _impl != nullptr );
    return _impl->read( size );
}

int32_t packet_list::consume( uint32_t size )
{
    if( 0 == size )
    {
        return errno_invalid_param;
    }

    TARO_ASSERT( _impl != nullptr );
    return _impl->consume( size );
}

void packet_list::reset()
{
    TARO_ASSERT( _impl != nullptr );
    return _impl->reset();
}

bool packet_list::search( const char* str, uint32_t offset, uint32_t& pos )
{
    TARO_ASSERT( _impl != nullptr );
    return _impl->search( str, offset, pos );
}

NAMESPACE_TARO_BASIC_END

