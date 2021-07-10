
#include "infra/time.h"
#include "utils/assert.h"
#include "string_tool_impl.h"
#include <sstream>

NAMESPACE_TARO_INFRA_BEGIN

extern std::string hex_to_str( std::string const& str )
{
    std::string ret;
    std::stringstream ss;
    for( uint32_t i = 0; i < str.size(); ++i )
    {
        ss << std::hex << str[i];
        uint32_t temp;
        if( ( i & 1 ) == 1 )
        {
            ss >> temp;
            ss.clear();
            ret.push_back( char( temp ) );
        }
    }
    return ret;
}

extern std::string trim( std::string const& str, const char* f )
{
    auto s = str.find_first_not_of( f );
    auto e = str.find_last_not_of( f );
    return str.substr( s, e - s + 1 );
}

extern std::string random_str( uint32_t len )
{
    ::srand( ( uint32_t )sys_time::current_ms() );

    std::stringstream ss;
    for( uint32_t i = 0; i < len; ++i )
    {
        auto temp = ( uint8_t )( rand() % 256 );
        if( temp == 0 )   // 随机数不要0, 0 会干扰对字符串长度的判断
        {
            temp = 128;
        }
        ss << temp;
    }
    return ss.str();
}

bool url_parser::impl::parse( const char* url )
{
    if( nullptr == url || 0 == strlen( url ) )
    {
        return false;
    }

    std::string tmp = url;
    std::string query_str;

    const char* scheme_split = "://";
    auto pos = tmp.find( scheme_split );
    if( pos != std::string::npos )
    {
        // 获取协议
        parts[scheme] = tmp.substr( 0, pos );
        tmp = tmp.substr( pos + strlen( scheme_split ) );
    }

    // 获取锚
    pos = tmp.find( "#" );
    if( pos != std::string::npos )
    {
        parts[fragment] = tmp.substr( pos + 1 );
        tmp = tmp.substr( 0, pos );
    }

    // 获取query
    pos = tmp.find( "?" );
    if( pos != std::string::npos )
    {
        query_str = tmp.substr( pos + 1 );
        tmp = tmp.substr( 0, pos );
    }

    // 获取path
    pos = tmp.find( "/" );
    if( pos != std::string::npos )
    {
        parts[path] = tmp.substr( pos );
        tmp = tmp.substr( 0, pos );
    }

    // 获取主机地址
    if( !tmp.empty() )
    {
        parts[host] = tmp;
    }

    return split_parts( query_str );
}

bool url_parser::impl::compose()
{
    if( parts[scheme].empty() || parts[host].empty() )
    {
        return false;
    }

    std::stringstream ss;
    ss << parts[scheme] << "://" << parts[host];
    if( port > 0 )
    {
        ss << ":" << port;
    }

    if( !parts[path].empty() )
    {
        ss << parts[path];
    }

    if( !query.empty() )
    {
        ss << "?";
        uint32_t i = 0;
        for( auto const& one : query )
        {
            ss << one.first << "=" << one.second;
            if( i < query.size() - 1 )
            {
                ss << "&";
            }
            ++i;
        }
    }

    if( !parts[fragment].empty() )
    {
        ss << "#" << parts[fragment];
    }

    str = ss.str();
    return true;
}

bool url_parser::impl::split_parts( std::string& value )
{
    auto& host_str = parts[host];
    auto pos       = host_str.find( ":" );
    if( pos != std::string::npos )
    {
        port     = atoi( host_str.substr( pos + 1 ).c_str() );
        host_str = host_str.substr( 0, pos );
    }

    if( value.empty() )
    {
        return true;
    }

    while( ( pos = value.find( "&" ) ) != std::string::npos )
    {
        auto tmp = value.substr( 0, pos );
        value    = value.substr( pos + 1 );

        pos = tmp.find( "=" );
        if( pos == std::string::npos )
        {
            return false;
        }
        query[tmp.substr( 0, pos )] = tmp.substr( pos + 1 );
    }

    pos = value.find( "=" );
    if( pos == std::string::npos )
    {
        return false;
    }
    query[value.substr( 0, pos )] = value.substr( pos + 1 );
    return true;
}

url_parser::url_parser()
    : _impl( new impl )
{

}

url_parser::url_parser( const char* url )
    : _impl( new impl )
{
    if( !_impl->parse( url ) )
    {
        _impl.reset();
    }
}

bool url_parser::valid() const
{
    return nullptr != _impl;
}

std::string& url_parser::operator[]( type const& t )
{
    TARO_ASSERT( nullptr != _impl );
    return _impl->parts[t];
}

int32_t& url_parser::port()
{
    TARO_ASSERT( nullptr != _impl );
    return _impl->port;
}

bool url_parser::add_query( const char* key, const char* value )
{
    if( nullptr == key   || 0 == strlen( key )
     || nullptr == value || 0 == strlen( value ) )
    {
        return false;
    }

    TARO_ASSERT( nullptr != _impl );
    return _impl->query.insert( std::make_pair( key, value ) ).second;
}

bool url_parser::first_query()
{
    TARO_ASSERT( nullptr != _impl );

    if( _impl->query.empty() )
    {
        return false;
    }
    _impl->iter = _impl->query.begin();
    return true;
}

bool url_parser::next_query()
{
    TARO_ASSERT( nullptr != _impl );

    ++_impl->iter;
    return _impl->iter != _impl->query.end();
}

std::string url_parser::key() const
{
    TARO_ASSERT( nullptr != _impl && _impl->iter != _impl->query.end() );
    return _impl->iter->first;
}

std::string url_parser::value() const
{
    TARO_ASSERT( nullptr != _impl && _impl->iter != _impl->query.end() );
    return _impl->iter->second;
}

std::string url_parser::find_query( const char* key )
{
    std::string ret;

    if( nullptr == key || 0 == strlen( key ) )
    {
        return ret;
    }

    TARO_ASSERT( nullptr != _impl );

    auto it = _impl->query.find( key );
    if( it != _impl->query.end() )
    {
        ret = it->second;
    }
    return ret;
}

url_parser::operator const char*() const
{
    TARO_ASSERT( nullptr != _impl );

    if( !_impl->compose() )
    {
        return nullptr;
    }
    return _impl->str.c_str();
}

NAMESPACE_TARO_INFRA_END
