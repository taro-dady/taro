
#include "dyn_packet.h"
#include "basic/packet_creater.h"
#include <string.h>

NAMESPACE_TARO_BASIC_BEGIN

#define ALIGN( x, o ) ( ( x + (o - 1) )&~( o - 1 ) )

dyn_packet::dyn_packet( uint32_t extra_size, uint32_t power, destruct_extra_func const& func )
    : _buffer( nullptr )
    , _sz( 0 )
    , _cap( 0 )
    , _offset( 0 )
    , _align( 1 << power )
    , _extra( extra_size )
    , _func( func )
{
    
}

dyn_packet::~dyn_packet()
{
    if( _func && _extra.size() > 0 )
    {
        _func( _extra.data(), ( uint32_t )_extra.size() );
    }

    if( nullptr != _buffer )
    {
        free( _buffer );
        _buffer = nullptr;
    }
}

ret_val dyn_packet::append( uint8_t* buffer, uint32_t len )
{
    ret_val ret;

    if( ( nullptr == buffer ) || ( 0 == len ) )
    {
        ret.set_errno( errno_invalid_param );
        return ret;
    }

    auto rest = _cap - _sz - _offset;
    if( rest < ( uint32_t )len )
    {
        auto ex_ret = expand( _sz + _offset + len );
        if( !ex_ret )
        {
            return ex_ret;
        }
    }

    memcpy( _buffer + _sz + _offset, buffer, len );
    _sz += len;
    return ret;
}

ret_val dyn_packet::expand( uint32_t sz )
{
    ret_val ret;

    if( sz <= _cap )
    {
        return ret;
    }

    auto mem_sz = ALIGN( sz, _align );
    if( nullptr == _buffer )
    {
        _buffer = ( uint8_t* )malloc( mem_sz );
    }
    else
    {
        auto* tmp = ( uint8_t* )realloc( _buffer, mem_sz );
        if( nullptr == tmp )
        {
            ret.set_errno( errno_memory_out );
            return ret;
        }
        _buffer = tmp;
    }
    _cap = sz;
    return ret;
}

uint32_t dyn_packet::size() const
{
    return _sz;
}

ret_val dyn_packet::resize( uint32_t sz )
{
    ret_val ret;

    if( ( _cap - _offset ) < sz )
    {
        ret.set_errno( errno_invalid_param );
        return ret;
    }
    _sz = sz;
    return ret;
}

uint32_t dyn_packet::cap() const
{
    return _cap;
}

uint8_t* dyn_packet::buffer() const
{
    if( nullptr == _buffer )
    {
        return nullptr;
    }
    return _buffer + _offset;
}

ret_val dyn_packet::set_head_offset( uint32_t offset )
{
    ret_val ret;

    if( _cap < offset )
    {
        ret.set_errno( errno_invalid_param );
        return ret;
    }
    _offset = offset;
    return ret;
}

uint32_t dyn_packet::get_head_offset() const
{
    return _offset;
}

uint8_t* dyn_packet::extra_data() const
{
    if( _extra.empty() )
    {
        return nullptr;
    }
    return ( uint8_t* )_extra.data();
}

uint32_t dyn_packet::extra_data_size() const
{
    return ( uint32_t )_extra.size();
}

ret_val dyn_packet::expand_extra_data( uint32_t sz )
{
    _extra.resize( sz );
    return ret_val();
}

memory::packet_ptr create_packet( uint32_t cap, uint32_t extra_size, uint32_t power, destruct_extra_func const& func )
{
    memory::packet_ptr pkt = std::make_shared< dyn_packet >( extra_size, power, func );
    if( pkt == nullptr )
    {
        return pkt;
    }

    if( cap > 0 )
    {
        if( !pkt->expand( cap ) )
        {
            return memory::packet_ptr();
        }
    }
    return pkt;
}

NAMESPACE_TARO_BASIC_END
