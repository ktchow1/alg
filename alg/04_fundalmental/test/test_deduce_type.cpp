#include<iostream>
#include<cassert>
#include<cstdint>
#include<utility.h>



namespace toy_example
{
    struct M{};
    struct X
    { 
        M    m0; 
        M&   m1;
        M&&  m2;

        // Default constructor is not implemented by default, as there is user defined copy constructor.
        X(M   _m0, 
          M&  _m1, 
          M&& _m2) : m0(_m0)
                   , m1(_m1)
                   , m2(std::move(_m2))
        {
        }

        // Copy constructor is not implemented by default, as there are reference type members.
        X(const X& rhs) : m0(rhs.m0)
                        , m1(rhs.m1)
                        , m2(std::move(rhs.m2))
        {
        }
    };

    M        m; 
    X        named_x{m,m,std::move(m)};
    X&       named_rx  = named_x;
    const X& named_crx = named_x;
    X&&      named_rrx = std::move(named_x);
    X        unnamed_x()   { return named_x; }
    X&       unnamed_rx()  { return named_x; } 
    const X& unnamed_crx() { return named_x; } 
    X&&      unnamed_rrx() { return std::move(named_x); } 


    std::uint32_t bind = 0;
    template<typename T> void fct(T&)  { bind = 1; }
    template<typename T> void fct(T&&) { bind = 2; }
}
using namespace toy_example;



