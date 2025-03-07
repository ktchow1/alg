#include<iostream>
#include<cstdint>
#include<cassert>
#include<memory>
#include<utility.h>


// *************************************************************** //
// *** Priority : conversion constructor > conversion operator *** //
// *************************************************************** //
namespace test0
{
    std::uint32_t function_invoked = 0;

    struct B;
    struct A
    {
        A(std::uint32_t x) : m_a(x) {}
        explicit A(const B& b);
        operator B() const;

        std::uint32_t m_a;
    };

    struct B 
    {
        B(std::string x) : m_b(x) {} 
        explicit B(const A& a);
        operator A() const;

        std::string m_b;
    };
        
    A::A(const B& b) : m_a(std::stoi(b.m_b))
    {
        function_invoked = 1;
    }

    A::operator B() const
    {
        function_invoked = 2;
        return B{std::to_string(m_a)};
    }

    B::B(const A& a) : m_b(std::to_string(a.m_a))
    {
        function_invoked = 3;
    }

    B::operator A() const
    {
        function_invoked = 4;
        return A{static_cast<std::uint32_t>(std::stoi(m_b))};
    }
}

void test_cast0()
{
    test0::A a(12345);
    test0::B b("0");    
    
    b = static_cast<test0::B>(a); 
    assert(test0::function_invoked == 3);
    assert(b.m_b == "12345");

    a = static_cast<test0::A>(b);
    assert(test0::function_invoked == 1);
    assert(a.m_a == 12345);

//  b = dynamic_cast<test0::B>(a); // <--- compile error : dynamic_cast works only for reference or pointer
//  a = dynamic_cast<test0::A>(b); // <--- compile error : dynamic_cast works only for reference or pointer
}


// ******************************************************************************* //
// *** When conversion constructor does not exist, conversion operator is used *** //
// ******************************************************************************* //
// similar to previous case, just remove all conversion constructor
//
namespace test1
{
    std::uint32_t function_invoked = 0;

    struct B;
    struct A
    {
        operator B() const;
        std::uint32_t m_a;
    };

    struct B 
    {
        operator A() const;
        std::string m_b;
    };
        
    A::operator B() const
    {
        function_invoked = 2;
        return B{std::to_string(m_a)};
    }

    B::operator A() const
    {
        function_invoked = 4;
        return A{static_cast<std::uint32_t>(std::stoi(m_b))};
    }
}

void test_cast1()
{
    test1::A a(12345);
    test1::B b("0");    
    
    b = static_cast<test1::B>(a); 
    assert(test1::function_invoked == 2);
    assert(b.m_b == "12345");

    a = static_cast<test1::A>(b);
    assert(test1::function_invoked == 4);
    assert(a.m_a == 12345);
}


// ************************************************************************ //
// *** static_cast vs dynamic_cast in inheritance without virtual table *** //
// ************************************************************************ //
// dynamic_cast does not work without virtual function
//
namespace test2
{
    struct A 
    {
        std::uint32_t m_a;
    };

    struct B : public A
    {
        std::uint32_t m_b0;
        std::uint32_t m_b1;
    };
        
    struct C : public A
    {
        std::uint32_t m_c0;
        std::uint32_t m_c1;
        std::uint32_t m_c2;
        std::uint32_t m_c3;
    };
}

void test_cast2()
{
    test2::A* pA(new test2::A(100));
    test2::A* pB(new test2::B(test2::A{101},200,201));
    test2::A* pC(new test2::C(test2::A{102},300,301,302,303)); // <--- aggregate initialization even when there is base class

    assert(pA->m_a == 100);
    assert(pB->m_a == 101);
    assert(pC->m_a == 102);
//  assert(pB->m_b == 200);                           // compile error : no such member for test2::A
  
    assert( static_cast<test2::B*>(pB)->m_b0 == 200);
    assert( static_cast<test2::B*>(pB)->m_b1 == 201);
    assert( static_cast<test2::C*>(pC)->m_c0 == 300);
    assert( static_cast<test2::C*>(pC)->m_c1 == 301);
    assert( static_cast<test2::C*>(pC)->m_c2 == 302);
    assert( static_cast<test2::C*>(pC)->m_c3 == 303);
//  assert( static_cast<test2::C*>(pB)->m_c3 == xxx); // compile ok, invalid memory access, undefined value or crash
//  assert( static_cast<test2::B*>(pC)->m_b1 == xxx); // compile ok, invalid memory access, undefined value or crash
//  assert(dynamic_cast<test2::B*>(pB)->m_b0 == 200); // compile error : cannot use dynamic_cast for non-polymorphic class A (no virtual function)
//  assert(dynamic_cast<test2::C*>(pC)->m_c0 == 300); // compile error : cannot use dynamic_cast for non-polymorphic class A (no virtual function)
}


