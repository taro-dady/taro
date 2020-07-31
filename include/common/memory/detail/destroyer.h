
#pragma once

#include "memory/defs.h"

NAMESPACE_TARO_MEMORY_BEGIN

namespace detail 
{
    struct destroyer
    {
        virtual void release() = 0;
    };
    
    template<class T>
    struct normal_destroyer : public destroyer
    {
        normal_destroyer( T* p )
            : _p( p )
        {

        }

        virtual void release() override
        {
            if( _p != nullptr )
                delete _p;
            delete this;
        }

        T* _p;
    };

    template<class T, class D>
    struct user_destroyer : public destroyer
    {
        user_destroyer( T* p, D const& del )
            : _p( p )
            , _del( del )
        {

        }

        virtual void release() override
        {
            if( _p != nullptr )
                _del( _p );
            delete this;
        }

        T* _p;
        D  _del;
    };

}

NAMESPACE_TARO_MEMORY_END
