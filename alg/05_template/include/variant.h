#pragma once
#include<cstdint>
#include<type_traits> // for std::aligned_storage



// *************** //
// *** Variant *** //
// *************** //
// Variant is like a generalized optional, which can take value of one of the union-type.
//
//
// Needs 2 helper traits :
// * find_max_size
// * find_type_index
//


namespace alg
{
    template<typename...Ts> // interface
    struct find_max_size 
    {
        static constexpr std::uint32_t value = 0;
    };

    template<typename T, typename...Ts> // recursion
    struct find_max_size 
    {
        static constexpr std::uint32_t value = std::max(sizeof(T), find_max_size<Ts...>::value);
    };

    template<typename T> // boundary
    struct find_max_size 
    {
        static constexpr std::uint32_t value = sizeof(T);
    };



    template<typename TARGET, typename...Ts>
    struct find_type_index
    {
        static constexpr std::uint32_t value = -1;
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
