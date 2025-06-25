#pragma once
#include<functional>


namespace alg
{
    template<typename F, typename...ARGS>
    auto invoke_naive(F&& fct, ARGS&&... args)
    {
        return std::forward<F>(fct)(std::forward<ARGS>(args)...);
    }
}
