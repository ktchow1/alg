#include<iostream>
#include<cassert>
#include<memory>
#include<utility.h>


namespace test_shared
{
    std::uint32_t invoked = 0;

    struct A            { virtual void f() { invoked = 11; }    void a_fct() { invoked = 21; } };
    struct B : public A { virtual void f() { invoked = 12; }    void b_fct() { invoked = 22; } };
    struct C : public A { virtual void f() { invoked = 13; }    void c_fct() { invoked = 23; } };
    struct D : public A { virtual void f() { invoked = 14; }    void d_fct() { invoked = 24; } };
}

void test_memory_shared_ptr()
{
    // *********************************** //
    // *** dynamic cast for shared_ptr *** //
    // *********************************** //
    std::shared_ptr<test_shared::A> pA = std::make_shared<test_shared::A>();
    std::shared_ptr<test_shared::A> pB = std::make_shared<test_shared::B>();
    std::shared_ptr<test_shared::A> pC = std::make_shared<test_shared::C>();
    std::shared_ptr<test_shared::A> pD = std::make_shared<test_shared::D>();

    pA->f();       assert(test_shared::invoked == 11);
    pB->f();       assert(test_shared::invoked == 12);
    pC->f();       assert(test_shared::invoked == 13);
    pD->f();       assert(test_shared::invoked == 14);
    pA->a_fct();   assert(test_shared::invoked == 21);
    pB->a_fct();   assert(test_shared::invoked == 21);
    pC->a_fct();   assert(test_shared::invoked == 21);
    pD->a_fct();   assert(test_shared::invoked == 21);

    // Difference between :
    // *  static_pointer_cast
    // * dynamic_pointer_cast
    //
    // - both can cast base pointer to derived pointer
    // - if the cast is invalid,  static_pointer_cast may lead to crash
    // - if the cast is invalid, dynamic_pointer_cast will check and throw
    // - hence static_pointer_cast is faster (for low latency)
    //

    std::static_pointer_cast<test_shared::B>(pB)->b_fct();
    assert(test_shared::invoked == 22);
    std::static_pointer_cast<test_shared::C>(pC)->c_fct();
    assert(test_shared::invoked == 23);
    std::static_pointer_cast<test_shared::D>(pD)->d_fct();
    assert(test_shared::invoked == 24);

    // The following will crash straight away.
//  std::static_pointer_cast<test_shared::C>(pB)->c_fct();
//  std::static_pointer_cast<test_shared::D>(pC)->d_fct();
//  std::static_pointer_cast<test_shared::B>(pD)->b_fct();
    
    std::dynamic_pointer_cast<test_shared::B>(pB)->b_fct();
    assert(test_shared::invoked == 22);
    std::dynamic_pointer_cast<test_shared::C>(pC)->c_fct();
    assert(test_shared::invoked == 23);
    std::dynamic_pointer_cast<test_shared::D>(pD)->d_fct();
    assert(test_shared::invoked == 24);
  
    // The following will throw.
    try
    {
        std::dynamic_pointer_cast<test_shared::C>(pB)->c_fct();
        std::dynamic_pointer_cast<test_shared::D>(pC)->d_fct();
        std::dynamic_pointer_cast<test_shared::B>(pD)->b_fct();
    }
    catch(std::exception& ex)
    {
        std::cout << ex.what();
    }
    print_summary("shared_ptr - dynamic cast", "succeeded");
}



