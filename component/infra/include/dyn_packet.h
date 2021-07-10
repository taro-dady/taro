
#pragma once

#include "memory/packet.h"
#include "function/function.h"
#include "infra/packet_manager.h"
#include <vector>

NAMESPACE_TARO_INFRA_BEGIN

/// 动态数据包
class dyn_packet : PUBLIC memory::packet
{
PUBLIC: /// 公共函数

    /*
    * @brief      构造函数
    */
    dyn_packet( uint32_t extra_size, uint32_t power, destruct_extra_func const& func );

    /*
    * @brief      构造函数
    */
    dyn_packet( dyn_packet const& other );

    /*
    * @brief      析构函数
    */
    ~dyn_packet();

    /*
    * @brief      添加数据
    *
    * @param[in]  buffer  数据指针
    * @param[in]  len     数据字节数
    */
    virtual int32_t append( uint8_t* buffer, uint32_t len ) override final;

    /*
    * @brief      扩展存储大小
    *
    * @param[in]  sz 需要扩展的内存大小
    */
    virtual int32_t expand( uint32_t sz ) override final;

    /*
    * @brief      获取数据大小
    *
    * @return     uint32_t
    */
    virtual uint32_t size() const override final;

    /*
    * @brief      重置数据大小
    *
    * @param[in]  sz 数据大小
    */
    virtual int32_t resize( uint32_t sz ) override final;

    /*
    * @brief      获取内存容量
    *
    * @return     int32_t
    */
    virtual uint32_t cap() const override final;

    /*
    * @brief      获取数据地址 数据地址= 缓冲起始地址 + 头部偏移(由set_offset设置若不设置则为0)
    *
    * @return     uint8_t*
    * @retval     数据地址
    */
    virtual uint8_t* buffer() const override final;

    /*
    * @brief      设置头部的偏移，用于头的填充，需要手动调用resize重新调整packet数据大小( size - offset )
    *
    * @param[in]  offset 偏移大小
    */
    virtual int32_t set_head_offset( uint32_t offset ) override final;

    /*
    * @brief      获取头部偏移
    *
    * @return     int32_t 偏移大小
    */
    virtual int32_t get_head_offset() const override final;

    /*
    * @brief      获取附加数据
    *
    * @return     uint8_t* 扩展数据指针
    */
    virtual uint8_t* extra_data() const override final;

    /*
    * @brief      获取附加数据大小
    *
    * @return     uint32_t 扩展数据大小
    */
    virtual uint32_t extra_data_size() const override final;

    /*
    * @brief      扩展附加数据大小
    *
    * @param[in]  sz 需要扩展的内存大小
    */
    virtual int32_t expand_extra_data( uint32_t sz ) override final;

PRIVATE: /// 私有类型

    /// 附加数据类型
    using extra_type = std::vector<uint8_t>;

PRIVATE: /// 私有变量
    
    bool                     _ref;
    uint32_t                 _sz;     /// 数据大小
    uint32_t                 _cap;    /// 缓冲大小
    uint32_t                 _offset; /// 头部偏移
    uint32_t                 _align;  /// 内存对齐字节
    extra_type               _extra;  /// 附加数据区
    destruct_extra_func      _func;   /// 附加数据区析构函数
    std::shared_ptr<uint8_t> _buffer; /// 数据缓冲
};

NAMESPACE_TARO_INFRA_END
