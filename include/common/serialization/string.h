
#pragma once

#include "base/type_traits.h"
#include "serialization/defs.h"
#include <string>
#include <sstream>

NAMESPACE_TARO_SERIALIZATION_BEGIN

template< typename A >
typename std::enable_if< is_string< A >::value, std::string >::type
    type_to_str( A&& t )
{
    return std::move( t );
}

template< typename A >
typename std::enable_if< !is_string< A >::value, std::string >::type
    type_to_str( A&& t )
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

template< typename A >
typename std::enable_if< is_string< A >::value, A >::type
    str_to_value( const char* str )
{
    return str;
}

template< typename A >
typename std::enable_if< !is_string< A >::value, A >::type
    str_to_value( const char* str )
{
    std::stringstream ss( str );
    A value;
    ss >> value;
    return value;
}

NAMESPACE_TARO_SERIALIZATION_END
