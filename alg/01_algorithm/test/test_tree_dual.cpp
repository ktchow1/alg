#include<iostream>
#include<iomanip>
#include<cassert>
#include<tree_dual.h>
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

    lru.set("abc_100", 100);
    lru.set("abc_101", 101);
    lru.set("abc_102", 102);
    lru.set("abc_103", 103);
    lru.set("abc_104", 104);
    compare(lru.peek(), 
    {    
        {"abc_104", 104},
        {"abc_103", 103},
        {"abc_102", 102},
        {"abc_101", 101},
        {"abc_100", 100}
    });

    lru.set("abc_105", 105);
    lru.set("abc_106", 106);
    compare(lru.peek(), 
    {    
        {"abc_106", 106},
        {"abc_105", 105},
        {"abc_104", 104},
        {"abc_103", 103},
        {"abc_102", 102}
    });

    lru.set("abc_102", 202);
    lru.set("abc_103", 203);
    compare(lru.peek(), 
    {    
        {"abc_103", 203},
        {"abc_102", 202},
        {"abc_106", 106},
        {"abc_105", 105},
        {"abc_104", 104}
    });

    lru.set("abc_107", 107);
    lru.set("abc_108", 108);
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
    lru.set("abc_109", 109);
    lru.set("abc_110", 110);
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


void test_value_indexed_map()
{
    alg::value_indexed_map<std::string, std::uint32_t> stocks;
    {
        auto x = stocks.get_top(1);
        assert(x.empty());
    }

    // *** case 1 *** //
    stocks.set("stock_0000", 100);
    stocks.set("stock_0001", 101);
    stocks.set("stock_0002", 102);
    stocks.set("stock_0003", 102);
    {
        auto x0 = stocks.get_by_key("stock_0000");
        auto x1 = stocks.get_by_key("stock_0001");
        auto x2 = stocks.get_by_key("stock_0002");
        auto x3 = stocks.get_by_key("stock_0003");
        auto x4 = stocks.get_by_key("stock_0004");

        assert( x0);
        assert( x1);
        assert( x2);
        assert( x3);
        assert(!x4);
        assert(*x0 == 100);
        assert(*x1 == 101);
        assert(*x2 == 102);
        assert(*x3 == 102);
    
        auto y0 = stocks.get_by_value(99);
        auto y1 = stocks.get_by_value(101);
        auto y2 = stocks.get_by_value(102);
        assert(y0.size() == 0);
        assert(y1.size() == 1);
        assert(y2.size() == 2);
        assert(std::find(y1.begin(), y1.end(), "stock_0001") != y1.end());
        assert(std::find(y2.begin(), y2.end(), "stock_0002") != y2.end());
        assert(std::find(y2.begin(), y2.end(), "stock_0003") != y2.end());

        auto z0 = stocks.get_top(2);
        auto z1 = stocks.get_top(4);
        auto z2 = stocks.get_top(6);
        assert(z0.size() == 2);
        assert(z1.size() == 4);
        assert(z2.size() == 4);
        assert(z0[0].second == 102);
        assert(z0[1].second == 102);
        assert(z1[0].second == 102);
        assert(z1[1].second == 102);
        assert(z1[2].second == 101);
        assert(z1[3].second == 100);
        assert(z2[0].second == 102);
        assert(z2[1].second == 102);
        assert(z2[2].second == 101);
        assert(z2[3].second == 100);
        assert(z2[2].first == "stock_0001");
        assert(z2[3].first == "stock_0000");
    }

    // *** case 2 *** //
    stocks.set("stock_0000", 105);
    stocks.set("stock_0001", 104);
    stocks.set("stock_0002", 104);
    stocks.set("stock_0003", 103);
    {
        auto x0 = stocks.get_by_key("stock_0000");
        auto x1 = stocks.get_by_key("stock_0001");
        auto x2 = stocks.get_by_key("stock_0002");
        auto x3 = stocks.get_by_key("stock_0003");
        auto x4 = stocks.get_by_key("stock_0004");

        assert( x0);
        assert( x1);
        assert( x2);
        assert( x3);
        assert(!x4);
        assert(*x0 == 105);
        assert(*x1 == 104);
        assert(*x2 == 104);
        assert(*x3 == 103);
    
        auto y0 = stocks.get_by_value(102);
        auto y1 = stocks.get_by_value(103);
        auto y2 = stocks.get_by_value(104);
        assert(y0.size() == 0);
        assert(y1.size() == 1);
        assert(y2.size() == 2);
        assert(std::find(y1.begin(), y1.end(), "stock_0003") != y1.end());
        assert(std::find(y2.begin(), y2.end(), "stock_0002") != y2.end());
        assert(std::find(y2.begin(), y2.end(), "stock_0001") != y2.end());
  
        auto z0 = stocks.get_top(2);
        auto z1 = stocks.get_top(4);
        auto z2 = stocks.get_top(6);
        assert(z0.size() == 2);
        assert(z1.size() == 4);
        assert(z2.size() == 4);
        assert(z0[0].second == 105);
        assert(z0[1].second == 104);
        assert(z1[0].second == 105);
        assert(z1[1].second == 104);
        assert(z1[2].second == 104);
        assert(z1[3].second == 103);
        assert(z2[0].second == 105);
        assert(z2[1].second == 104);
        assert(z2[2].second == 104);
        assert(z2[3].second == 103);
        assert(z0[0].first == "stock_0000");
        assert(z1[0].first == "stock_0000"); 
        assert(z1[3].first == "stock_0003"); 
    }
    print_summary("value indexed map", "succeeded");
}


void test_tree_dual()
{
    test_lru_map();
    test_value_indexed_map();
}
