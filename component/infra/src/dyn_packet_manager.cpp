
#include "dyn_packet.h"
#include "dyn_packet_manager_impl.h"
#include <string.h>

NAMESPACE_TARO_INFRA_BEGIN

static packet_allocator_ptr dyn_pkt_alloctor = std::make_shared< default_allocator >();

packet_allocator_ptr get_allocator()
{
    return dyn_pkt_alloctor;
}

void* default_allocator::alloc( uint32_t bytes )
{
    if( 0 == bytes )
    {
        return nullptr;
    }
    return malloc( bytes );
}

void* default_allocator::realloc( void* old_ptr, uint32_t bytes )
{
    if( nullptr == old_ptr || 0 == bytes )
    {
        return nullptr;
    }
    return realloc( old_ptr, bytes );
}

bool default_allocator::release( void* mem_ptr )
{
    if( nullptr == mem_ptr )
    {
        return false;
    }
    free( mem_ptr );
    return true;
}

packet_manager& packet_manager::instance()
{
    static packet_manager inst;
    return inst;
}

packet_manager::packet_manager()
{
    dyn_pkt_alloctor = std::make_shared<default_allocator>();
}

void packet_manager::set_allocator( packet_allocator_ptr const& allocator )
{
    TARO_ASSERT( nullptr != allocator );
    dyn_pkt_alloctor = allocator;
}

memory::packet_ptr create_default_packet( uint32_t cap,
                                          uint32_t extra_size,
                                          uint32_t power,
                                          destruct_extra_func const& func )
{
    memory::packet_ptr pkt = std::make_shared< dyn_packet >( extra_size, power, func );
    if( pkt == nullptr )
    {
        return pkt;
    }

    if( cap > 0 )
    {
        if( errno_ok != pkt->expand( cap ) )
        {
            return memory::packet_ptr();
        }
    }
    return pkt;
}

memory::packet_ptr create_default_ref_packet( memory::packet_ptr const& in )
{
    if( nullptr == in )
    {
        return memory::packet_ptr();
    }
    return std::make_shared< dyn_packet >( *std::dynamic_pointer_cast< dyn_packet >( in ) );
}

NAMESPACE_TARO_INFRA_END
