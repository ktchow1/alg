#include<iostream>
#include<cassert>
#include<cstdint>
#include<cstring>
#include<string>
#include<vector>
#include<utility.h>


void test_placement_new_basic()
{
    struct A
    {
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };

    char  stack_buf[sizeof(A)];
    char*  heap_buf = new char[sizeof(A)];

    auto* p0 = new (stack_buf) A{11,12,13};
    auto* p1 = new ( heap_buf) A{21,22,23};
    auto* p2 = reinterpret_cast<A*>(stack_buf);
    auto* p3 = reinterpret_cast<A*>( heap_buf);
    assert(p0->m_x == 11 && p0->m_y == 12 && p0->m_z == 13); 
    assert(p1->m_x == 21 && p1->m_y == 22 && p1->m_z == 23); 
    assert(p2->m_x == 11 && p2->m_y == 12 && p2->m_z == 13); 
    assert(p3->m_x == 21 && p3->m_y == 22 && p3->m_z == 23); 

    print_summary("placement new - basic", "succeeded");
    delete[] heap_buf; 
}


// ************************************************************* //
// 1. placement new    : for construct struct, given char buffer  
// 2. reinterpret_cast : for accessing struct, given char buffer
// ************************************************************* //
void test_placement_new_vs_reinterpret_cast()
{
    struct A
    {
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };
    struct B
    {
        std::string m_x;
        std::string m_y;
        std::string m_z;
    };

    char buffer[1024];
    new (buffer) A(11,12,13); 
    assert(reinterpret_cast<A*>(buffer)->m_x == 11);
    assert(reinterpret_cast<A*>(buffer)->m_y == 12);
    assert(reinterpret_cast<A*>(buffer)->m_z == 13);

    new (buffer) A(21,22,23); 
    assert(reinterpret_cast<A*>(buffer)->m_x == 21);
    assert(reinterpret_cast<A*>(buffer)->m_y == 22);
    assert(reinterpret_cast<A*>(buffer)->m_z == 23);

    new (buffer) B("abcde", "klmno", "pqrst");
    assert(reinterpret_cast<B*>(buffer)->m_x == "abcde");
    assert(reinterpret_cast<B*>(buffer)->m_y == "klmno");
    assert(reinterpret_cast<B*>(buffer)->m_z == "pqrst");

    new (buffer) B("123456", "xyz", "$%^&*");
    assert(reinterpret_cast<B*>(buffer)->m_x == "123456");
    assert(reinterpret_cast<B*>(buffer)->m_y == "xyz");
    assert(reinterpret_cast<B*>(buffer)->m_z == "$%^&*");
    print_summary("placement new - ve reinterpret_cast", "succeeded");
}


// ********************************** //
// Simple vector 
// - needs placement new
// - needs char buffer implementation
//
// For detailed implementation
// refer to 05_template/array.h
// ********************************** //
namespace alg
{
    template<typename T, std::uint32_t N> 
    class simple_vector_fail 
    {
    public:
        template<typename...ARGS> 
        void emplace_back(ARGS&&... args) 
        {
            if (m_size == N) return;
            m_impl[m_size] = T(std::forward<ARGS>(args)...); // Problem : duplicated construction
            ++m_size;
        }

        const T* access(std::uint32_t n) const noexcept
        {
            if (n < m_size)
            {
                return &m_impl[n];
            }
            else return nullptr;
        }

    private:
        T m_impl[N];
        std::uint32_t m_size = 0;
    };

    template<typename T, std::uint32_t N> 
    class simple_vector
    {
    public:
        ~simple_vector() 
        {                
            // Problem : Don't forget to destruct
            for(std::uint32_t n=0; n!=m_size; ++n)
            {            
                reinterpret_cast<T*>(m_impl + n * sizeof(T))->~T();            
            }
        }

        template<typename...ARGS> 
        void emplace_back(ARGS&&... args) 
        {
            if (m_size == N) return;
            new (m_impl + m_size * sizeof(T)) T(std::forward<ARGS>(args)...);
            ++m_size;
        }

        const T* access(std::uint32_t n) const noexcept
        {
            if (n < m_size)
            {
                return reinterpret_cast<const T*>(m_impl + n * sizeof(T));
            }
            else return nullptr;
        }

    private:
        char m_impl[sizeof(T) * N];
        std::uint32_t m_size = 0;
    };
}


void test_placement_new_in_simple_vector()
{    
    struct A
    {
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };
    A a(11,12,13); 

    alg::simple_vector_fail<A,5> vec0;
    vec0.emplace_back(a);
    vec0.emplace_back(A{21,22,23}); 
    vec0.emplace_back(  31,32,33);  
    assert(vec0.access(0)->m_x == 11);
    assert(vec0.access(0)->m_y == 12);
    assert(vec0.access(0)->m_z == 13);
    assert(vec0.access(1)->m_x == 21);
    assert(vec0.access(1)->m_y == 22);
    assert(vec0.access(1)->m_z == 23);
    assert(vec0.access(2)->m_x == 31);
    assert(vec0.access(2)->m_y == 32);
    assert(vec0.access(2)->m_z == 33);
    assert(vec0.access(3) == nullptr);

    alg::simple_vector<A,5> vec1;
    vec1.emplace_back(a);
    vec1.emplace_back(A{41,42,43}); 
    vec1.emplace_back(  51,52,53);  
    assert(vec1.access(0)->m_x == 11);
    assert(vec1.access(0)->m_y == 12);
    assert(vec1.access(0)->m_z == 13);
    assert(vec1.access(1)->m_x == 41);
    assert(vec1.access(1)->m_y == 42);
    assert(vec1.access(1)->m_z == 43);
    assert(vec1.access(2)->m_x == 51);
    assert(vec1.access(2)->m_y == 52);
    assert(vec1.access(2)->m_z == 53);
    assert(vec1.access(3) == nullptr);

    print_summary("placement new - in simple vector", "succeeded");
}

void test_memory_placement_new()
{
    test_placement_new_basic();
    test_placement_new_vs_reinterpret_cast();
    test_placement_new_in_simple_vector();
}
