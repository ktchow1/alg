#include<iostream>
#include<cstdint>
#include<cstring>
#include<cassert>
#include<string>
#include<traits.h>
#include<utility.h>


// ***************************************************************************************************************************** //
// To trigger compile time computation, we need :
// 1. declare function constexpr, do not declare arg in prototype as constexpr (compile error), as they can be changed by caller
// 2. declare  input variables constexpr, see line A
// 3. declare output variables constexpr, see line B
// 4. no need to use "if constexpr" in constexpr function, compiler will do the rest, see conditional_add()
// 
// Compiler starts checking for constexpr when it comes across static_assert, or anything known in compile time :
// 1. compiler sees line C
// 2. compiler checks if a2 in line B is constexpr
// 3. compiler checks if a1 in line A is constexpr
// 4. compiler checks if function test::add is constexpr
// 5. repeat checking along dependency graph
// 6. compile error if there are non-constexpr in the dependency path
//
// In practice :
// * variable initialized with  constant-expression (like hard coded values) must be declared constexpr
// * variable initialized with non-const-expression (like arg in function prototype) must NOT be constexpr
// * variable that needs dynamic allocation, must NOT be declared constexpr
// 
// What is consteval? 
// * similar to constexpr, but :
// - constexpr add()              :     allow runtime computation, only check if its inputs are constexpr on static_assert()
// - consteval compile_time_add() : not allow runtime computation,      check if its inputs are constexpr on every invocation
// * cannot declare variable consteval
// * only   declare function consteval   
//
// What is constinit?
// * constinit is not about const expression, it is not even constant in runtime
// * constinit is     about static variable / global variable to be initialized in compile time, while mutable in runtime 
// * particular when we have no control over the order of static variable initialization across different .cpp
// ***************************************************************************************************************************** //
namespace test
{
    struct A
    {
        std::uint64_t m_x;
        std::uint64_t m_y;
        std::uint64_t m_z;
    };

    constexpr A add(const A& a0, const A& a1) 
    {
        return { a0.m_x + a1.m_x,
                 a0.m_y + a1.m_y,
                 a0.m_z + a1.m_z };
    }
      
    constexpr A conditional_add(const A& a0, const A& a1, std::uint32_t option) 
    {
        if      (option == 0) return { a0.m_x + a1.m_x, a0.m_y, a0.m_z };
        else if (option == 1) return { a0.m_x, a0.m_y + a1.m_y, a0.m_z };
        else                  return { a0.m_x, a0.m_y, a0.m_z + a1.m_z };
    } 

    consteval A compile_time_add(const A& a0, const A& a1) 
    {
        return { a0.m_x + a1.m_x,
                 a0.m_y + a1.m_y,
                 a0.m_z + a1.m_z };
    }

    struct B
    {
        // Default-default constructor is declared constexpr, so that of struct C should be ...
        constexpr A add(const A& a) const
        {
            return { m_x + a.m_x,
                     m_y + a.m_y,
                     m_z + a.m_z };
        }

        std::uint64_t m_x;
        std::uint64_t m_y;
        std::uint64_t m_z;
    };

    struct C
    {
        // Manual-defined-default-constructor should be declared constexpr
        constexpr C(std::uint64_t x, std::uint64_t y, std::uint64_t z) : m_x(x), m_y(y), m_z(z)
        {
        }

        constexpr A add(const A& a) const
        {
            return { m_x + a.m_x,
                     m_y + a.m_y,
                     m_z + a.m_z };
        }

        std::uint64_t m_x;
        std::uint64_t m_y;
        std::uint64_t m_z;
    };

    // *************************************************************** //
    // When to use "if constexpr" then? Only for template :
    //
    // - when we want to replace SFINAE
    // - when we want to tell compiler NOT to compile some IF branches
    // *************************************************************** //
    template<typename T> 
    constexpr auto sum(const T& array_or_pair)
    {
        // Unlike conditional_add, "if constexpr" is necessary here :
        //
        // - to avoid accessing operator[] for non-array
        // - to avoid accessing .first or .second for non-pair
        // - to avoid compile error
        if constexpr (alg::is_array_v<T>) 
        {
            typename T::value_type ans = 0;
            for(const auto& x:array_or_pair)
            {
                ans += x;
            }
            return ans;
        }
        else
        {
            return array_or_pair.first + array_or_pair.second;
        }
    }

    constinit A global_a0{100,101,102};
              A global_a1{200,201,202};
}


