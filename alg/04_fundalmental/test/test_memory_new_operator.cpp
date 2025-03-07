#include<iostream>
#include<cstdint>
#include<cassert>
#include<string>
#include<vector>
#include<utility.h>


// *********************************************************************** //
//   new statement invokes :
// * new operator, with size = sizeof(T)
// * T::T()
// 
//
//   delete statement invokes :
// * T::~T()
// * delete operator
//
//
//   new[] statement invokes :
// * new[] operator, with size = sizeof(T) * N + (8 bytes)
// * T::T() * N
// * reset  *reinterpret_cast<std::uint64_t*>(ptr) = N
// * return ptr + (8 bytes)
//
//
//   delete[] statement invokes :
// * T::~T() * N
// * delete[] operator
// * free ptr - (8 bytes)
// *********************************************************************** //
namespace alg
{
    std::vector<std::string> new_operator_invoked;
    char buffer[1024];

    struct A
    {
        A(std::uint32_t x, std::uint32_t y, std::uint32_t z) : m_x(x), m_y(y), m_z(z)
        {
            new_operator_invoked.push_back("constructor invoked");
        }

        ~A()
        {
            new_operator_invoked.push_back("destructor invoked");
        }

        void* operator new (size_t n) 
        { 
            std::stringstream ss;
            ss << "new operator invoked, " << n << " bytes"; 
            new_operator_invoked.push_back(ss.str());
            return buffer;
        }

        void* operator new[] (size_t n) 
        { 
            std::stringstream ss;
            ss << "new[] operator invoked, " << n << " bytes"; 
            new_operator_invoked.push_back(ss.str());
            return buffer;
        }

        void operator delete (void* ptr) 
        {
            new_operator_invoked.push_back("delete operator invoked");
        }

        void operator delete[] (void* ptr) 
        {
            new_operator_invoked.push_back("delete[] operator invoked");
        }

        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };
}


void test_memory_new_operator()
{
    // *************** //
    // *** Pointer *** //
    // *************** //
    auto* p0 = new alg::A{11,12,13};
    assert((std::uint64_t)p0 == (std::uint64_t)alg::buffer);
    delete p0;

    assert("new operator invoked, 12 bytes"); // <--- new operator is invoked before constructor
    assert("constructor invoked");
    assert("destructor invoked");
    assert("delete operator invoked");        // <--- delete operator is invoked after destructor
    alg::new_operator_invoked.clear();


    // ******************** //
    // *** Array size 1 *** //
    // ******************** //
    auto* p1 = new alg::A[1]
    {
        {11,12,13}
    };
    auto* p1_offset = reinterpret_cast<char*>(p1) - sizeof(std::uint64_t);
    assert((std::uint64_t)p1 == (std::uint64_t)alg::buffer + sizeof(std::uint64_t));
    assert(*(reinterpret_cast<std::uint64_t*>(alg::buffer)) == 1); 
    assert(*(reinterpret_cast<std::uint64_t*>(p1_offset))   == 1);  
    delete[] p1;

    assert("new[] operator invoked, 20 bytes"); // 1 * 12 + 8 = 20
    assert("constructor invoked");
    assert("destructor invoked");
    assert("delete[] operator invoked");
    alg:: new_operator_invoked.clear();


    // ******************** //
    // *** Array size 4 *** //
    // ******************** //
    auto* p2 = new alg::A[4]
    {
        {11,12,13},
        {21,22,23},
        {31,32,33},
        {41,42,43}
    };
    auto* p2_offset = reinterpret_cast<char*>(p2) - sizeof(std::uint64_t);
    assert((std::uint64_t)p2       == (std::uint64_t)alg::buffer + sizeof(std::uint64_t));
    assert((std::uint64_t)(&p2[0]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 0);
    assert((std::uint64_t)(&p2[1]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 1);
    assert((std::uint64_t)(&p2[2]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 2);
    assert((std::uint64_t)(&p2[3]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 3);
    assert(*(reinterpret_cast<std::uint64_t*>(alg::buffer)) == 4); 
    assert(*(reinterpret_cast<std::uint64_t*>(p1_offset))   == 4); 
    delete[] p2;

    assert("new[] operator invoked, 56 bytes"); // 4 * 12 + 8 = 56
    assert("constructor invoked");
    assert("constructor invoked");
    assert("constructor invoked");
    assert("constructor invoked");
    assert("destructor invoked");
    assert("destructor invoked");
    assert("destructor invoked");
    assert("destructor invoked");
    assert("delete[] operator invoked");
    alg:: new_operator_invoked.clear();


    // ******************** //
    // *** Array size 7 *** //
    // ******************** //
    auto* p3 = new alg::A[7]
    {
        {11,12,13},
        {21,22,23},
        {31,32,33},
        {41,42,43},
        {51,52,53},
        {61,62,63},
        {71,72,73}
    };
    auto* p3_offset = reinterpret_cast<char*>(p3) - sizeof(std::uint64_t);
    assert((std::uint64_t)p3       == (std::uint64_t)alg::buffer + sizeof(std::uint64_t));
    assert((std::uint64_t)(&p3[0]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 0);
    assert((std::uint64_t)(&p3[1]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 1);
    assert((std::uint64_t)(&p3[2]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 2);
    assert((std::uint64_t)(&p3[3]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 3);
    assert((std::uint64_t)(&p3[4]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 4);
    assert((std::uint64_t)(&p3[5]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 5);
    assert((std::uint64_t)(&p3[6]) == (std::uint64_t)alg::buffer + sizeof(std::uint64_t) + sizeof(alg::A) * 6);
    assert(*(reinterpret_cast<std::uint64_t*>(alg::buffer)) == 7); 
    assert(*(reinterpret_cast<std::uint64_t*>(p1_offset))   == 7); 
    delete[] p3;

    assert("new[] operator invoked, 92 bytes"); // 7 * 12 + 8 = 92
    assert("constructor invoked");
    assert("constructor invoked");
    assert("constructor invoked");
    assert("constructor invoked");
    assert("constructor invoked");
    assert("constructor invoked");
    assert("constructor invoked");
    assert("destructor invoked");
    assert("destructor invoked");
    assert("destructor invoked");
    assert("destructor invoked");
    assert("destructor invoked");
    assert("destructor invoked");
    assert("destructor invoked");
    assert("delete[] operator invoked");
    alg:: new_operator_invoked.clear();


    print_summary("new and delete operator", "succeeded");
}    



