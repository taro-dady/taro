
#pragma once

#include "infra/thread.h"
#include "utils/assert.h"
#include <memory>
#include <atomic>
#include <windows.h>
#include <process.h>

NAMESPACE_TARO_INFRA_BEGIN

// 线程内部变量
class thread_impl
{
PUBLIC: // 公共函数

    /*
    * @brief      构造函数
    *
    * @param[in]  name       线程名称
    */
    thread_impl( const char* name );

    /*
    * @brief      析构函数
    */
    ~thread_impl();

    /*
    * @brief      启动线程
    * 
    * @param[in]  cb 线程回调函数
    * @param[in]  stack_size 线程栈大小  =0 使用默认栈大小
    * @param[in]  priority   线程优先级 
    */
    int32_t start( thread::thread_cb_t const& cb, uint32_t stack_size, thread::thread_priority_e priority );

    /*
    * @brief      停止线程
    */
    int32_t stop();

    /*
    * @brief      运行线程回调
    */
    void run_thrd_cb();
    
PRIVATE: // 私有变量

    uint32_t                  _stack_bytes;  // 栈大小
	uintptr_t                 _thrd;         // 线程句柄
    std::string               _name;         // 线程名称
    std::atomic_bool          _exit;         // 是否已退出
    std::atomic_bool          _start;        // 启动状态
    thread::thread_cb_t       _cb;           // 业务回调
    thread::thread_priority_e _priority;     // 函数优先级
};

NAMESPACE_TARO_INFRA_END
