
#pragma once

#include "memory/defs.h"
#include <memory>

NAMESPACE_TARO_MEMORY_BEGIN

// 数据包接口
class packet
{
public: // 公共函数定义

    /*
    * @brief      虚析构函数
    */
    virtual ~packet(){}

    /*
    * @brief      添加数据
    *
    * @param[in]  buffer  数据指针
    * @param[in]  len     数据字节数
    */
    virtual int32_t append( uint8_t* buffer, uint32_t len ) = 0;

    /*
    * @brief      扩展存储大小
    *
    * @param[in]  sz 需要扩展的内存大小
    */
    virtual int32_t expand( uint32_t sz ) = 0;

    /*
    * @brief      获取数据大小
    *
    * @return     uint32_t
    */
    virtual uint32_t size() const = 0;

    /*
    * @brief      重置数据大小
    *
    * @param[in]  sz 数据大小
    */
    virtual int32_t resize( uint32_t sz ) = 0;

    /*
    * @brief      获取内存容量
    *
    * @return     int32_t
    */
    virtual uint32_t cap() const = 0;

    /*
    * @brief      获取数据地址 数据地址= 缓冲起始地址 + 头部偏移(由set_offset设置若不设置则为0)
    *
    * @return     uint8_t*
    * @retval     数据地址
    */
    virtual uint8_t* buffer() const = 0;
    
    /*
    * @brief      设置头部的偏移，用于头的填充，需要手动调用resize重新调整packet数据大小( size - offset )
    *
    * @param[in]  offset 偏移大小
    */
    virtual int32_t set_head_offset( uint32_t offset ) = 0;

    /*
    * @brief      获取头部偏移
    *
    * @return     int32_t 偏移大小
    */
    virtual int32_t get_head_offset() const = 0;

    /*
    * @brief      获取附加数据
    *
    * @return     uint8_t* 扩展数据指针
    */
    virtual uint8_t* extra_data() const = 0;

    /*
    * @brief      获取附加数据大小
    *
    * @return     uint32_t 扩展数据大小
    */
    virtual uint32_t extra_data_size() const = 0;

    /*
    * @brief      扩展附加数据大小
    *
    * @param[in]  sz 需要扩展的内存大小
    */
    virtual int32_t expand_extra_data( uint32_t sz ) = 0;
};

using packet_ptr = std::shared_ptr<packet>;

NAMESPACE_TARO_MEMORY_END
