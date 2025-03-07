#pragma once
#include<cstdint>
#include<cassert>
#include<concepts>
#include<utility>       // for std::forward
#include<type_traits>   // for std::remove_reference


// 1. rvalueness of an expression is an attribute
//    rvalue reference is a type
// 2. rvalue reference can bind to rvalue expression
//    rvalue reference by itself is lvalue expression, if it is named
// 3. rvalue expression can be converted to lvalue by assigning it with named variable
//    lvalue expression can be converted to rvalue by static_cast std::move
//
namespace alg
{
    template<typename T> typename std::remove_reference<T>::type&& move(T&& x) noexcept
    {
        return static_cast<typename std::remove_reference<T>::type&&>(x);
    }

    template<typename T> T&& forward(typename std::remove_reference<T>::type& x) noexcept
    {
        return static_cast<T&&>(x);
    }
}

namespace alg
{
    enum class object_source
    {
        default_constructed,
        copy_constructed,
        copy_assigned,
        move_constructed,
        move_assigned
    };

    template<typename T>
    requires std::copyable<T>
    class rvalue_factory
    {
    public:
        template<typename...ARGS>
        rvalue_factory(ARGS...args) : m_value{alg::forward<ARGS>(args)...} // note : this is alg::forward
        {
        }

        // Though it returns m_value for all create function, the return type determines what is returned.
        // Without std::move for create_xvalue(), compile error : cannot bind rvalue reference to lvalue.
        const T&  create_const_lvalue() const noexcept { return  m_value;            }
              T&  create_lvalue()             noexcept { return  m_value;            }
              T&& create_xvalue()             noexcept { return  alg::move(m_value); } // note : this is alg::move
              T   create_prvalue()      const noexcept { return  m_value;            }
        const T*  get_pointer()         const noexcept { return &m_value;            }
               
    private:
        T m_value;
    }; 
                                                                      //  const lvalue | lvalue | xvalue | prvalue
    template<typename T> void binding_const_lvalue_ref(const T&) {}   //           Y   |    Y   |    Y   |    Y
    template<typename T> void binding_lvalue_ref(T&) {}               //           N   |    Y   |    N   |    N
    template<typename T> void binding_rvalue_ref(T&&) {}              //           N   |    N   |    Y   |    Y
    template<typename T> void binding_value(T) {}                     //           Y   |    Y   |    Y   |    Y
}    

//                          | DC  CC  CA  MC  MA  DD 
// -------------------------+-------------------------
// if member initialized DC | *   *   *   *   *   * 
// if emplicitly defined DC | -   *   *   *   *   * 
// if emplicitly defined CC |     -   *   *   *   *
// if emplicitly defined CA | *   *   -           * 
// if emplicitly defined MC |             -       *
// if emplicitly defined MA | *               -   *       
// if emplicitly defined DD |         *   *   *   -    ... then asterisked functions will be compiler generated 
//      
namespace alg
{
    // for test_rvalue.cpp
    struct member_initialized_DC { private:  std::uint32_t m_x = 123;   };
    struct explicitly_defined_DC { private:  explicitly_defined_DC() {} };
    struct explicitly_defined_CC { private:  explicitly_defined_CC           (const explicitly_defined_CC&) {               } };
    struct explicitly_defined_CA { private:  explicitly_defined_CA& operator=(const explicitly_defined_CA&) { return *this; } };
    struct explicitly_defined_MC { private:  explicitly_defined_MC           (explicitly_defined_MC&&)      {               } };
    struct explicitly_defined_MA { private:  explicitly_defined_MA& operator=(explicitly_defined_MA&&)      { return *this; } };
    struct explicitly_defined_DD { private: ~explicitly_defined_DD() {} };

    // for test_std_container.cpp
    struct reference_member
    {
        reference_member(const std::uint32_t& x) : m_x(x) {}
        reference_member() = delete;
        reference_member(const reference_member&) = default;
        reference_member(reference_member&&) = default;
        reference_member& operator=(const reference_member&) = default;
        reference_member& operator=(reference_member&&) = default;

        const std::uint32_t& m_x;
    };

