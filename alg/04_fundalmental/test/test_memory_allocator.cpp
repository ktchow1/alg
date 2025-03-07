#include<iostream>
#include<cassert>
#include<vector>
#include<list>
#include<sstream>
#include<utility.h>


namespace alg
{
    std::vector<std::string> allocator_invoked;

    template<typename T> 
    class allocator
    {
    public: 
        // STL requires the following typedef
        typedef size_t    size_type;
        typedef ptrdiff_t difference_type;
        typedef T*        pointer;
        typedef const T*  const_pointer;
        typedef T&        reference;
        typedef const T&  const_reference;
        typedef T         value_type;

    public:
        T* allocate(size_t n, const void* hints=0) // n = num of T-instances (not bytes)
        {
            std::stringstream ss;
            ss << "allocate " << n << " objects.";
            allocator_invoked.push_back(ss.str());

            T* p = new T[n];
            return p;
        }

        void deallocate(void* p, size_t n)
        {
            std::stringstream ss;
            ss << "deallocate " << n << " objects.";
            allocator_invoked.push_back(ss.str());

            delete[] reinterpret_cast<T*>(p);
        }
    };
}


// ************************************ //
// *** Test allocator for container *** //
// ************************************ //
void test_memory_allocator()
{
    struct A
    {
        std::uint64_t m_x;
        std::uint64_t m_y;
        std::uint64_t m_z;
    };

    {
        std::vector<A, alg::allocator<A>> vec;
        std::list  <A, alg::allocator<A>> list;

        for(int n=0; n!=100; ++n) vec.push_back(A{});
        for(int n=0; n!=5;   ++n) vec.pop_back();
        for(int n=0; n!=5;   ++n) list.push_back(A{});
    }

    std::uint32_t n = 0;
    assert(alg::allocator_invoked[n] == "allocate 1 objects.");        ++n;
    assert(alg::allocator_invoked[n] == "allocate 2 objects.");        ++n; // <--- std::vector   allocates first
    assert(alg::allocator_invoked[n] == "deallocate 1 objects.");      ++n; // <--- std::vector deallocates after
    assert(alg::allocator_invoked[n] == "allocate 4 objects.");        ++n;
    assert(alg::allocator_invoked[n] == "deallocate 2 objects.");      ++n;
    assert(alg::allocator_invoked[n] == "allocate 8 objects.");        ++n;
    assert(alg::allocator_invoked[n] == "deallocate 4 objects.");      ++n;
    assert(alg::allocator_invoked[n] == "allocate 16 objects.");       ++n;
    assert(alg::allocator_invoked[n] == "deallocate 8 objects.");      ++n;
    assert(alg::allocator_invoked[n] == "allocate 32 objects.");       ++n;
    assert(alg::allocator_invoked[n] == "deallocate 16 objects.");     ++n;
    assert(alg::allocator_invoked[n] == "allocate 64 objects.");       ++n;
    assert(alg::allocator_invoked[n] == "deallocate 32 objects.");     ++n;
    assert(alg::allocator_invoked[n] == "allocate 128 objects.");      ++n;
    assert(alg::allocator_invoked[n] == "deallocate 64 objects.");     ++n;
    assert(alg::allocator_invoked[n] == "allocate 1 objects.");        ++n;
    assert(alg::allocator_invoked[n] == "allocate 1 objects.");        ++n;
    assert(alg::allocator_invoked[n] == "allocate 1 objects.");        ++n;
    assert(alg::allocator_invoked[n] == "allocate 1 objects.");        ++n;
    assert(alg::allocator_invoked[n] == "allocate 1 objects.");        ++n;
    assert(alg::allocator_invoked[n] == "deallocate 1 objects.");      ++n;
    assert(alg::allocator_invoked[n] == "deallocate 1 objects.");      ++n;
    assert(alg::allocator_invoked[n] == "deallocate 1 objects.");      ++n;
    assert(alg::allocator_invoked[n] == "deallocate 1 objects.");      ++n;
    assert(alg::allocator_invoked[n] == "deallocate 1 objects.");      ++n;
    assert(alg::allocator_invoked[n] == "deallocate 128 objects.");     ++n;
    print_summary("allocator for container", "succeeded");
}

