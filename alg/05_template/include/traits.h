#pragma once
#include<iostream>
#include<cstdint>
#include<memory>
#include<vector>
#include<type_traits> // for std::is_lvalue_reference


// ********************* //
// *** Simple traits *** //
// ********************* //
namespace alg 
{
    template<typename T> struct type2value_traits                { static constexpr std::int8_t value =  0; };
    template<>           struct type2value_traits<std::uint8_t>  { static constexpr std::int8_t value = +1; };
    template<>           struct type2value_traits<std::uint16_t> { static constexpr std::int8_t value = +2; };
    template<>           struct type2value_traits<std::uint32_t> { static constexpr std::int8_t value = +3; };
    template<>           struct type2value_traits<std::uint64_t> { static constexpr std::int8_t value = +4; };
    template<>           struct type2value_traits<std::int8_t>   { static constexpr std::int8_t value = -1; };
    template<>           struct type2value_traits<std::int16_t>  { static constexpr std::int8_t value = -2; };
    template<>           struct type2value_traits<std::int32_t>  { static constexpr std::int8_t value = -3; };
    template<>           struct type2value_traits<std::int64_t>  { static constexpr std::int8_t value = -4; };

    template<typename T0,typename T1,typename T2,int N> 
    struct value2type_traits 
    {
       using type = value2type_traits<T0,T1,T2,N-1>::type; 
    };

    template<typename T0,typename T1,typename T2> struct value2type_traits<T0,T1,T2, 0> { using type = T0; };
    template<typename T0,typename T1,typename T2> struct value2type_traits<T0,T1,T2,10> { using type = T1; };
    template<typename T0,typename T1,typename T2> struct value2type_traits<T0,T1,T2,20> { using type = T2; };

    // create shortcut with variable template
    template<typename T> constexpr std::int8_t type2value_traits_v = type2value_traits<T>::value;

    // create shortcut with alias template
    template<typename T0,typename T1,typename T2,int N> using value2type_traits_t = value2type_traits<T0,T1,T2,N>::type;
}


// ********************** //
// *** Runtime traits *** //
// ********************** //
// Inspired by Maven interview : 
// * objective is given by hardcoded approach
// * please re-implement in variadic approach
//
namespace alg
{
    struct baseN
    { 
        virtual void dummy() {} 
    };

    template<int N> 
    struct derivedN : public baseN
    {
    };


    // 1. hardcoded approach 
    template<int N0, int N1, int N2, int N3, int N4>
    struct value2type_hardcoded_runtime
    {
        static std::shared_ptr<baseN> create(int n)
        {
            if      (n == N0) return std::shared_ptr<baseN>(new derivedN<N0>{});
            else if (n == N1) return std::shared_ptr<baseN>(new derivedN<N1>{});
            else if (n == N2) return std::shared_ptr<baseN>(new derivedN<N2>{});
            else if (n == N3) return std::shared_ptr<baseN>(new derivedN<N3>{});
            else if (n == N4) return std::shared_ptr<baseN>(new derivedN<N4>{});
            else              return nullptr;
        }
    };
  
    template<int N, int N0, int N1, int N2, int N3, int N4>
    struct value2type_hardcoded_compiletime { using type = void; }; 
    template<int N0, int N1, int N2, int N3, int N4>
    struct value2type_hardcoded_compiletime<N0,N0,N1,N2,N3,N4> { using type = derivedN<N0>; };
    template<int N0, int N1, int N2, int N3, int N4>
    struct value2type_hardcoded_compiletime<N1,N0,N1,N2,N3,N4> { using type = derivedN<N1>; };
    template<int N0, int N1, int N2, int N3, int N4>
    struct value2type_hardcoded_compiletime<N2,N0,N1,N2,N3,N4> { using type = derivedN<N2>; };
    template<int N0, int N1, int N2, int N3, int N4>
    struct value2type_hardcoded_compiletime<N3,N0,N1,N2,N3,N4> { using type = derivedN<N3>; };
    template<int N0, int N1, int N2, int N3, int N4>
    struct value2type_hardcoded_compiletime<N4,N0,N1,N2,N3,N4> { using type = derivedN<N4>; };


