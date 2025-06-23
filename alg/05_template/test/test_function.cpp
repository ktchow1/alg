#include<iostream>
#include<cassert>
#include<string>
#include<vector>
#include<function.h>
#include<function_test.h>
#include<utility.h>
using namespace function_test;


// ******************************************************************* //
// Objective - Test these classes :
// * alg::simple_function and 
// * alg::function
//      
// for binding callable :
// 1. function pointer
// 2. functor (rvalue) 
//    functor (lvalue) 
// 3. member pointer          <--- not yet supported, need alg::bind 
// 4. lambda 
// 5. std::function           <--- not yet supported
// 6. std::bind               <--- not yet supported
// 7. std::reference_wrapper  <--- not yet supported
// ******************************************************************* //
void test_alg_simple_function()
{
    reset_counters();
    nullary_functor f;
    std::vector<alg::simple_function> fs; 

    // using raw type
    fs.push_back(&nullary_function);   // <--- cannot compile all these lines, if "explicit" is added to simple_function constructor
    fs.push_back(nullary_functor{});
    fs.push_back(f);                   // <--- cannot compile this line, if std::decay_t is removed
    fs.push_back([]()
    { 
        ++nullary_lambda_count;
    });

    // using simple_function type
    alg::simple_function f0(&nullary_function);
    alg::simple_function f1(nullary_functor{});
    alg::simple_function f2(f);
    alg::simple_function f3([]()
    {
        ++nullary_lambda_count;
    });
    fs.push_back(f0);                  // <--- cannot compile all these lines, if unique_ptr is used instead of shared_ptr inside simple_function
    fs.push_back(f1);                  //      std::unique_ptr makes simple_function non-copyable 
    fs.push_back(f2);                  //      std::shared_ptr makes simple_function     copyable 
    fs.push_back(f3);

    for(const auto& f:fs) f();    
    assert(nullary_function_count == 2);
    assert(nullary_functor_count  == 4);
    assert(nullary_lambda_count   == 2);
    print_summary("alg::function - simple version", "succeeded");
}
  

void test_alg_function()
{
    reset_counters();
    N_ary_functor f;
    std::vector<alg::function<std::string,int,int>> fs; 

    // using raw type
    fs.push_back(&N_ary_function);   
    fs.push_back(N_ary_functor{});
    fs.push_back(f);           
    fs.push_back([](int,int) -> std::string
    {
        ++N_ary_lambda_count;
        return "zzz"; 
    });

    // using simple_function type
    alg::function<std::string,int,int> f0(&N_ary_function);
    alg::function<std::string,int,int> f1(N_ary_functor{});
    alg::function<std::string,int,int> f2(f);
    alg::function<std::string,int,int> f3([](int,int) -> std::string
    {
        ++N_ary_lambda_count;
        return "zzz"; 
    });
    fs.push_back(f0); 
    fs.push_back(f1);
    fs.push_back(f2);
    fs.push_back(f3);

    for(const auto& f:fs) f(123,123);    
    assert(N_ary_function_count == 2);
    assert(N_ary_functor_count  == 4);
    assert(N_ary_lambda_count   == 2);

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