// ********************************************************************* //
// *** static_cast vs dynamic_cast in inheritance with virtual table *** //
// ********************************************************************* //
namespace test3
{
    struct A 
    {
        A(std::uint32_t x) : m_a(x) // <--- aggregate initialization cannot work when there is virtual function, hence need to define constructor
        {
        }

        virtual void fct() 
        { 
            m_a *= 2; 
        }

        std::uint32_t m_a;
    };

    struct B : public A
    {
        B(const A& a, 
          std::uint32_t x0,
          std::uint32_t x1) : A(a), m_b0(x0), m_b1(x1) 
        {
        } 

        virtual void fct() 
        { 
            m_b0 *= 2; 
            m_b1 *= 2; 
        }

        std::uint32_t m_b0;
        std::uint32_t m_b1;
    };
        
    struct C : public A
    {
        C(const A& a, 
          std::uint32_t x0,
          std::uint32_t x1,
          std::uint32_t x2,
          std::uint32_t x3) : A(a), m_c0(x0), m_c1(x1), m_c2(x2), m_c3(x3) {} 

        virtual void fct() 
        { 
            m_c0 *= 2; 
            m_c1 *= 2; 
            m_c2 *= 2; 
            m_c3 *= 2; 
        }

        std::uint32_t m_c0;
        std::uint32_t m_c1;
        std::uint32_t m_c2;
        std::uint32_t m_c3;
    };
}

void test_cast3()
{
    test3::A* pA(new test3::A(100));
    test3::A* pB(new test3::B(test3::A{101},200,201));
    test3::A* pC(new test3::C(test3::A{102},300,301,302,303));

    assert(pA->m_a == 100);
    assert(pB->m_a == 101);
    assert(pC->m_a == 102);
  
    // both static_cast & dynamic_cast work
    assert( static_cast<test3::B*>(pB)->m_b0 == 200);
    assert( static_cast<test3::B*>(pB)->m_b1 == 201);
    assert( static_cast<test3::C*>(pC)->m_c0 == 300);
    assert( static_cast<test3::C*>(pC)->m_c1 == 301);
    assert( static_cast<test3::C*>(pC)->m_c2 == 302);
    assert( static_cast<test3::C*>(pC)->m_c3 == 303);
    assert(dynamic_cast<test3::B*>(pB)->m_b0 == 200);
    assert(dynamic_cast<test3::B*>(pB)->m_b1 == 201);
    assert(dynamic_cast<test3::C*>(pC)->m_c0 == 300);
    assert(dynamic_cast<test3::C*>(pC)->m_c1 == 301);
    assert(dynamic_cast<test3::C*>(pC)->m_c2 == 302);
    assert(dynamic_cast<test3::C*>(pC)->m_c3 == 303);

    // invalid cast with static_cast : no type-check, undefined behaviour or crash
    auto pC0 = static_cast<test3::C*>(pB); 
    auto pB0 = static_cast<test3::B*>(pC); 
    assert(pC0 != nullptr);
    assert(pB0 != nullptr);
    
    // invalid cast with dynamic_cast : with type-check, return nullptr
    auto pC1 = dynamic_cast<test3::C*>(pB);
    auto pB1 = dynamic_cast<test3::B*>(pC);
    assert(pC1 == nullptr);
    assert(pB1 == nullptr);
}


// ****************************************** //
// *** static_cast vs conversion operator *** //
// ****************************************** //
// when class A has conversion operator to B*, 
// what does static_cast<B*>() invoke ?
//
//  static_cast<B*>( a) invokes conversion operator
//  static_cast<B*>(pa) invokes down cast (no type check)
// dynamic_cast<B*>( a) does not compile
// dynamic_cast<B*>(pa) invokes down cast (with type check)
//
namespace test4
{
    std::uint32_t function_invoked = 0;
    inline void reset() { function_invoked = 0; } 

    struct B;
    struct A 
    {
        A(std::uint32_t x) : m_a(x)
        {
        } 

        operator B () const;
        operator B*() const;
        virtual void fct() 
        { 
            m_a *= 2;
        }

        std::uint32_t m_a;
    };

    struct B : public A
    {
        B(const A& a, 
          std::uint32_t x0,
          std::uint32_t x1) : A(a), m_b0(x0), m_b1(x1) 
        {
        } 

        virtual void fct() 
        { 
            m_b0 *= 2; 
            m_b1 *= 2; 
        }

        std::uint32_t m_b0;
        std::uint32_t m_b1;
    };

    A::operator B() const
    {
        function_invoked = 1;
        return B{*this, 1000 + m_a, 2000 + m_a};
    }

