#include<iostream>
#include<cassert>
#include<type_traits>
#include<optional_ref.h>
#include<literal.h>
#include<utility.h>



struct A // default constructible
{
    std::uint32_t m_x;
    std::uint32_t m_y;
    std::uint32_t m_z;
};

struct B // non default constructible
{
    B(std::uint32_t x, std::uint32_t y, std::uint32_t z) : m_x(x), m_y(y), m_z(z) {}

    std::uint32_t m_x;
    std::uint32_t m_y;
    std::uint32_t m_z;
};

struct C // non copyable, but assignable
{
    C() = default;                                                                    // add this to make it default-constructible
    C(std::uint32_t x, std::uint32_t y, std::uint32_t z) : m_x(x), m_y(y), m_z(z) {}  // add this to make it         constructible
    C(const C&) = delete;

    std::uint32_t m_x;
    std::uint32_t m_y;
    std::uint32_t m_z;
};

struct D // copyable, but non assignable
{
    D& operator=(const D&) = delete;

    std::uint32_t m_x;
    std::uint32_t m_y;
    std::uint32_t m_z;
};

struct DA : public A {};
struct DB : public B {};
struct DC : public C {};
struct DD : public D {};



template<typename T> 
bool operator==(const T& lhs, const T& rhs) // T is a tri-integers POD
{
    return lhs.m_x == rhs.m_x &&
           lhs.m_y == rhs.m_y &&
           lhs.m_z == rhs.m_z;
}

template<typename T, template<typename> typename reference_wrapper>
bool compare_address(const reference_wrapper<const T>& rx, const T& x)
{
    return &rx.get() == &x;
}



// ************************ //
// *** Constructibility *** //
// ************************ //
// Unfortunately, there is no std traits to check re-assignability.
//
void test_constructibility()
{
    static_assert(std::is_default_constructible_v<A*>                                       ==  true, "constructiblility is wrong");
    static_assert(std::is_default_constructible_v<A&>                                       == false, "constructiblility is wrong");
    static_assert(std::is_default_constructible_v<std::reference_wrapper<A>>                == false, "constructiblility is wrong");
    static_assert(std::is_default_constructible_v<std::optional<A>>                         ==  true, "constructiblility is wrong");
    static_assert(std::is_default_constructible_v<std::optional<std::reference_wrapper<A>>> ==  true, "constructiblility is wrong");
//  static_assert(std::is_default_constructible_v<std::optional<A&>>                        == false, "CANNOT COMPILE");

    static_assert(std::is_copy_constructible_v<A*>                                       ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_constructible_v<A&>                                       ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_constructible_v<std::reference_wrapper<A>>                ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_constructible_v<std::optional<A>>                         ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_constructible_v<std::optional<std::reference_wrapper<A>>> ==  true, "constructiblility is wrong");
//  static_assert(std::is_copy_constructible_v<std::optional<A&>>                        == false, "CANNOT COMPILE");
  
    static_assert(std::is_copy_assignable_v<A*>                                       ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_assignable_v<A&>                                       ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_assignable_v<std::reference_wrapper<A>>                ==  true, "constructiblility is wrong"); 
    static_assert(std::is_copy_assignable_v<std::optional<A>>                         ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_assignable_v<std::optional<std::reference_wrapper<A>>> ==  true, "constructiblility is wrong");
//  static_assert(std::is_copy_assignable_v<std::optional<A&>>                        == false, "CANNOT COMPILE");

    static_assert(std::is_default_constructible_v<std::vector<A*>>                                       ==  true, "constructiblility is wrong");
//  static_assert(std::is_default_constructible_v<std::vector<A&>>                                       == false, "CANNOT COMPILE");
    static_assert(std::is_default_constructible_v<std::vector<std::reference_wrapper<A>>>                ==  true, "constructiblility is wrong");
    static_assert(std::is_default_constructible_v<std::vector<std::optional<A>>>                         ==  true, "constructiblility is wrong");
    static_assert(std::is_default_constructible_v<std::vector<std::optional<std::reference_wrapper<A>>>> ==  true, "constructiblility is wrong");

    static_assert(std::is_default_constructible_v<A> ==  true, "constructiblility is wrong");
    static_assert(std::is_default_constructible_v<B> == false, "constructiblility is wrong");
    static_assert(std::is_default_constructible_v<C> ==  true, "constructiblility is wrong");
    static_assert(std::is_default_constructible_v<D> ==  true, "constructiblility is wrong");

    static_assert(std::is_copy_constructible_v<A> ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_constructible_v<B> ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_constructible_v<C> == false, "constructiblility is wrong");
    static_assert(std::is_copy_constructible_v<D> ==  true, "constructiblility is wrong");
 
    static_assert(std::is_copy_assignable_v<A> ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_assignable_v<B> ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_assignable_v<C> ==  true, "constructiblility is wrong");
    static_assert(std::is_copy_assignable_v<D> == false, "constructiblility is wrong");
    print_summary("std::reference constructiblility", "succeeded");
}



