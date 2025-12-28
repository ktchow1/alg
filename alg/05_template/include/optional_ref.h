#pragma once
#include<cstring>     // for std::memcmp
#include<optional>
#include<functional>  // for std::reference_wrapper
#include<type_traits> // for std::aligned_storage



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
        // ******************* //
        // *** Constructor *** //
        // ******************* //
        template<typename U>
        requires std::convertible_to<U*,T*>
        reference_wrapper(U& u) : m_ptr(&u)
        {
        }


    public:
        // ******************************* //
        // *** Destroy, copy (no move) *** //
        // ******************************* //
       ~reference_wrapper() 
        {
            m_ptr = nullptr;
        } 

        template<typename U>
        requires std::convertible_to<U*,T*>
        reference_wrapper(const reference_wrapper<U>& rhs) : m_ptr(rhs.m_ptr)
        {
        }


    public:
        // **************************** //
        // *** Rebinding and access *** //
        // **************************** //
        template<typename U>
        requires std::convertible_to<U*,T*>
        reference_wrapper<T>& operator=(const reference_wrapper<U>& rhs) 
        {
            m_ptr = rhs.m_ptr;
            return *this;
        }

        constexpr T& get() const noexcept
        {
            return *m_ptr;
        }


    public:
        // Conversion operator (to T&, NOT to T)
        constexpr operator T&() const noexcept
        {
            return *m_ptr;
        }

        // If T is callable, allow operator().
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
        static_assert(!std::is_reference_v<T>, "optional<T&> is illegal");

    public:
        // ******************* //
        // *** Constructor *** //
        // ******************* //
        optional() : m_flag(false)
        {
        }

        optional(const nullopt_t&) : m_flag(false)
        {
        }

        optional(const T& t) : m_flag(true) 
        {
            new (m_impl) T{t};
        }

        optional(T&& t) : m_flag(true) 
        {
            new (m_impl) T{std::move(t)};
        }


    public:
        // *************************** //
        // *** Destroy, copy, move *** //
        // *************************** //
        // Why can't we declare =default for 
        // * copy / move constructor
        // * copy / move assignment
        //
        // because default implementation is memcpy, without T::T{} construction,
        // when the optional goes out of scope, it will call ~T() on destruction,
        // resulting in construction / destruction mismatch.
        //

       ~optional()
        {
            destroy();
        }

        optional(const optional<T>& rhs) : m_flag(rhs.m_flag)
        {
            if (m_flag)
            {
                new (m_impl) T{*rhs}; 
            }
        }

        optional(optional<T>&& rhs) : m_flag(rhs.m_flag)
        {
            if (m_flag)
            {
                new (m_impl) T{std::move(*rhs)};
            }
        }


    public:
        // ***************** //
        // *** Rebinding *** //
        // ***************** //
        // Rebinding with assignment may result in null value.
        // Rebinding with emplace must result in valid value.
        //
        optional<T>& operator=(const optional<T>& rhs) 
        {
            if (this != &rhs)
            {
                // Destruct old instance
                destroy(); 

                // Construct new instance
                m_flag = rhs.m_flag;
                if (m_flag) new (m_impl) T{*rhs};
            }
            return *this;
        }

        optional<T>& operator=(optional<T>&& rhs) 
        {
            if (this != &rhs)
            {
                // Destruct old instance
                destroy(); 

                // Construct new instance
                m_flag = rhs.m_flag;
                if (m_flag) new (m_impl) T{std::move(*rhs)}; // <--- note : move(*rhs), NOT move(rhs)
            }
            return *this;
        }

        template<typename...ARGS>
        T& emplace(ARGS&&...args) 
        {
            // Destruct old instance
            destroy(); 

            // Construct new instance
            m_flag = true;
            new (m_impl) T{std::forward<ARGS>(args)...};

            return *get_ptr();
        }

       
    public:
        // ************** //
        // *** Access *** //
        // ************** //
        const T& operator *() const noexcept { return *get_ptr(); }
        const T* operator->() const noexcept { return  get_ptr(); }
        T& operator *()             noexcept { return *get_ptr(); }
        T* operator->()             noexcept { return  get_ptr(); }


    public:
        // Conversion operator
        operator bool() const 
        {
            return m_flag;
        }

        bool operator==(const optional<T>& rhs) const noexcept 
        {
            if (m_flag && rhs.m_flag)
            {
            //  return std::memcmp(m_impl, rhs.m_impl, sizeof(T)) == 0; // BUG
                return *get_ptr() == *rhs.get_ptr();                    // should forward to bool T::operator==(const T&) 
            }
            else
            {
                return m_flag == rhs.m_flag;
            }
        } 


    private:
        constexpr const T* get_ptr() const
        {
            return reinterpret_cast<const T*>(m_impl);
        }

        constexpr T* get_ptr()
        {
            return reinterpret_cast<T*>(m_impl);
        }

        void destroy()
        {
            if (m_flag)
            {
                m_flag = false;
                get_ptr()->~T(); // reverse placement new
            }
        }


    private:
        bool m_flag;
    //  char m_impl[sizeof(T)];                                   // this does not consider memory alignment, may not work in some CPU
    //  std::aligned_storage<sizeof(T), alignof(T)>::type m_impl; // this is deprecated in C++23
        alignas(T) std::byte m_impl[sizeof(T)];                   // this is the best option
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

// ************** //
// *** Remark *** //
// ************** //
//
// If we implement std::optional<T> :
// * with std::aligned_storage<sizeof(t), alignof(T)> m_impl, instead of ...
// * with std::byte m_impl[sizeof(T)]
//
// then we need to :
// * access with "&impl", instead of ...
// * access with  "impl"
//
// in all the following situations : 
// * placement new
// * reinterpret_cast
// * std::memcpy

