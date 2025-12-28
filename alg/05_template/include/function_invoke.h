#pragma once
#include<functional>
#include<traits.h>


namespace alg
{
    template<typename F, typename...ARGS>
    auto invoke_naive(F&& fct, ARGS&&... args)
    {
        return std::forward<F>(fct)(std::forward<ARGS>(args)...);
    }
}



namespace alg
{
    // ******************** //
    // *** General case *** //
    // ******************** //
    // ARG can be the 1st argument
    // ARG can be the object for member pointer
    //
    // 
    // Return "decltype(auto)" instead of "auto" because
    // we want to forward the fct return type.

    template<typename F, typename ARG, typename...ARGS>
    decltype(auto) invoke(F&& fct, ARG&& arg, ARGS&&... args)
    {
        if constexpr (std::is_member_function_pointer_v<std::decay_t<F>>)
        {
            if constexpr (is_reference_wrapper_v<std::decay_t<ARG>>)
            { 
                return (std::forward<ARG>(arg).get() .* std::forward<F>(fct)) ( std::forward<ARGS>(args)... );
            }
            else
            {
                return (std::forward<ARG>(arg) .* std::forward<F>(fct)) ( std::forward<ARGS>(args)... );
            }
        }
        else if constexpr (std::is_member_object_pointer_v<std::decay_t<F>>)
        {
            if constexpr (is_reference_wrapper_v<std::decay_t<ARG>>)
            { 
                return std::forward<ARG>(arg).get() .* std::forward<F>(fct);
            }
            else
            { 
                return std::forward<ARG>(arg) .* std::forward<F>(fct);
            }
        }
        else
        {
            return std::forward<F>(fct)(std::forward<ARG>(arg), std::forward<ARGS>(args)...);
        } 
    }


    // ****************************** //
    // *** Special case - nullity *** //
    // ****************************** //
    template<typename F>
    decltype(auto) invoke(F&& fct)
    {
        return std::forward<F>(fct)();
    }
}
