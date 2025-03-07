// ******************************************************************* //
// "04_fundalmental/test_optional.cpp" tests std::optional only, while
// "05_template/test_optional_ref.cpp" tests alg::optional
// ******************************************************************* //
#include<iostream>
#include<cstdint>
#include<cassert>
#include<optional>
#include<utility.h>


namespace test
{
    struct A
    {
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };
}


void test_optional()
{
    // ******************************* //
    // *** Usage of std::reference *** //
    // ******************************* //
    test::A a{11,22,33};
    auto  ra = std::ref(a);  
    auto  a0 = ra.get();
    auto& a1 = static_cast<test::A&>(ra); // There is conversion operator in std::reference_wrapper.

    assert((std::is_same_v<decltype(ra), std::reference_wrapper<test::A>>));
    assert((std::is_same_v<decltype(a0), test::A>));
    assert((std::is_same_v<decltype(a1), test::A&>)); 
    assert(ra.get().m_x == 11);
    assert(ra.get().m_y == 22);
    assert(ra.get().m_z == 33);
    assert(a0.m_x == 11);
    assert(a0.m_y == 22);
    assert(a0.m_z == 33);
    assert(a1.m_x == 11);
    assert(a1.m_y == 22);
    assert(a1.m_z == 33);
    assert(&ra.get().m_x == &a.m_x);
    assert(&ra.get().m_y == &a.m_y);
    assert(&ra.get().m_z == &a.m_z);
    assert(&a0.m_x != &a.m_x);
    assert(&a0.m_y != &a.m_y);
    assert(&a0.m_z != &a.m_z);
    assert(&a1.m_x == &a.m_x);
    assert(&a1.m_y == &a.m_y);
    assert(&a1.m_z == &a.m_z);


    // ****************************** //
    // *** Usage of std::optional *** //
    // ****************************** //  
    std::optional<test::A> oa0;
    std::optional<test::A> oa1(test::A{11,22,33});
    std::optional<test::A> oa2 = test::A{11,22,33};
    auto oa3 = std::make_optional<test::A>({11,22,33});

    assert(oa0 == std::nullopt);
    assert(oa1->m_x == 11);
    assert(oa1->m_y == 22);
    assert(oa1->m_z == 33);
    assert(oa2->m_x == 11);
    assert(oa2->m_y == 22);
    assert(oa2->m_z == 33);
    assert(oa3->m_x == 11);
    assert(oa3->m_y == 22);
    assert(oa3->m_z == 33);


    // **************************************** //
    // *** Usage of std::optional reference *** //
    // **************************************** //  
//  std::optional<test::A&> ora; // compile error
    std::optional<std::reference_wrapper<test::A>> ora0;
    std::optional<std::reference_wrapper<test::A>> ora1(std::ref(a));
    std::optional<std::reference_wrapper<test::A>> ora2 = std::ref(a); 
    auto ora3 = std::make_optional<std::reference_wrapper<test::A>>(std::ref(a)); 

    assert(ora0 == std::nullopt);
    assert(&(ora1->get().m_x) == &a.m_x);
    assert(&(ora1->get().m_y) == &a.m_y);
    assert(&(ora1->get().m_z) == &a.m_z);
    assert(&(ora2->get().m_x) == &a.m_x);
    assert(&(ora2->get().m_y) == &a.m_y);
    assert(&(ora2->get().m_z) == &a.m_z);
    assert(&(ora3->get().m_x) == &a.m_x);
    assert(&(ora3->get().m_y) == &a.m_y);
    assert(&(ora3->get().m_z) == &a.m_z);
    print_summary("optional", "succeeded");
}

