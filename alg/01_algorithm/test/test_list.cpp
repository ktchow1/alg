#include<iostream>
#include<iomanip>
#include<cassert>
#include<deque>
#include<list.h>
#include<utility.h>


void test_singly_list()
{
    alg::list::singly_list<std::uint32_t> list;
    assert(alg::list::compare(list, {}));

    auto n0 = list.insert_before_head((std::uint32_t)0);
    assert(alg::list::compare(list, {0}));

    list.reverse();
    assert(alg::list::compare(list, {0}));

    auto n1 = list.insert_before_head((std::uint32_t)1);
    auto n2 = list.insert_before_head((std::uint32_t)2);
    auto n3 = list.insert_before_head((std::uint32_t)3);
    assert(alg::list::compare(list, {3,2,1,0}));

    auto n4 = list.insert_before(n3, (std::uint32_t)4);
    auto n5 = list.insert_before(n4, (std::uint32_t)5);
    auto n6 = list.insert_before(n5, (std::uint32_t)6);
    assert(alg::list::compare(list, {6,5,4,3,2,1,0}));
     
    auto n7 = list.insert_before(list.end(), (std::uint32_t)7);
    auto n8 = list.insert_before(list.end(), (std::uint32_t)8);
    auto n9 = list.insert_before(list.end(), (std::uint32_t)9);
    assert(alg::list::compare(list, {6,5,4,3,2,1,0,7,8,9}));

    auto n10 = list.insert_before(n7, (std::uint32_t)10);
    auto n11 = list.insert_before(n7, (std::uint32_t)11);
    auto n12 = list.insert_before(n7, (std::uint32_t)12);
    assert(alg::list::compare(list, {6,5,4,3,2,1,0,10,11,12,7,8,9}));

    auto n13 = list.insert_after(n6, (std::uint32_t)13);
    auto n14 = list.insert_after(n6, (std::uint32_t)14);
    auto n15 = list.insert_after(n6, (std::uint32_t)15);
    assert(alg::list::compare(list, {6,15,14,13,5,4,3,2,1,0,10,11,12,7,8,9}));

    auto n16 = list.insert_after(n9,  (std::uint32_t)16);
    auto n17 = list.insert_after(n16, (std::uint32_t)17);
    auto n18 = list.insert_after(n17, (std::uint32_t)18);
    assert(alg::list::compare(list, {6,15,14,13,5,4,3,2,1,0,10,11,12,7,8,9,16,17,18}));

    list.reverse();
    assert(alg::list::compare(list, {18,17,16,9,8,7,12,11,10,0,1,2,3,4,5,13,14,15,6}));
  
    list.reverse();
    assert(alg::list::compare(list, {6,15,14,13,5,4,3,2,1,0,10,11,12,7,8,9,16,17,18}));

    list.erase_head();
    list.erase_head();
    list.erase_head();
    assert(alg::list::compare(list, {13,5,4,3,2,1,0,10,11,12,7,8,9,16,17,18}));

    list.erase(n0);
    list.erase(n2);
    list.erase(n4);
    assert(alg::list::compare(list, {13,5,3,1,10,11,12,7,8,9,16,17,18}));

    list.erase(n18);
    list.erase(n17);
    list.erase(n16);
    assert(alg::list::compare(list, {13,5,3,1,10,11,12,7,8,9}));

    auto n19 = list.insert_after(n1, (std::uint32_t)19);
    auto n20 = list.insert_after(n3, (std::uint32_t)20);
    auto n21 = list.insert_after(n5, (std::uint32_t)21);
    assert(alg::list::compare(list, {13,5,21,3,20,1,19,10,11,12,7,8,9}));

    while(list.empty()) list.erase_head();
    assert(alg::list::compare(list, {}));

    auto n22 = list.insert_before_head((std::uint32_t)22);
    auto n23 = list.insert_before_head((std::uint32_t)23);
    auto n24 = list.insert_before_head((std::uint32_t)24);
    assert(alg::list::compare(list, {24,23,22}));
    print_summary("singly_list", "succeeded [Todo : avoid memleak]");
}