void test_decltype()
{
    // decltype(x)
    static_assert(std::is_same_v<decltype(named_x  ),       X>,   "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rx ),       X&>,  "decltype() failure");
    static_assert(std::is_same_v<decltype(named_crx), const X&>,  "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rrx),       X&&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(std::move(named_x)  ),       X&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_rx) ),       X&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_crx)), const X&&>, "decltype() failure"); // What does it mean by const X&&?
    static_assert(std::is_same_v<decltype(std::move(named_rrx)),       X&&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(unnamed_x()  ),       X>,   "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rx() ),       X&>,  "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_crx()), const X&>,  "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rrx()),       X&&>, "decltype() failure");

    // decltype((x)) 
    static_assert(std::is_same_v<decltype((named_x  )),       X&>, "decltype(()) failure");             // explanation :       X   + & =       X&
    static_assert(std::is_same_v<decltype((named_rx )),       X&>, "decltype(()) failure");             // explanation :       X&  + & =       X&
    static_assert(std::is_same_v<decltype((named_crx)), const X&>, "decltype(()) failure");             // explanation : const X&  + & = const X&
    static_assert(std::is_same_v<decltype((named_rrx)),       X&>, "decltype(()) failure");             // explanation :       X&& + & =       X&

    static_assert(std::is_same_v<decltype((std::move(named_x)  )),       X&&>, "decltype(()) failure"); // explanation :       X&& + && =       X&&
    static_assert(std::is_same_v<decltype((std::move(named_rx) )),       X&&>, "decltype(()) failure"); // explanation :       X&& + && =       X&&
    static_assert(std::is_same_v<decltype((std::move(named_crx))), const X&&>, "decltype(()) failure"); // explanation : const X&& + && = const X&&
    static_assert(std::is_same_v<decltype((std::move(named_rrx))),       X&&>, "decltype(()) failure"); // explanation :       X&& + && =       X&&

    static_assert(std::is_same_v<decltype((unnamed_x()  )),       X>,   "decltype(()) failure");        // explanation :       X   + nothing =       X  
    static_assert(std::is_same_v<decltype((unnamed_rx() )),       X&>,  "decltype(()) failure");        // explanation :       X&  + nothing =       X& 
    static_assert(std::is_same_v<decltype((unnamed_crx())), const X&>,  "decltype(()) failure");        // explanation : const X&  + nothing = const X& 
    static_assert(std::is_same_v<decltype((unnamed_rrx())),       X&&>, "decltype(()) failure");        // explanation :       X&& + nothing =       X&&
                                                                                                        //               <---+--->             <---+--->
                                                                                                        //                   |                     |  
                                                                                                        //                   |                  decltype(())
                                                                                                        //                decltype() 
    // valueness check (bind to which function?)
    fct(named_x  );                 assert((bind == 1, "valueness failure"));
    fct(named_rx );                 assert((bind == 1, "valueness failure"));
    fct(named_crx);                 assert((bind == 1, "valueness failure"));
    fct(named_rrx);                 assert((bind == 1, "valueness failure"));

    fct(std::move(named_x)  );      assert((bind == 2, "valueness failure"));
    fct(std::move(named_rx) );      assert((bind == 2, "valueness failure"));
    fct(std::move(named_crx));      assert((bind == 2, "valueness failure")); 
    fct(std::move(named_rrx));      assert((bind == 2, "valueness failure"));

    fct(unnamed_x()  );             assert((bind == 2, "valueness failure"));
    fct(unnamed_rx() );             assert((bind == 2, "valueness failure"));
    fct(unnamed_crx());             assert((bind == 2, "valueness failure"));
    fct(unnamed_rrx());             assert((bind == 2, "valueness failure"));

    print_summary("decltype", "succeeded in compile time");
}



void test_decltype_member_access()
{
    // decltype(x.m0)
    static_assert(std::is_same_v<decltype(named_x  .m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rx .m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_crx.m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rrx.m0), M>, "decltype() failure");

    static_assert(std::is_same_v<decltype(std::move(named_x)  .m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_rx) .m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_crx).m0), M>, "decltype() failure"); 
    static_assert(std::is_same_v<decltype(std::move(named_rrx).m0), M>, "decltype() failure");

    static_assert(std::is_same_v<decltype(unnamed_x()  .m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rx() .m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_crx().m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rrx().m0), M>, "decltype() failure");

    // decltype(x.m1)
    static_assert(std::is_same_v<decltype(named_x  .m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rx .m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_crx.m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rrx.m1), M&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(std::move(named_x)  .m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_rx) .m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_crx).m1), M&>, "decltype() failure"); 
    static_assert(std::is_same_v<decltype(std::move(named_rrx).m1), M&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(unnamed_x()  .m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rx() .m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_crx().m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rrx().m1), M&>, "decltype() failure");

    // decltype(x.m2)
    static_assert(std::is_same_v<decltype(named_x  .m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rx .m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_crx.m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rrx.m2), M&&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(std::move(named_x)  .m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_rx) .m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_crx).m2), M&&>, "decltype() failure"); 
    static_assert(std::is_same_v<decltype(std::move(named_rrx).m2), M&&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(unnamed_x()  .m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rx() .m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_crx().m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rrx().m2), M&&>, "decltype() failure");


    // [Explanation]
    // +  means valueness       progagted from object to expression using rule 1 in README.txt
    // () means const/reference progagted from object to member     using rule 2 in README.txt

    // decltype((x.m0))
    static_assert(std::is_same_v<decltype((named_x  .m0)),       M&>, "decltype(()) failure");             // explanation : M +       &  =       M&  (expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rx .m0)),       M&>, "decltype(()) failure");             // explanation : M +       &  =       M&  (expression is lvalue)
    static_assert(std::is_same_v<decltype((named_crx.m0)), const M&>, "decltype(()) failure");             // explanation : M + const &  = const M&  (expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rrx.m0)),       M&>, "decltype(()) failure");             // explanation : M +       &  =       M&  (expression is lvalue)

    static_assert(std::is_same_v<decltype((std::move(named_x)  .m0)),       M&&>, "decltype(()) failure"); // explanation : M +       &  =       M&& (expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rx) .m0)),       M&&>, "decltype(()) failure"); // explanation : M +       &  =       M&& (expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_crx).m0)), const M&&>, "decltype(()) failure"); // explanation : M + const &  = const M&& (expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rrx).m0)),       M&&>, "decltype(()) failure"); // explanation : M +       &  =       M&& (expression is xvalue)

    static_assert(std::is_same_v<decltype((unnamed_x()  .m0)),       M&&>, "decltype(()) failure");        // explanation : M +       &  =       M&& (expression is prvalue)
    static_assert(std::is_same_v<decltype((unnamed_rx() .m0)),       M&>,  "decltype(()) failure");        // explanation : M +       &  =       M&  (expression is prvalue)
    static_assert(std::is_same_v<decltype((unnamed_crx().m0)), const M&>,  "decltype(()) failure");        // explanation : M + const &  = const M&  (expression is prvalue)
    static_assert(std::is_same_v<decltype((unnamed_rrx().m0)),       M&&>, "decltype(()) failure");        // explanation : M +       &  =       M&& (expression is prvalue)

    // decltype((x.m1))
    static_assert(std::is_same_v<decltype((named_x  .m1)), M&>, "decltype(()) failure");                   // explanation : M& + & = M& (expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rx .m1)), M&>, "decltype(()) failure");                   // explanation : M& + & = M& (expression is lvalue)
    static_assert(std::is_same_v<decltype((named_crx.m1)), M&>, "decltype(()) failure");                   // explanation : M& + & = M& (expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rrx.m1)), M&>, "decltype(()) failure");                   // explanation : M& + & = M& (expression is lvalue)
                                               
    static_assert(std::is_same_v<decltype((std::move(named_x)  .m1)), M&>, "decltype(()) failure");        // explanation : M& + & = M& (expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rx) .m1)), M&>, "decltype(()) failure");        // explanation : M& + & = M& (expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_crx).m1)), M&>, "decltype(()) failure");        // explanation : M& + & = M& (expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rrx).m1)), M&>, "decltype(()) failure");        // explanation : M& + & = M& (expression is xvalue)

    static_assert(std::is_same_v<decltype((unnamed_x()  .m1)), M&>, "decltype((())failure");               // explanation : M& + & = M& (expression is prvalue)
    static_assert(std::is_same_v<decltype((unnamed_rx() .m1)), M&>, "decltype((())failure");               // explanation : M& + & = M& (expression is prvalue)
    static_assert(std::is_same_v<decltype((unnamed_crx().m1)), M&>, "decltype((())failure");               // explanation : M& + & = M& (expression is prvalue)
    static_assert(std::is_same_v<decltype((unnamed_rrx().m1)), M&>, "decltype((())failure");               // explanation : M& + & = M& (expression is prvalue)

    // decltype((x.m2))
    static_assert(std::is_same_v<decltype((named_x  .m2)), M&>, "decltype(()) failure");                   // explanation : M&& + & = M& (expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rx .m2)), M&>, "decltype(()) failure");                   // explanation : M&& + & = M& (expression is lvalue)
    static_assert(std::is_same_v<decltype((named_crx.m2)), M&>, "decltype(()) failure");                   // explanation : M&& + & = M& (expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rrx.m2)), M&>, "decltype(()) failure");                   // explanation : M&& + & = M& (expression is lvalue)
  
    static_assert(std::is_same_v<decltype((std::move(named_x)  .m2)), M&>, "decltype(()) failure");        // explanation : M&& + & = M& (expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rx) .m2)), M&>, "decltype(()) failure");        // explanation : M&& + & = M& (expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_crx).m2)), M&>, "decltype(()) failure");        // explanation : M&& + & = M& (expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rrx).m2)), M&>, "decltype(()) failure");        // explanation : M&& + & = M& (expression is xvalue)
 
    static_assert(std::is_same_v<decltype((unnamed_x()  .m2)), M&>, "decltype(()) failure");               // explanation : M&& + & = M& (expression is prvalue)
    static_assert(std::is_same_v<decltype((unnamed_rx() .m2)), M&>, "decltype(()) failure");               // explanation : M&& + & = M& (expression is prvalue)
    static_assert(std::is_same_v<decltype((unnamed_crx().m2)), M&>, "decltype(()) failure");               // explanation : M&& + & = M& (expression is prvalue)
    static_assert(std::is_same_v<decltype((unnamed_rrx().m2)), M&>, "decltype(()) failure");               // explanation : M&& + & = M& (expression is prvalue)

    print_summary("decltype for member access", "succeeded in compile time");
}


