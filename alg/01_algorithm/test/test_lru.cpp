#include<iostream>
#include<iomanip>
#include<cassert>
#include<lru.h>
#include<utility.h>

void debug(const auto& vec)
{
    std::cout << "\n";
    for(const auto& x:vec)
    {
        std::cout << x.first << ":" << x.second << ", ";
    }
}

void test_lru_map()
{
    alg::lru_map<std::string, std::uint32_t, 5> lru;


    lru.add("abc_100", 100);
    lru.add("abc_101", 101);
    lru.add("abc_102", 102);
    lru.add("abc_103", 103);
    lru.add("abc_104", 104);
    debug(lru.peek());

    lru.add("abc_105", 105);
    lru.add("abc_106", 106);
    debug(lru.peek());

    lru.add("abc_102", 205);
    lru.add("abc_103", 206);
    debug(lru.peek());

    lru.add("abc_107", 107);
    lru.add("abc_108", 108);
    debug(lru.peek());





    print_summary("lru map (least recently used)", "succeeded");
}
