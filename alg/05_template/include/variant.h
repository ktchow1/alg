#pragma once
#include<cstdint>
#include<limits>
#include<type_traits> // for std::aligned_storage



// ********************* //
// *** Useful traits *** //
// ********************* //
// Useful traits :
// * max_size
// * max_align
// * type_index
// * type_of (NOT used)


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

    // ******************************************** //
    // For type that does not exist, 
    // it will return 1 + 1 + 1 + ...... + 1 + 0
    //                <-------------------->
    //                sizeof...(Ts) number of 1s
    // ******************************************** //
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

    // *************************** // 
    // *** type_of is NOT USED *** //
    // *************************** // 
    // alg::type_of is not used in alg::variant, because it is compile time dispatch of :
    // * destructor         
    // * copy constructor   
    // * move constructor   
    //
    //
    // This is compile time dispatch (with constexpr N).
    //
    //    using T = typename alg::type_of<N,A,B,C,D>::type;
    //
    //    reinterpret_cast<T*>(m_ptr)->~T();                                           // for destroy
    //    new (m_ptr) T          (*reinterpret_cast<T*>(rhs_ptr));                     // for copy construct
    //    new (m_ptr) T(std::move(*reinterpret_cast<T*>(rhs_ptr)));                    // for move construct
    //
    // This is runtime dispatch (with member m_index).
    //
    //    if (m_index == 0) reinterpret_cast<A*>(m_ptr)->~A();                         // for destroy
    //    if (m_index == 1) reinterpret_cast<B*>(m_ptr)->~B();
    //    if (m_index == 2) reinterpret_cast<C*>(m_ptr)->~C();
    //    if (m_index == 0) new (m_ptr) A(*reinterpret_cast<A*>(rhs_ptr));             // for copy construct
    //    if (m_index == 1) new (m_ptr) B(*reinterpret_cast<B*>(rhs_ptr));
    //    if (m_index == 2) new (m_ptr) C(*reinterpret_cast<C*>(rhs_ptr));
    //    if (m_index == 0) new (m_ptr) A(std::move(*reinterpret_cast<A*>(rhs_ptr)));  // for move construct
    //    if (m_index == 1) new (m_ptr) B(std::move(*reinterpret_cast<B*>(rhs_ptr)));
    //    if (m_index == 2) new (m_ptr) C(std::move(*reinterpret_cast<C*>(rhs_ptr)));
    //
    //
    // Solution : Use alg::runtime_dispatcher, which is the same approach as Maven, see 05_template/include/traits.h 
    // * it uses runtime dispatch
    // * it uses variadic, hence no need to list all cases with if-else 
}



// *************** //
// *** Concept *** //
// *************** //
// Cannot be declared inside class

namespace alg
{
    template<typename T, typename...Ts>
    concept one_of = (std::same_as<T,Ts> || ...);
}



// ************************** //
// *** Runtime dispatcher *** //
// ************************** //
// Same technique as Maven in traits.h

namespace alg
{
    // *** interface *** //
    template<typename...Ts>
    struct runtime_dispatcher; 
    

    // *** recursion *** //
    template<typename T, typename...Ts>
    struct runtime_dispatcher<T,Ts...> 
    {
        static void destroy(std::size_t index, void* ptr)
        {
            if (index != 0)  runtime_dispatcher<Ts...>::destroy(index-1, ptr); 
            else             reinterpret_cast<T*>(ptr)->~T(); 
        }

        static void copy(std::size_t index, const void* from_ptr, void* to_ptr)
        {
            if (index != 0)  runtime_dispatcher<Ts...>::copy(index-1, from_ptr, to_ptr); 
            else             new (to_ptr) T(*reinterpret_cast<const T*>(from_ptr)); 
        }

        static void move(std::size_t index, void* from_ptr, void* to_ptr)
        {
            if (index != 0)  runtime_dispatcher<Ts...>::move(index-1, from_ptr, to_ptr);
            else             new (to_ptr) T(std::move(*reinterpret_cast<T*>(from_ptr)));
        }
    };


