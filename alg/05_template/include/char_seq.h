#pragma once
#include <utility>

namespace alg 
{
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wpedantic"
    #ifdef __clang__
    #pragma GCC diagnostic ignored "-Wgnu-string-literal-operator-template"
    #endif

    template<char... chars>
    requires(sizeof...(chars) > 0)
    struct char_sequence
    {
        static constexpr char name[] = { static_cast<char>(chars)..., '\0' };
    };

    template<typename T, T... chars>
    constexpr char_sequence<chars...> operator""_char_seq()
    {
        return {};
    }

    #pragma GCC diagnostic pop
}