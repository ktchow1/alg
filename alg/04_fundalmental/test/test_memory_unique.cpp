#include<iostream>
#include<cassert>
#include<memory>
#include<utility.h>


namespace test
{
    std::uint32_t invoked = 0;

    void f(std::unique_ptr<std::string> up) // accept rvalue only
    {
        invoked = 1;
    }

    void g(std::unique_ptr<std::string>& up) // accept lvalue only
    {
        invoked = 2;
    }
}


void test_memory_unique_ptr()
{
    // ************* //
    // *** Basic *** //
    // ************* //
    {
        auto up0 = std::make_unique<std::string>("abc");
        auto up1 = std::make_unique<std::string>("abc");

        test::f(std::move(up0));
    //  test::f(up0);                 // compile error
        assert(test::invoked == 1);

    //  test::g(std::move(up1));      // compile error
        test::g(up1);
        assert(test::invoked == 2);
    }


    // ***************************************************** //
    // *** Pointing to non copyable / non movable object *** //
    // ***************************************************** //
    struct noncopyable
    {
        noncopyable() = default;
        noncopyable(const noncopyable&)  = delete;
        noncopyable      (noncopyable&&) = default;
        constexpr noncopyable& operator=(const noncopyable&)  = delete;
                  noncopyable& operator=      (noncopyable&&) = default; // Compile error : default move operator should return non-const&
    };

    struct nonmovable
    {
        nonmovable() = default;
        nonmovable(const nonmovable&)  = default;
        nonmovable      (nonmovable&&) = delete;
        constexpr nonmovable& operator=(const nonmovable&)  = default;  // Compile error : default copy operator should return constexpr&
                  nonmovable& operator=      (nonmovable&&) = delete;
    };

    {
        auto sp0 = std::make_unique<noncopyable>();
        auto sp1 = std::make_unique<nonmovable>();
    //  auto sp2 = sp0;                                                 // Compile error : copy operator of unique_ptr is deleted
        auto sp3 = std::move(sp0);
    //  auto sp4 = sp1;                                                 // Compile error : copy operator of unique_ptr is deleted
        auto sp5 = std::move(sp1);
    } 


    // ************************* //
    // *** Pointing to array *** //
    // ************************* //
    struct A
    {
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    // We can consider :
    // std::unique_ptr<T>   is a smart pointer for : T* ptr = new T();
    // std::unique_ptr<T[]> is a smart pointer for : T* ptr = new T[100];
    //
    // In fact, both are T* under the hood.
    // 1. unique_ptr<A[]> is     A*
    // 2. unique_ptr<A[]> is not A[]
    //
    // There is operator*  for the former (but not the latter).
    // There is operator[] for the latter (but not the former).
    //
    static_assert(! std::is_same<A*,  A[]                                                   >::value); // false
    static_assert(  std::is_same<A*,  decltype(  std::declval<std::unique_ptr<A  >>().get())>::value); //  true
    static_assert(  std::is_same<A&,  decltype(*(std::declval<std::unique_ptr<A  >>()     ))>::value); //  true
    static_assert(  std::is_same<A*,  decltype(  std::declval<std::unique_ptr<A[]>>().get())>::value); //  true
    static_assert(! std::is_same<A**, decltype(  std::declval<std::unique_ptr<A[]>>().get())>::value); // false
    static_assert(! std::is_same<A[], decltype(  std::declval<std::unique_ptr<A[]>>().get())>::value); // false
    static_assert(  std::is_same<A&,  decltype( (std::declval<std::unique_ptr<A[]>>()[0]  ))>::value); //  true

    std::unique_ptr<A>   a (new A(11,12,13));
    std::unique_ptr<A[]> as(new A[5]{{11,12,13},
                                     {21,22,23}, 
                                     {31,32,33},
                                     {41,42,43},
                                     {51,52,53}});

    assert(as[0].m_x == 11 && as[0].m_y == 12 && as[0].m_z == 13);
    assert(as[1].m_x == 21 && as[1].m_y == 22 && as[1].m_z == 23);
    assert(as[2].m_x == 31 && as[2].m_y == 32 && as[2].m_z == 33);
    assert(as[3].m_x == 41 && as[3].m_y == 42 && as[3].m_z == 43);
    assert(as[4].m_x == 51 && as[4].m_y == 52 && as[4].m_z == 53);
    print_summary("unique_ptr", "succeeded");
}



