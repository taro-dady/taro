
#pragma once

#include "memory/defs.h"
#include "utils/assert.h"
#include <typeinfo>
#include <algorithm>

NAMESPACE_TARO_MEMORY_BEGIN

class any
{
public: // 公共函数

    any() : _content( nullptr ) {}

    template<typename ValueType>
    explicit any( const ValueType& value ) 
        : _content( new holder<ValueType>( value ) )
    {

    }

    any( const any& other )
        : _content( other._content ? other._content->clone() : 0 )
    {

    }

    any& swap( any& rhs )
    {
        std::swap( _content, rhs._content );
        return *this;
    }

    ~any()
    {
        if( _content != nullptr )
        {
            delete _content;
            _content = nullptr;
        }
    }

    template<typename ValueType>
    any& operator=( const ValueType& rhs )
    {
        any( rhs ).swap( *this );
        return *this;
    }

    any& operator=( const any& rhs )
    {
        if( _content == rhs._content )
        {
            return *this;
        }

        if( rhs._content == nullptr )
        {
            delete _content; //_content必然不为nullptr
            _content = nullptr;
            return *this;
        }

        _content = rhs._content->clone();
        return *this;
    }

    bool empty() const
    {
        return !_content;
    }

    const std::type_info& type() const
    {
        return _content ? _content->type() : typeid( void );
    }

    template<typename ValueType>
    ValueType cast() const
    {
        if( type() != typeid( ValueType ) )
        {
            TARO_ASSERT( 0 );
        }
        return ( ( holder<ValueType>* )( _content ) )->_held;
    }

private: // 私有类型定义

    class placeholder
    {
    public: // 公共函数
        virtual ~placeholder() = default;
        virtual const std::type_info& type() const = 0;
        virtual placeholder* clone() const = 0;
    };

    template<typename ValueType>
    class holder : public placeholder 
    {
    public: // 公共函数

        holder( const ValueType& value ) 
            : _held( value ) 
        {
            
        }

        virtual const std::type_info& type() const
        {
            return typeid( ValueType );
        }

        virtual placeholder* clone() const
        {
            return new holder( _held );
        }

    public: // 公共变量

        ValueType _held;
    };

private: // 私有变量定义

    placeholder* _content;
};

NAMESPACE_TARO_MEMORY_END
