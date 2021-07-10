
#pragma once

#include "infra/defs.h"
#include "function/inc.h"
#include "memory/packet.h"
#include "memory/scoped_ptr.h"
#include <string>

NAMESPACE_TARO_INFRA_BEGIN

// 包链表，用于协议解析时的数据结构
class packet_list
{
public: // 公共类型定义

    /*
    * @brief      字符比较函数
    *
    * @param[in]  比较的字符串
    * @param[in]  比较的字符串
    * @return     true 相等  false 不相等
    */
    using equal_string_cb_t = std::function<bool( std::string const&, std::string const& )>;

public: // 公共函数定义

    /*
    * @brief      构造函数
    */
    packet_list();

    /*
    * @brief      析构函数
    */
    ~packet_list();

    /*
    * @brief      添加数据包
    *
    * @param[in]  p   数据包
    */
    int32_t append( memory::packet_ptr const& p );

    /*
    * @brief      获取大小
    *
    * @return     int32_t
    * @retval     大小
    */
    int32_t size() const;

    /*
    * @brief      尝试读取字节(不影响原有数据)
    *
    * @param[out] buf    输出buffer
    * @param[in]  size   需要读取的字节数
    * @param[in]  offset 偏移位置
    * @return     int32_t
    * @retval     实际读取的字节
    */
    int32_t try_read( uint8_t* buf, uint32_t size, uint32_t offset );

    /*
    * @brief      读取数据包(读取数据后删除指定长度数据)
    *
    * @param[in]  size  需要读取的字节数
    * @return     memory::packet_ptr
    * @retval     数据包
    */
    memory::packet_ptr read( uint32_t size );

    /*
    * @brief      消费数据(读取数据后删除指定长度数据)
    *
    * @param[in]  size  需要读取的字节数
    * @return     int32_t
    * @retval     实际消费的字节数
    */
    int32_t consume( uint32_t size );

    /*
    * @brief      字符串查找
    *
    * @param[in]  str    需要查找的字符串
    * @param[in]  offset 查找的偏移地址
    * @param[out] pos    字符串的位置
    * @param[in]  cb     字符是否相等的判断回调，不赋值则使用string的等于比较
    * @return     bool
    * @retval     true 成功 false 失败
    */
    bool search( const char* str, uint32_t offset, uint32_t& pos, equal_string_cb_t cb = equal_string_cb_t() );

    /*
    * @brief      重置,清空内部数据包
    */
    void reset();

private: // 私有变量

    struct impl;
    memory::scoped_ptr<impl> _impl;
};

NAMESPACE_TARO_INFRA_END
