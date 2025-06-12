#pragma once
#include<cstdint>
#include<type_traits> // for std::aligned_storage



// *************** //
// *** Variant *** //
// *************** //
// Variant is like a generalized optional, which can take value of one of the union-type.
//
// Needs 2 helper traits :
// * find_max_size
// * find_type_index
//
//

namespace alg
{
    template<typename...Ts> // interface
    struct find_max_size 
    {
        static constexpr std::size_t value = 0;
    };

    template<typename T, typename...Ts> // recursion
    struct find_max_size<T,Ts...>
    {
        static constexpr std::size_t value = std::max(sizeof(T), find_max_size<Ts...>::value);
    };

    template<typename T> // boundary
    struct find_max_size<T>
    {
        static constexpr std::size_t value = sizeof(T);
    };
}


namespace alg
{
    template<std::size_t N, typename TARGET, typename...Ts> // interface
    struct find_type_index_impl;

    template<std::size_t N, typename TARGET, typename T, typename...Ts> // recursion
    struct find_type_index_impl<N,TARGET,T,Ts...>
    {
        static constexpr std::size_t value = std::is_same_v<TARGET,T>? 0 : 1+find_type_index_impl<N,TARGET,Ts...>::value;
    };

    template<std::size_t N, typename TARGET> // boundary
    struct find_type_index_impl<N,TARGET>
    {
        static constexpr std::size_t value = N;
    };

    template<typename TARGET, typename...Ts>
    struct find_type_index
    {
        static constexpr std::size_t value = find_type_index_impl<sizeof...(Ts),TARGET,Ts...>::value;
    };
}


namespace alg
{
    template<typename...Ts>
    class variant
    {
    public:

    private:

    };

}
