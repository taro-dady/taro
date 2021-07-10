
#pragma once

#include "infra/thread.h"
#include "model/singleton.h"
#include <list>

NAMESPACE_TARO_INFRA_BEGIN

// 线程管理对象
class thrd_manger
{
PUBLIC: // 公共函数

    /*
    * @brief      添加线程
    *
    * @param[in]  lite 线程对象
    * @retval     true 成功 false 失败
    */
    bool add_thrd( thread_info_t const& lite );

    /*
    * @brief      删除线程
    *
    * @param[in]  name       线程名称
    * @retval     true 成功 false 失败
    */
    bool remove_thrd( const char* name );

    /*
    * @brief      更新线程状态
    *
    * @param[in]  name       线程名称
    * @param[in]  running    运行状态 true 运行 false 停止
    */
    void update_status( const char* name, bool running );

    /*
    * @brief      添加线程
    *
    * @retval     线程信息
    */
    std::vector< thread_info_t > get_thrd_info() const;
    
PRIVATE: // 私有变量

    mutable std::mutex         _mutex;
    std::list< thread_info_t > _lites;
};

NAMESPACE_TARO_INFRA_END
