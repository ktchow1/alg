#pragma once
#include<iostream>
#include<cstdint>


// ***********************************************************************
// Template basic                                                                              
// *     function template   with auto deduction / NTTP vs TTP / auto NTTP
// *        class template   with CTAD / deduction guide 
// * class member template                                                                     
// *     variable template                                                                     
// *        alias template                                                                     
// *     template template                                                                     
//
//
// Template definition
// * generalization 
// * specialization
// ***********************************************************************
namespace alg 
{
    namespace global
    {
        std::uint32_t overload = 0;
    }



    // ************************* //
    // *** function template *** //
    // ************************* //
    template<typename T, typename U>
    void function_template(const T& t, const U& u)
    {
        global::overload = 1;
    }

    template<typename U>
    void function_template(const std::string& str, const U& u)
    {
        global::overload = 2;
    }


    // ************************************************************** //
    // *** function template (NTTP - non type template parameter) *** //
    // ************************************************************** //
    template<typename T, std::uint32_t N>             // <--- specific NTTP
    void function_template_with_NTTP(const T& t)
    {
        for(std::uint32_t n=0; n!=N; ++n) t();
    }

    template<typename T, auto N>                      // <--- any NTTP 
    void function_template_with_NTTP(const T& t)
    {
        for(decltype(N) n=0; n!=N; ++n) t();
    }

    template<const char* str>                         // <--- char[] as NTTP
    auto function_template_with_char_array_NTTP()
    {
        return std::string{str};
    } 


    // ************************************* //
    // *** Abbreviated function template *** //
    // ************************************* //
    void abbre_function_template(const auto& x, auto&& y) // <--- support universal reference
    {
        auto temp = std::forward<decltype(y)>(y); // <--- how universal reference can be forwarded
    }



    // ********************** //
    // *** class template *** //
    // ********************** //
    // There are 6 cases :
    // class template with template type deduced by CTAD         
    // class template with template type deduced by deduction guide
    // class template with template type explicit specified      
    // 
    // CTAD            = compiler deduces all template types in constructor
    // deduction guide = compiler deduces all template types given hint 
    
    template<typename T, typename U, typename V>
    struct class_template
    {
        using type0 = T;
        using type1 = U;
        using type2 = V;

        class_template() = default;

        class_template(const T& t, const U& u) : m_t(t), m_u(u) // support deduction guide A
        {
        }
        class_template(const T& t, const U& u, const V& v) : m_t(t), m_u(u), m_v(v) // support CTAD
        {
        }
        void fct() const noexcept;
    
    private:
        T m_t; 
        U m_u;
        V m_v;
    };

    template<typename T, typename U, typename V>
    void class_template<T,U,V>::fct() const noexcept
    {
        global::overload = 3;
    }


    // *** specialization *** //
    template<typename T, typename U>
    struct class_template<T,U,std::string>
    {
        using type0 = T;
        using type1 = U;
        using type2 = std::string;

        class_template() = default;

        class_template(const T& t) : m_t(t), m_u(), m_str("xxx") // support deduction guide B
        {
        }
        class_template(const T& t, const U& u, const std::string& str) : m_t(t), m_u(u), m_str(str) // support CTAD
        {
        }
        void fct() const noexcept;

    private:
        T m_t;
        U m_u;
        std::string m_str; 
    };

    template<typename T, typename U>
    void class_template<T,U,std::string>::fct() const noexcept
    {
        global::overload = 4;
    }


    // *** deduction guide *** //
    template<typename T, typename U>
    class_template(const T&, const U&) -> class_template<T,U,int>;         // <--- deduction guide A
    template<typename T>
    class_template(const T&) -> class_template<T,std::string,std::string>; // <--- deduction guide B



    // ***************************** //
    // *** class member template *** //
    // ***************************** //
    template<typename T>
    struct class_member_template
    {
        template<typename U>
        void fct() const noexcept;
    };

    template<typename T>
    template<typename U>
    void class_member_template<T>::template fct() const noexcept // BUG : cannot compile without keyword "template"
    {
        global::overload = 5;
    }

    template<>
    struct class_member_template<std::string>
    {
        template<typename U>
        void fct() const noexcept;
    };

//  template<> // BUG : cannot compile with keyword "template"
    template<typename U>
    void class_member_template<std::string>::template fct() const noexcept 
    {
        global::overload = 6;
    }

    template<typename T>
    void class_member_template_caller(const class_member_template<T>& x)
    {
        x.template fct<std::string>(); // BUG : cannot compile without keyword "template" 
                                       // (let compiler knows template member inside template fct/class)
    }



