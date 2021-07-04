
#pragma once

#include "memory/defs.h"

NAMESPACE_TARO_MEMORY_BEGIN

namespace detail 
{
    struct destroyer
    {
        virtual void release() = 0;
        virtual ~destroyer() {}
    };
    
    template<class T>
    struct normal_destroyer : public destroyer
    {
        explicit normal_destroyer( T* p )
            : _p( p )
        {

        }

        virtual void release() override
        {
            if( _p != nullptr )
                delete _p;
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
        }

        T* _p;
        D  _del;
    };

}

NAMESPACE_TARO_MEMORY_END
