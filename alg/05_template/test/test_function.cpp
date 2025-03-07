#include<iostream>
#include<cassert>
#include<string>
#include<vector>
#include<function.h>
#include<utility.h>

std::uint32_t nullary_function_count = 0;
std::uint32_t nullary_functor_count = 0;
std::uint32_t nullary_lambda_count = 0;
std::uint32_t N_ary_function_count = 0;
std::uint32_t N_ary_functor_count = 0;
std::uint32_t N_ary_lambda_count = 0;


void nullary_function() 
{ 
    ++nullary_function_count;
}

struct nullary_functor
{
    void operator()() const 
    {
        ++nullary_functor_count;
    }
};

std::string N_ary_function(int, int) 
{
    ++N_ary_function_count;
    return "xxx"; 
}

struct N_ary_functor
{
    std::string operator()(int, int) const 
    { 
        ++N_ary_functor_count;
        return "xxx"; 
    }
};

// *******************************************//
// Objective : function class that can wrap :
// * function pointer
// * functor
// * lambda 
// *******************************************//
void test_simple_function()
{
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
    assert(nullary_functor_count == 4);
    assert(nullary_lambda_count == 2);
    print_summary("alg::simple_function - nullary returning void", "succeeded");
}
  
void test_general_function()
{
    N_ary_functor f;
    std::vector<alg::function<std::string,int,int>> fs; 

    // using raw type
    fs.push_back(&N_ary_function);   
    fs.push_back(N_ary_functor{});
    fs.push_back(f);           
    fs.push_back([](int,int) -> std::string
    {
        ++N_ary_lambda_count;
        return "xxx"; 
    });

    // using simple_function type
    alg::function<std::string,int,int> f0(&N_ary_function);
    alg::function<std::string,int,int> f1(N_ary_functor{});
    alg::function<std::string,int,int> f2(f);
    alg::function<std::string,int,int> f3([](int,int) -> std::string
    {
        ++N_ary_lambda_count;
        return "xxx"; 
    });
    fs.push_back(f0); 
    fs.push_back(f1);
    fs.push_back(f2);
    fs.push_back(f3);

    for(const auto& f:fs) f(123,123);    
    assert(N_ary_function_count == 2);
    assert(N_ary_functor_count == 4);
    assert(N_ary_lambda_count == 2);
    print_summary("alg::function - N-ary returning R", "succeeded");
}

void test_function()
{
    test_simple_function();
    test_general_function();
}

