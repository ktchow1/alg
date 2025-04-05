#include<iostream>
#include<iomanip>
#include<cassert>
#include<lru.h>
#include<utility.h>

void compare(const std::list<std::pair<std::string, std::uint32_t>>& list0, 
             const std::list<std::pair<std::string, std::uint32_t>>& list1)
{
    assert(list0.size() == list1.size());
    auto iter0 = list0.begin();
    auto iter1 = list1.begin();
    while(iter0 != list0.end() && iter1 != list1.end())
    {
        assert(iter0->first  == iter1->first);
        assert(iter0->second == iter1->second);
        ++iter0;
        ++iter1;
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
    compare(lru.peek(), 
    {    
        {"abc_104", 104},
        {"abc_103", 103},
        {"abc_102", 102},
        {"abc_101", 101},
        {"abc_100", 100}
    });

    lru.add("abc_105", 105);
    lru.add("abc_106", 106);
    compare(lru.peek(), 
    {    
        {"abc_106", 106},
        {"abc_105", 105},
        {"abc_104", 104},
        {"abc_103", 103},
        {"abc_102", 102}
    });

    lru.add("abc_102", 202);
    lru.add("abc_103", 203);
    compare(lru.peek(), 
    {    
        {"abc_103", 203},
        {"abc_102", 202},
        {"abc_106", 106},
        {"abc_105", 105},
        {"abc_104", 104}
    });

    lru.add("abc_107", 107);
    lru.add("abc_108", 108);
    compare(lru.peek(), 
    {    
        {"abc_108", 108},
        {"abc_107", 107},
        {"abc_103", 203},
        {"abc_102", 202},
        {"abc_106", 106}
    });

    lru.get("abc_107");
    lru.get("abc_106");
    lru.add("abc_109", 109);
    lru.add("abc_110", 110);
    compare(lru.peek(), 
    {    
        {"abc_110", 110},
        {"abc_109", 109},
        {"abc_106", 106},
        {"abc_107", 107},
        {"abc_108", 108}
    });

    print_summary("lru map (least recently used)", "succeeded");
}
