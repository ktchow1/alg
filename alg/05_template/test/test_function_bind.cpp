#include<iostream>
#include<cassert>
#include<function_bind.h>
#include<function_test.h>
#include<utility.h>
using namespace function_test;


void test_function_bind()
{
    N1_ary_functor f;


    // ************************ //
    // *** Bound to nullary *** //
    // ************************ //
    {
        count::instance().reset();
        auto x0 = alg::bind(&N1_ary_function,  11, 12, "aaa");
        auto x1 = alg::bind( N1_ary_functor{}, 21, 22, "bbb");
        auto x2 = alg::bind(&N1_ary_functor::process, std::ref(f), 31, 32, "ccc");
        auto x3 = alg::bind([](int,int,const std::string& str)
        { 
            ++count::instance().N1_ary_lambda;    
            return str; 
        }, 41, 42, "ddd");
        auto x4 = alg::bind(std::bind(&N1_ary_functor::process, std::ref(f), _1, _2, _3), 51, 52, "eee");
        auto x5 = alg::bind(std::ref(f), 61, 62, "fff");


        // Checking 
        auto s0 = x0();
        auto s1 = x1();
        auto s2 = x2(); 
        auto s3 = x3();
        auto s4 = x4();
        auto s5 = x5();

        assert(count::instance().N1_ary_function == 1);   
        assert(count::instance().N1_ary_functor  == 2);   
        assert(count::instance().N1_ary_lambda   == 1);   
        assert(count::instance().N1_ary_member   == 2); 

        assert(s0 == std::string{"aaa"});
        assert(s1 == std::string{"bbb"});
        assert(s2 == std::string{"ccc"});
        assert(s3 == std::string{"ddd"});
        assert(s4 == std::string{"eee"});
        assert(s5 == std::string{"fff"});
    }


    // ********************** //
    // *** Bound to unary *** //
    // ********************** //
    {
        count::instance().reset();
        auto x0 = alg::bind(&N1_ary_function,  11, 12, alg::placeholders::_1);
        auto x1 = alg::bind( N1_ary_functor{}, 21, 22, alg::placeholders::_1);
        auto x2 = alg::bind(&N1_ary_functor::process, std::ref(f), 31, 32, alg::placeholders::_1);
        auto x3 = alg::bind([](int,int,const std::string& str)
        { 
            ++count::instance().N1_ary_lambda;    
            return str; 
        }, 41, 42, alg::placeholders::_1);
        auto x4 = alg::bind(std::bind(&N1_ary_functor::process, std::ref(f), _1, _2, _3), 51, 52, alg::placeholders::_1);
        auto x5 = alg::bind(std::ref(f), 61, 62, alg::placeholders::_1);


        // Checking 
        auto s0 = x0("aaa");
        auto s1 = x1("bbb");
        auto s2 = x2("ccc"); 
        auto s3 = x3("ddd");
        auto s4 = x4("eee");
        auto s5 = x5("fff");

        assert(count::instance().N1_ary_function == 1);   
        assert(count::instance().N1_ary_functor  == 2);   
        assert(count::instance().N1_ary_lambda   == 1);   
        assert(count::instance().N1_ary_member   == 2); 

        assert(s0 == std::string{"aaa"});
        assert(s1 == std::string{"bbb"});
        assert(s2 == std::string{"ccc"});
        assert(s3 == std::string{"ddd"});
        assert(s4 == std::string{"eee"});
        assert(s5 == std::string{"fff"});
    }


    // *********************** //
    // *** Bound to binary *** //
    // *********************** //
    {
        count::instance().reset();
        auto x0 = alg::bind(&N1_ary_function,  11, alg::placeholders::_1, alg::placeholders::_2);
        auto x1 = alg::bind( N1_ary_functor{}, 21, alg::placeholders::_1, alg::placeholders::_2);
        auto x2 = alg::bind(&N1_ary_functor::process, std::ref(f), 31, alg::placeholders::_1, alg::placeholders::_2);
        auto x3 = alg::bind([](int,int,const std::string& str)
        { 
            ++count::instance().N1_ary_lambda;    
            return str; 
        }, 41, alg::placeholders::_1, alg::placeholders::_2);
        auto x4 = alg::bind(std::bind(&N1_ary_functor::process, std::ref(f), _1, _2, _3), 51, alg::placeholders::_1, alg::placeholders::_2);
        auto x5 = alg::bind(std::ref(f), 61, alg::placeholders::_1, alg::placeholders::_2);


        // Checking 
        auto s0 = x0(12, "aaa");
        auto s1 = x1(22, "bbb");
        auto s2 = x2(32, "ccc"); 
        auto s3 = x3(42, "ddd");
        auto s4 = x4(52, "eee");
        auto s5 = x5(62, "fff");

        assert(count::instance().N1_ary_function == 1);   
        assert(count::instance().N1_ary_functor  == 2);   
        assert(count::instance().N1_ary_lambda   == 1);   
        assert(count::instance().N1_ary_member   == 2); 

        assert(s0 == std::string{"aaa"});
        assert(s1 == std::string{"bbb"});
        assert(s2 == std::string{"ccc"});
        assert(s3 == std::string{"ddd"});
        assert(s4 == std::string{"eee"});
        assert(s5 == std::string{"fff"});
    }
    print_summary("alg::bind", "succeeded");
}
