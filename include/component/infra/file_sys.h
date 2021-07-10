
#pragma once

#include "infra/defs.h"
#include <list>
#include <string>

NAMESPACE_TARO_INFRA_BEGIN

// 文件系统
class file_sys
{
public: // 公共类型

    // 节点类型
    enum node_type
    {
        node_type_dir  = 1 << 0, // 目录
        node_type_file = 1 << 1, // 文件  
    };

    // 文件系统节点
    struct fsys_node
    {
        node_type   type;  //  节点类型
        std::string name;  //  节点名称
        int64_t     mtime; //  修改时间
        uint64_t    size;  //  大小
    };

    // 磁盘信息
    struct disk_info
    {
        uint64_t free_bytes;      // 空闲字节
        uint64_t total_bytes;     // 总字节数量
        uint32_t occupy_percent;  // 占用百分比
    };

public: // 公共函数

    /*
    * @brief 获取单例
    */
    static file_sys& instance();

    /*
    * @brief  获取目录当中所有节点
    * 
    * @param[in]  dir    目录的路径
    * @param[in]  cond   获取条件 默认都获取
    * @return     获取的文件节点
    */
    std::list<fsys_node> get_nodes( const char* dir, uint32_t cond = ( node_type_dir | node_type_file ) );

    /*
    * @brief  获取磁盘信息
    *
    * @param[in]  dir       目录的路径
    * @return     获取的磁盘信息
    */
    int32_t get_disk_info( const char* dir, disk_info& info );

    /*
    * @brief  目录是否存在
    *
    * @param[in]  dir   目录的路径
    */
    int32_t is_dir_exist( const char* dir );

    /*
    * @brief  创建目录
    *
    * @param[in]  dir   目录的路径
    */
    int32_t create_dir( const char* dir );

    /*
    * @brief  删除目录
    *
    * @param[in]  dir   目录的路径
    */
    int32_t remove_dir( const char* dir );

    /*
    * @brief  拷贝目录
    *
    * @param[in]  src 源目录
    * @param[in]  dst 目标目录
    */
    int32_t copy_dir( const char* src, const char* dst );

    /*
    * @brief  文件是否存在
    *
    * @param[in]  file  文件名称
    */
    int32_t is_file_exist( const char* file );

    /*
    * @brief  获取文件大小
    *
    * @param[in]  file 文件名称
    */
    int32_t get_file_size( const char* file, uint64_t& size );

    /*
    * @brief  拷贝文件
    *
    * @param[in]  src 源文件
    * @param[in]  dst 目标文件
    */
    int32_t copy_file( const char* src, const char* dst );

    /*
    * @brief  删除文件
    *
    * @param[in]  path 文件路径
    */
    int32_t remove_file( const char* path );

private: // 私有函数

    /*
    * @brief 构造函数
    */
    file_sys();

    /*
    * @brief 析构函数
    */
    ~file_sys();

    /*
    * @brief 禁止拷贝构造
    */
    file_sys( file_sys const& );
    file_sys& operator=( file_sys const& );
};

NAMESPACE_TARO_INFRA_END
