#include<iostream>
#include<cassert>
#include<cstdint>
#include<vector>
#include<algorithm>


// For alg
#include<std_alg.h>
#include<utility.h>


void test_lower_upper_bound()
{
    std::uint32_t num_trial = 1000;
    std::uint32_t size      =   50;
    std::uint32_t min       =   10;  
    std::uint32_t max       =   30;  
    std::uint32_t error0    =    0;
    std::uint32_t error1    =    0;
  
    for(std::uint32_t t=0; t!=num_trial; ++t)
    {
        std::uint32_t target = (min-5) + rand() % ((max+5) - (min-5));
        auto data = gen_random_sorted_vec(size, min, max);
        auto ans0 = alg::lower_bound(data.begin(), data.end(), target);
        auto ans1 = std::lower_bound(data.begin(), data.end(), target);
        auto ans2 = alg::upper_bound(data.begin(), data.end(), target);
        auto ans3 = std::upper_bound(data.begin(), data.end(), target);

        if (ans0 != ans1) ++error0;
        if (ans2 != ans3) ++error1;
        if (ans2 != ans3)
        {
            std::cout << "\ntarget = " << target << ", vec = ";
            for(const auto& x:data) std::cout << x << ",";
            std::cout << "  ans2 = " << std::distance(data.begin(),ans2);
            std::cout << "  ans3 = " << std::distance(data.begin(),ans3);
        }
    }
    print_summary("std_alg - lower_bound", error0, num_trial);
    print_summary("std_alg - upper_bound", error1, num_trial);
}


void test_permutation()
{
    std::uint32_t size = 9;
    std::vector<std::uint32_t> vec0;
    std::vector<std::uint32_t> vec1;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        vec0.push_back(n);
        vec1.push_back(n);
    }

    while(true)
    {
        bool flag0 = std::next_permutation(vec0.begin(), vec0.end());
        bool flag1 = alg::next_permutation(vec1.begin(), vec1.end());
        if (flag0 != flag1) 
        {
            break;
        }
        else if (flag0 == false)
        {
            break;
        }
        else if (!compare(vec0,vec1))
        {
            break;
        }
    }
    print_summary("std_alg - permutation", "succeeded");
}


void test_hashmap()
{
    alg::hashmap<std::string, std::uint32_t> map{8}; // size to be power of 2
     
    assert( map.set("aaa", 100));
    assert( map.set("bbb", 101));
    assert( map.set("ccc", 102));
    assert( map.set("ddd", 103));
    assert( map.set("eee", 104));
    assert( map.set("fff", 105));
    assert( map.set("ggg", 106));
    assert( map.set("hhh", 107));
    assert(!map.set("iii", 108));
    assert(!map.set("jjj", 109));
 
    auto op0 = map.get("aaa");  assert( op0 && *op0 == 100);
    auto op1 = map.get("bbb");  assert( op1 && *op1 == 101);
    auto op2 = map.get("ccc");  assert( op2 && *op2 == 102);
    auto op3 = map.get("ddd");  assert( op3 && *op3 == 103);
    auto op8 = map.get("iii");  assert(!op8);
    auto op9 = map.get("jjj");  assert(!op9);

    assert( map.set("aaa", 200));
    assert( map.set("bbb", 201));
    assert( map.set("ccc", 202));
    assert( map.set("ddd", 203));

    auto op10 = map.get("aaa");  assert(op10 && *op10 == 200);
    auto op11 = map.get("bbb");  assert(op11 && *op11 == 201);
    auto op12 = map.get("ccc");  assert(op12 && *op12 == 202);
    auto op13 = map.get("ddd");  assert(op13 && *op13 == 203);

    print_summary("std_alg - hashmap", "succeeded");
}


void test_std_alg()
{
    test_lower_upper_bound();
    test_permutation();
    test_hashmap();
}