void test_constexpr_usage()
{
    // ************************** //
    // *** constexpr function *** //
    // ************************** //
    constexpr test::A a0(11,12,13);                         // <--- line A
    constexpr test::A a1(21,22,23);                         // <--- line A
    constexpr test::A a2 = test::add(a0,a1);                // <--- line B
    static_assert(a2.m_x == 32, "failed to use constexpr"); // <--- line C
    static_assert(a2.m_y == 34, "failed to use constexpr");
    static_assert(a2.m_z == 36, "failed to use constexpr");
    std::array<test::A, a2.m_x> dummy;

    constexpr test::A a3 = test::conditional_add(a0,a1,0);
    constexpr test::A a4 = test::conditional_add(a0,a1,1);
    constexpr test::A a5 = test::conditional_add(a0,a1,2);
    static_assert(a3.m_x == 32, "failed to use constexpr");
    static_assert(a3.m_y == 12, "failed to use constexpr");
    static_assert(a3.m_z == 13, "failed to use constexpr");
    static_assert(a4.m_x == 11, "failed to use constexpr");
    static_assert(a4.m_y == 34, "failed to use constexpr");
    static_assert(a4.m_z == 13, "failed to use constexpr");
    static_assert(a5.m_x == 11, "failed to use constexpr");
    static_assert(a5.m_y == 12, "failed to use constexpr");
    static_assert(a5.m_z == 36, "failed to use constexpr");

    // It works for runtime as well. 
    test::A a6(11,12,13);
    test::A a7(21,22,23);
    test::A a8 = test::add(a0,a1); 
    assert(a8.m_x == 32);
    assert(a8.m_y == 34);
    assert(a8.m_z == 36);


    // ********************************* //
    // *** constexpr member function *** //
    // ********************************* //
    constexpr test::B b0{31,32,33};
    constexpr test::A a9 = b0.add(a0);
    static_assert(a9.m_x == 42, "failed to use constexpr");
    static_assert(a9.m_y == 44, "failed to use constexpr");
    static_assert(a9.m_z == 46, "failed to use constexpr");
  
    constexpr test::C c0{41,42,43};
    constexpr test::A a10 = c0.add(a0);
    static_assert(a10.m_x == 52, "failed to use constexpr");
    static_assert(a10.m_y == 54, "failed to use constexpr");
    static_assert(a10.m_z == 56, "failed to use constexpr"); 

    // It works for runtime as well. 
    test::B b1(51,52,53);
    test::A a11 = b1.add(a0); 
    assert(a11.m_x == 62);
    assert(a11.m_y == 64);
    assert(a11.m_z == 66);

    test::C c1(61,62,63);
    test::A a12 = c1.add(a0); 
    assert(a12.m_x == 72);
    assert(a12.m_y == 74);
    assert(a12.m_z == 76);


    // ******************** //
    // *** if constexpr *** //
    // ******************** //
    constexpr std::array<std::uint64_t,4> array0{1,2,3,4};
    constexpr std::array<std::uint64_t,6> array1{1,2,3,4,5,6};
    constexpr std::array<std::uint64_t,8> array2{1,2,3,4,5,6,7,8};
    constexpr std::pair<std::uint64_t,std::uint64_t> pair0{100,11};
    constexpr std::pair<std::uint64_t,std::uint64_t> pair1{200,22};
    constexpr std::pair<std::uint64_t,std::uint64_t> pair2{300,33};

    constexpr std::uint64_t s0 = test::sum(array0);
    constexpr std::uint64_t s1 = test::sum(array1);
    constexpr std::uint64_t s2 = test::sum(array2);
    constexpr std::uint64_t s3 = test::sum(pair0);
    constexpr std::uint64_t s4 = test::sum(pair1);
    constexpr std::uint64_t s5 = test::sum(pair2);

    static_assert(s0 ==  10, "failed to use constexpr");
    static_assert(s1 ==  21, "failed to use constexpr");
    static_assert(s2 ==  36, "failed to use constexpr");
    static_assert(s3 == 111, "failed to use constexpr");
    static_assert(s4 == 222, "failed to use constexpr");
    static_assert(s5 == 333, "failed to use constexpr");
    print_summary("constexpr", "succeeded");
}


void test_consteval_usage()
{
    // ************************** //
    // *** consteval function *** //
    // ************************** //
    constexpr test::A a0(11,12,13);
    constexpr test::A a1(21,22,23);
    constexpr test::A a2 = test::compile_time_add(a0,a1);   // as compile_time_add is consteval, a0 & a1 must be constexpr
    static_assert(a2.m_x == 32, "failed to use constexpr"); // with    static_assert, we must declare a2 constexpr 
    static_assert(a2.m_y == 34, "failed to use constexpr"); // without static_assert, we can declare a2 const, but a0 & a1 must be constexpr
    static_assert(a2.m_z == 36, "failed to use constexpr"); 

    // It does NOT work for runtime. Even without static_assert.
    test::A a3(11,12,13);
    test::A a4(21,22,23);
//  test::A a5 = test::compile_time_add(a3,a4); // Compile error : consteval function needs input to be constexpr, even without static_assert
    print_summary("consteval", "succeeded");
}


void test_constinit_usage()
{
//  static_assert(test::global_a0.m_x == 101, ""); // Compile error : as global_a0 is not constexpr
    assert(test::global_a0.m_x == 100);
    assert(test::global_a0.m_y == 101);
    assert(test::global_a0.m_z == 102);
    assert(test::global_a1.m_x == 200);
    assert(test::global_a1.m_y == 201);
    assert(test::global_a1.m_z == 202);
    print_summary("constinit", "succeeded");
}


void test_constexpr()
{
    test_constexpr_usage();
    test_consteval_usage();
    test_constinit_usage();
}
