
#include "dyn_packet.h"
#include "infra/error.h"
#include <string.h>

NAMESPACE_TARO_INFRA_BEGIN

extern packet_allocator_ptr get_allocator();

static void free_dyn_packet( void* mem )
{
    get_allocator()->release( mem );
}

static void* malloc_dyn_packet( uint32_t sz )
{
    return get_allocator()->alloc( sz );
}

dyn_packet::dyn_packet( uint32_t extra_size, uint32_t power, destruct_extra_func const& func )
    : _ref( false )
    , _sz( 0 )
    , _cap( 0 )
    , _offset( 0 )
    , _align( 1 << power )
    , _extra( extra_size )
    , _func( func )
{
    
}

dyn_packet::dyn_packet( dyn_packet const& other )
    : _ref( true )
    , _sz( other._sz )
    , _cap( other._cap )
    , _offset( other._offset )
    , _align( other._align )
    , _extra( other._extra )
    , _func( other._func )
    , _buffer( other._buffer )
{

}

dyn_packet::~dyn_packet()
{
    if( _func && _extra.size() > 0 )
    {
        _func( _extra.data(), ( uint32_t )_extra.size() );
    }
}

int32_t dyn_packet::append( uint8_t* buffer, uint32_t len )
{
    if( ( nullptr == buffer ) || ( 0 == len ) )
    {
        set_err_msg( "invalid parameter" );
        return errno_invalid_param;
    }

    if( _ref )
    {
        set_err_msg( "reference packet not support" );
        return errno_unspported;
    }

    auto rest = _cap - _sz - _offset;
    if( rest < ( uint32_t )len )
    {
        auto ex_ret = expand( _sz + _offset + len );
        if( ex_ret != errno_ok )
        {
            set_err_msg( "expand packet failed" );
            return ex_ret;
        }
    }

    memcpy( _buffer.get() + _sz + _offset, buffer, len );
    _sz += len;
    return errno_ok;
}

int32_t dyn_packet::expand( uint32_t sz )
{
    if( _ref )
    {
        set_err_msg( "reference packet not support" );
        return errno_unspported;
    }

    if( sz <= _cap )
    {
        set_err_msg( "cap too small" );
        return errno_memory_out;
    }

    auto mem_sz = TARO_ALIGN( sz, _align );
    if( nullptr == _buffer )
    {
        _buffer.reset( ( uint8_t* )malloc_dyn_packet( mem_sz ), free_dyn_packet );
    }
    else
    {
        auto* tmp = ( uint8_t* )malloc_dyn_packet( mem_sz );
        if( nullptr == tmp )
        {
            set_err_msg( "malloc failed." );
            return errno_memory_out;
        }
        memcpy( tmp, _buffer.get(), _sz );
        _buffer.reset( tmp, free_dyn_packet );
    }
    _cap = sz;
    return errno_ok;
}

uint32_t dyn_packet::size() const
{
    return _sz;
}

int32_t dyn_packet::resize( uint32_t sz )
{
    if( ( _cap - _offset ) < sz )
    {
        set_err_msg( "parameter invalid." );
        return errno_invalid_param;
    }
    _sz = sz;
    return errno_ok;
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
    return _buffer.get() + _offset;
}

int32_t dyn_packet::set_head_offset( uint32_t offset )
{
    if( _cap < offset )
    {
        set_err_msg( "parameter invalid." );
        return errno_invalid_param;
    }
    _offset = offset;
    return errno_ok;
}

int32_t dyn_packet::get_head_offset() const
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

int32_t dyn_packet::expand_extra_data( uint32_t sz )
{
    _extra.resize( sz );
    return errno_ok;
}

NAMESPACE_TARO_INFRA_END
