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
    typename T,                                       // T must support construction from 3 integers
    typename DT,                                      // DT is derived class of T
    template<typename> typename reference_wrapper,    // reference wrapper under test
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
        auto rx2 =  ref(x);
        auto ry2 = cref(y);

        static_assert(std::is_same_v<decltype(rx2), reference_wrapper<T>>,       "failed to test reference_wrapper");
        static_assert(std::is_same_v<decltype(ry2), reference_wrapper<const T>>, "failed to test reference_wrapper");
        static_assert(std::is_same_v<typename decltype(rx2)::type, T>,           "failed to test reference_wrapper");
        static_assert(std::is_same_v<typename decltype(ry2)::type, const T>,     "failed to test reference_wrapper");
        assert(&rx .get() == &x);
        assert(&ry .get() == &x);
        assert(&rx2.get() == &x);
        assert(&ry2.get() == &x);
    }


    // ************************************************ //
    // *** Construct T or T& from reference_wrapper *** //
    // ************************************************ //
    if constexpr (std::is_copy_constructible_v<T>)
    {
        T x2(rx);
        T y2(ry);
        assert(&x2 != &rx.get());
        assert(&y2 != &ry.get());
        assert(x2 == x);
        assert(y2 == x);
    }
/*
    // 2b. construct T& from reference_wrapper 
    T& z(rx0);  
    assert(&rx0.get() == &z);
    assert(&rx1.get() == &z);
    assert(&rx2.get() == &z);
    assert(&crx.get() == &z);
    assert(z.m_x == x.m_x);
    assert(z.m_y == x.m_y);
    assert(z.m_z == x.m_z);


    // ********************************************************** //
    // *** Construct reference_wrapper from reference_wrapper *** //
    // ********************************************************** //
    reference_wrapper<T> rx3{rx0};
    reference_wrapper<T> rx4{std::move(rx0)}; // move construction becomes copy construction
    assert(&rx0.get() == &x);                // hence ... rx0 is still valid
    assert(&rx1.get() == &x); 
    assert(&rx2.get() == &x);
    assert(&rx3.get() == &x);
    assert(&rx4.get() == &x);
    assert(&crx.get() == &x);


    // ************************* //
    // *** Modify and rebind *** //
    // ************************* //
    rx0.get().m_x = 20;
    rx0.get().m_y = 21;
    rx0.get().m_z = 22;
 // crx.get().m_x = 30; // compile error
    assert(rx1.get().m_x == x.m_x);
    assert(rx1.get().m_y == x.m_y);
    assert(rx1.get().m_z == x.m_z);
    assert(rx2.get().m_x == 20);
    assert(rx2.get().m_y == 21);
    assert(rx2.get().m_z == 22);


    // *********************** //
    // *** Usage in vector *** //
    // *********************** //
//  std::vector<T&> vec; // compile error, this is why we have std::reference_wrapper
    std::vector<reference_wrapper<T>> vec;
    vec.push_back(rx0);
    vec.push_back(rx1);
    vec.push_back(rx2);
    vec.push_back(rx3);
    vec.push_back(rx4);

    // 4b. used in function
    for(const auto& w:vec) 
    {
        assert((compare_address<T, reference_wrapper>(w, x)));
        assert((compare_address<T, reference_wrapper>(w, y)));
        assert((compare_address<T, reference_wrapper>(w, z))); 
    }


    // **************************** //
    // *** Usage in inheritance *** //
    // **************************** //
    DT dx({90,91,91});
    reference_wrapper <T>  rx(dx);
    reference_wrapper<DT> rdx(dx);
    assert( &rx.get() == &dx);
    assert(&rdx.get() == &dx); */
    print_summary(test_name, "succeeded");
}


// **************** //
// *** Optional *** //
// **************** //
template<typename T>
requires std::same_as<T,std::optional<A>> || 
         std::same_as<T,alg::optional<A>>
void fct_for_optional(const T& oa, std::uint32_t count)
{
    if      (count == 0) assert(!oa);
    else if (count == 1) assert(oa && oa->m_x == 50 && oa->m_y == 51 && oa->m_z == 52);
    else if (count == 2) assert(oa && oa->m_x == 20 && oa->m_y == 21 && oa->m_z == 22);
    else if (count == 3) assert(oa && oa->m_x == 30 && oa->m_y == 31 && oa->m_z == 32);
    else if (count == 4) assert(oa && oa->m_x == 40 && oa->m_y == 41 && oa->m_z == 42);
    else if (count == 5) assert(oa && oa->m_x == 50 && oa->m_y == 51 && oa->m_z == 52);
    else if (count == 6) assert(oa && oa->m_x == 50 && oa->m_y == 51 && oa->m_z == 52);
}