    // 2. variadic approach  
    //
    // Question : Can we remove interface and consider recursion as interface? 
    //            No, recursion requires at least 1 int, while boundary has 0 int.
    //
    template<int...Ns> 
    struct value2type_variadic_runtime                     // interface
    {
    };
    template<int N, int...Ns> 
    struct value2type_variadic_runtime<N,Ns...>            // recursion
    {
        static std::shared_ptr<baseN> create(int n)
        {
            if (n==N) return std::shared_ptr<baseN>(new derivedN<N>{});
            else      return value2type_variadic_runtime<Ns...>::create(n);
        }
    };
    template<> 
    struct value2type_variadic_runtime<>                   // boundary case
    {
        static std::shared_ptr<baseN> create(int n)
        {
            return nullptr;
        }
    };
   
    template<int N, int...Ns>            
    struct value2type_variadic_compiletime                 // interface and boundary case
    { 
        using type = void; 
    };   
    template<int N, int N0, int...Ns>
    struct value2type_variadic_compiletime<N,N0,Ns...>     // recursion
    { 
        using type = typename value2type_variadic_compiletime<N,Ns...>::type; 
    }; 
    template<int N, int...Ns>
    struct value2type_variadic_compiletime<N,N,Ns...>      // boundary case
    { 
        using type = derivedN<N>; 
    }; 
}


// ********************* //
// *** SFINAE traits *** //
// ********************* //
namespace alg 
{
    // ***************************** //
    // *** step 1 : true & false *** //
    // ***************************** //
    template<typename T, T N>
    struct integral_constant
    {
        using value_type = T;
        static constexpr T value = N;
    };
    using  true_type = integral_constant<bool,  true>;
    using false_type = integral_constant<bool, false>;
      
    template<typename T, typename U> struct is_same      : public false_type {};
    template<typename T>             struct is_same<T,T> : public  true_type {};
    template<typename T, typename U> constexpr bool is_same_v = is_same<T,U>::value;


    // *********************** //
    // *** step 2 : remove *** //
    // *********************** //
    template<typename T> struct remove_const          { using type = T; };
    template<typename T> struct remove_const<const T> { using type = T; };
    template<typename T> struct remove_ref            { using type = T; };
    template<typename T> struct remove_ref<T&>        { using type = T; };
    template<typename T> struct remove_ref<T&&>       { using type = T; };
    template<typename T> struct remove_ptr            { using type = T; }; 
    template<typename T> struct remove_ptr<T*>        { using type = T; };
    template<typename T> struct remove_ptr<T* const>  { using type = T; }; // necessary for removing const pointer (for const pointee / non-const pointee)
    template<typename T> struct remove_ptr<T**>       { using type = T; };
    template<typename T> struct decay
    {
        using type = typename remove_const<typename remove_ref<typename remove_ptr<T>::type>::type>::type;
    };


    // *********************** //
    // *** step 3 : traits *** //
    // *********************** //
    template<typename T> struct is_ptr_impl     : public false_type {};
    template<typename T> struct is_ptr_impl<T*> : public  true_type {};
    template<typename T> struct is_ptr : is_ptr_impl<typename remove_const<typename remove_ref<T>::type>::type> {};
    template<typename T> constexpr bool  is_ptr_v = is_ptr<T>::value;

    // is_array
    template<typename V> struct is_array_impl                  : public false_type {};
    template<typename T, 
        std::uint64_t N> struct is_array_impl<std::array<T,N>> : public  true_type {};
    template<typename V> struct is_array : is_array_impl<typename decay<V>::type>  {};
    template<typename V> constexpr bool    is_array_v = is_array<V>::value;

    // is_vec
    template<typename V> struct is_vec_impl                 : public false_type {};
    template<typename T> struct is_vec_impl<std::vector<T>> : public  true_type {};
    template<typename V> struct is_vec : is_vec_impl<typename decay<V>::type> {};
    template<typename V> constexpr bool  is_vec_v = is_vec<V>::value;

    // is_vec_naive
    template<typename V> struct is_vec_naive                              { using type = std::false_type; };
    template<typename T> struct is_vec_naive<      std::vector<T>       > { using type = std::true_type;  };
    template<typename T> struct is_vec_naive<const std::vector<T>       > { using type = std::true_type;  };
    template<typename T> struct is_vec_naive<      std::vector<T>&      > { using type = std::true_type;  };
    template<typename T> struct is_vec_naive<const std::vector<T>&      > { using type = std::true_type;  };
    template<typename T> struct is_vec_naive<      std::vector<T>&&     > { using type = std::true_type;  };
    template<typename T> struct is_vec_naive<      std::vector<T>*      > { using type = std::true_type;  };
    template<typename T> struct is_vec_naive<const std::vector<T>*      > { using type = std::true_type;  };
    template<typename T> struct is_vec_naive<const std::vector<T>* const> { using type = std::true_type;  };
    template<typename T> struct is_vec_naive<      std::vector<T>* const> { using type = std::true_type;  };
    template<typename V> constexpr bool is_vec_naive_v = is_vec_naive<V>::type::value;