// ************************* //
// *** Reference wrapper *** //
// ************************* //
template
<
    typename  T,                                      //  T is constructed from 3 integers
    typename DT,                                      // DT is derived class of T
    template<typename> typename reference_wrapper,    // reference wrapper 
    reference_wrapper<      T>(* ref)(      T&),      // factory 
    reference_wrapper<const T>(*cref)(const T&)       // factory
>
void test_reference(const std::string& test_name)
{
    T  x(10,11,12);
    T& y(x);


    // ********************** // 
    // *** Cannot compile *** //
    // ********************** // 
    {   
    //  reference_wrapper<T> rx;                 // cannot bind to null
    //  reference_wrapper<T> rx(T{10,11,12});    // cannot bind to prvalue
    //  reference_wrapper<T> rx(std::move(x0));  // cannot bind to xvalue
    }
    

    // ************************************************************** //
    // *** Construct reference_wrapper from T or T& (and factory) *** //
    // ************************************************************** //
    reference_wrapper<T> rx(x); 
    reference_wrapper<T> ry(y); 
    {
        auto rx0 =  ref(x);
        auto ry0 = cref(y);

        static_assert(std::is_same_v<decltype(rx0), reference_wrapper<T>>,       "failed to test reference_wrapper");
        static_assert(std::is_same_v<decltype(ry0), reference_wrapper<const T>>, "failed to test reference_wrapper");
        static_assert(std::is_same_v<typename decltype(rx0)::type, T>,           "failed to test reference_wrapper");
        static_assert(std::is_same_v<typename decltype(ry0)::type, const T>,     "failed to test reference_wrapper");
        assert(&rx .get() == &x);
        assert(&ry .get() == &x);
        assert(&rx0.get() == &x);
        assert(&ry0.get() == &x);
    }


    // ************************************************ //
    // *** Construct T or T& from reference_wrapper *** //
    // ************************************************ //
    if constexpr (std::is_copy_constructible_v<T>)
    {
        T x0(rx);
        T y0(ry);

        assert(&x0 != &rx.get());
        assert(&y0 != &ry.get());
        assert( x0 == x);
        assert( y0 == x);
    }
    // for both copy-constructible or non-copy-constructible
    {
        T& x0(rx);  
        T& y0(ry);  

        assert(&x0 == &rx.get());
        assert(&y0 == &ry.get());
        assert(&x0 == &x);
        assert(&y0 == &x);
        assert( x0 ==  x);
        assert( y0 ==  x);
    }

  
    // ********************************************************** //
    // *** Construct reference_wrapper from reference_wrapper *** //
    // ********************************************************** //
    {
        reference_wrapper<T> rx0{rx};
        assert(&rx0.get() == &x);

        reference_wrapper<T> rx1{std::move(rx0)};
        assert(&rx1.get() == &x);
    }


    // ************** //
    // *** Modify *** //
    // ************** //
    {
        auto rx0 =  ref(x);
        auto ry0 = cref(y);
        rx0.get().m_x = 20;
        rx0.get().m_y = 21;
        rx0.get().m_z = 22;
    //  ry0.get().m_x = 30; // compile error
      
        assert((rx0.get() == T{20,21,22}));
        assert((ry0.get() == T{20,21,22}));
        assert((x == T{20,21,22}));
        assert((y == T{20,21,22}));
    }

    // ************** //
    // *** Rebind *** //
    // ************** //
    {
        auto rx0 =  ref(x);
        auto ry0 = cref(y);
        assert(&rx0.get() == &x);
        assert(&ry0.get() == &x);

        T z{90,91,92};
        rx0 =  ref(z);
        ry0 = cref(z);
        assert(&rx0.get() != &x);
        assert(&ry0.get() != &x);
        assert(&rx0.get() == &z);
        assert(&ry0.get() == &z);
    }

  
    // *********************** //
    // *** Usage in vector *** //
    // *********************** //
    {
    //  std::vector<T&> vec;                             // compile error : cannot construct vector of T&
        std::vector<reference_wrapper<T>> vec;           // can rebind reference, can    modify value   
        std::vector<reference_wrapper<const T>> cvec;    // can rebind reference, cannot modify value

        auto rx0 = ref(x);
        vec.push_back(rx);
        vec.push_back(rx0);

        auto ry0 = cref(y);
        cvec.push_back(ry);
        cvec.push_back(ry0);
    
        for(const auto& temp:vec) 
        {
            static_assert(std::is_same_v<decltype(temp), const reference_wrapper<T>&>, "failed to test reference_wrapper");
            assert((compare_address<T, reference_wrapper>(temp, x))); // pass reference to function
        }
        for(const auto& temp:cvec) 
        {
            static_assert(std::is_same_v<decltype(temp), const reference_wrapper<const T>&>, "failed to test reference_wrapper");
            assert((compare_address<T, reference_wrapper>(temp, x)));
        }

        // *** Rebind *** //
        T z{90,91,92};
        for(auto& temp:vec) 
        {
            temp = ref(z);
        }
        for(const auto& temp:vec) 
        {
            assert((!compare_address<T, reference_wrapper>(temp, x)));
            assert(( compare_address<T, reference_wrapper>(temp, z)));
        }
    }


    // **************************** //
    // *** Usage in inheritance *** //
    // **************************** //
    {
        DT z({90,91,91});
        reference_wrapper <T> rz0(z);
        reference_wrapper<DT> rz1(z);

        assert(&rz0.get() == &z);
        assert(&rz1.get() == &z); 
    }
    print_summary(test_name, "succeeded");
}



