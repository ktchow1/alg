#include<iostream>
#include<cassert>
#include<type_traits>
#include<optional_ref.h>
#include<utility.h>


struct A
{
    int m_x;
    int m_y;
    int m_z;
};

bool operator==(const A& lhs, const A& rhs) 
{
    return lhs.m_x == rhs.m_x &&
           lhs.m_y == rhs.m_y &&
           lhs.m_z == rhs.m_z;
}

struct B : public A
{
};


// ***************** //
// *** Reference *** //
// ***************** //
//
// ******************************************************** //
// 1st argument of fct_for_reference() can bind to either :
// * const reference_wrapper<const A> ...  or
// * const reference_wrapper<A> 
// 
// When it binds to the latter, a temporary instance of
// reference_wrapper<const A> is created, which is rvalue.
// 
// reference_wrapper can be
// * std::reference_wrapper or
// * alg::reference_wrapper
// ******************************************************** //
//
template<template<typename> typename reference_wrapper>
void fct_for_reference(const reference_wrapper<const A>& cr, const A& a, bool equal)
{
    if (equal)  assert(&cr.get() == &a);
    else        assert(&cr.get() != &a);
}


template<template<typename> typename reference_wrapper,
         reference_wrapper<      A>(* ref)(      A&),
         reference_wrapper<const A>(*cref)(const A&)>
void test_reference(const std::string& test_name)
{
    A a0(10,11,12);
    A& a1(a0);

    // 1a. construct reference_wrapper from A
//  reference_wrapper<A> r;                // compile error : cannot bind to nothing
//  reference_wrapper<A> r(A{10,11,12});   // compile error : cannot bind to rvalue
//  reference_wrapper<A> r(std::move(a0)); // compile error : cannot bind to rvalue
    reference_wrapper<A> r0(a0); 
    reference_wrapper<A> r1(a1); 
    auto r2 = ref(a0);
    auto cr = cref(a0);

//  static_assert(std::is_same_v<decltype(r2), reference_wrapper<A>>,       "failed to test reference_wrapper");
//  static_assert(std::is_same_v<decltype(cr), reference_wrapper<const A>>, "failed to test reference_wrapper");
    static_assert(std::is_same_v<typename decltype(r2)::type, A>,           "failed to test reference_wrapper");
    static_assert(std::is_same_v<typename decltype(cr)::type, const A>,     "failed to test reference_wrapper");
    assert(&r0.get() == &a0);
    assert(&r1.get() == &a0);
    assert(&r2.get() == &a0);
    assert(&cr.get() == &a0);
    assert(r0.get().m_x == a0.m_x);
    assert(r0.get().m_y == a0.m_y);
    assert(r0.get().m_z == a0.m_z);

    // 1b. modify content
    r0.get().m_x = 20;
    r0.get().m_y = 21;
    r0.get().m_z = 22;
//  cr.get().m_x = 30; // compile error
    assert(r1.get().m_x == a0.m_x);
    assert(r1.get().m_y == a0.m_y);
    assert(r1.get().m_z == a0.m_z);
    assert(r2.get().m_x == 20);
    assert(r2.get().m_y == 21);
    assert(r2.get().m_z == 22);

    // 2a. construct A from reference_wrapper 
    A a2(r0);
    assert(&r0.get() != &a2);
    assert(&r1.get() != &a2);
    assert(&r2.get() != &a2);
    assert(&cr.get() != &a2);
    assert(a2.m_x == a0.m_x);
    assert(a2.m_y == a0.m_y);
    assert(a2.m_z == a0.m_z);

    // 2b. construct A& from reference_wrapper 
    A& a3(r0);  
    assert(&r0.get() == &a3);
    assert(&r1.get() == &a3);
    assert(&r2.get() == &a3);
    assert(&cr.get() == &a3);
    assert(a3.m_x == a0.m_x);
    assert(a3.m_y == a0.m_y);
    assert(a3.m_z == a0.m_z);

    // 3. construct reference_wrapper from reference_wrapper
    reference_wrapper<A> r3{r0};
    reference_wrapper<A> r4{std::move(r0)}; // move construction becomes copy construction
    assert(&r0.get() == &a0);               // hence ... r0 is still valid
    assert(&r1.get() == &a0); 
    assert(&r2.get() == &a0);
    assert(&r3.get() == &a0);
    assert(&r4.get() == &a0);
    assert(&cr.get() == &a0);

    // 4a. used in vector
//  std::vector<A&> vec; // compile error
    std::vector<reference_wrapper<A>> vec;
    vec.push_back(r0);
    vec.push_back(r1);
    vec.push_back(r2);
    vec.push_back(r3);
    vec.push_back(r4);

    // 4b. used in function
    for(const auto& x:vec) 
    {
        fct_for_reference<reference_wrapper>(x, a0, true);
        fct_for_reference<reference_wrapper>(x, a1, true);
        fct_for_reference<reference_wrapper>(x, a2, false);
        fct_for_reference<reference_wrapper>(x, a3, true); 
    }

    // 5. derived class
    B b({90,91,91});
    reference_wrapper<A> rb0(b);
    reference_wrapper<B> rb1(b);
    assert(&rb0.get() == &b);
    assert(&rb1.get() == &b);
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
        oa4 = std::make_optional<A>(40,41,42);  
    }
    if constexpr (std::is_same_v<optional<A>, alg::optional<A>>)
    {
        oa4 = alg::make_optional<A>(40,41,42);  
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


void test_optional_ref()
{
    test_reference<std::reference_wrapper, std::ref, std::cref>   ("std::reference");
    test_optional<std::optional, std_nullopt>                     ("std::optional");
    test_optional_reference<std::reference_wrapper, std::optional>("std::optional of std::reference");

    test_reference<alg::reference_wrapper, alg::ref, alg::cref>   ("alg::reference");
    test_optional<alg::optional, alg_nullopt>                     ("alg::optional");
//  test_optional_reference<alg::reference_wrapper, alg::optional>("alg::optioanl of alg::reference"); <--- Todo


    // The failure in the last case above, is likely, to be solved 
    // by changing implementation of optional from value into ptr.
}

