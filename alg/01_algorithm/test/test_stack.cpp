#include<iostream>
#include<iomanip>
#include<cassert>
#include<stack.h>
#include<utility.h>


// ******************************************************************* //
// Compile error in remark 0 
// * cannot resolve FP0 for std::queue<>::push, as there are overloads
// * can    resolve FP1 for alg::queue<>::push, as no overload
// ******************************************************************* //
void test_queue_from_stack()
{
    using comparator_type = alg::container_comparator<std::queue<std::uint32_t>, alg::queue<std::uint32_t>>;
    void (std::queue<std::uint32_t>::* std_push)(const std::uint32_t&)    = &std::queue<std::uint32_t>::push;
    void (alg::queue<std::uint32_t>::* alg_push)(const std::uint32_t&)    = &alg::queue<std::uint32_t>::push;
    void (std::queue<std::uint32_t>::* std_pop)()                         = &std::queue<std::uint32_t>::pop;
    void (alg::queue<std::uint32_t>::* alg_pop)()                         = &alg::queue<std::uint32_t>::pop;
    const std::uint32_t& (std::queue<std::uint32_t>::* std_front)() const = &std::queue<std::uint32_t>::front;
    const std::uint32_t& (alg::queue<std::uint32_t>::* alg_front)() const = &alg::queue<std::uint32_t>::front;
    comparator_type tester;


    // [empty case] apply0 fct_ptr directly
    tester.apply0(&std::queue<std::uint32_t>::push, &alg::queue<std::uint32_t>::push, (std::uint32_t)100);
    tester.apply0(&std::queue<std::uint32_t>::pop,  &alg::queue<std::uint32_t>::pop);
    assert(tester.compare(std_front, alg_front));
     
    // [empty case] apply0 fct_ptr as variable
    tester.apply0(std_push, alg_push, (std::uint32_t)100); 
    tester.apply0(std_pop,  alg_pop);
    assert(tester.compare(std_front, alg_front));
     
    // [empty case] apply1 fct_ptr directly
//  tester.apply1(&std::queue<std::uint32_t>::push, &alg::queue<std::uint32_t>::push, (std::uint32_t)100); // remark 0 
//  tester.apply1(&std::queue<std::uint32_t>::pop,  &alg::queue<std::uint32_t>::pop);                      
    assert(tester.compare(std_front, alg_front)); 
     
    // [empty case] apply1 fct_ptr as variable
    tester.apply1(std_push, alg_push, 100); 
    tester.apply1(std_pop,  alg_pop);
    assert(tester.compare(std_front, alg_front));

    // [normal case]
    std::uint32_t trial = 1000;
    std::uint32_t error = 0;
    for(std::uint32_t t=0; t!=trial; ++t)
    {
        for(std::uint32_t n=0; n!=300; ++n)
        {
            if  (tester.empty() || rand()%3>0)
                 tester.apply1(std_push, alg_push, 100+n);
            else tester.apply1(std_pop,  alg_pop);
        }
        if (!tester.compare(std_front, alg_front)) ++error;
    }
    print_summary("queue_from_stack", error, trial);
}


// ******************************************************** //
// Compare remark 1&2
// * there is  a must to cast the element type for apply0()
// * there is no need to cast the element type for apply1()
// ******************************************************** //
void test_stack_from_queue()
{
    using comparator_type = alg::container_comparator<std::stack<std::uint32_t>, alg::stack<std::uint32_t>>;
    void (std::stack<std::uint32_t>::* std_push)(const std::uint32_t&)  = &std::stack<std::uint32_t>::push;
    void (alg::stack<std::uint32_t>::* alg_push)(const std::uint32_t&)  = &alg::stack<std::uint32_t>::push;
    void (std::stack<std::uint32_t>::* std_pop)()                       = &std::stack<std::uint32_t>::pop;
    void (alg::stack<std::uint32_t>::* alg_pop)()                       = &alg::stack<std::uint32_t>::pop;
    const std::uint32_t& (std::stack<std::uint32_t>::* std_top)() const = &std::stack<std::uint32_t>::top;
    const std::uint32_t& (alg::stack<std::uint32_t>::* alg_top)() const = &alg::stack<std::uint32_t>::top;
    comparator_type tester;


    // [empty case] apply0 fct_ptr as variable
    tester.apply0(std_push, alg_push, (std::uint32_t)100); // remark 1
    tester.apply0(std_pop,  alg_pop);
    assert(tester.compare(std_top, alg_top));

    // [empty case] apply1 fct_ptr as variable
    tester.apply1(std_push, alg_push, 100);                // remark 2
    tester.apply1(std_pop,  alg_pop);
    assert(tester.compare(std_top, alg_top));

    // [normal case]
    std::uint32_t trial = 1000;
    std::uint32_t error = 0;
    for(std::uint32_t t=0; t!=trial; ++t)
    {
        for(std::uint32_t n=0; n!=300; ++n)
        {
            if  (tester.empty() || rand()%3>0)
                 tester.apply1(std_push, alg_push, 100+n);
            else tester.apply1(std_pop,  alg_pop);
        }
        if (!tester.compare(std_top, alg_top)) ++error;
    }
    print_summary("stack_from_queue", error, trial);
}


