#include<iostream>
#include<cassert>
#include<function_bind.h>
#include<function_test.h>
#include<utility.h>
using namespace function_test;


void test_function_bind()
{
    count::instance().reset();
    N_ary_functor f;
    N1_ary_functor f1;



    // Invoke unnamed callables
    auto x0 = alg::bind(&N_ary_function, 1, 2);
//  auto x1 = alg::bind(N_ary_functor{}, 1, 2);
//  auto x2 = alg::bind(&N_ary_functor::process, std::ref(f), 1, 2);
//  auto x3 = alg::bind([](int,int)
//  { 
//      ++count::instance().N_ary_lambda;    
//      return "uuu"; 
//  }, 1, 2);
//  auto x4 = alg::invoke_naive(N_ary_std_function, 1, 2);                                      
//  auto x5 = alg::invoke_naive(std::bind(&N1_ary_functor::process, std::ref(f1), _1, _2, "vvv"), 1, 2);
//  auto x6 = alg::invoke_naive(std::ref(f), 1, 2);



    print_summary("alg::bind", "succeeded");
}
