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
    const X& named_crx = named_x;
    X&       named_rx  = named_x;
    X&&      named_rrx = std::move(named_x);
    const X& unnamed_crx() { return named_x; } 
    X&       unnamed_rx()  { return named_x; } 
    X&&      unnamed_rrx() { return std::move(named_x); } 
    X        unnamed_x()   { return named_x; }


    std::uint32_t bind = 0;
    template<typename T> void fct(T&)  { bind = 1; }
    template<typename T> void fct(T&&) { bind = 2; }
}
using namespace toy_example;



void test_valueness()
{
    assert( std::is_lvalue_reference_v<decltype((named_crx))> && !std::is_rvalue_reference_v<decltype((named_crx))>); // lvalue
    assert( std::is_lvalue_reference_v<decltype((named_rx ))> && !std::is_rvalue_reference_v<decltype((named_rx ))>); // lvalue
    assert( std::is_lvalue_reference_v<decltype((named_rrx))> && !std::is_rvalue_reference_v<decltype((named_rrx))>); // lvalue
    assert( std::is_lvalue_reference_v<decltype((named_x  ))> && !std::is_rvalue_reference_v<decltype((named_x  ))>); // lvalue

    assert(!std::is_lvalue_reference_v<decltype((std::move(named_crx)))> && std::is_rvalue_reference_v<decltype((std::move(named_crx)))>); // xvalue
    assert(!std::is_lvalue_reference_v<decltype((std::move(named_rx )))> && std::is_rvalue_reference_v<decltype((std::move(named_rx )))>); // xvalue
    assert(!std::is_lvalue_reference_v<decltype((std::move(named_rrx)))> && std::is_rvalue_reference_v<decltype((std::move(named_rrx)))>); // xvalue
    assert(!std::is_lvalue_reference_v<decltype((std::move(named_x  )))> && std::is_rvalue_reference_v<decltype((std::move(named_x  )))>); // xvalue

    assert( std::is_lvalue_reference_v<decltype((unnamed_crx()))> && !std::is_rvalue_reference_v<decltype((unnamed_crx()))>); //  lvalue
    assert( std::is_lvalue_reference_v<decltype((unnamed_rx() ))> && !std::is_rvalue_reference_v<decltype((unnamed_rx() ))>); //  lvalue
    assert(!std::is_lvalue_reference_v<decltype((unnamed_rrx()))> &&  std::is_rvalue_reference_v<decltype((unnamed_rrx()))>); //  xvalue
    assert(!std::is_lvalue_reference_v<decltype((unnamed_x()  ))> && !std::is_rvalue_reference_v<decltype((unnamed_x()  ))>); // prvalue

    print_summary("deduce valueness", "succeeded");
}



