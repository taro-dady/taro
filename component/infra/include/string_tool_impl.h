
#pragma once

#include "infra/string_tool.h"
#include <map>

NAMESPACE_TARO_INFRA_BEGIN

using url_query_t    = std::map< std::string, std::string >;
using url_query_iter = std::map< std::string, std::string >::iterator;

struct url_parser::impl
{
    /*
    * @brief 构造函数
    */
    impl() : port( -1 ) {}

    /*
    * @brief 解析URL
    */
    bool parse( const char* url );

    /*
    * @brief 组装字符串
    */
    bool compose();

    /*
    * @ brief 分割部分参数
    */
    bool split_parts( std::string& value );

    int32_t        port;
    std::string    parts[url_parser::fragment + 1];
    url_query_t    query;
    std::string    str;
    url_query_iter iter;
};

NAMESPACE_TARO_INFRA_END