// **************** //
// *** Optional *** //
// **************** //
template
<
    typename T,
    template<typename> typename optional, 
    typename nullopt
>
void test_optional(const std::string& test_name)
{
    T  x(10,11,12);
    T& y = x;


    // ***************************************************** //
    // *** Construct optional from T or T& (and factory) *** //
    // ***************************************************** //
    {
        optional<T> ox0;                  // default initialization
        optional<T> ox1(y);               // direct initialization from lvalue
        optional<T> ox2(T{20,21,22});     // direct initialization from rvalue
        optional<T> ox3 = T{30,31,32};    // copy initialization
        optional<T> ox4;                  // factory

        if constexpr (std::is_same_v<optional<T>, std::optional<T>>)
        {
            ox4 = std::make_optional<T>(40_u32,41_u32,42_u32);  
        }
        if constexpr (std::is_same_v<optional<T>, alg::optional<T>>)
        {
            ox4 = alg::make_optional<T>(40_u32,41_u32,42_u32);  
        } 

        assert(!ox0);
        assert(ox1);
        assert(ox2);
        assert(ox3);
        assert(ox4);
        assert(ox0 == nullopt::value);
        assert((*ox1 == T{10,11,12}));
        assert((*ox2 == T{20,21,22}));
        assert((*ox3 == T{30,31,32}));
        assert((*ox4 == T{40,41,42}));
        assert(&(*ox1) != &x);            // optional owns a separate instance
        assert(&(*ox1) != &y); 
    }
    

    // *************************************** //
    // *** Construct T or T& from optional *** //
    // *************************************** //
    {
        optional<T> ox(x);
        T  x0 = *ox;
        T& x1 = *ox;

        assert(&x0 != &x);
        assert(&x1 != &x);
        assert(&x0 != &(*ox));
        assert(&x1 == &(*ox));
        assert((x0 == T{10,11,12}));
        assert((x1 == T{10,11,12}));
    }


    // **************************************** //
    // *** Construct optional from optional *** //
    // **************************************** //
    {
        optional<T> ox(x);
        optional<T> ox0{ox};
        optional<T> ox1{std::move(ox)};

        assert(ox0);
        assert(ox1);
        assert(&(*ox0) != &x);
        assert(&(*ox1) != &x);
        assert((*ox0 == T{10,11,12}));
        assert((*ox1 == T{10,11,12}));
    }


    // ************** //
    // *** Modify *** //
    // ************** //
    {
        optional<T> ox(x);
        auto old_address = &(*ox);
        ox->m_x = 50;
        ox->m_y = 51;
        ox->m_z = 52; 

        assert((  x == T{10,11,12})); 
        assert((*ox == T{50,51,52}));
        assert((&(*ox) == old_address));
    }


    // ************** //
    // *** Rebind *** //
    // ************** //
    {
        optional<T> ox(x);
        auto old_address = &(*ox);
        ox = T{50,51,52};

        assert((  x == T{10,11,12})); 
        assert((*ox == T{50,51,52}));
        assert((&(*ox) == old_address)); // unchanged, since it is not implemented as T*

        ox = nullopt::value;
        assert(ox == nullopt::value);
        assert(!ox);
    }
        

    // *********************** //
    // *** Usage in vector *** //
    // *********************** //
    std::vector<optional<T>> vec;
    for(std::uint32_t n=0; n!=5; ++n)
    {
        if constexpr (std::is_same_v<optional<T>, std::optional<T>>)
        {
            vec.push_back(std::make_optional<T>(10*(n+1), 10*(n+1)+1, 10*(n+1)+2));  
        }
        if constexpr (std::is_same_v<optional<T>, alg::optional<T>>)
        {
            vec.push_back(alg::make_optional<T>(10*(n+1), 10*(n+1)+1, 10*(n+1)+2));  
        }
    }

    std::uint32_t temp = 10;
    for(const auto& ox:vec)
    {
        assert((*ox == T{temp, temp+1, temp+2}));
        temp += 10;
    }
    print_summary(test_name, "succeeded");
}