void test_doubly_list()
{
    alg::list::doubly_list<std::uint32_t> list;
    assert(alg::list::compare(list, {}));

    auto n0 = list.insert_before_head((std::uint32_t)0);
    assert(alg::list::compare(list, {0}));

    list.reverse();
    assert(alg::list::compare(list, {0}));

    auto n1 = list.insert_before_head((std::uint32_t)1);
    auto n2 = list.insert_before_head((std::uint32_t)2);
    auto n3 = list.insert_before_head((std::uint32_t)3);
    assert(alg::list::compare(list, {3,2,1,0}));

    auto n4 = list.insert_after_tail((std::uint32_t)4);
    auto n5 = list.insert_after_tail((std::uint32_t)5);
    auto n6 = list.insert_after_tail((std::uint32_t)6);
    assert(alg::list::compare(list, {3,2,1,0,4,5,6}));
  
    auto n7 = list.insert_before(n3, (std::uint32_t)7);
    auto n8 = list.insert_before(n7, (std::uint32_t)8);
    auto n9 = list.insert_before(n8, (std::uint32_t)9);
    assert(alg::list::compare(list, {9,8,7,3,2,1,0,4,5,6}));
     
    auto n10 = list.insert_before(list.end(), (std::uint32_t)10);
    auto n11 = list.insert_before(list.end(), (std::uint32_t)11);
    auto n12 = list.insert_before(list.end(), (std::uint32_t)12);
    assert(alg::list::compare(list, {9,8,7,3,2,1,0,4,5,6,10,11,12}));

    auto n13 = list.insert_before(n10, (std::uint32_t)13);
    auto n14 = list.insert_before(n10, (std::uint32_t)14);
    auto n15 = list.insert_before(n10, (std::uint32_t)15);
    assert(alg::list::compare(list, {9,8,7,3,2,1,0,4,5,6,13,14,15,10,11,12}));
  
    auto n16 = list.insert_after(n9, (std::uint32_t)16);
    auto n17 = list.insert_after(n9, (std::uint32_t)17);
    auto n18 = list.insert_after(n9, (std::uint32_t)18);
    assert(alg::list::compare(list, {9,18,17,16,8,7,3,2,1,0,4,5,6,13,14,15,10,11,12}));

    auto n19 = list.insert_after(n12, (std::uint32_t)19);
    auto n20 = list.insert_after(n19, (std::uint32_t)20);
    auto n21 = list.insert_after(n20, (std::uint32_t)21);
    assert(alg::list::compare(list, {9,18,17,16,8,7,3,2,1,0,4,5,6,13,14,15,10,11,12,19,20,21}));

    list.reverse();
    assert(alg::list::compare(list, {21,20,19,12,11,10,15,14,13,6,5,4,0,1,2,3,7,8,16,17,18,9}));

    list.reverse();
    assert(alg::list::compare(list, {9,18,17,16,8,7,3,2,1,0,4,5,6,13,14,15,10,11,12,19,20,21}));

    list.erase_head();
    list.erase_head();
    list.erase_head();
    assert(alg::list::compare(list, {16,8,7,3,2,1,0,4,5,6,13,14,15,10,11,12,19,20,21}));

    list.erase_tail();
    list.erase_tail();
    list.erase_tail();
    assert(alg::list::compare(list, {16,8,7,3,2,1,0,4,5,6,13,14,15,10,11,12}));

    list.erase(n0);
    list.erase(n2);
    list.erase(n4);
    assert(alg::list::compare(list, {16,8,7,3,1,5,6,13,14,15,10,11,12}));

    list.erase(n10);
    list.erase(n11);
    list.erase(n12);
    assert(alg::list::compare(list, {16,8,7,3,1,5,6,13,14,15}));
  
    auto n22 = list.insert_after(n1, (std::uint32_t)22);
    auto n23 = list.insert_after(n3, (std::uint32_t)23);
    auto n24 = list.insert_after(n5, (std::uint32_t)24);
    assert(alg::list::compare(list, {16,8,7,3,23,1,22,5,24,6,13,14,15}));

    while(list.empty()) list.erase_head();
    assert(alg::list::compare(list, {}));

    auto n25 = list.insert_before_head((std::uint32_t)25);
    auto n26 = list.insert_before_head((std::uint32_t)26);
    auto n27 = list.insert_before_head((std::uint32_t)27);
    assert(alg::list::compare(list, {27,26,25})); 
    print_summary("doubly_list", "succeeded [Todo : avoid memleak]");
}

void test_nth_node_from_tail()
{
    alg::list::singly_list<std::uint32_t> list;
    for(std::uint32_t n=0; n!=10; ++n)
    {
        list.insert_before_head(n);
    } 
    assert(alg::list::compare(list, {9,8,7,6,5,4,3,2,1,0}));

    for(std::uint32_t n=0; n!=10; ++n)
    {
        assert(alg::list::find_nth_node_before_tail(list.begin(), n)->m_value == n);
    } 
    assert(alg::list::find_nth_node_before_tail(list.begin(), 10) == nullptr);
    assert(alg::list::find_nth_node_before_tail(list.begin(), 11) == nullptr);
    assert(alg::list::find_nth_node_before_tail(list.begin(), 12) == nullptr);
    print_summary("nth_node_from_tail", "succeeded");
}

void test_circular_list()
{
    alg::list::singly_list<std::uint32_t> list;
    std::deque<alg::list::node<uint32_t>*> nodes;
    
    for(std::uint32_t n=0; n!=50; ++n)
    {
        nodes.push_front(list.insert_before_head(49-n));
    } 
    alg::list::loop_detection detector(list.begin());
    // nodes[0] == list.begin()   with value 0
    // nodes[1] == list.begin()+1 with value 1
    // nodes[2] == list.begin()+2 with value 2 ... etc


    nodes[49]->m_next = nodes[20];
    assert(detector.is_looped() == true);
    assert(detector.loop_length() == 49-20+1);
    assert(detector.loop_entry_node() == nodes[20]);

    nodes[49]->m_next = nodes[30];
    assert(detector.is_looped() == true);
    assert(detector.loop_length() == 49-30+1);
    assert(detector.loop_entry_node() == nodes[30]);

    nodes[49]->m_next = nodes[40];
    assert(detector.is_looped() == true);
    assert(detector.loop_length() == 49-40+1);
    assert(detector.loop_entry_node() == nodes[40]);

    nodes[49]->m_next = nullptr;
    assert(detector.is_looped() == false);
    print_summary("circular_list", "succeeded");
}

void test_list()
{
    test_singly_list();
    test_doubly_list();
    test_nth_node_from_tail();
    test_circular_list();
}
