#include<iostream>
#include<cassert>
#include<string>
#include<vector>
#include<function.h>
#include<utility.h>

std::uint32_t nullary_function_count = 0;
std::uint32_t nullary_functor_count  = 0;
std::uint32_t nullary_lambda_count   = 0;
std::uint32_t N_ary_function_count   = 0;
std::uint32_t N_ary_functor_count    = 0;
std::uint32_t N_ary_lambda_count     = 0;
std::uint32_t N1_ary_function_count  = 0;
std::uint32_t N1_ary_functor_count   = 0;
std::uint32_t N1_ary_member_count    = 0;

void reset_counters()
{
    nullary_function_count = 0;
    nullary_functor_count  = 0;
    nullary_lambda_count   = 0;
    N_ary_function_count   = 0;
    N_ary_functor_count    = 0;
    N_ary_lambda_count     = 0;
    N1_ary_function_count  = 0;
    N1_ary_functor_count   = 0;
    N1_ary_member_count    = 0;
}

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
        return "yyy"; 
    }
};

std::string N1_ary_function(int, int, const std::string& str) 
{
    ++N1_ary_function_count;
    return str; 
}

struct N1_ary_functor
{
    std::string operator()(int, int, const std::string& str) const 
    { 
        ++N1_ary_functor_count;
        return str; 
    }

    std::string process(int, int, const std::string& str) const 
    { 
        ++N1_ary_member_count;
        return str; 
    }
};



// ********************************* //
// Objective - Test these classes :
// * alg::simple_function and 
// * alg::function
//      
// for binding :
// * function pointer
// * functor (rvalue) 
// * functor (lvalue) 
// * lambda 
// ********************************* //
void test_alg_simple_function()
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
    assert(nullary_functor_count  == 4);
    assert(nullary_lambda_count   == 2);
    print_summary("alg::simple_function - nullary returning void", "succeeded");
}
  

void test_alg_function()
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
    print_summary("alg::function - N-ary returning R", "succeeded");
}



// ********************************* //
// Objective - Test these :
// * template parameter
// * std::function
//      
// for binding :
// * function pointer
// * functor (rvalue) 
// * functor (lvalue) 
// * lambda 
// * std::bind function
// * std::bind functor
// * std::bind class member
// ********************************* //
template<typename T, typename F>
auto invoke_as_template_para(T x, T y, const F& fct)
{
    return fct(x,y);
}

template<typename T, typename U>
auto invoke_as_std_function(T x, T y, const std::function<U(T,T)>& fct)
{
    return fct(x,y);
}


