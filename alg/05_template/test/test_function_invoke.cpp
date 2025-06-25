#include<iostream>
#include<cassert>
#include<function_invoke.h>
#include<function_test.h>
#include<utility.h>
using namespace function_test;
using namespace std::placeholders;


// ***************************************** //
// *** Test of std::invoke & alg::invoke *** //
// ***************************************** //
void test_alg_invoke_naive()
{
    count::instance().reset();
    N_ary_functor f;
    N1_ary_functor f1;

     

    // Invoke unnamed callables
    auto s0 = alg::invoke_naive(&N_ary_function, 1, 2);
    auto s1 = alg::invoke_naive(N_ary_functor{}, 1, 2);
//  auto s2 = alg::invoke_naive(&N_ary_functor::process, std::ref(f), 1, 2); // cannot compile
    auto s3 = alg::invoke_naive([](int,int)
    { 
        ++count::instance().N_ary_lambda;    
        return "uuu"; 
    }, 1, 2);
    auto s4 = alg::invoke_naive(N_ary_std_function, 1, 2);                                      
    auto s5 = alg::invoke_naive(std::bind(&N1_ary_functor::process, std::ref(f1), _1, _2, "vvv"), 1, 2);
    auto s6 = alg::invoke_naive(std::ref(f), 1, 2);



    // Invoke named callables
    auto x0 = &N_ary_function;
    auto x1 = f;           
    auto x2 = &N_ary_functor::process;
    auto x3 = [](int,int)
    { 
        ++count::instance().N_ary_lambda;    
        return "uuu"; 
    };
    auto x4 = N_ary_std_function;                                      
    auto x5 = std::bind(&N1_ary_functor::process, std::ref(f1), _1, _2, "vvv");
    auto x6 = std::ref(f);



    // Checking
    alg::invoke_naive(x0, 1, 2);
    alg::invoke_naive(x1, 1, 2);
//  alg::invoke_naive(x2, std::ref(f), 1, 2); // cannot compile
    alg::invoke_naive(x3, 1, 2);
    alg::invoke_naive(x4, 1, 2);
    alg::invoke_naive(x5, 1, 2);
    alg::invoke_naive(x6, 1, 2);

    assert(count::instance().N_ary_function     == 2);   
    assert(count::instance().N_ary_functor      == 4);   
    assert(count::instance().N_ary_lambda       == 2);   
    assert(count::instance().N_ary_std_function == 2);   
    assert(count::instance().N1_ary_member      == 2); 

    assert(s0 == std::string{"xxx"});
    assert(s1 == std::string{"yyy"});
    assert(s3 == std::string{"uuu"});
    assert(s4 == std::string{"www"});
    assert(s5 == std::string{"vvv"});
    assert(s6 == std::string{"yyy"});
    print_summary("alg::invoke_naive", "succeeded");
}



void test_alg_invoke()
{
    count::instance().reset();
    N_ary_functor f;
    N1_ary_functor f1;

     

    // Invoke unnamed callables
    auto s0 = alg::invoke(&N_ary_function, 1, 2);
    auto s1 = alg::invoke(N_ary_functor{}, 1, 2);
    auto s2 = alg::invoke(&N_ary_functor::process, std::ref(f), 1, 2); 
    auto s3 = alg::invoke([](int,int)
    { 
        ++count::instance().N_ary_lambda;    
        return "uuu"; 
    }, 1, 2);
    auto s4 = alg::invoke(N_ary_std_function, 1, 2);                                      
    auto s5 = alg::invoke(std::bind(&N1_ary_functor::process, std::ref(f1), _1, _2, "vvv"), 1, 2);
    auto s6 = alg::invoke(std::ref(f), 1, 2);



    // Invoke named callables
    auto x0 = &N_ary_function;
    auto x1 = f;           
    auto x2 = &N_ary_functor::process;
    auto x3 = [](int,int)
    { 
        ++count::instance().N_ary_lambda;    
        return "uuu"; 
    };
    auto x4 = N_ary_std_function;                                      
    auto x5 = std::bind(&N1_ary_functor::process, std::ref(f1), _1, _2, "vvv");
    auto x6 = std::ref(f);



    // Checking
    alg::invoke(x0, 1, 2);
    alg::invoke(x1, 1, 2);
    alg::invoke(x2, std::ref(f), 1, 2);
    alg::invoke(x3, 1, 2);
    alg::invoke(x4, 1, 2);
    alg::invoke(x5, 1, 2);
    alg::invoke(x6, 1, 2);

    assert(count::instance().N_ary_function     == 2);   
    assert(count::instance().N_ary_functor      == 4);   
    assert(count::instance().N_ary_member       == 2); 
    assert(count::instance().N_ary_lambda       == 2);   
    assert(count::instance().N_ary_std_function == 2);   
    assert(count::instance().N1_ary_member      == 2); 

    assert(s0 == std::string{"xxx"});
    assert(s1 == std::string{"yyy"});
    assert(s3 == std::string{"uuu"});
    assert(s4 == std::string{"www"});
    assert(s5 == std::string{"vvv"});
    assert(s6 == std::string{"yyy"});
    print_summary("alg::invoke", "succeeded");
}



void test_function_invoke()
{
    test_alg_invoke_naive();
    test_alg_invoke();
}
