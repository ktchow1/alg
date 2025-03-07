#pragma once
#include<optional>
#include<functional> // for std::reference_wrapper


// [Different referencing] 
// T&                             : cannot be null, not assignable, not used in vector
// reference_wrapper<T>           : cannot be null,     assignable,     used in vector
// optional<T&>                   : cannot compile
// optional<erference_wrapper<T>> : can    be null,     assignable,     used in vector
// T*                             : can    be null,     assignable,     used in vector, need managing
//
//
//
// [Problem of std::optional<T&>]
// std::optional<T&> is not allowed, why? 
//
// std::optional<int&> i = x;
// i = y; <--- what does that mean? ambiguous : 
//             rebinding i to y? or
//             assigning x with y's value?
//
//
//
// [Solution to std::optional<T&>]
// std::optional<std::reference_wrapper<T>> is used instead
//
//
//
// [Dereferencing]
// for std::tuple,             use std::get<>()
// for std::variant,           use std::get<>()
// for std::reference_wrapper, use std::reference_wrapper<>::get()
// for std::optional,          use std::optional<>::operator*() 
//
//
//
// [Base reference pointing derived reference]
// in the following implementation, it allows :
// alg::reference_wrapper<BASE> points to alg::reference_wrapper<DERIVED>
//
// which is (I think) is more complicated than shared_ptr,
// as for shared_ptr, it is done by std::dynamic_pointer_case.


namespace alg
{
    template<typename T> class reference_wrapper
    {
    public:
        using type = T;

        // Allow the following : 
        // * reference_wrapper<const T> pointing to reference_wrapper<T>     
        // * reference_wrapper<BASE>    pointing to reference_wrapper<DERIVED> 
        template<typename U>
        requires std::convertible_to<U*,T*>
        reference_wrapper(U& u) : m_ptr(&u)
        {
        }

       ~reference_wrapper() 
        {
            m_ptr = nullptr;
        } 
        
        template<typename U>
        requires std::convertible_to<U*,T*>
        reference_wrapper(const reference_wrapper<U>& rhs) : m_ptr(rhs.m_ptr)
        {
        }

        template<typename U>
        requires std::convertible_to<U*,T*>
        reference_wrapper<T>& operator=(const reference_wrapper<U>& rhs) 
        {
            m_ptr = rhs.m_ptr;
            return *this;
        }

        // conversion operator (to T&, NOT to T)
        constexpr operator T&() const noexcept
        {
            return *m_ptr;
        }

        constexpr T& get() const noexcept
        {
            return *m_ptr;
        }

    public: 
        // Todo - should be private, but don't know how to grant friendship
        // from alg::reference_wrapper<T> 
        //   to alg::reference_wrapper<U> 
        T* m_ptr; 
    };

    template<typename T> 
    auto ref(T& t)
    {
        return reference_wrapper<T>(t);
    }

    template<typename T> 
    auto cref(T& t)
    {
        return reference_wrapper<const T>(t);
    }
}


namespace alg
{
    struct nullopt_t {};
    constexpr nullopt_t nullopt{}; 
}
// no namespace
using std_nullopt = std::integral_constant<std::nullopt_t, std::nullopt>;
using alg_nullopt = std::integral_constant<alg::nullopt_t, alg::nullopt>;


namespace alg
{
    template<typename T> class optional
    {
    public:
        optional() : m_flag(false)
        {
        }

        optional(const T& t) : m_flag(true), m_value(t)
        {
        }

//      template<typename...ARGS>
//      optional(ARGS&&...args) : m_flag(true), m_value{std::forward<ARGS>(args)...}
//      {
            // Do not provide this constructor, 
            // otherwise it hides copy-constructor 
            // and fail my test (why is that?) 
            // Because it can convert to bool 
            // and construct struct A in test?
//      }

        optional(const nullopt_t&) : m_flag(false)
        {
        }

        optional(const optional<T>&) = default;
        optional<T>& operator=(const optional<T>&) = default;
        optional(optional<T>&&) = default;
        optional<T>& operator=(optional<T>&&) = default;

       
    public:
        // please replace by spaceship operator
        bool operator==(const optional<T>& rhs) const noexcept 
        {
            if (m_flag && rhs.m_flag)
            {
                return m_value == rhs.m_value;
            }
            else
            {
                return m_flag == rhs.m_flag;
            }
        } 

        operator bool() const 
        {
            return m_flag;
        }

        const T& operator *() const noexcept { return  m_value; }
        const T* operator->() const noexcept { return &m_value; }
        T& operator *()             noexcept { return  m_value; }
        T* operator->()             noexcept { return &m_value; }

    private:
        bool m_flag;
        T m_value;
    };
    
    template<typename T, typename...ARGS> 
    auto make_optional(ARGS&&...args)
    {
        return optional<T>
        {
            T{std::forward<ARGS>(args)...}
        };
    }
}


