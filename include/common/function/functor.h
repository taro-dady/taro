
#pragma once

#include "function/function.h"
#include "function/function_traits.h"
#include "function/do_tuple_functor.h"

NAMESPACE_TARO_FUNCTION_BEGIN

template<class R>
class functor_base
{
public: // 公共函数定义

    /*
    * @brief: 通用调用接口
    */
    virtual R operator()() = 0;

    /*
    * @brief: 虚析构
    */
    virtual ~functor_base() {}
};

template<class B, class R, class ...Args>
class binder_functor : public functor_base<R>
{
public: // 公共函数定义

    /*
    * @brief: 构造函数
    */
    binder_functor( B const& binder, Args&&... args )
        : _binder( binder )
        , _tuple( std::forward<Args>( args )... )
    {

    }

    /*
    * @brief: 通用调用接口
    */
    virtual R operator()() final
    {
        return do_tuple_functor< tuple_type >::call( type<R>(),
                                                     std::forward<B>( _binder ),
                                                     std::forward< tuple_type >( _tuple ) );
    }

private: // 私有类型定义

    template<typename T>
    using decay_t    = typename std::decay<T>::type;
    using tuple_type = std::tuple<decay_t<Args>...>;

private: // 私有变量

    B                            _binder;
    std::tuple<decay_t<Args>...> _tuple;
};

template<class R, class ...Args>
class tuple_functor : public functor_base<R>
{
public: // 公共函数定义

    /*
    * @brief: 普通函数构造
    */
    tuple_functor( R( *f )( Args... ), Args&&... args )
        : _func( f )
        , _tuple( std::forward<Args>( args )... )
    {

    }


    /*
    * @brief: 共享指针函数构造
    */
    template<class T>
    tuple_functor( R( T::*f )( Args... ), std::shared_ptr<T> const& obj, Args&&... args )
        : _func( f, obj )
        , _tuple( std::forward<Args>( args )... )
    {

    }

    /*
    * @brief: 弱引用指针函数构造
    */
    template<class T>
    tuple_functor( R( T::*f )( Args... ), std::weak_ptr<T> const& obj, Args&&... args )
        : _func( f, obj )
        , _tuple( std::forward<Args>( args )... )
    {

    }

    /*
    * @brief: 普通函数构造
    */
    template<class F>
    tuple_functor( F f, Args&&... args )
        : _func( f )
        , _tuple( std::forward<Args>( args )... )
    {

    }

    /*
    * @brief: 通用调用接口
    */
    virtual R operator()() final
    {
        return do_tuple_functor< tuple_type >::call( type<R>(),
                                                     std::forward< function<R( Args... )> >( _func ), 
                                                     std::forward< tuple_type >( _tuple ) );
    }

private: // 私有类型定义

    template<typename T>
    using decay_t    = typename std::decay<T>::type;
    using tuple_type = std::tuple<decay_t<Args>...>;

private: // 私有变量定义

    function<R( Args... )>       _func;
    std::tuple<decay_t<Args>...> _tuple;
};

template<class R>
class functor 
{
public: // 公共函数

    /*
    * @brief: 构造函数
    */
    functor() 
    {
        
    }

    /*
    * @brief: 构造函数
    */
    explicit functor( std::shared_ptr< functor_base<R> > const& p ) 
        : _base( p )
    {
        
    }

    /*
    * @brief: 调用接口
    */
    R operator()()
    {
        if( !_base )
        {
            throw exception_functor_invalid;
        }
        return ( *_base )();
    }

    /*
    * @brief: 有效性判断
    */
    operator bool() const
    {
        return _base != nullptr;
    }

private: // 私有变量声明

    std::shared_ptr< functor_base<R> > _base;
};

// 普通函数
template<typename F>
typename std::enable_if< !is_member_func< F >::value && !is_obj_func<F>::value, func::function< typename function_traits<F>::function_type > >::type
bind( F const& f )
{
    return func::function< typename function_traits<F>::function_type >( f );
}

// 类函数
template<typename F, typename Class>
typename std::enable_if< std::is_member_function_pointer< F >::value, func::function< typename function_traits<F>::function_type > >::type
bind( F const& f, Class const& obj )
{
    return func::function< typename function_traits<F>::function_type>( f, obj );
}

// 对象类函数
template<typename F>
typename std::enable_if< is_obj_func<F>::value, func::function< typename function_traits<F>::function_type > >::type
bind( F const& f )
{
    return func::function< typename function_traits<F>::function_type >( f );
}

// 普通函数
template<typename F, typename... Args>
typename std::enable_if< !std::is_member_function_pointer<F>::value && !is_obj_func<F>::value, func::functor< typename std::result_of<F( Args... )>::type > >::type
make_functor( F const& f, Args&&... args )
{
    using R      = typename function_traits<F>::result_type;
    using func_t = typename func::function< typename function_traits<F>::function_type >;
    auto ptr     = std::make_shared< binder_functor<func_t, R, Args...> >( func_t( f ), std::forward<Args>( args )... );
    return functor< R >( std::dynamic_pointer_cast< functor_base< R > >( ptr ) );
}

// 类函数
template<typename F, typename Class, typename... Args>
typename std::enable_if< std::is_member_function_pointer< F >::value, func::functor< typename function_traits<F>::result_type > >::type
make_functor( F const& f, Class const& obj, Args&&... args )
{
    using R      = typename function_traits<F>::result_type;
    using func_t = typename func::function< typename function_traits<F>::function_type >;
    auto ptr     = std::make_shared< binder_functor<func_t, R, Args...> >( func_t( f, obj ), std::forward<Args>( args )... );
    return functor< R >( std::dynamic_pointer_cast< functor_base< R > >( ptr ) );
}

// 对象类函数
template<typename F, typename... Args>
typename std::enable_if< is_obj_func<F>::value, func::functor< typename std::result_of<F( Args... )>::type > >::type
make_functor( F const& f, Args&&... args )
{
    using R  = typename std::result_of<F( Args... )>::type;
    auto ptr = std::make_shared< binder_functor<F, R, Args...> >( f, std::forward<Args>( args )... );
    return functor<R>( std::dynamic_pointer_cast< functor_base<R> >( ptr ) );
}

NAMESPACE_TARO_FUNCTION_END