    // ************************* //
    // *** variable template *** //
    // ************************* //
    template<typename T> constexpr int variable_template = 0;
    template<>           constexpr int variable_template<int> = 1;
    template<>           constexpr int variable_template<std::string> = 2;
    template<>           constexpr int variable_template<std::pair<int,int>> = 3;
    template<>           constexpr int variable_template<std::pair<int,std::string>> = 4;
    template<>           constexpr int variable_template<std::pair<std::string,int>> = 5;
    template<>           constexpr int variable_template<std::pair<std::string,std::string>> = 6;

    // Another usage
    template<typename T> std::vector<T> variable_template2 = {1,2,3,4,5};



    // ********************** //
    // *** alias template *** //
    // ********************** //
    // Does not allow specialization
    template<typename C> using alias_template = typename C::value_type;



    // ************************* // 
    // *** template template *** //
    // ************************* // 
    template<template<typename,typename> typename MAP, typename K, typename V>
    struct template_template
    {
         using type = MAP<K,V>;
    };



    // ************************************************************** //
    // *** function pointer as     type-template-parameter  (TTP) *** //
    // *** function pointer as non-type-template-parameter (NTTP) *** //
    // ************************************************************** //
    // invoke_fct__TTP0   : FCT_PTR is deduced from 1st input argument                        (support lambda)
    // invoke_fct__TTP1   : FCT_PTR is provided by caller as     type-template-paremeter (cant support lambda) 
    // invoke_fct_NTTP0/1 : FCT_PTR is provided by caller as non-type-template-paremeter  
    // invoke_fct__TTP0/1 : can work with fct pointers having different signatures
    // invoke_fct_NTTP0/1 : can work with fct pointers having specific signature only
    // 
    namespace fct_space
    {
        std::string   fct1(const std::string& s, std::uint32_t n, std::uint32_t m) { return std::to_string(std::stol(s) + n + m + 1000); }
        std::string   fct2(const std::string& s, std::uint32_t n, std::uint32_t m) { return std::to_string(std::stol(s) + n + m + 2000); }
        std::uint32_t fct3(std::uint32_t n, std::uint32_t m) { return n + m + 3000; }
        std::uint32_t fct4(std::uint32_t n, std::uint32_t m) { return n + m + 4000; }
        std::string   fct5() { return "aaaa"; }
        std::string   fct6() { return "bbbb"; }
    }

    template<typename FCT_PTR, typename...ARGS> 
    auto invoke_fct_TTP(FCT_PTR fct_ptr, ARGS&&... args) // use 1st arg to deduce FCT_PTR 
    {
        return (*fct_ptr)(std::forward<ARGS>(args)...);
    }

    template<auto fct_ptr, typename...ARGS> 
    auto invoke_fct_NTTP(ARGS&&... args)  
    {
        return (*fct_ptr)(std::forward<ARGS>(args)...);
    }

    template<std::uint32_t (*fct_ptr)(std::uint32_t, std::uint32_t), typename...ARGS>
    auto invoke_fct_NTTP0(ARGS&&... args)  
    {
        return (*fct_ptr)(std::forward<ARGS>(args)...);
    }

    template<std::string (*fct_ptr)()> 
    auto invoke_fct_NTTP1()  
    {
        return (*fct_ptr)();
    }



    // ************************************************************ //
    // *** member pointer as     type-template-parameter  (TTP) *** //
    // *** member pointer as non-type-template-parameter (NTTP) *** //
    // ************************************************************ //
    // See remark for FCT_PTR
    //
    struct fct_group
    {
        std::string fct1(const std::string& s, std::uint32_t n, std::uint32_t m) { return std::to_string(std::stol(s) + n + m + 1000); }
        std::string fct2(const std::string& s, std::uint32_t n, std::uint32_t m) { return std::to_string(std::stol(s) + n + m + 2000); }
        std::uint32_t fct3(std::uint32_t n, std::uint32_t m) { return n + m + 3000; }
        std::uint32_t fct4(std::uint32_t n, std::uint32_t m) { return n + m + 4000; }
        std::string fct5() { return "aaaa"; }
        std::string fct6() { return "bbbb"; }
    };


    template<typename MEM_PTR, typename...ARGS> 
    auto invoke_mem_TTP(MEM_PTR mem_ptr, ARGS&&... args) 
    {
        fct_group x;
        return (x.*mem_ptr)(std::forward<ARGS>(args)...);
    }

    template<auto mem_ptr, typename...ARGS> 
    auto invoke_mem_NTTP(ARGS&&... args)  
    {
        fct_group x;
        return (x.*mem_ptr)(std::forward<ARGS>(args)...);
    }

    template<std::uint32_t (fct_group::* mem_ptr)(std::uint32_t, std::uint32_t), typename...ARGS>
    auto invoke_mem_NTTP0(ARGS&&... args)  
    {
        fct_group x;
        return (x.*mem_ptr)(std::forward<ARGS>(args)...);
    }

    template<std::string (fct_group::* mem_ptr)()> 
    auto invoke_mem_NTTP1()  
    {
        fct_group x;
        return (x.*mem_ptr)();
    }
}


