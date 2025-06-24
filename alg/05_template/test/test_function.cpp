#include<iostream>
#include<cassert>
#include<string>
#include<vector>
#include<function.h>
#include<function_test.h>
#include<utility.h>
using namespace function_test;


// **************************** //
// Test of alg::simple_function // 
//       & alg::function        //
// **************************** //
void test_alg_simple_function()
{
    count::instance().reset();
    std::vector<alg::simple_function> fs; 
    nullary_functor f;



    // Add unnamed callable
    fs.push_back(&nullary_function);   // <--- cannot compile all these lines, if "explicit" is added to alg::simple_function constructor
    fs.push_back(nullary_functor{});
    fs.push_back(f);        
    fs.push_back([]() { ++count::instance().nullary_lambda; });
    fs.push_back(std::function<void()>{nullary_function});
    fs.push_back(std::bind(nullary_function));
    fs.push_back(std::ref(f));


    // Add named callable
    alg::simple_function f0(&nullary_function);
    alg::simple_function f1(nullary_functor{});
    alg::simple_function f2(f);
    alg::simple_function f3([]() { ++count::instance().nullary_lambda; });
    alg::simple_function f4(std::function<void()>{nullary_function});
    alg::simple_function f5(std::bind(nullary_function));
    alg::simple_function f6(std::ref(f));

    fs.push_back(f0);                  // <--- cannot compile all these lines, if std::unique_ptr is used instead of std::shared_ptr inside alg::simple_function
    fs.push_back(f1);                  //      std::unique_ptr makes simple_function non-copyable 
    fs.push_back(f2);                  //      std::shared_ptr makes simple_function     copyable 
    fs.push_back(f3);
    fs.push_back(f4);
    fs.push_back(f5);
    fs.push_back(f6);



    for(const auto& f:fs) f();    
    assert(count::instance().nullary_function == 6);
    assert(count::instance().nullary_functor  == 6);
    assert(count::instance().nullary_lambda   == 2);
    print_summary("alg::function - simple version", "succeeded");
}
  


void test_alg_function()
{
    count::instance().reset();
    std::vector<alg::function<std::string,int,int>> fs; 
    N_ary_functor f;



    // Add unnamed callable
    fs.push_back(&N_ary_function);   
    fs.push_back(N_ary_functor{});
    fs.push_back(f);           
    fs.push_back([](int,int) -> std::string
    {
        ++count::instance().N_ary_lambda;
        return "zzz"; 
    });


    // Add named callable
    alg::function<std::string,int,int> f0(&N_ary_function);
    alg::function<std::string,int,int> f1(N_ary_functor{});
    alg::function<std::string,int,int> f2(f);
    alg::function<std::string,int,int> f3([](int,int) -> std::string
    {
        ++count::instance().N_ary_lambda;
        return "zzz"; 
    });
    fs.push_back(f0); 
    fs.push_back(f1);
    fs.push_back(f2);
    fs.push_back(f3);



    for(const auto& f:fs) f(123,123);    
    assert(count::instance().N_ary_function == 2);
    assert(count::instance().N_ary_functor  == 4);
    assert(count::instance().N_ary_lambda   == 2);

    assert(fs[0](123,123) == "xxx");
    assert(fs[1](123,123) == "yyy");
    assert(fs[2](123,123) == "yyy");
    assert(fs[3](123,123) == "zzz");
    assert(fs[4](123,123) == "xxx");
    assert(fs[5](123,123) == "yyy");
    assert(fs[6](123,123) == "yyy");
    assert(fs[7](123,123) == "zzz");
    print_summary("alg::function - genera version", "succeeded");
}



void test_function()
{
    test_alg_simple_function();
    test_alg_function();
}