    struct purely_copyable
    {
        purely_copyable(std::uint32_t x) : m_x(x){}
        purely_copyable() = delete;
        purely_copyable(const purely_copyable&) = default;
        purely_copyable(purely_copyable&&) = delete;
        purely_copyable& operator=(const purely_copyable&) = default;
        purely_copyable& operator=(purely_copyable&&) = delete;

        std::uint32_t m_x;
    };

    struct purely_movable
    {
        purely_movable(std::uint32_t x) : m_x(x){}
        purely_movable() = delete;
        purely_movable(const purely_movable&) = delete;
        purely_movable(purely_movable&&) = default;
        purely_movable& operator=(const purely_movable&) = delete;
        purely_movable& operator=(purely_movable&&) = default;

        std::uint32_t m_x;
    };

    struct copyable
    {
        copyable() = default;
        copyable(const copyable&) = default;
        copyable(copyable&&) = delete;
        copyable& operator=(const copyable&) = default;
        copyable& operator=(copyable&&) = delete;
    };

    struct movable
    {
        movable() = default;
        movable(const movable&) = delete;
        movable(movable&&) = default;
        movable& operator=(const movable&) = delete;
        movable& operator=(movable&&) = default;
    };

    struct comparable
    { 
        comparable(std::uint32_t x) : m_x(x){}
        comparable() = delete;
        comparable(const comparable&) = default;
        comparable(comparable&&) = delete;
        comparable& operator=(const comparable&) = default;
        comparable& operator=(comparable&&) = delete;

        auto operator<=>(const comparable& rhs) const { return m_x <=> rhs.m_x; } // BUG : Do not forget const-ness
        std::uint32_t m_x;
    };

    struct hashable
    {
        hashable(const std::string& s) : m_s(s){}
        hashable() = delete;
        hashable(const hashable&) = default;
        hashable(hashable&&) = delete;
        hashable& operator=(const hashable&) = default;
        hashable& operator=(hashable&&) = delete;

        auto operator==(const hashable& rhs) const { return m_s == rhs.m_s; } // BUG : Do not forget const-ness
        std::string m_s;
    };

    struct hash
    {
        std::size_t operator()(const hashable& x) const // BUG : Do not forget const-ness
        {
            std::size_t ans = 0;
            for(const char c:x.m_s)
            {
                std::size_t i;
                i = c ^ (c << 13);
                i = i ^ (i >> 17);
                i = i ^ (i <<  5);
                ans = 31 * ans + i;
            }
            return ans;
        }
    };
}

// Universal reference T&& or auto&&
// T ---> X& for lvalue 
// T ---> X  for rvalue
//
namespace alg
{
    struct X{};

    template<typename EXPECTED_TYPE, typename T> void universal_reference_fct(T&& x)
    {
        static_assert(std::is_same_v<T,EXPECTED_TYPE>, "universal reference deduction incorrect");
    }

    inline void universal_reference_deduction()
    {
        X x;
        universal_reference_fct<X&>(x);             //  lvalue
        universal_reference_fct<X>(std::move(x));   //  xvalue 
        universal_reference_fct<X>(X{});            // prvalue

        // constness is kept
        const X cx;
        universal_reference_fct<const X&>(cx);  
        universal_reference_fct<const X>(std::move(cx)); 

        // reference is stripped
        X& rx = x;
        universal_reference_fct<X&>(rx);  
        universal_reference_fct<X>(std::move(rx)); 
        
        // constness is kept, reference is stripped
        const X& crx = cx;
        universal_reference_fct<const X&>(crx);  
        universal_reference_fct<const X>(std::move(crx)); 
    }

    inline void universal_reference_collapsing()
    {   
        using lvalue_reference = X&;
        using rvalue_reference = X&&;
        static_assert(std::is_same_v<lvalue_reference & , X& >, "universal reference collapsing incorrect");
        static_assert(std::is_same_v<lvalue_reference &&, X& >, "universal reference collapsing incorrect");
        static_assert(std::is_same_v<rvalue_reference & , X& >, "universal reference collapsing incorrect");
        static_assert(std::is_same_v<rvalue_reference &&, X&&>, "universal reference collapsing incorrect");
    }
}