// ************************** // 
// *** Optional reference *** //
// ************************** // 
template
<
    typename T,
    template<typename> typename reference_wrapper,
    template<typename> typename optional,
    typename nullopt
>
void test_optional_reference(const std::string& test_name) 
{
    optional<reference_wrapper<T>> orx;
    T x0{10,11,12};
    T x1{20,21,22};


    // Support nullity
    assert(!orx);

    // Support rebind
    orx = reference_wrapper<T>(x0);
    assert(orx);
    assert(( orx->get() == T{10,11,12}));
    assert((&orx->get() == &x0));

    // Support rebind again
    orx = reference_wrapper<T>(x1);
    assert(orx);
    assert(( orx->get() == T{20,21,22}));
    assert((&orx->get() == &x1));

    // Support modifying value
    orx->get().m_x = 30;
    orx->get().m_y = 31;
    orx->get().m_z = 32;
    assert(orx);
    assert((         x1 == T{30,31,32}));
    assert(( orx->get() == T{30,31,32}));
    assert((&orx->get() == &x1));

    // Support rebind to null
    orx = nullopt::value;
    assert(!orx);

    print_summary(test_name, "succeeded");
}



// ***************** //
// *** Main test *** //
// ***************** //
void test_optional_and_reference()
{
    test_constructibility();
    test_reference<A, DA, std::reference_wrapper, std::ref, std::cref>("std::reference for general class");
    test_reference<A, DA, alg::reference_wrapper, alg::ref, alg::cref>("alg::reference for general class");
    test_reference<B, DB, std::reference_wrapper, std::ref, std::cref>("std::reference for non-default-constructible");
    test_reference<B, DB, alg::reference_wrapper, alg::ref, alg::cref>("alg::reference for non-default-constructible");
    test_reference<C, DC, std::reference_wrapper, std::ref, std::cref>("std::reference for non-copy-constructible");
    test_reference<C, DC, alg::reference_wrapper, alg::ref, alg::cref>("alg::reference for non-copy-constructible");
    test_reference<D, DD, std::reference_wrapper, std::ref, std::cref>("std::reference for non-copy-assignable");
    test_reference<D, DD, alg::reference_wrapper, alg::ref, alg::cref>("alg::reference for non-copy-assignable");

    test_optional<A, std::optional, std_nullopt>("std::optional for general class");
    test_optional<A, alg::optional, alg_nullopt>("alg::optional for general class");
    test_optional<B, std::optional, std_nullopt>("std::optional for non-default-constructible");
    test_optional<B, alg::optional, alg_nullopt>("alg::optional for non-default-constructible");
    // do not support 
    // * non-copy-constructible, like C
    // * non-copy-assignable,    like D

    test_optional_reference<A,std::reference_wrapper, std::optional, std_nullopt>("std::optional of std::reference for general class");
    test_optional_reference<A,alg::reference_wrapper, alg::optional, alg_nullopt>("alg::optioanl of alg::reference for general class"); 
    test_optional_reference<B,std::reference_wrapper, std::optional, std_nullopt>("std::optional of std::reference for non-default-constructible");
    test_optional_reference<B,alg::reference_wrapper, alg::optional, alg_nullopt>("alg::optioanl of alg::reference for non-default-constructible"); 
}