void test_std_function()
{
    N_ary_functor f;
    N1_ary_functor f1;
    reset_counters();

    // ************************** //
    // *** template parameter *** //
    // ************************** //
    {
        // Part 1
        auto s0 = invoke_as_template_para(1, 2, &N_ary_function);   
        auto s1 = invoke_as_template_para(1, 2, N_ary_functor{});
        auto s2 = invoke_as_template_para(1, 2, f);           
        auto s3 = invoke_as_template_para(1, 2, [](int,int) -> std::string                      { ++N_ary_lambda_count;  return "zzz"; });
        auto s4 = invoke_as_template_para(1, 2, std::bind(&N1_ary_function,                       std::placeholders::_1, std::placeholders::_2, "uuu"));
        auto s5 = invoke_as_template_para(1, 2, std::bind(&N1_ary_functor::process, std::ref(f1), std::placeholders::_1, std::placeholders::_2, "vvv"));
        auto s6 = invoke_as_template_para(1, 2, std::bind(                          std::ref(f1), std::placeholders::_1, std::placeholders::_2, "www"));

        assert(N_ary_function_count  == 1);
        assert(N_ary_functor_count   == 2);
        assert(N_ary_lambda_count    == 1);
        assert(N1_ary_function_count == 1);
        assert(N1_ary_functor_count  == 1);
        assert(N1_ary_member_count   == 1);

        assert(s0 == std::string{"xxx"});
        assert(s1 == std::string{"yyy"});
        assert(s2 == std::string{"yyy"});
        assert(s3 == std::string{"zzz"});
        assert(s4 == std::string{"uuu"});
        assert(s5 == std::string{"vvv"});
        assert(s6 == std::string{"www"});
    }


    // ********************* //
    // *** std::function *** //
    // ********************* //
    {
        // Part 2A
    //  auto s0 = invoke_as_std_function(1, 2, &N_ary_function);                 // cannot compile
    //  auto s1 = invoke_as_std_function(1, 2, N_ary_functor{});                 // cannot compile
    //  auto s2 = invoke_as_std_function(1, 2, f);                               // cannot compile
    //  auto s3 = invoke_as_std_function(1, 2, [](int,int) -> std::string ...    // cannot compile
      
        // Part 2B
        auto s0 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{&N_ary_function});   
        auto s1 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{N_ary_functor{}});
        auto s2 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{f});           
        auto s3 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{[](int,int) -> std::string                      { ++N_ary_lambda_count;  return "zzz"; }});
        auto s4 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{std::bind(&N1_ary_function,                       std::placeholders::_1, std::placeholders::_2, "uuu")});
        auto s5 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{std::bind(&N1_ary_functor::process, std::ref(f1), std::placeholders::_1, std::placeholders::_2, "vvv")});
        auto s6 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{std::bind(                          std::ref(f1), std::placeholders::_1, std::placeholders::_2, "www")});
                                               
        assert(N_ary_function_count  == 2);
        assert(N_ary_functor_count   == 4);
        assert(N_ary_lambda_count    == 2);
        assert(N1_ary_function_count == 2);
        assert(N1_ary_functor_count  == 2);
        assert(N1_ary_member_count   == 2);

        assert(s0 == std::string{"xxx"});
        assert(s1 == std::string{"yyy"});
        assert(s2 == std::string{"yyy"});
        assert(s3 == std::string{"zzz"});
        assert(s4 == std::string{"uuu"});
        assert(s5 == std::string{"vvv"});
        assert(s6 == std::string{"www"});
    }


    // ***************************************************** //
    // *** std::function (providing template parameters) *** //
    // ***************************************************** //
    {
        auto s0 = invoke_as_std_function<int,std::string>(1, 2, &N_ary_function);   
        auto s1 = invoke_as_std_function<int,std::string>(1, 2, N_ary_functor{});
        auto s2 = invoke_as_std_function<int,std::string>(1, 2, f);           
        auto s3 = invoke_as_std_function<int,std::string>(1, 2, [](int,int) -> std::string                      { ++N_ary_lambda_count;  return "zzz"; });
        auto s4 = invoke_as_std_function<int,std::string>(1, 2, std::bind(&N1_ary_function,                       std::placeholders::_1, std::placeholders::_2, "uuu"));
        auto s5 = invoke_as_std_function<int,std::string>(1, 2, std::bind(&N1_ary_functor::process, std::ref(f1), std::placeholders::_1, std::placeholders::_2, "vvv"));
        auto s6 = invoke_as_std_function<int,std::string>(1, 2, std::bind(                          std::ref(f1), std::placeholders::_1, std::placeholders::_2, "www"));

        assert(N_ary_function_count  == 3);
        assert(N_ary_functor_count   == 6);
        assert(N_ary_lambda_count    == 3);
        assert(N1_ary_function_count == 3);
        assert(N1_ary_functor_count  == 3);
        assert(N1_ary_member_count   == 3);

        assert(s0 == std::string{"xxx"});
        assert(s1 == std::string{"yyy"});
        assert(s2 == std::string{"yyy"});
        assert(s3 == std::string{"zzz"});
        assert(s4 == std::string{"uuu"});
        assert(s5 == std::string{"vvv"});
        assert(s6 == std::string{"www"});

    }
    print_summary("std::function - binding", "succeeded");
}

// *************************************************************************************************************************************** //
// In the above ...
//
// Why part 1 can compile? Because
// - compiler can deduce both T and F from template function instantiation 
// - given 1st and 2nd arg, it deduces T = int 
// - given 3rd arg,         it deduces F = std::string (*)(int,int)     for s0
// - given 3rd arg,         it deduces F = N_ary_functor                for s1
// - given 3rd arg,         it deduces F = N_ary_functor                for s2
// - given 3rd arg,         it deduces F = lambda ...                   for s3
//
// Why part 2A cannot compile?
// - compiler cannot deduce U from template function instantiation
// - given 1st and 2nd arg, it deduces T = int 
// - given 3rd arg,         it cannot match std::function<U(T,T) with std::string (*)(int,int)   for s0
// - given 3rd arg,         it cannot match std::function<U(T,T) with N_ary_functor              for s1
// - given 3rd arg,         it cannot match std::function<U(T,T) with N_ary_functor              for s2
// - given 3rd arg,         it cannot match std::function<U(T,T) with lambda ...                 for s3
//
// Why part 2B can compile?
// - given 3rd arg,         it match std::function<U(T,T) with std::function<std::string(int,int)> to deduce U = std::string   for s0
// - given 3rd arg,         it match std::function<U(T,T) with std::function<std::string(int,int)> to deduce U = std::string   for s1
// - given 3rd arg,         it match std::function<U(T,T) with std::function<std::string(int,int)> to deduce U = std::string   for s2
// - given 3rd arg,         it match std::function<U(T,T) with std::function<std::string(int,int)> to deduce U = std::string   for s3
// *************************************************************************************************************************************** //


void test_function()
{
    test_alg_simple_function();
    test_alg_function();
    test_std_function();
}