    A::operator B*() const
    {
        function_invoked = 2;
        return new B{*this, 1000 + m_a, 2000 + m_a};
    }
}

void test_cast4()
{
    test4::A* pA(new test4::A(100));
    test4::A* pB(new test4::B(test4::A{101},200,201));

    // static_cast A object
    auto pB0 = static_cast<test4::B*>(*pA); 
    assert(test4::function_invoked == 2);
    assert(pB0 != nullptr);
    assert(pB0->m_a  ==  100);
    assert(pB0->m_b0 == 1100);
    assert(pB0->m_b1 == 2100);
    delete pB0; 
    test4::reset();

    auto pB1 = static_cast<test4::B*>(*pB); 
    assert(test4::function_invoked == 2);
    assert(pB1 != nullptr);
    assert(pB1->m_a  ==  101);
    assert(pB1->m_b0 == 1101);
    assert(pB1->m_b1 == 2101);
    delete pB1; 
    test4::reset();


    // static_cast A pointer
    auto pB2 = static_cast<test4::B*>(pA);
    assert(test4::function_invoked == 0);
    assert(pB2 != nullptr);               // <--- no type-check, invalid memory access
    test4::reset();

    auto pB3 = static_cast<test4::B*>(pB); 
    assert(test4::function_invoked == 0);
    assert(pB3 != nullptr);
    assert(pB3->m_a  == 101);
    assert(pB3->m_b0 == 200);
    assert(pB3->m_b1 == 201);
    test4::reset();

    // dynamic_cast A object
//  auto pB4 = dynamic_cast<test4::B*>(*pA); // compile error
//  auto pB5 = dynamic_cast<test4::B*>(*pB); // compile error
  
    // dynamic_cast A pointer
    auto pB6 = dynamic_cast<test4::B*>(pA);
    assert(test4::function_invoked == 0);
    assert(pB6 == nullptr);               // <--- with type-check, return nullptr
    test4::reset();

    auto pB7 = static_cast<test4::B*>(pB); 
    assert(test4::function_invoked == 0);
    assert(pB7 != nullptr);
    assert(pB7->m_a  == 101);
    assert(pB7->m_b0 == 200);
    assert(pB7->m_b1 == 201);
}


// ************************ //
// *** std::common_type *** //
// ************************ //
// Return the common type, to which, each ARGS... can explicitly convert to.
//
//     A     B
//      \   /|
//       \ / | 
//        C  |  D
//         \ | /
//          \|/ 
//           E
//            
void test_common_type()
{
    struct A {                                                  };
    struct B {                                                  };
    struct C {   C(const A&){}   C(const B&){}                  };
    struct D {                                                  };
    struct E {   E(const B&){}   E(const C&){}   E(const D&){}  };

    // no conversion
    static_assert(std::is_same_v<std::common_type_t<A,A>, A>, "failed in std::common_type");
    static_assert(std::is_same_v<std::common_type_t<B,B>, B>, "failed in std::common_type");
    static_assert(std::is_same_v<std::common_type_t<C,C>, C>, "failed in std::common_type");
    static_assert(std::is_same_v<std::common_type_t<D,D>, D>, "failed in std::common_type");
    static_assert(std::is_same_v<std::common_type_t<E,E>, E>, "failed in std::common_type");

    // via 1 conversion
    static_assert(std::is_same_v<std::common_type_t<A,C>, C>, "failed in std::common_type");
    static_assert(std::is_same_v<std::common_type_t<B,C>, C>, "failed in std::common_type");
    static_assert(std::is_same_v<std::common_type_t<B,E>, E>, "failed in std::common_type");
    static_assert(std::is_same_v<std::common_type_t<C,E>, E>, "failed in std::common_type");
    static_assert(std::is_same_v<std::common_type_t<D,E>, E>, "failed in std::common_type");

    // via 2 conversions (NOT supported, compile error)
//  static_assert(std::is_same_v<std::common_type_t<A,E>, E>, "failed in std::common_type");

    // V shape conversion (NOT supported, compile error)
//  static_assert(std::is_same_v<std::common_type_t<A,B>, C>, "failed in std::common_type");

    // multi types
    static_assert(std::is_same_v<std::common_type_t<B,C,E>, E>, "failed in std::common_type");
//  static_assert(std::is_same_v<std::common_type_t<B,D,E>, E>, "failed in std::common_type"); // compile error : no common type, like common_type_t<A,B>
//  static_assert(std::is_same_v<std::common_type_t<C,D,E>, E>, "failed in std::common_type"); // compile error : no common type, like common_type_t<A,B>
}


void test_cast()
{
    test_cast0();
    test_cast1();
    test_cast2();
    test_cast3();
    test_cast4();
    test_common_type();
    print_summary("cast", "succeeded");
}
