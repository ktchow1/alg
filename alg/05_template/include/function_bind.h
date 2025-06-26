#pragma once
#include<cstdint>
#include<functional>


namespace alg
{
    template<std::uint32_t> 
    struct placeholder {};

    namespace placeholders
    {
        inline placeholder<1> _1;
        inline placeholder<2> _2;
        inline placeholder<3> _3;
        inline placeholder<4> _4;
        inline placeholder<5> _5;
    }


    template<typename T>
    decltype(auto) unwrap(T&& x)
    {
        return std::forward<T>(x);
    }

    template<typename T>
    decltype(auto) unwrap(std::reference_wrapper<T> x)
    {
        return x.get();
    }
}



// **************************************************************************************** //
// Bind 
//
// 1. alg::bind works with 2 variadic parameter pack : 
// *  variadic bound-arguments (on calling alg::bind)
// *  variadic  call-arguments (on calling std::function created by alg::bind)
// 2. as there are 2 packs, the unpack is a nested variadic parameter unpack 
//
// 
//
// The creation of std::function by alg::bind is done in 2 steps (for better readabilty)
// 1. create alg::bound_function, store bound-arguments into std::tuple 
// 2. create std::function,       using bound_function as the callable
// 
// On invoking std::function, bound_function::operator() will be invoked, which :
// 1. extract bound-arguments from std::tuple 
// 2. replace placeholders by call-arguments
//
//
//
// We have different treatments for 2 parameter packs : 
// 1. bound-arguments are stored by value,     using std::      make_tuple    <--- N+M
// 2.  call-arguments are passed by reference, using std::forward_as_tuple    <---   M
//
// where N = number of non-placeholders bound args
//       M = number of     placeholders 
//         = number of call args
// **************************************************************************************** //
namespace alg
{
    template<typename F, typename...BOUND_ARGS>
    class bound_function
    {
    public:
        bound_function(F&& fct, BOUND_ARGS&&...bound_args) // size = N+M, includes placeholders 
                     : m_fct(std::forward<F>(fct))
                     , m_bound_args(std::forward<BOUND_ARGS>(bound_args)...) 
        {
        }

    public:
        template<typename...CALL_ARGS>
        decltype(auto) operator()(CALL_ARGS&&...call_args)
        {
            return implement_2D_unpack
            (
                std::index_sequence_for<BOUND_ARGS...>{}, // <--- create std::index_sequence<0, 1, 2, ..., sizeof...(BOUND_ARGS)-1>
                std::forward<CALL_ARGS>(call_args)...
            );
        }


    private:
        template<std::size_t...Ns, typename...CALL_ARGS>
        decltype(auto) implement_2D_unpack(std::index_sequence<Ns...>, CALL_ARGS&&...call_args)
        {
            return std::invoke
            (
                m_fct,      //            size = N+M                                       vvv----------- this unpacks CALL_ARGS (inner loop)
                resolve_args(std::get<Ns>(m_bound_args), std::forward<CALL_ARGS>(call_args)...) ...
                            //                                                   size = M       ^^^------ this unpacks Ns (outer loop)
            );
        }


    private:
        template<typename T, typename...CALL_ARGS>
        decltype(auto) resolve_args(T&& bound_arg, CALL_ARGS&&...call_args) // case 1 : non-placeholder
        {
            return unwrap(std::forward<T>(bound_arg));
        }

        template<std::size_t N, typename...CALL_ARGS>
        decltype(auto) resolve_args(placeholder<N> dummy, CALL_ARGS&&...call_args) // case 2 : placeholder
        {
            return std::get<N>(std::forward_as_tuple(std::forward<CALL_ARGS>(call_args)...));

        }


    private:
        F m_fct;
        std::tuple<BOUND_ARGS...> m_bound_args;
    };



    // *********************** //
    // *** Deduction guide *** //
    // *********************** //
    // Variadic template supports CTAD 
    // only if deduction guide is provided.

    template<typename F, typename...BOUND_ARGS>
    bound_function(F&&, BOUND_ARGS&&...) -> bound_function<std::decay_t<F>, std::decay_t<BOUND_ARGS>...>;
    


    // ************************** //
    // *** Bind, as a factory *** //
    // ************************** //
    // Cannot use decltype(auto) here as the return value is temporary,
    // hence returning by decltype(auto) will result in rvalue reference to temporary,
    // hence dangling ...

    template<typename F, typename...BOUND_ARGS>
    auto bind(F&& fct, BOUND_ARGS&&...bound_args)   
    {
        return bound_function {std::forward<F>(fct), std::forward<BOUND_ARGS>(bound_args)...};
    }
}
