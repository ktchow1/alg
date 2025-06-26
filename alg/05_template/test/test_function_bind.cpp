#include<iostream>
#include<cassert>
#include<function_bind.h>
#include<function_test.h>
#include<utility.h>
using namespace function_test;


void test_function_bind()
{
    count::instance().reset();
    N1_ary_functor f;


    // ************************ //
    // *** Bound to nullary *** //
    // ************************ //
    auto x0 = alg::bind(&N1_ary_function, 11, 12, "aaa");
    auto x1 = alg::bind(N1_ary_functor{}, 21, 22, "bbb");
    auto x2 = alg::bind(&N1_ary_functor::process, std::ref(f), 31, 32, "ccc");
    auto x3 = alg::bind([](int,int,const std::string& str)
    { 
        ++count::instance().N1_ary_lambda;    
        return str; 
    }, 41, 42, "ddd");
    auto x4 = alg::bind(std::bind(&N1_ary_functor::process, std::ref(f), _1, _2, _3), 51, 52, "eee");
    auto x5 = alg::bind(std::ref(f), 61, 62, "fff");


    // Checking 
    auto sx0 = x0();
    auto sx1 = x1();
    auto sx2 = x2(); 
    auto sx3 = x3();
    auto sx4 = x4();
    auto sx5 = x5();

    assert(count::instance().N1_ary_function == 1);   
    assert(count::instance().N1_ary_functor  == 2);   
    assert(count::instance().N1_ary_lambda   == 1);   
    assert(count::instance().N1_ary_member   == 2); 

    assert(sx0 == std::string{"aaa"});
    assert(sx1 == std::string{"bbb"});
    assert(sx2 == std::string{"ccc"});
    assert(sx3 == std::string{"ddd"});
    assert(sx4 == std::string{"eee"});
    assert(sx5 == std::string{"fff"});



    // ********************** //
    // *** Bound to unary *** //
    // ********************** //
//  auto y0 = alg::bind(&N1_ary_function, 1, 2, alg::placeholders::_1);
//  auto y1 = alg::bind(N1_ary_functor{}, 1, 2, alg::placeholders::_1);
//  auto y2 = alg::bind(&N1_ary_functor::process, std::ref(f), 1, 2, alg::placeholders::_1);
//  auto y3 = alg::bind([](int,int, const std::string& str)
//  { 
//      ++count::instance().N1_ary_lambda;    
//      return str; 
//  }, 1, 2, alg::placeholders::_1);
//  auto y4 = alg::bind(std::bind(&N1_ary_functor::process, std::ref(f1), _1, _2), 1, 2, alg::placeholders::_1);
//  auto y5 = alg::bind(std::ref(f1), 1, 2, alg::placeholders::_1);




//  y0("xxx");
//  y1("yyy");
//  y2("zzz"); 
//  y3("www");
//  y4("uuu");
//  y5("vvv");

    print_summary("alg::bind", "succeeded");
}
