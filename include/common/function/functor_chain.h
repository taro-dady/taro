
#pragma once

#include "function/functor.h"
#include "function/function_traits.h"
#include <list>

NAMESPACE_TARO_FUNCTION_BEGIN

// 参数基类
struct chain_arg_base
{
    virtual ~chain_arg_base() {}
};

// 参数
template< typename A >
struct chain_arg : public chain_arg_base
{
   /**
    * @brief     构造函数
    */
    chain_arg( A arg )
        : _value( arg )
    {

    }

    A _value;
};
using chain_arg_ptr = std::shared_ptr<chain_arg_base>;

// 链式函数基类
struct chain_functor_base
{
   /**
    * @brief     虚析构
    */
    virtual ~chain_functor_base()
    {

    }

   /**
    * @brief     函数调用
    */
    virtual void invoke( chain_arg_ptr const& in, chain_arg_ptr& out ) = 0;
};

// 无参数的链式函数
template<typename F, typename R>
struct chain_functor_0 : public chain_functor_base
{
public: // 公共函数

   /**
    * @brief     构造函数
    */
    chain_functor_0( F const& f )
        : _f( f )
    {

    }

   /**
    * @brief     函数调用
    */
    virtual void invoke( chain_arg_ptr const&, chain_arg_ptr& out )
    {
        out = std::make_shared< chain_arg< R > >( _f() );
    }

private: // 私有变量

    F _f;
};

// 无参数返回类型为void的链式函数
template<typename F>
struct chain_functor_0<F, void> : public chain_functor_base
{
public: // 公共函数

   /**
    * @brief     构造函数
    */
    chain_functor_0( F const& f )
        : _f( f )
    {

    }

   /**
    * @brief     函数调用
    */
    virtual void invoke( chain_arg_ptr const&, chain_arg_ptr& )
    {
        _f();
    }

private: // 私有变量

    F _f;
};

// 1个参数的链式函数
template<typename F, typename A, typename R>
struct chain_functor_1 : public chain_functor_base
{
public: // 公共函数

    chain_functor_1( F const& f )
        : _f( f )
    {

    }

    virtual void invoke( chain_arg_ptr const& in, chain_arg_ptr& out )
    {
        auto t = std::dynamic_pointer_cast< chain_arg<A> >( in );
        out = std::make_shared< chain_arg< R > >( _f( t->_value ) );
    }

private: // 私有变量

    F _f;
};

// 1个参数返回类型为void的链式函数
template<typename F, typename A>
struct chain_functor_1<F, A, void> : public chain_functor_base
{
public: // 公共函数

    chain_functor_1( F const& f )
        : _f( f )
    {

    }

    virtual void invoke( chain_arg_ptr const& in, chain_arg_ptr& )
    {
        auto t = std::dynamic_pointer_cast< chain_arg<A> >( in );
        _f( t->_value );
    }

private: // 私有变量

    F _f;
};

template<typename F, bool>
struct chain_functor_creater {};

template<typename F>
struct chain_functor_creater<F, false>
{
    static std::shared_ptr<chain_functor_base> create( F f )
    {
        using arg_type = typename function_traits<F>::template arg<0>::type;
        return std::shared_ptr<chain_functor_base>( new chain_functor_1<F, arg_type, typename function_traits<F>::result_type>( f ) );
    }
};

template<typename F>
struct chain_functor_creater<F, true>
{
    static std::shared_ptr<chain_functor_base> create( F f )
    {
        return std::shared_ptr<chain_functor_base>( new chain_functor_0<F, typename function_traits<F>::result_type>( f ) );
    }
};

// 链式函数
struct functor_chain
{
public: // 公共函数

    template<class F>
    functor_chain& then( F f )
    {
        auto ptr = chain_functor_creater<F, function_traits<decltype( f )>::arg_num == 0>::create( f );
        _func_list.emplace_back( ptr );
        return *this;
    }

    void operator()()
    {
        chain_arg_ptr in;
        chain_arg_ptr out;
        for( auto& it : _func_list )
        {
            it->invoke( in, out );
            in = out;
        }
    }

    template<typename P>
    void async_call( P& thrd_pool )
    {
        thrd_pool.push( func::bind(
            &functor_flow<P>::handle, std::make_shared< functor_flow<P> >( thrd_pool, _func_list )
        ) );
    }

private: // 私有变量

    // 链式函数链表
    using chain_functor_list = std::list< std::shared_ptr<chain_functor_base> >;

    // 函数流程对象
    template<typename P>
    struct functor_flow : std::enable_shared_from_this< functor_flow<P> >
    {
        functor_flow( P& pool, chain_functor_list const& l )
            : _thrdpool( pool )
            , _index( 0 )
            , _list( l )
        {

        }

        void handle()
        {
            auto it = std::next( _list.begin(), _index++ );
            chain_arg_ptr out;
            ( *it )->invoke( _in, out );
            _in = out;
            if( _index < ( int32_t )_list.size() )
            {
                _thrdpool.push( func::bind( &functor_flow<P>::handle, this->shared_from_this() ) );
            }
        }

        P&                 _thrdpool;
        int32_t            _index;
        chain_arg_ptr      _in;
        chain_functor_list _list;
    };

private: // 私有变量

    chain_functor_list _func_list;
};

NAMESPACE_TARO_FUNCTION_END
