#include<cassert>
#include<array.h>
#include<utility.h>


struct A
{
    int x;
    int y;
    int z;
};

inline bool operator==(const A& lhs, const A& rhs)
{
    return lhs.x == rhs.x &&
           lhs.y == rhs.y &&
           lhs.z == rhs.z;
}

void test_array()
{
    A a{10,11,12};

    // 1. push_back, emplace and access
    alg::array<A> ar0(5);
    ar0.push_back(a);
    ar0.push_back(std::move(a));
    ar0.push_back(A{20,21,22});
    ar0.emplace(30,31,32);
    
    assert(ar0.capacity() == 5);
    assert(ar0.size() == 4);
    assert((ar0[0] == A{10,11,12})); // extra bracket as assert is macro
    assert((ar0[1] == A{10,11,12})); 
    assert((ar0[2] == A{20,21,22})); 
    assert((ar0[3] == A{30,31,32})); 

    // 2. copy semantics
    alg::array<A> ar1(ar0);
    alg::array<A> ar2{10};

    assert(ar1.capacity() == 5);
    assert(ar1.size() == 4);
    assert((ar1[0] == A{10,11,12}));
    assert((ar1[1] == A{10,11,12})); 
    assert((ar1[2] == A{20,21,22})); 
    assert((ar1[3] == A{30,31,32})); 
    assert(ar2.capacity() == 10);
    assert(ar2.size() == 0);

    ar2 = ar1;
    assert(ar2.capacity() == 5);
    assert(ar2.size() == 4);
    assert((ar2[0] == A{10,11,12}));
    assert((ar2[1] == A{10,11,12})); 
    assert((ar2[2] == A{20,21,22})); 
    assert((ar2[3] == A{30,31,32})); 
    assert(ar1.capacity() == 5);
    assert(ar1.size() == 4);

    // 3. move semantics
    alg::array<A> ar3(std::move(ar1));
    alg::array<A> ar4{10};

    assert(ar3.capacity() == 5);
    assert(ar3.size() == 4);
    assert((ar3[0] == A{10,11,12}));
    assert((ar3[1] == A{10,11,12})); 
    assert((ar3[2] == A{20,21,22})); 
    assert((ar3[3] == A{30,31,32})); 
    assert(ar1.capacity() == 0);
    assert(ar1.size() == 0);

    ar4 = std::move(ar2);
    assert(ar4.capacity() == 5);
    assert(ar4.size() == 4);
    assert((ar4[0] == A{10,11,12}));
    assert((ar4[1] == A{10,11,12})); 
    assert((ar4[2] == A{20,21,22})); 
    assert((ar4[3] == A{30,31,32})); 
    assert(ar2.capacity() == 0);
    assert(ar2.size() == 0);

    print_summary("alg::array", "succeeded");
}
