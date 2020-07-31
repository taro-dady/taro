
#include "packet_list_impl.h"
#include "basic/packet_creater.h"
#include <string.h>

NAMESPACE_TARO_BASIC_BEGIN

packet_list::impl::impl()
    : _size( 0 )
{

}

void packet_list::impl::append( memory::packet_ptr const& p )
{
    _size += p->size();
    _list.emplace_back( p );
}

int32_t packet_list::impl::size() const
{
    return ( int32_t )_size;
}

int32_t packet_list::impl::try_read( uint8_t* buf, uint32_t size, uint32_t offset )
{
    if( offset >= _size )
    {
        return errno_invalid_param;
    }

    /// 定位到指定位置
    uint32_t bytes = 0, temp = 0;
    auto it = _list.begin();
    for( ; it != _list.end(); ++it )
    {
        /// 判断是否在范围之内
        if( offset <= bytes + ( *it )->size() )
        {
            temp = ( offset - bytes ); /// 记录该包的偏移位置
            break;
        }

        /// 记录已扫描的大小
        bytes += ( *it )->size();
    }

    /// 读取数据
    int32_t read_bytes = 0;
    for( ; it != _list.end(); ++it )
    {
        uint32_t to_read_bytes = size - read_bytes;
        uint32_t no_read_bytes = ( *it )->size() - temp;

        if( to_read_bytes <= no_read_bytes )
        {
            memcpy( buf + read_bytes, ( *it )->buffer() + temp, to_read_bytes );
            read_bytes += to_read_bytes;
            return read_bytes;
        }

        memcpy( buf + read_bytes, ( *it )->buffer() + temp, no_read_bytes );
        read_bytes += no_read_bytes;
        temp = 0;
    }
    return read_bytes;
}

memory::packet_ptr packet_list::impl::read( uint32_t size )
{
    memory::packet_ptr p;
    if( size > _size )
    {
        return p;
    }

    /// 删除整个packet
    auto it = _list.begin();
    if( ( *it )->size() == ( int32_t )size )
    {
        p = *it;
        _list.erase( it );
        _size -= size;
        return p;
    }

    p = create_packet( size );
    if( nullptr == p )
    {
        return p;
    }

    int32_t read_bytes = 0;
    while( it != _list.end() )
    {
        uint32_t to_read_bytes = size - read_bytes;
        uint32_t no_read_bytes = ( *it )->size();
        if( to_read_bytes == no_read_bytes )
        {
            p->append( ( *it )->buffer(), to_read_bytes );
            _size -= to_read_bytes;
            it     = _list.erase( it );
            break;
        }
        else if( to_read_bytes < no_read_bytes )
        {
            p->append( ( *it )->buffer(), to_read_bytes );
            _size -= to_read_bytes;
            uint32_t head_offset = ( *it )->get_head_offset();
            head_offset += to_read_bytes;
            ( *it )->set_head_offset( head_offset );
            ( *it )->resize( ( *it )->size() - to_read_bytes );
            break;
        }

        p->append( ( *it )->buffer(), ( *it )->size() );

        read_bytes += ( *it )->size();
        _size      -= ( *it )->size();
        it          = _list.erase( it );
    }
    return p;
}

int32_t packet_list::impl::consume( uint32_t size )
{
    if( size > _size )
    {
        return errno_invalid_param;
    }

    if( size == _size )
    {
        reset();
        return size;
    }

    int32_t read_bytes = 0;
    auto it = _list.begin();
    while( it != _list.end() )
    {
        uint32_t to_read_bytes = size - read_bytes;
        uint32_t no_read_bytes = ( *it )->size();
        if( to_read_bytes == no_read_bytes )
        {
            _size -= to_read_bytes;
            it     = _list.erase( it );
            break;
        }
        else if( to_read_bytes < no_read_bytes )
        {
            _size -= to_read_bytes;
            uint32_t head_offset = ( *it )->get_head_offset();
            head_offset += to_read_bytes;
            ( *it )->set_head_offset( head_offset );
            ( *it )->resize( ( *it )->size() - to_read_bytes );
            break;
        }

        read_bytes += ( *it )->size();
        _size      -= ( *it )->size();

        it = _list.erase( it );
    }
    return size;
}

void packet_list::impl::reset()
{
    _list.clear();
    _size = 0;
}

bool packet_list::impl::search( const char* str, uint32_t offset, uint32_t& pos )
{
    if( nullptr == str || 0 == strlen( str ) )
    {
        return false;
    }

    uint32_t size = ( uint32_t )strlen( str );

    std::string read_str( size, 0 );
    while( try_read( ( uint8_t* )read_str.c_str(), size, offset ) == size )
    {
        if( read_str == str )
        {
            pos = offset;
            return true;
        }
        ++offset;
    }
    return false;
}

NAMESPACE_TARO_BASIC_END