    // *** boundary *** //
    template<>
    struct runtime_dispatcher<> 
    {
        static void destroy(std::size_t index, void* ptr)
        {
            throw std::runtime_error("index in runtime_dispatcher exceeds number of args");
        }

        static void copy(std::size_t index, const void* from_ptr, void* to_ptr)
        {
            throw std::runtime_error("index in runtime_dispatcher exceeds number of args");
        }

        static void move(std::size_t index, void* from_ptr, void* to_ptr)
        {
            throw std::runtime_error("index in runtime_dispatcher exceeds number of args");
        }
    };
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
        static constexpr std::size_t monostate = sizeof...(Ts); // imply that m_impl is NOT constructed yet

    public:
        variant() = default;
        
        template<typename T> requires one_of<T,Ts...>
        variant(const T& x) : m_index(type_index<T,Ts...>::value) // Note : This is not copy constructor.
        {
            new (m_impl) T{x};

        }

        template<typename T> requires one_of<T,Ts...>
        variant(T&& x) : m_index(type_index<T,Ts...>::value) // Note : This is not move constructor.
        {
            new (m_impl) T{std::move(x)};
        }


    public:
        // *********************************************************** //
        // *** Destroy, copy, move - that needs runtime_dispatcher *** //
        // *********************************************************** //
       ~variant()
        {
            if (m_index != monostate)
            {
                runtime_dispatcher<Ts...>::destroymove(m_index, m_impl);
            }
        }

        variant(const variant<Ts...>& rhs) : m_index(rhs.m_index)
        {
            if (m_index != monostate)
            {
                runtime_dispatcher<Ts...>::copy(m_index, rhs.m_impl, m_impl);
            }
        }

        variant(variant<Ts...>&& rhs)
        {
            if (m_index != monostate)
            {
                runtime_dispatcher<Ts...>::move(m_index, rhs.m_impl, m_impl);
            }
        }


        // ***************** //
        // *** Rebinding *** //
        // ***************** //
        variant<Ts...> operator=(const variant<Ts...>& rhs) 
        {
            if (this != &rhs)
            {
                // Destruct old instance
                if (m_index != monostate)
                {
                    runtime_dispatcher<Ts...>::destroy(m_index, m_impl);
                }

                // Construct new instance
                m_index = rhs.m_index;
                if (m_index != monostate)
                {
                    runtime_dispatcher<Ts...>::copy(m_index, m_impl);
                }
            }
            return *this;
        }

        variant<Ts...> operator=(variant<Ts...>&& rhs)
        {
            if (this != &rhs)
            {
                // Destruct old instance
                if (m_index != monostate)
                {
                    runtime_dispatcher<Ts...>::destroy(m_index, rhs.m_impl, m_impl);
                }

                // Construct new instance
                m_index = rhs.m_index;
                if (m_index != monostate)
                {
                    runtime_dispatcher<Ts...>::move(m_index, rhs.m_impl, m_impl);
                }
            }
            return *this;
        }


    public:
        std::size_t index() const noexcept // return sizeof...(Ts) means nullity
        {
            return m_index;
        }

        template<typename T>
        bool is_type() const
        {
            return type_index<T,Ts...>::value == m_index;
        }


    private:
        alignas(max_align<Ts...>) std::byte m_impl[max_size<Ts...>::value]; 

    private:
        std::size_t m_index = monostate;  
    };

}



// ****************** //
// *** Comparison *** //
// ****************** //
//
// method       |    Ts                        runtime vs compile       speed
// -------------+----------------------------------------------------------------------------------------------
// std::tuple   |    set of unrelated types    compile time dispatch    linear if-else scan (in compile time)
// std::variant |    set of unrelated types    runtime dispatch         linear if-else scan
// type-erasure |    set of unrelated types    runtime dispatch         N levels of redirection
// polymorphism |    set of derived   types    runtime dispatch         2 levels of redirection
//
//