template<template<typename> typename optional, typename nullopt>
void test_optional(const std::string& test_name)
{
    A a(10,11,12);
    A& ra = a;

    // 1a. construct optional from A and from A&
    optional<A> oa0;                                // default initialization
    optional<A> oa1(ra);                            // direct initialization
    optional<A> oa2(A{20,21,22});                   // direct initializtion
    optional<A> oa3 = A{30,31,32};                  // copy initialization
    optional<A> oa4;                                // factory
    if constexpr (std::is_same_v<optional<A>, std::optional<A>>)
    {
        oa4 = std::make_optional<A>(40_u32,41_u32,42_u32);  
    }
    if constexpr (std::is_same_v<optional<A>, alg::optional<A>>)
    {
        oa4 = alg::make_optional<A>(40_u32,41_u32,42_u32);  
    }

    assert(oa0 == nullopt::value);
    assert(!oa0);
    oa0 = a;
    assert(oa0 != nullopt::value);
    assert(oa0->m_x == 10 && oa0->m_y == 11 && oa0->m_z == 12);
    assert(oa1->m_x == 10 && oa1->m_y == 11 && oa1->m_z == 12);
    assert(oa2->m_x == 20 && oa2->m_y == 21 && oa2->m_z == 22);
    assert(oa3->m_x == 30 && oa3->m_y == 31 && oa3->m_z == 32);
    assert(oa4->m_x == 40 && oa4->m_y == 41 && oa4->m_z == 42);
    assert(&(*oa0) != &(*oa1)); 
                                
    // 1b. modify content
    oa0 = A{50,51,52};
    oa1 = oa0;
    assert(oa0->m_x == 50 && oa0->m_y == 51 && oa0->m_z == 52);
    assert(oa1->m_x == 50 && oa1->m_y == 51 && oa1->m_z == 52);
    
    oa0 = nullopt::value;
    assert(oa0 == nullopt::value);
    assert(!oa0);
  
    // 2a. construct A from optional 
//  A a1(oa1);   // compile error

    A a1(*oa1);
    assert(&a1 != &(*oa1));

    // 2b. construct A& from optional 
    A& a2(*oa1);  
    assert(&a2 == &(*oa1));

    // 3. construct optional from optional
    optional<A> oa5{oa1};
    optional<A> oa6{std::move(oa1)};
    assert(oa1); // still valid
    assert(oa1->m_x == 50 && oa1->m_y == 51 && oa1->m_z == 52);
    assert(oa2->m_x == 20 && oa2->m_y == 21 && oa2->m_z == 22);
    assert(oa3->m_x == 30 && oa3->m_y == 31 && oa3->m_z == 32);
    assert(oa4->m_x == 40 && oa4->m_y == 41 && oa4->m_z == 42);
    assert(oa5->m_x == 50 && oa5->m_y == 51 && oa5->m_z == 52);
    assert(oa6->m_x == 50 && oa6->m_y == 51 && oa6->m_z == 52);
    assert(&(*oa5) != &(*oa1));
    assert(&(*oa6) != &(*oa1));

    // 4a. used in vector
    std::vector<optional<A>> vec;
    vec.push_back(oa0);
    vec.push_back(oa1);
    vec.push_back(oa2);
    vec.push_back(oa3);
    vec.push_back(oa4);
    vec.push_back(oa5);
    vec.push_back(oa6);

    // 4b. used in function
    std::uint32_t count = 0;
    for(const auto& x:vec) 
    {
        fct_for_optional(x, count);
        ++count;
    }
    print_summary(test_name, "succeeded");
}


// ************************** // 
// *** Optional reference *** //
// ************************** // 
template<template<typename> typename reference_wrapper,
         template<typename> typename optional>
void test_optional_reference(const std::string& test_name) 
{
//  optional<A&> ora; // compile error
    optional<reference_wrapper<A>> ora;

    // 1. can be null
    assert(!ora);

    A a0{10,11,12};
    ora = reference_wrapper<A>(a0);
    assert(ora);
    assert(ora->get().m_x == 10 && ora->get().m_y == 11 && ora->get().m_z == 12);

    // 2. can be re-assigned
    A a1{20,21,22};
    ora = reference_wrapper<A>(a1);
    assert(ora);
    assert(ora->get().m_x == 20 && ora->get().m_y == 21 && ora->get().m_z == 22);

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

//  test_optional<std::optional, std_nullopt>("std::optional");
//  test_optional<alg::optional, alg_nullopt>("alg::optional");
//  test_optional_reference<std::reference_wrapper, std::optional>("std::optional of std::reference");
//  test_optional_reference<alg::reference_wrapper, alg::optional>("alg::optioanl of alg::reference"); <--- Todo

    // The failure in the last case above, is likely, to be solved 
    // by changing implementation of optional from value into ptr.
}