void test_stack_with_vec()
{
    namespace obj = alg::obj_pool;
    using comparator_type = alg::container_comparator<std::stack<std::uint32_t>, obj::stack<std::uint32_t>>;
    void (std::stack<std::uint32_t>::* std_push)(const std::uint32_t&)  = &std::stack<std::uint32_t>::push;
    void (obj::stack<std::uint32_t>::* obj_push)(const std::uint32_t&)  = &obj::stack<std::uint32_t>::push;
    void (std::stack<std::uint32_t>::* std_pop)()                       = &std::stack<std::uint32_t>::pop;
    void (obj::stack<std::uint32_t>::* obj_pop)()                       = &obj::stack<std::uint32_t>::pop;
    const std::uint32_t& (std::stack<std::uint32_t>::* std_top)() const = &std::stack<std::uint32_t>::top;
    const std::uint32_t& (obj::stack<std::uint32_t>::* obj_top)() const = &obj::stack<std::uint32_t>::top;

    std::stack<std::uint32_t> s0;
    std::stack<std::uint32_t> s1;
    std::stack<std::uint32_t> s2;
    obj::obj_pool<std::uint32_t> pool(1900);
    obj::stack<std::uint32_t> S0(pool);
    obj::stack<std::uint32_t> S1(pool);
    obj::stack<std::uint32_t> S2(pool);
    comparator_type tester0(s0,S0);
    comparator_type tester1(s1,S1);
    comparator_type tester2(s2,S2);

    // [normal case]
    std::uint32_t trial  = 1000;
    std::uint32_t error0 = 0;
    std::uint32_t error1 = 0;
    std::uint32_t error2 = 0;
    for(std::uint32_t t=0; t!=trial; ++t)
    {
        for(std::uint32_t n=0; n!=300; ++n)
        {
            if  (tester0.empty() || rand()%3>0)
                 tester0.apply1(std_push, obj_push, rand()%100);
            else tester0.apply1(std_pop,  obj_pop);
  
            if  (tester1.empty() || rand()%3>0)
                 tester1.apply1(std_push, obj_push, rand()%100);
            else tester1.apply1(std_pop,  obj_pop);

            if  (tester2.empty() || rand()%3>0)
                 tester2.apply1(std_push, obj_push, rand()%100);
            else tester2.apply1(std_pop,  obj_pop);   
        }
        if (!tester0.compare(std_top, obj_top)) ++error0;
        if (!tester1.compare(std_top, obj_top)) ++error1;
        if (!tester2.compare(std_top, obj_top)) ++error2;
    }
    print_summary("stack_with_vec0", error0, trial);
    print_summary("stack_with_vec1", error1, trial);
    print_summary("stack_with_vec2", error2, trial);
}


void test_stack_with_min()
{
    alg::stack_with_min_comparator<alg::stack_with_min> tester;

    // [normal case]
    std::uint32_t trial = 1000;
    std::uint32_t error = 0;
    for(std::uint32_t t=0; t!=trial; ++t)
    {
        for(std::uint32_t n=0; n!=80; ++n)
        {
            if  (tester.empty() || rand()%3>0)
                 tester.push(std::int32_t(100 - n + rand()%80));
            else tester.pop();
        }
        assert(tester.compare());
        tester.clear();
    }
    print_summary("stack_with_min", error, trial);
}


void test_stack()
{
    test_queue_from_stack();
    test_stack_from_queue();
    test_stack_with_vec();
    test_stack_with_min();
}