void test_auto()
{
    // *** Simple version *** //
    auto   x0 = named_x;     
    auto&  x1 = named_x;    
    auto&& x2 = named_x;   
    auto&& x3 = std::move(named_x); 
        
    static_assert(std::is_same_v<decltype(x0), X>,   "auto failure"); 
    static_assert(std::is_same_v<decltype(x1), X&>,  "auto failure"); // auto&  can bind to lvalue only
    static_assert(std::is_same_v<decltype(x2), X&>,  "auto failure"); // auto&& can bind to lvalue and rvalue
    static_assert(std::is_same_v<decltype(x3), X&&>, "auto failure");


    // *** Comprehensive version - auto *** //
    auto a0 = named_crx;                static_assert(std::is_same_v<decltype(a0), X>, "incorrect auto deduce");
    auto a1 = named_rx;                 static_assert(std::is_same_v<decltype(a1), X>, "incorrect auto deduce");
    auto a2 = named_rrx;                static_assert(std::is_same_v<decltype(a2), X>, "incorrect auto deduce");
    auto a3 = named_x;                  static_assert(std::is_same_v<decltype(a3), X>, "incorrect auto deduce");

    auto a4 = std::move(named_crx);     static_assert(std::is_same_v<decltype(a4), X>, "incorrect auto deduce");
    auto a5 = std::move(named_rx);      static_assert(std::is_same_v<decltype(a5), X>, "incorrect auto deduce");
    auto a6 = std::move(named_rrx);     static_assert(std::is_same_v<decltype(a6), X>, "incorrect auto deduce");
    auto a7 = std::move(named_x);       static_assert(std::is_same_v<decltype(a7), X>, "incorrect auto deduce");

    auto a8 = unnamed_crx();            static_assert(std::is_same_v<decltype(a8), X>, "incorrect auto deduce");
    auto a9 = unnamed_rx();             static_assert(std::is_same_v<decltype(a9), X>, "incorrect auto deduce");
    auto aA = unnamed_rrx();            static_assert(std::is_same_v<decltype(aA), X>, "incorrect auto deduce");
    auto aB = unnamed_x();              static_assert(std::is_same_v<decltype(aB), X>, "incorrect auto deduce");


    // *** Comprehensive version - auto& *** //
    auto& b0 = named_crx;               static_assert(std::is_same_v<decltype(b0), const X&>, "incorrect auto deduce"); // explanation : retain const for reference type
    auto& b1 = named_rx;                static_assert(std::is_same_v<decltype(b1),       X&>, "incorrect auto deduce");
    auto& b2 = named_rrx;               static_assert(std::is_same_v<decltype(b2),       X&>, "incorrect auto deduce");
    auto& b3 = named_x;                 static_assert(std::is_same_v<decltype(b3),       X&>, "incorrect auto deduce");

    auto& b4 = std::move(named_crx);    static_assert(std::is_same_v<decltype(b4), const X&>, "incorrect auto deduce");
//  auto& b5 = std::move(named_rx);     cannot compile
//  auto& b6 = std::move(named_rrx);    cannot compile
//  auto& b7 = std::move(named_x);      cannot compile

    auto& b8 = unnamed_crx();           static_assert(std::is_same_v<decltype(b8), const X&>, "incorrect auto deduce");
    auto& b9 = unnamed_rx();            static_assert(std::is_same_v<decltype(b9),       X&>, "incorrect auto deduce");
//  auto& bA = unnamed_rrx();           cannot compile 
//  auto& bB = unnamed_x();             cannot compile


    // *** Comprehensive version - auto&& *** //
    auto&& c0 = named_crx;              static_assert(std::is_same_v<decltype(c0), const X&>,  "incorrect auto deduce"); // explanation : named expression is lvalue
    auto&& c1 = named_rx;               static_assert(std::is_same_v<decltype(c1),       X&>,  "incorrect auto deduce"); // explanation : named expression is lvalue
    auto&& c2 = named_rrx;              static_assert(std::is_same_v<decltype(c2),       X&>,  "incorrect auto deduce"); // explanation : named expression is lvalue
    auto&& c3 = named_x;                static_assert(std::is_same_v<decltype(c3),       X&>,  "incorrect auto deduce"); // explanation : named expression is lvalue

    auto&& c4 = std::move(named_crx);   static_assert(std::is_same_v<decltype(c4), const X&&>, "incorrect auto deduce"); // explanation : moved expression is xvalue
    auto&& c5 = std::move(named_rx);    static_assert(std::is_same_v<decltype(c5),       X&&>, "incorrect auto deduce"); // explanation : moved expression is xvalue
    auto&& c6 = std::move(named_rrx);   static_assert(std::is_same_v<decltype(c6),       X&&>, "incorrect auto deduce"); // explanation : moved expression is xvalue
    auto&& c7 = std::move(named_x);     static_assert(std::is_same_v<decltype(c7),       X&&>, "incorrect auto deduce"); // explanation : moved expression is xvalue

    auto&& c8 = unnamed_crx();          static_assert(std::is_same_v<decltype(c8), const X&>,  "incorrect auto deduce"); // explanation : unnamed expression ->  lvalue
    auto&& c9 = unnamed_rx();           static_assert(std::is_same_v<decltype(c9),       X&>,  "incorrect auto deduce"); // explanation : unnamed expression ->  lvalue
    auto&& cA = unnamed_rrx();          static_assert(std::is_same_v<decltype(cA),       X&&>, "incorrect auto deduce"); // explanation : unnamed expression ->  xvalue
    auto&& cB = unnamed_x();            static_assert(std::is_same_v<decltype(cB),       X&&>, "incorrect auto deduce"); // explanation : unnamed expression -> prvalue

    print_summary("deduce by auto", "succeeded in compile time");
}


  
void test_decltype()
{
    // decltype(x)
    static_assert(std::is_same_v<decltype(named_crx), const X&>,  "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rx ),       X&>,  "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rrx),       X&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_x  ),       X>,   "decltype() failure");

    static_assert(std::is_same_v<decltype(std::move(named_crx)), const X&&>, "decltype() failure"); 
    static_assert(std::is_same_v<decltype(std::move(named_rx) ),       X&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_rrx)),       X&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_x)  ),       X&&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(unnamed_crx()), const X&>,  "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rx() ),       X&>,  "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rrx()),       X&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_x()  ),       X>,   "decltype() failure");

    // decltype((x)) 
    static_assert(std::is_same_v<decltype((named_crx)), const X&>, "decltype(()) failure");             // explanation : const X&  + & = const X&
    static_assert(std::is_same_v<decltype((named_rx )),       X&>, "decltype(()) failure");             // explanation :       X&  + & =       X&
    static_assert(std::is_same_v<decltype((named_rrx)),       X&>, "decltype(()) failure");             // explanation :       X&& + & =       X&
    static_assert(std::is_same_v<decltype((named_x  )),       X&>, "decltype(()) failure");             // explanation :       X   + & =       X&

    static_assert(std::is_same_v<decltype((std::move(named_crx))), const X&&>, "decltype(()) failure"); // explanation : const X&& + && = const X&&
    static_assert(std::is_same_v<decltype((std::move(named_rx) )),       X&&>, "decltype(()) failure"); // explanation :       X&& + && =       X&&
    static_assert(std::is_same_v<decltype((std::move(named_rrx))),       X&&>, "decltype(()) failure"); // explanation :       X&& + && =       X&&
    static_assert(std::is_same_v<decltype((std::move(named_x)  )),       X&&>, "decltype(()) failure"); // explanation :       X&& + && =       X&&

    static_assert(std::is_same_v<decltype((unnamed_crx())), const X&>,  "decltype(()) failure");        // explanation : const X&  + &       = const X& 
    static_assert(std::is_same_v<decltype((unnamed_rx() )),       X&>,  "decltype(()) failure");        // explanation :       X&  + &       =       X& 
    static_assert(std::is_same_v<decltype((unnamed_rrx())),       X&&>, "decltype(()) failure");        // explanation :       X&& + &&      =       X&&
    static_assert(std::is_same_v<decltype((unnamed_x()  )),       X>,   "decltype(()) failure");        // explanation :       X   + nothing =       X  
                                                                                                        //               <---+--->   <--+-->   <---+--->
                                                                                                        //                   |          |          |  
                                                                                                        //                   |          |       decltype(())
                                                                                                        //                decltype()    +---------------------- valueness of expression, see test_valueness()
    // valueness check (bind to which function?)
    //
    // rule 1. all named      expressions are  lvalue
    // rule 2. all std::move  expressions are  xvalue (i.e. expiring values)
    // rule 3. all literal    expressions are prvalue (not applicable in this example)
    // rule 4. unnamed return values from factory are  
    // -  lvalue if the factory return T& or const T&
    // -  xvalue if the factory return T&&
    // - prvalue if the factory return T
    //
    fct(named_crx);                 assert((bind == 1, "valueness failure")); //  lvalue
    fct(named_rx );                 assert((bind == 1, "valueness failure")); //  lvalue
    fct(named_rrx);                 assert((bind == 1, "valueness failure")); //  lvalue
    fct(named_x  );                 assert((bind == 1, "valueness failure")); //  lvalue 

    fct(std::move(named_crx));      assert((bind == 2, "valueness failure")); //  xvalue
    fct(std::move(named_rx) );      assert((bind == 2, "valueness failure")); //  xvalue
    fct(std::move(named_rrx));      assert((bind == 2, "valueness failure")); //  xvalue
    fct(std::move(named_x)  );      assert((bind == 2, "valueness failure")); //  xvalue

    fct(unnamed_crx());             assert((bind == 2, "valueness failure")); //  lvalue
    fct(unnamed_rx() );             assert((bind == 2, "valueness failure")); //  lvalue
    fct(unnamed_rrx());             assert((bind == 2, "valueness failure")); //  xvalue
    fct(unnamed_x()  );             assert((bind == 2, "valueness failure")); // prvalue

    print_summary("deduce by decltype", "succeeded in compile time");
}