/*
void test_auto_summary()
{
    int    i = 123;
    auto   x = i;     
    auto&  y = i;    
    auto&& z = i;   
    auto&& w = std::move(i); 
        
    static_assert(std::is_same_v<decltype(x), int>,   "deduce by auto failed"); 
    static_assert(std::is_same_v<decltype(y), int&>,  "deduce by auto failed"); // auto&  can bind to lvalue only
    static_assert(std::is_same_v<decltype(z), int&>,  "deduce by auto failed"); // auto&& can bind to lvalue and rvalue
    static_assert(std::is_same_v<decltype(w), int&&>, "deduce by auto failed");

    POD pod{123, i, std::move(i)};

    static_assert(std::is_same_v<decltype(pod.a), int>,   "deduce by auto failed");
    static_assert(std::is_same_v<decltype(pod.b), int&>,  "deduce by auto failed");
    static_assert(std::is_same_v<decltype(pod.c), int&&>, "deduce by auto failed");
    static_assert(std::is_same_v<decltype(std::move(pod).a), int>,   "deduce by auto failed");
    static_assert(std::is_same_v<decltype(std::move(pod).b), int&>,  "deduce by auto failed");
    static_assert(std::is_same_v<decltype(std::move(pod).c), int&&>, "deduce by auto failed");
    static_assert(std::is_same_v<decltype(POD{123, i, std::move(i)}.a), int>,   "deduce by auto failed");
    static_assert(std::is_same_v<decltype(POD{123, i, std::move(i)}.b), int&>,  "deduce by auto failed");
    static_assert(std::is_same_v<decltype(POD{123, i, std::move(i)}.c), int&&>, "deduce by auto failed");
    
    static_assert(std::is_same_v<decltype((pod.a)), int&>, "deduce by auto failed");                        // expression is lvalue, hence : int   + &
    static_assert(std::is_same_v<decltype((pod.b)), int&>, "deduce by auto failed");                        // expression is lvalue, hence : int&  + &
    static_assert(std::is_same_v<decltype((pod.c)), int&>, "deduce by auto failed");                        // expression is lvalue, hence : int&& + & 
    static_assert(std::is_same_v<decltype((std::move(pod).a)), int&&>, "deduce by auto failed");            // expression is xvalue, hence : int   + && = int&&
    static_assert(std::is_same_v<decltype((std::move(pod).b)), int&>,  "deduce by auto failed");            // expression is lvalue, hence : int&  + &  = int&
    static_assert(std::is_same_v<decltype((std::move(pod).c)), int&>,  "deduce by auto failed");            // expression is lvalue, hence : int&& + &  = int&
    static_assert(std::is_same_v<decltype((POD{123, i, std::move(i)}.a)), int&&>, "deduce by auto failed"); // expression is xvalue, hence : int   + && = int&&
    static_assert(std::is_same_v<decltype((POD{123, i, std::move(i)}.b)), int&>,  "deduce by auto failed"); // expression is lvalue, hence : int&  + &  = int&
    static_assert(std::is_same_v<decltype((POD{123, i, std::move(i)}.c)), int&>,  "deduce by auto failed"); // expression is lvalue, hence : int&& + &  = int&

    print_summary("deduce - by auto (summary)", "succeeded in compile time");
}



void test_auto()
{
          X    x;
          X&  rx = x;
    const X& rcx = x;
          X*  px = new X;
    const X* pcx = px;

    auto a0 = x;                     static_assert(std::is_same_v<decltype(a0),       X>,     "incorrect auto deduce");
    auto a1 = rx;                    static_assert(std::is_same_v<decltype(a1),       X>,     "incorrect auto deduce");
    auto a2 = rcx;                   static_assert(std::is_same_v<decltype(a2),       X>,     "incorrect auto deduce");
    auto a3 = px;                    static_assert(std::is_same_v<decltype(a3),       X*>,    "incorrect auto deduce");
    auto a4 = pcx;                   static_assert(std::is_same_v<decltype(a4), const X*>,    "incorrect auto deduce");
    auto a5 = px->m;                 static_assert(std::is_same_v<decltype(a5),       M>,     "incorrect auto deduce");
    auto a6 = pcx->m;                static_assert(std::is_same_v<decltype(a6),       M>,     "incorrect auto deduce");

    auto& b0 = x;                    static_assert(std::is_same_v<decltype(b0),       X  &>,  "incorrect auto deduce");
    auto& b1 = rx;                   static_assert(std::is_same_v<decltype(b1),       X  &>,  "incorrect auto deduce");
    auto& b2 = rcx;                  static_assert(std::is_same_v<decltype(b2), const X  &>,  "incorrect auto deduce");
    auto& b3 = px;                   static_assert(std::is_same_v<decltype(b3),       X* &>,  "incorrect auto deduce"); // <--- please note
    auto& b4 = pcx;                  static_assert(std::is_same_v<decltype(b4), const X* &>,  "incorrect auto deduce"); // <--- please note
    auto& b5 = px->m;                static_assert(std::is_same_v<decltype(b5),       M  &>,  "incorrect auto deduce");
    auto& b6 = pcx->m;               static_assert(std::is_same_v<decltype(b6), const M  &>,  "incorrect auto deduce");

    // Assignment statments are lvalue, hence same as b0-b6
    auto&& c0 = x;                   static_assert(std::is_same_v<decltype(c0),       X  &>,  "incorrect auto deduce");
    auto&& c1 = rx;                  static_assert(std::is_same_v<decltype(c1),       X  &>,  "incorrect auto deduce");
    auto&& c2 = rcx;                 static_assert(std::is_same_v<decltype(c2), const X  &>,  "incorrect auto deduce");
    auto&& c3 = px;                  static_assert(std::is_same_v<decltype(c3),       X* &>,  "incorrect auto deduce");
    auto&& c4 = pcx;                 static_assert(std::is_same_v<decltype(c4), const X* &>,  "incorrect auto deduce");
    auto&& c5 = px->m;               static_assert(std::is_same_v<decltype(c5),       M  &>,  "incorrect auto deduce");
    auto&& c6 = pcx->m;              static_assert(std::is_same_v<decltype(c6), const M  &>,  "incorrect auto deduce");

    // Assignment statments are rvalue
    auto&& d0 = std::move(x);        static_assert(std::is_same_v<decltype(d0),       X  &&>, "incorrect auto deduce");
    auto&& d1 = std::move(rx);       static_assert(std::is_same_v<decltype(d1),       X  &&>, "incorrect auto deduce");
    auto&& d2 = std::move(rcx);      static_assert(std::is_same_v<decltype(d2), const X  &&>, "incorrect auto deduce");
    auto&& d3 = std::move(px);       static_assert(std::is_same_v<decltype(d3),       X* &&>, "incorrect auto deduce");
    auto&& d4 = std::move(pcx);      static_assert(std::is_same_v<decltype(d4), const X* &&>, "incorrect auto deduce");
    auto&& d5 = std::move(px->m);    static_assert(std::is_same_v<decltype(d5),       M  &&>, "incorrect auto deduce");
    auto&& d6 = std::move(pcx->m);   static_assert(std::is_same_v<decltype(d6), const M  &&>, "incorrect auto deduce");

    print_summary("deduce - by auto", "succeeded in compile time");
}

*/



void test_deduce_type()
{
    test_decltype();
    test_decltype_member_access();
//  test_auto_summary();
//  test_auto();
}
