
#pragma once

#include "base/base.h"
#include <string>
#include <type_traits>

NAMESPACE_TARO_BEGIN

template< typename T >
struct unwapper_type
{
    using type = typename std::remove_cv<typename std::remove_reference< T >::type>::type;
};

template< typename T >
struct is_string
{
    enum { value = false };
};

template<>
struct is_string<std::string>
{
    enum { value = true };
};

template<>
struct is_string<char*>
{
    enum { value = true };
};

template<>
struct is_string<const char*>
{
    enum { value = true };
};

NAMESPACE_TARO_END