#pragma once
#include<cstring>    // for std::memcmp
#include<optional>
#include<functional> // for std::reference_wrapper



// ************************* //
// *** Reference wrapper *** //
// ************************* //
// Should support the following :
// * construct T from reference_wrapper<T>
// * reference_wrapper<const T> pointing to reference_wrapper<T>     
// * reference_wrapper<BASE>    pointing to reference_wrapper<DERIVED> 
// * reference_wrapper<callable> and invoked by ref(a,b,c...)
//
// 2 main access functions : 
// reference_wrapper<T>::get()                                  <--- for reading / writing to reference variable
// reference_wrapper<T>::operator=(const reference_wrapper<T>&) <--- for re-binding to another reference variable
//
namespace alg
{
      
    template<typename T> 
    class reference_wrapper
    {
    public:
        using type = T;

        template<typename U>
        friend class reference_wrapper; // declare reference_wrapper<U> as friend

    public:
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

    public:
        // conversion operator (to T&, NOT to T)
        constexpr operator T&() const noexcept
        {
            return *m_ptr;
        }

        constexpr T& get() const noexcept
        {
            return *m_ptr;
        }

        // if T is callable, operator() will invoke T
        template<typename...ARGS>
        decltype(auto) operator()(ARGS&&...args) const
        {
            return *m_ptr(std::forward<ARGS>(args)...);
        }

    private: 
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



// **************** //
// *** Optional *** //
// **************** //
// 3 main access functions : 
// optional<T>::operator*()                        <--- for reading / writing to owned variable
// optional<T>::operator=(const T&)                <--- for re-binding to new instance (i.e. destruct curent instance and construct new one) 
// optional<T>::operator=(const std::optional<T>&) <--- for re-binding to new instance (i.e. destruct curent instance and construct new one) 
//
namespace alg
{
    struct    nullopt_t {};
    constexpr nullopt_t nullopt{}; 
}

// no namespace
using std_nullopt = std::integral_constant<std::nullopt_t, std::nullopt>;
using alg_nullopt = std::integral_constant<alg::nullopt_t, alg::nullopt>;

namespace alg
{
    template<typename T> 
    class optional
    {
    public:
        optional() : m_flag(false)
        {
        }

        optional(const T& t) : m_flag(true) 
        {
            new (m_impl) T{t};
        }

//      template<typename...ARGS>
//      optional(ARGS&&...args) : m_flag(true), m_value{std::forward<ARGS>(args)...}
//      {
            // Do not provide this constructor, 
            // otherwise it hides copy-constructor 
//      }

        optional(const nullopt_t&) : m_flag(false)
        {
        }

        // Trivial copyable by memcpy
        optional(const optional<T>&) = default;
        optional<T>& operator=(const optional<T>&) = default;
        optional(optional<T>&&) = default;
        optional<T>& operator=(optional<T>&&) = default;

       
    public:
        bool operator==(const optional<T>& rhs) const noexcept 
        {
            if (m_flag && rhs.m_flag)
            {
                return (std::memcmp(m_impl, rhs.m_impl, sizeof(T)) == 0);
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

        const T& operator *() const noexcept { return *get_ptr(); }
        const T* operator->() const noexcept { return  get_ptr(); }
        T& operator *()             noexcept { return *get_ptr(); }
        T* operator->()             noexcept { return  get_ptr(); }

    private:
        constexpr const T* get_ptr() const
        {
            return reinterpret_cast<const T*>(m_impl);
        }

        constexpr T* get_ptr()
        {
            return reinterpret_cast<T*>(m_impl);
        }

    private:
        bool m_flag;
        char m_impl[sizeof(T)];
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


