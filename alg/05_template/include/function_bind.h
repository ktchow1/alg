#pragma once
#include<cstdint>
#include<functional>


namespace alg
{
    template<std::uint32_t> 
    struct placeholder {};

    namespace placeholders
    {
        using _1 = placeholder<1>;
        using _2 = placeholder<2>;
        using _3 = placeholder<3>;
        using _4 = placeholder<4>;
        using _5 = placeholder<5>;
    }
}


// ************ //
// *** Bind *** //
// ************ //
// 1. alg::bind works with std::function, NOT alg::function.
// 2. alg::bind works with 2 variadic parameter pack : 
// *  variadic bound-arguments (on calling alg::bind)
// *  variadic  call-arguments (on calling std::function created by alg::bind)
// 3. as there are 2 packs, the unpack is a nested variadic parameter unpack 
//
// 
//
// The creation of std::function by alg::bind is done in 2 steps (for better readabilty)
// 1. create alg::bound_function_object, store bound-arguments into std::tuple 
// 2. create std::function,            extract bound-arguments from std::tuple and from call-arguments
//
// We have different treatments for 2 parameter packs : 
// 1. bound-arguments are stored by value,     using std::      make_tuple
// 2.  call-arguments are passed by reference, using std::forward_as_tuple
//

namespace alg
{
    template<typename F, typename...BOUND_ARGS>
    class bound_function_object
    {
    public:
        bound_function_object(F&& fct, BOUND_ARGS&&...bound_args) 
                            : m_fct(std::forward<F>(fct))
                            , m_bound_args(std::forward<BOUND_ARGS>(bound_args)...)
        {
        }



        // ****************** //
        // *** Invocation *** //
        // ****************** //
        template<typename...CALL_ARGS>
        decltype(auto) operator()(CALL_ARGS&&...call_args)
        {

        }




    private:
        // Todo - This implementation involves :
        // * deep copy of function and
        // * deep copy of all bound arguments

        F m_fct;
        std::tuple<BOUND_ARGS...> m_bound_args;
        


    };

    


}
