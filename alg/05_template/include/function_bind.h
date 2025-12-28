#pragma once
#include<cstdint>
#include<functional>


namespace alg
{
    template<std::uint32_t N> 
    struct placeholder 
    {
        static constexpr std::uint32_t value = N;
    };

    namespace placeholders
    {
        inline placeholder<1> _1;
        inline placeholder<2> _2;
        inline placeholder<3> _3;
        inline placeholder<4> _4;
        inline placeholder<5> _5;
    }

    template<typename T>
    constexpr bool is_placeholder_v = false;
    template<std::uint32_t N>
    constexpr bool is_placeholder_v<placeholder<N>> = true;



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
// *  variadic  call-arguments (on calling alg::bound_function created by alg::bind)
// 2. as there are 2 packs, the unpack is a nested variadic parameter unpack 
//
// 
//
// On construction of alg::bound_function :
// 1. alg::bound_function, store bound-arguments into std::tuple 
// 
// On invocation of alg::bound_function :
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
//      bound_function(F&& fct, BOUND_ARGS&&...bound_args) // Why is this not preferred? Compile error. See remark 1
//                   : m_fct(std::forward<F>(fct))
//                   , m_bound_args(std::forward<BOUND_ARGS>(bound_args)...) 
//      {
//      }

        bound_function(F fct, BOUND_ARGS...bound_args) // size = N+M, includes placeholders 
                     : m_fct(std::move(fct))
                     , m_bound_args(std::move(bound_args)...) 
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
        decltype(auto) resolve_args(T&& bound_arg, CALL_ARGS&&...call_args) 
        {
            if constexpr (is_placeholder_v<std::decay_t<T>>)
            {
                constexpr std::uint32_t N = std::decay_t<T>::value;
                return std::get<N-1>(std::forward_as_tuple(std::forward<CALL_ARGS>(call_args)...));
            }
            else
            {
                return unwrap(std::forward<T>(bound_arg));
            }
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




// Remark 1 
// The commented constructor does not work, as there is compilation error when there is placeholder in alg::bind.
// * when call alg::bind with placeholder, BOUND_ARGS&& is deduced, hence BOUND_ARGS&& is universal reference that binds to lvalue placeholders::_1
// * when call bound_function constructor, BOUND_ARGS&& is NOT deduced, as it is assigned by deduction guide, hence BOUND_ARGS&& is rvalue reference
// * when binding rvalue reference BOUND_ARGS&& to lvalue placeholders::_1, there will be ERROR 
//
// Therefore :
// 1. CTAD has to kicks in order to make BOUND_ARGS&& a universal reference in bound_function constructor
// 2. CTAD will be bypassed when there is deduction guide
// 3. CTAD will be bypassed for variadic template without deduction guide
// 4. hence no matter whether there is deduction guide, BOUND_ARGS&& is a rvalue reference
//
// Solution : 
// * use another constructor that pass by value and then move
// 
