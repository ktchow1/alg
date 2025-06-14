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
// Dispatch means the way to forward the implementation from :
// * constructor             of variant<Ts...>
// * destructor              of variant<Ts...>
// * copy / move constructor of variant<Ts...>
// * copy / move assignment  of variant<Ts...>
//
// to :
// * destructor       of T 
// * copy constructor of T 
// * move constructor of T 
//   where T is one of Ts...
//
//
//
// Problem  : Cannot use alg::type_of to dispatch because alg::type_of is a compile time traits.
// Solution : Use alg::runtime_dispatcher, which is the same approach as Maven, see 05_template/include/traits.h 
// * it uses runtime dispatch, which depends on m_index
// * it uses variadic, hence no need to list all cases Ts... with if-else 
//
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
// List of 11 constructor / desstructor / assignment / emplace : 
//
// 1. Construct default                                       <--- dispatch to new T{}
// 2. Destruct  default                                       <--- dispatch to runtime_dispatcher::destroy
//
// 3. Construct  |
//    -----------+--------------------------------------- 
//    copy from  |  lvalue T     lvalue variant<Ts...>        <--- dispatch to runtime_dispatcher::copy  
//    move from  |  rvalue T     rvalue variant<Ts...>        <--- dispatch to runtime_dispatcher::move
//                           
// 4. Rebinding  |           
//    -----------+--------------------------------------- 
//    copy from  |  lvalue T     lvalue variant<Ts...>        <--- dispatch to runtime_dispatcher::copy   
//    move from  |  rvalue T     rvalue variant<Ts...>        <--- dispatch to runtime_dispatcher::move   
//    emplace    |  ARGS...                                   <--- dispatch to new T{args...}
//
  
namespace alg
{
    template<typename...Ts>
    class variant
    {
    public:
        static constexpr std::size_t monostate = sizeof...(Ts); // imply that m_impl is NOT constructed yet

    public:
        variant() = default;
        
        template<typename T> 
        requires one_of<T,Ts...>
        variant(const T& x) : m_index(type_index<T,Ts...>::value) // Note : This is not copy constructor.
        {
            new (m_impl) T{x};
        }

        template<typename T> 
        requires one_of<T,Ts...>
        variant(T&& x) : m_index(type_index<T,Ts...>::value) // Note : This is not move constructor.
        {
            new (m_impl) T{std::move(x)};
        }


    public:
        // *************************** //
        // *** Destroy, copy, move *** //
        // *************************** //
       ~variant()
        {
            if (m_index != monostate)
            {
                runtime_dispatcher<Ts...>::destroy(m_index, m_impl);
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


        // ****************************** //
        // *** Rebinding with variant *** //
        // ****************************** //
        variant<Ts...>& operator=(const variant<Ts...>& rhs) 
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
                    runtime_dispatcher<Ts...>::copy(m_index, rhs.m_impl, m_impl);
                }
            }
            return *this;
        }

        variant<Ts...>& operator=(variant<Ts...>&& rhs)
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
                    runtime_dispatcher<Ts...>::move(m_index, rhs.m_impl, m_impl);
                }
            }
            return *this;
        }


        // ************************ //
        // *** Rebinding with T *** //
        // ************************ //
        template<typename T>
        requires one_of<T,Ts...>
        variant<Ts...>& operator=(const T& rhs) 
        {
            // Destruct old instance
            if (m_index != monostate)
            {
                runtime_dispatcher<Ts...>::destroy(m_index, m_impl);
            }

            // Construct new instance
            m_index = type_index<T,Ts...>::value;
            runtime_dispatcher<Ts...>::copy(m_index, &rhs, m_impl);
            return *this;
        }

        template<typename T>
        requires one_of<T,Ts...>
        variant<Ts...>& operator=(T&& rhs) 
        {
            // Destruct old instance
            if (m_index != monostate)
            {
                runtime_dispatcher<Ts...>::destroy(m_index, m_impl);
            }

            // Construct new instance
            m_index = type_index<T,Ts...>::value;
            runtime_dispatcher<Ts...>::move(m_index, &rhs, m_impl);
            return *this;
        }


        // ****************************** //
        // *** Rebinding with emplace *** //
        // ****************************** //
        template<typename T, typename...ARGS>
        requires one_of<T,Ts...>
        void emplace(ARGS&&...args)
        {
            // Destruct old instance
            if (m_index != monostate)
            {
                runtime_dispatcher<Ts...>::destroy(m_index, m_impl);
            }

            // Construct new instance
            m_index = type_index<T,Ts...>::value;
            new (m_impl) T{std::forward<ARGS>(args)...};
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

        template<typename T>
        T& get()
        {
            if (is_type<T>())
            {
                return *reinterpret_cast<T*>(m_impl);
            }
            throw std::runtime_error("getting invalid type from variant");
        }

        template<std::size_t N>
        typename type_of<N,Ts...>::type& get()
        {
            if (N == m_index && N < monostate)
            {
                return *reinterpret_cast<typename type_of<N,Ts...>::type*>(m_impl);
            }
            throw std::runtime_error("getting invalid type from variant");
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
