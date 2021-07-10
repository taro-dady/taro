
#pragma once

#include "infra/defs.h"
#include "memory/scoped_ptr.h"
#include <string>

NAMESPACE_TARO_INFRA_BEGIN

// 时间对象
class sys_time
{
public: // 公共函数定义

    /*
    * @brief:  构造函数
    */
    sys_time();

    /*
    * @brief:  构造函数
    */
    sys_time( sys_time const& r );

    /*
    * @brief:  构造函数
    * 
    * @param[in] t 时间 ms
    */
    explicit sys_time( int64_t t );

    /*
    * @brief:  构造函数 将日期字符串转换为时间
    *   字符格式说明
    *   %Y:年                    %H:小时  00-23
    *   %m:年份中的月份,01-12     %M:分,  00-59
    *   %d 月份中的日期,01-31     %S:秒,  00-60
    *
    * @param[in] date     时间字符串
    * @param[in] date_tag 标识时间字符串为日期字符
    * @param[in] format   字符串格式
    */
    sys_time( const char* date, const char* format = "%Y-%m-%d %H:%M:%S" );

    /*
    * @brief:  赋值函数
    */
    sys_time& operator=( sys_time const& r );

    /*
    * @brief:  时间转换为日期
    * 
    * @param[in] format 字符串格式 如 "2018-01-01"
    * @return    日期字符串
    */
    std::string to_str( const char* format = "%Y-%m-%d %H:%M:%S" ) const;

    /*
    * @brief:  求差
    */
    int64_t diff( sys_time const& t );

    /*
    * @brief:  等于比较
    */
    bool operator==( sys_time const& t ) const;

    /*
    * @brief:  不等于比较
    */
    bool operator!=( sys_time const& t ) const;

    /*
    * @brief:  小于比较
    */
    bool operator<( sys_time const& t ) const;

    /*
    * @brief:  小于等于比较
    */
    bool operator<=( sys_time const& t ) const;

    /*
    * @brief:  大于比较
    */
    bool operator>( sys_time const& t ) const;

    /*
    * @brief:  大于等于比较
    */
    bool operator>=( sys_time const& t ) const;

    /*
    * @brief:  加法
    */
    sys_time& operator+=( sys_time const& t );

    /*
    * @brief:  加法
    */
    sys_time& operator+( sys_time const& t );

    /*
    * @brief:  减法
    */
    sys_time& operator-=( sys_time const& t );

    /*
    * @brief:  减法
    */
    sys_time& operator-( sys_time const& t );

    /*
    * @brief:  获取系统当前时间(毫秒)
    */
    static int64_t current_ms();

    /*
    * @brief:  获取系统当前时间(秒)
    */
    static int64_t current_sec();

private: // 私有变量

    struct impl;
    memory::scoped_ptr<impl> _impl;
};

NAMESPACE_TARO_INFRA_END