    // is_vec_except_pointer
    template<typename V>
    struct is_vec_except_pointer
    {
    private:
        using V_REMOVED = std::remove_cvref_t<V>;

        template<typename T>
        static auto impl(std::vector<T>*) -> std::true_type;
        static std::false_type impl(void*); // no definition needed

    public:
        using type = decltype(impl(std::declval<V_REMOVED*>()));
    };

    template<typename V> 
    constexpr bool is_vec_except_pointer_v = is_vec_except_pointer<V>::type::value;


    // is_convertible will be replaced by is_base_of
    template<typename B> false_type is_inherit_impl(const void*); // general case (no need to implement, used in decltype)
    template<typename B>  true_type is_inherit_impl(const B*);    // special case (no need to implement, used in decltype)
    template<typename B, typename D> struct is_inherit : public decltype(is_inherit_impl<B>(std::declval<D*>())) {}; // BUG : compile error without <B>, as special case can bind to any type
    template<typename B, typename D> constexpr bool is_inherit_v = is_inherit<B,D>::value;

  

    // **************************** //
    // *** step 4 : conditional *** //
    // **************************** //
    // Deduce different types depending on boolean value B
    template<bool B, typename T, typename F> struct condition           { using type = F; };
    template<        typename T, typename F> struct condition<true,T,F> { using type = T; };
    
    // Allow compilation (or not) depending on boolean value B
    template<bool B, typename T=void> struct enable_if         {                 }; // no type here, this case will trigger compilation error
    template<        typename T     > struct enable_if<true,T> { using type = T; }; 


    // *********************** //
    // *** step 5 : sfinae *** //
    // *********************** //
    template<typename T, typename U> struct is_target_types                : public false_type {};
    template<typename T>             struct is_target_types<T,std::string> : public  true_type {};
    template<>                       struct is_target_types<int,int>       : public  true_type {};
    template<typename T, typename U> constexpr bool is_target_types_v = is_target_types<T,U>::value;

    // method 1 : introduce extra template non-type parameter (work for template function and class)
    template<typename T, typename U, typename enable_if<is_target_types_v<T,U>, int>::type dummy = 0> 
    bool sfinae_fct_method1()
    {
        return true;
    }

    template<typename T, typename U, typename enable_if<!is_target_types_v<T,U>, int>::type dummy = 0> 
    bool sfinae_fct_method1()
    {
        return false;
    }

    // method 2 : introduce return value (work for template function only)
    template<typename T, typename U> 
    typename enable_if<is_target_types_v<T,U>, bool>::type sfinae_fct_method2()
    {
        return true;
    }

    template<typename T, typename U> 
    typename enable_if<!is_target_types_v<T,U>, bool>::type sfinae_fct_method2()
    {
        return false;
    }

    // method 3 : introduce input argument (work for template function only)
    template<typename T, typename U> 
    bool sfinae_fct_method3(typename enable_if<is_target_types_v<T,U>, std::string>::type str = "")
    {
        return true;
    }

    template<typename T, typename U> 
    bool sfinae_fct_method3(typename enable_if<!is_target_types_v<T,U>, std::string>::type str = "")
    {
        return false;
    }

    // method 4 : use "if constexpr"
    template<typename T, typename U> 
    bool sfinae_fct_method4()
    {
        if constexpr (is_target_types_v<T,U>) 
             return true;
        else return false;
    }


    // ************************************** //
    // *** example 1 : perfect forwarding *** //
    // ************************************** //
    template<typename T, typename enable_if<std::is_lvalue_reference<T>::value, int>::type dummy = 0> 
    bool perfect_forwarding_impl(T&& arg)
    {
        return true;
    }

    template<typename T, typename enable_if<!std::is_lvalue_reference<T>::value, int>::type dummy = 0> 
    bool perfect_forwarding_impl(T&& arg)
    {
        return false;
    }

    template<typename T>
    bool perfect_forwarding(T&& arg) // return true for lvalue, false otherwise
    {
        return perfect_forwarding_impl(std::forward<T>(arg));
    }


    // *************************************************** //
    // *** example 2 : sfinae in class member function *** //
    // *************************************************** //
    // As class template type T is provided by caller, 
    // sfinae_fct() does not trigger type deduction, 
    // hence need to introduce extra template type U.
    // ************************************************* //
    struct target_A {};
    struct target_B {};