void test_decltype_member_access()
{
    // decltype(x.m0)
    static_assert(std::is_same_v<decltype(named_crx.m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rx .m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rrx.m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_x  .m0), M>, "decltype() failure");

    static_assert(std::is_same_v<decltype(std::move(named_crx).m0), M>, "decltype() failure"); 
    static_assert(std::is_same_v<decltype(std::move(named_rx) .m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_rrx).m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_x)  .m0), M>, "decltype() failure");

    static_assert(std::is_same_v<decltype(unnamed_crx().m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rx() .m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rrx().m0), M>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_x()  .m0), M>, "decltype() failure");

    // decltype(x.m1)
    static_assert(std::is_same_v<decltype(named_crx.m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rx .m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rrx.m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_x  .m1), M&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(std::move(named_crx).m1), M&>, "decltype() failure"); 
    static_assert(std::is_same_v<decltype(std::move(named_rx) .m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_rrx).m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_x)  .m1), M&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(unnamed_crx().m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rx() .m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rrx().m1), M&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_x()  .m1), M&>, "decltype() failure");

    // decltype(x.m2)
    static_assert(std::is_same_v<decltype(named_crx.m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rx .m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_rrx.m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(named_x  .m2), M&&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(std::move(named_crx).m2), M&&>, "decltype() failure"); 
    static_assert(std::is_same_v<decltype(std::move(named_rx) .m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_rrx).m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(std::move(named_x)  .m2), M&&>, "decltype() failure");

    static_assert(std::is_same_v<decltype(unnamed_crx().m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rx() .m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_rrx().m2), M&&>, "decltype() failure");
    static_assert(std::is_same_v<decltype(unnamed_x()  .m2), M&&>, "decltype() failure");


    // [Explanation]
    //  +   means valueness       progagted from object to expression using rule 1 in README.txt
    // (->) means const/reference progagted from object to member     using rule 2 in README.txt  
    //
    // Please refer to test_valueness() for the valueness of object.
                                                                                                           //                   type of object = decltype() in test_decltype(())
                                                                                                           //                         |
    // decltype((x.m0))                                                                                    //                     <---+--->
    static_assert(std::is_same_v<decltype((named_crx.m0)), const M&>, "decltype(()) failure");             // explanation : (M -> const M& ) + &  = const M&  (object is lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rx .m0)),       M&>, "decltype(()) failure");             // explanation : (M ->       M& ) + &  =       M&  (object is lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rrx.m0)),       M&>, "decltype(()) failure");             // explanation : (M ->       M& ) + &  =       M&  (object is lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((named_x  .m0)),       M&>, "decltype(()) failure");             // explanation : (M ->       M& ) + &  =       M&  (object is lvalue, expression is lvalue)

    static_assert(std::is_same_v<decltype((std::move(named_crx).m0)), const M&&>, "decltype(()) failure"); // explanation : (M -> const M&&) + && = const M&& (object is xvalue, expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rx) .m0)),       M&&>, "decltype(()) failure"); // explanation : (M ->       M&&) + && =       M&& (object is xvalue, expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rrx).m0)),       M&&>, "decltype(()) failure"); // explanation : (M ->       M&&) + && =       M&& (object is xvalue, expression is xvalue)
    static_assert(std::is_same_v<decltype((std::move(named_x)  .m0)),       M&&>, "decltype(()) failure"); // explanation : (M ->       M&&) + && =       M&& (object is xvalue, expression is xvalue)

    static_assert(std::is_same_v<decltype((unnamed_crx().m0)), const M&>,  "decltype(()) failure");        // explanation : (M -> const M& ) + &  = const M&  (object is  lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((unnamed_rx() .m0)),       M&>,  "decltype(()) failure");        // explanation : (M ->       M& ) + &  =       M&  (object is  lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((unnamed_rrx().m0)),       M&&>, "decltype(()) failure");        // explanation : (M ->       M&&) + && =       M&& (object is  xvalue, expression is xvalue)
    static_assert(std::is_same_v<decltype((unnamed_x()  .m0)),       M&&>, "decltype(()) failure");        // explanation : (M ->       M  ) + && =       M&& (object is prvalue, expression is xvalue)

    // decltype((x.m1))
    static_assert(std::is_same_v<decltype((named_crx.m1)), M&>, "decltype(()) failure");                   // explanation : M& + & = M& (object is lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rx .m1)), M&>, "decltype(()) failure");                   // explanation : M& + & = M& (object is lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rrx.m1)), M&>, "decltype(()) failure");                   // explanation : M& + & = M& (object is lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((named_x  .m1)), M&>, "decltype(()) failure");                   // explanation : M& + & = M& (object is lvalue, expression is lvalue)
                                               
    static_assert(std::is_same_v<decltype((std::move(named_crx).m1)), M&>, "decltype(()) failure");        // explanation : M& + & = M& (object is xvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rx) .m1)), M&>, "decltype(()) failure");        // explanation : M& + & = M& (object is xvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rrx).m1)), M&>, "decltype(()) failure");        // explanation : M& + & = M& (object is xvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((std::move(named_x)  .m1)), M&>, "decltype(()) failure");        // explanation : M& + & = M& (object is xvalue, expression is lvalue)

    static_assert(std::is_same_v<decltype((unnamed_crx().m1)), M&>, "decltype((())failure");               // explanation : M& + & = M& (object is  lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((unnamed_rx() .m1)), M&>, "decltype((())failure");               // explanation : M& + & = M& (object is  lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((unnamed_rrx().m1)), M&>, "decltype((())failure");               // explanation : M& + & = M& (object is  xvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((unnamed_x()  .m1)), M&>, "decltype((())failure");               // explanation : M& + & = M& (object is prvalue, expression is lvalue)

    // decltype((x.m2))
    static_assert(std::is_same_v<decltype((named_crx.m2)), M&>, "decltype(()) failure");                   // explanation : M&& + & = M& (object is lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rx .m2)), M&>, "decltype(()) failure");                   // explanation : M&& + & = M& (object is lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((named_rrx.m2)), M&>, "decltype(()) failure");                   // explanation : M&& + & = M& (object is lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((named_x  .m2)), M&>, "decltype(()) failure");                   // explanation : M&& + & = M& (object is lvalue, expression is lvalue)
  
    static_assert(std::is_same_v<decltype((std::move(named_crx).m2)), M&>, "decltype(()) failure");        // explanation : M&& + & = M& (object is xvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rx) .m2)), M&>, "decltype(()) failure");        // explanation : M&& + & = M& (object is xvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((std::move(named_rrx).m2)), M&>, "decltype(()) failure");        // explanation : M&& + & = M& (object is xvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((std::move(named_x)  .m2)), M&>, "decltype(()) failure");        // explanation : M&& + & = M& (object is xvalue, expression is lvalue)
 
    static_assert(std::is_same_v<decltype((unnamed_crx().m2)), M&>, "decltype(()) failure");               // explanation : M&& + & = M& (object is  lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((unnamed_rx() .m2)), M&>, "decltype(()) failure");               // explanation : M&& + & = M& (object is  lvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((unnamed_rrx().m2)), M&>, "decltype(()) failure");               // explanation : M&& + & = M& (object is  xvalue, expression is lvalue)
    static_assert(std::is_same_v<decltype((unnamed_x()  .m2)), M&>, "decltype(()) failure");               // explanation : M&& + & = M& (object is prvalue, expression is lvalue)

    print_summary("deduce by decltype (member access)", "succeeded in compile time");
}



void test_deduce_type()
{
    test_valueness();
    test_auto();
    test_decltype();
    test_decltype_member_access();
}
