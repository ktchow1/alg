#pragma once
#include<cstdint>
#include<limits>
#include<type_traits> // for std::aligned_storage



// ********************** //
// *** Variant helper *** //
// ********************** //
// Helper traits :
// * max_size
// * max_align
// * type_index
// * type_of


namespace alg
{
    template<typename...Ts> // interface
    struct max_size 
    {
        static constexpr std::size_t value = 0;
    };

    template<typename T, typename...Ts> // recursion
    struct max_size<T,Ts...>
    {
        static constexpr std::size_t value = std::max(sizeof(T), max_size<Ts...>::value);
    };

    template<typename T> // boundary
    struct max_size<T>
    {
        static constexpr std::size_t value = sizeof(T);
    };
}


namespace alg
{
    template<typename...Ts> // interface
    struct max_align
    {
        static constexpr std::size_t value = 0;
    };

    template<typename T, typename...Ts> // recursion
    struct max_align<T,Ts...>
    {
        static constexpr std::size_t value = std::max(alignof(T), max_align<Ts...>::value);
    };

    template<typename T> // boundary
    struct max_align<T>
    {
        static constexpr std::size_t value = alignof(T);
    };
}


namespace alg
{
    template<typename TARGET, typename...Ts> // interface
    struct type_index;

    template<typename TARGET, typename T, typename...Ts> // recursion
    struct type_index<TARGET,T,Ts...>
    {
        static constexpr std::size_t value = std::is_same_v<TARGET,T>? 0 : 1+type_index<TARGET,Ts...>::value;
    };

    template<typename TARGET> // boundary 
    struct type_index<TARGET>
    {
        static constexpr std::size_t value = 0; 
    };

    // For type that does not exist, 
    // it will return 1 + 1 + 1 + ...... + 1 + 0
    //                <-------------------->
    //                sizeof...(Ts) number of 1s
}


namespace alg
{
    template<std::size_t N, typename...Ts> // interface
    struct type_of;

    template<std::size_t N, typename T, typename...Ts> // recursion
    struct type_of<N,T,Ts...>
    {
        using type = type_of<N-1,Ts...>::type;
    };

    template<typename T, typename...Ts> // boundary 
    struct type_of<0,T,Ts...>
    {
        using type = T;
    };
}



// *************** //
// *** Concept *** //
// *************** //
// cannot be declared inside class

namespace alg
{
    template<typename T, typename...Ts>
    concept one_of = (std::same_as<T,Ts> || ...);
}



// *************** //
// *** Variant *** //
// *************** //
namespace alg
{

    template<typename...Ts>
    class variant
    {
    public:
        variant() = default;
        
        template<typename T> requires one_of<T,Ts...>
        variant(const T& t) : m_index(type_index<T,Ts...>::value)
        {
            new (&m_impl) T{t};

        }

        template<typename T> requires one_of<T,Ts...>
        variant(T&& t) : m_index(type_index<T,Ts...>::value)
        {
            new (&m_impl) T{std::move(t)};
        }



    private:
        void reset()
        {
        }

    private:
        std::aligned_storage<max_size <Ts...>::value, 
                             max_align<Ts...>::value> m_impl; 
        
        std::size_t m_index = sizeof...(Ts);  
    };

}
