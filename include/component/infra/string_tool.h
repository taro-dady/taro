
#pragma once

#include "infra/defs.h"
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include <string.h>

NAMESPACE_TARO_INFRA_BEGIN

// url解析器 scheme://host[:port][path][?query][#fragment]
struct url_parser
{
public: // 公共类型

    // URL类型
    enum type
    {
        scheme,   // 协议
        host,     // 地址
        path,     // 路径 
        fragment, // 锚
    };

public: // 公共函数

    /*
    * @ brief 构造函数
    */
    url_parser();

    /*
    * @ brief 构造函数
    */
    explicit url_parser( const char* url );

    /*
    * @ brief 有效性判断
    */
    bool valid() const;

    /*
    * @ brief URL的具体参数
    */
    std::string& operator[]( type const& t );

    /*
    * @ brief 端口号
    */
    int32_t& port();

    /*
    * @ brief 添加query
    */
    bool add_query( const char* key, const char* value );

    /*
    * @ brief 指向首个参数
    */
    bool first_query();

    /*
    * @ brief 指向下一个query参数
    */
    bool next_query();

    /*
    * @ brief 当前query的key
    */
    std::string key() const;

    /*
    * @ brief 当前query的value
    */
    std::string value() const;

    /*
    * @ brief 查询query值
    */
    std::string find_query( const char* key );

    /*
    * @ brief 获取URL字符串
    */
    operator const char*() const;

private: // 私有变量

    struct impl;
    std::shared_ptr<impl> _impl;
};

/*
* @brief 去除字符前后的指定符号
* 
* @param[in/out] str  需要处理的字符
* @param[in]     f    指定的符号
*/
extern std::string trim( std::string const& str, const char* f = " " );

/*
* @brief 获取随机字符串
*
* @param[in] len 需要的字符长度
*/
extern std::string random_str( uint32_t len );

/*
* @brief 16进制字符串变为ASC
*/
extern std::string hex_to_str( std::string const& str );

NAMESPACE_TARO_INFRA_END