    template<typename T> struct is_target : public std::false_type {};
    template<> struct is_target<target_A> : public std::true_type {};
    template<> struct is_target<target_B> : public std::true_type {};

    template<typename T>
    class sfinae_class
    {
    public:
        template<typename U=T> 
        typename std::enable_if<is_target<U>::value, bool>::type fct()
        {
            return true;
        }

        template<typename U=T> 
        typename std::enable_if<!is_target<U>::value, bool>::type fct()
        {
            return false;
        }
    };


    // ***************************************** //
    // *** step 6 : advance traits by sfinae *** //
    // ***************************************** //
    // 1. define template parameters T,B,D,SRC,DST 
    // 2. define template parameter CONSTRAINT as last parameter
    // 3. provide default type  for CONSTRAINT, as caller won't input it
    // 4. provide desired logic for CONSTRAINT in specialization
    // 5. ensure deduced CONSTRAINT type in specialization is same as default CONSTRAINT type in general case 
    //   (which is default_type in the following examples, it can be any types ... like void for simplicity)


    // helpers
    struct default_type {};
    template<typename T> using map_to_default = default_type; 
    template<typename T> default_type bind_to(T); // <--- template function that can take type T and return default_type 


    // is incrementable
    template<typename T, typename CONSTRAINT = default_type> 
    struct is_incrementable : public false_type {};
    template<typename T>
    struct is_incrementable<T,map_to_default<decltype(++std::declval<T>())>> : public true_type {};
    

    // has value (or ... is_container)
    template<typename T, typename CONSTRAINT = default_type> 
    struct has_value : public false_type {};
    template<typename T>
    struct has_value<T,map_to_default<typename T::value_type>> : public true_type {};
    
    // is convertible
    template<typename SRC, typename DST, typename CONSTRAINT = default_type>  
    struct is_convertible : public false_type {};
    template<typename SRC, typename DST> 
    struct is_convertible<SRC,DST,decltype(bind_to<DST>(std::declval<SRC>()))> : public true_type {};

    // is base of
    template<typename B, typename D, typename CONSTRAINT = default_type> 
    struct is_base_of : public false_type {}; 
    template<typename B, typename D> 
    struct is_base_of<B,D,decltype(bind_to<B*>(std::declval<D*>()))> : public true_type {};

    // is base of (alternative approach, but cannot compile for private inheritance, due to ERROR in static_cast ???)
    template <typename B> std::false_type is_ptr_convertible_to(const void*);
    template <typename B> std::true_type  is_ptr_convertible_to(const B*);
    template <typename B, typename D>
    struct is_base_of2 : public decltype(is_ptr_convertible_to<B>(static_cast<D*>(nullptr)))
    {
    };

    // shortcut
    template<typename T>                 constexpr bool is_incrementable_v = is_incrementable<T>::value;
    template<typename T>                 constexpr bool has_value_v        = has_value<T>::value;
    template<typename SRC, typename DST> constexpr bool is_convertible_v   = is_convertible<SRC,DST>::value;
    template<typename B,   typename D>   constexpr bool is_base_of_v       = is_base_of <B,D>::value;
    template<typename B,   typename D>   constexpr bool is_base_of_v2      = is_base_of2<B,D>::value;


    // ***************************************** //
    // *** example 3 : sfinae in inheritance *** //
    // ***************************************** //
    struct sfinae_base {};

    template<typename T, typename std::enable_if<alg::is_base_of_v<sfinae_base,T>, int>::type = 0>
    bool sfinae_inheritance_method1(const T& arg)
    {
        return true; // default case is true
    }

    template<typename T, typename std::enable_if<!alg::is_base_of_v<sfinae_base,T>, int>::type = 0>
    bool sfinae_inheritance_method1(const T& arg)
    {
        return false;
    }

    // *** Using concept *** //
    template<typename T>
    bool sfinae_inheritance_method2(const T& arg) 
    {
        return false; // default case is false
    }

    template<std::derived_from<sfinae_base> T>
    bool sfinae_inheritance_method2(const T& arg)
    {
        return true;
    }

    // comparator
    template<typename CMP> 
    struct comparator_traits {}; 
    template<typename T> 
    struct comparator_traits<std::less<T>> { using opposite_type = std::greater<T>; }; 
    template<typename T> 
    struct comparator_traits<std::greater<T>> { using opposite_type = std::less<T>; }; 
}



