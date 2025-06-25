#include<iostream>
#include<cassert>
#include<function_invoke.h>
#include<function_test.h>
#include<utility.h>
using namespace function_test;
using namespace std::placeholders;


// ***************************** //
// Objective - Test :
// * std::invoke
// * alg::invoke
//
// for all different callables.
// ***************************** //


void test_std_invoke()
{
    print_summary("std::invoke", "succeeded");
}


void test_alg_invoke()
{
    count::instance().reset();
    N_ary_functor f;
    N1_ary_functor f1;

    {
        // Invoke unnamed callables
        auto s0 = alg::invoke_naive(&N_ary_function, 1, 2);
        auto s1 = alg::invoke_naive(N_ary_functor{}, 1, 2);
        auto s2 = alg::invoke_naive(f,               1, 2);           
//      auto sx = alg::invoke_naive(&N_ary_functor::process, std::ref(f), 1, 2); // cannot compile
        auto s3 = alg::invoke_naive([](int,int)
        { 
            ++count::instance().N_ary_lambda;    
            return "uuu"; 
        }, 1, 2);
        auto s4 = alg::invoke_naive(N_ary_std_function, 1, 2);
        auto s5 = alg::invoke_naive(std::bind(&N1_ary_functor::process, std::ref(f1), _1, _2, "vvv"), 1, 2);
        auto s6 = alg::invoke_naive(std::ref(f), 1, 2);

        assert(count::instance().N_ary_function     == 1);   
        assert(count::instance().N_ary_functor      == 3);   
        assert(count::instance().N_ary_lambda       == 1);   
        assert(count::instance().N_ary_std_function == 1);   
        assert(count::instance().N1_ary_member      == 1);   

        assert(s0 == std::string{"xxx"});
        assert(s1 == std::string{"yyy"});
        assert(s2 == std::string{"yyy"});
        assert(s3 == std::string{"uuu"});
        assert(s4 == std::string{"www"});
        assert(s5 == std::string{"vvv"});
        assert(s6 == std::string{"yyy"});
    }

    print_summary("alg::invoke", "succeeded");
}



void test_function_invoke()
{
    test_std_invoke();
    test_alg_invoke();
}
