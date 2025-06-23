#include<iostream>
#include<cassert>
#include<function_invoke.h>
#include<function_test.h>
#include<utility.h>
using namespace function_test;


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


void test_std_invoke()
{
    count::instance().reset_counters();
    N_ary_functor f;
    N1_ary_functor f1;

    // ************************** //
    // *** template parameter *** //
    // ************************** //
    {
        // Part 1
        auto s0 = invoke_as_template_para(1, 2, &N_ary_function);   
        auto s1 = invoke_as_template_para(1, 2, N_ary_functor{});
        auto s2 = invoke_as_template_para(1, 2, f);           
        auto s3 = invoke_as_template_para(1, 2, [](int,int) -> std::string                      { ++count::instance().N_ary_lambda_count;  return "zzz"; });
        auto s4 = invoke_as_template_para(1, 2, std::bind(&N1_ary_function,                       std::placeholders::_1, std::placeholders::_2, "uuu"));
        auto s5 = invoke_as_template_para(1, 2, std::bind(&N1_ary_functor::process, std::ref(f1), std::placeholders::_1, std::placeholders::_2, "vvv"));
        auto s6 = invoke_as_template_para(1, 2, std::bind(                          std::ref(f1), std::placeholders::_1, std::placeholders::_2, "www"));

        assert(count::instance().N_ary_function_count  == 1);   
        assert(count::instance().N_ary_functor_count   == 2);   
        assert(count::instance().N_ary_lambda_count    == 1);   
        assert(count::instance().N1_ary_function_count == 1);   
        assert(count::instance().N1_ary_functor_count  == 1);   
        assert(count::instance().N1_ary_member_count   == 1);   

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
        auto s3 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{[](int,int) -> std::string                      { ++count::instance().N_ary_lambda_count;  return "zzz"; }});
        auto s4 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{std::bind(&N1_ary_function,                       std::placeholders::_1, std::placeholders::_2, "uuu")});
        auto s5 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{std::bind(&N1_ary_functor::process, std::ref(f1), std::placeholders::_1, std::placeholders::_2, "vvv")});
        auto s6 = invoke_as_std_function(1, 2, std::function<std::string(int,int)>{std::bind(                          std::ref(f1), std::placeholders::_1, std::placeholders::_2, "www")});
                                               
        assert(count::instance().N_ary_function_count  == 2);
        assert(count::instance().N_ary_functor_count   == 4);
        assert(count::instance().N_ary_lambda_count    == 2);
        assert(count::instance().N1_ary_function_count == 2);
        assert(count::instance().N1_ary_functor_count  == 2);
        assert(count::instance().N1_ary_member_count   == 2);

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
        auto s3 = invoke_as_std_function<int,std::string>(1, 2, [](int,int) -> std::string                      { ++count::instance().N_ary_lambda_count;  return "zzz"; });
        auto s4 = invoke_as_std_function<int,std::string>(1, 2, std::bind(&N1_ary_function,                       std::placeholders::_1, std::placeholders::_2, "uuu"));
        auto s5 = invoke_as_std_function<int,std::string>(1, 2, std::bind(&N1_ary_functor::process, std::ref(f1), std::placeholders::_1, std::placeholders::_2, "vvv"));
        auto s6 = invoke_as_std_function<int,std::string>(1, 2, std::bind(                          std::ref(f1), std::placeholders::_1, std::placeholders::_2, "www"));

        assert(count::instance().N_ary_function_count  == 3);
        assert(count::instance().N_ary_functor_count   == 6);
        assert(count::instance().N_ary_lambda_count    == 3);
        assert(count::instance().N1_ary_function_count == 3);
        assert(count::instance().N1_ary_functor_count  == 3);
        assert(count::instance().N1_ary_member_count   == 3);

        assert(s0 == std::string{"xxx"});
        assert(s1 == std::string{"yyy"});
        assert(s2 == std::string{"yyy"});
        assert(s3 == std::string{"zzz"});
        assert(s4 == std::string{"uuu"});
        assert(s5 == std::string{"vvv"});
        assert(s6 == std::string{"www"});

    }
    print_summary("std::invoke", "succeeded");
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


void test_function_invoke()
{
    test_std_invoke();
//  test_alg_invoke();
}
