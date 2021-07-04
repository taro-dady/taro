
#pragma once

#include "system/defs.h"
#include "model/observers.h"
#include "model/singleton.h"
#include "memory/object_pool.h"

NAMESPACE_TARO_SYSTEM_BEGIN

template<typename T, class... Args>
class event_center
{
public: // 公共类型

    // 事件通知函数
    using event_notify_cb_t = func::function<void( Args... )>;

public: // 公共函数

    /*
    * @brief      添加事件回调
    *
    * @param[in]  type 事件类型
    * @param[in]  cb   回调函数
    */
    bool add_event( T const& type, event_notify_cb_t const& cb )
    {
        if( !cb )
        {
            return false;
        }

        auto slot_ptr = _slots.find( type );
        if( nullptr != slot_ptr )
        {
            return slot_ptr->push_back( cb );
        }

        slot_ptr = std::make_shared< model::observers<Args...> >();
        slot_ptr->push_back( cb );
        ( void )_slots.add( type, slot_ptr );
        return true;
    }

    /*
    * @brief      删除事件回调
    *
    * @param[in]  type 事件类型
    * @param[in]  cb   回调函数
    */
    bool remove_event( T const& type, event_notify_cb_t const& cb )
    {
        if( !cb )
        {
            return false;
        }

        auto slot_ptr = _slots.find( type );
        if( nullptr != slot_ptr )
        {
            return slot_ptr->remove( cb );
        }
        return false;
    }

    /*
    * @brief      通知事件发生
    *
    * @param[in]  type  事件类型
    * @param[in]  args  事件参数
    */
    void signal( T const& type, Args... args )
    {
        auto slot_ptr = _slots.find( type );
        if( nullptr == slot_ptr )
        {
            return;
        }
        ( *slot_ptr )( args... );
    }

private: // 私有类型

    // 事件槽类型
    using slot_t = std::shared_ptr< model::observers<Args...> >;

    // 事件池类型
    using evt_map_t = memory::kobject_pool< T, slot_t >;

private: // 私有变量

    evt_map_t _slots;
};

template<typename T, class... Args>
using event_center_inst = model::singleton< event_center<T, Args...> >;

NAMESPACE_TARO_SYSTEM_END
