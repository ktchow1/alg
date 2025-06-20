#include<iostream>
#include<cassert>
#include<cstdint>
#include<utility.h>



// ************ //
// *** auto *** //
// ************ //
// Get type of assignment statement, ignoring const and &, unless specified in auto
//
// If T is typeof assignment statement :
// * for auto                              type = reference_collapse<      const_reference_trim<T>   >
// * for const auto&                       type = reference_collapse<const const_reference_trim<T>  &>
// * for auto&                             type = reference_collapse<            reference_trim<T>  &>
// * for auto&& and lvalue assignment      type = reference_collapse<            reference_trim<T>& &&>
// * for auto&& and rvalue assignment      type = reference_collapse<            reference_trim<T>  &&>  
//
//
// **************** //
// *** decltype *** //
// **************** //
// Get exact type of expression
//
// If T is typeof assignment statement :
// * for simple  expression                type = T 
// * for complex  lvalue expression        type = reference_collapse<T & >  
// * for complex  xvalue expression        type = reference_collapse<T &&> 
// * for complex prvalue expression        type = reference_collapse<T   > 
//
// see also test_rvalue.cpp : test_3_questions().
//
//
// ********************** //
// *** decltype(auto) *** //
// ********************** //
// When to use decltype(auto)? When we want to use decltype deduction instead of auto : 
//
//    decltype(expression) obj = expression;                
//    decltype(expression) fct() 
//    { 
//        ... 
//        return expression; 
//    }  
//
// simplified respectively as :
//
//    decltype(auto) obj = expression;
//    decltype(auto) fct() 
//    { 
//        ... 
//        return expression; 
//    } 
// 
// i.e. if we want to preserve const, reference ..., use decltype(auto)
//      if we want to remove   const, reference ..., use auto
//

struct M{      };
struct X{ M m; };



void test_deduce_auto()
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

    print_summary("deduce by auto", "succeeded in compile time");
}



void test_deduce_decltype()
{
          X    x;
          X&  rx = x;
    const X& rcx = x;
          X*  px = new X;
    const X* pcx = px;

    // Simple expression
    static_assert(std::is_same_v<decltype(x     ),       X >,     "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype(rx    ),       X&>,     "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype(rcx   ), const X&>,     "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype(px    ),       X*>,     "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype(pcx   ), const X*>,     "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype(px->m ),       M >,     "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype(pcx->m),       M >,     "incorrect decltype deduce");

    // Complex expression
    static_assert(std::is_same_v<decltype((x     )),       X  &>, "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype((rx    )),       X&  >, "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype((rcx   )), const X&  >, "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype((px    )),       X* &>, "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype((pcx   )), const X* &>, "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype((px->m )),       M  &>, "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype((pcx->m)), const M  &>, "incorrect decltype deduce");

    // Simple expression for lvalue, xvalue & prvalue
    static_assert(std::is_same_v<decltype(x           ), X  >,    "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype(std::move(x)), X&&>,    "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype(X{}         ), X  >,    "incorrect decltype deduce");

    // Complex expression for lvalue, xvalue & prvalue 
    static_assert(std::is_same_v<decltype((x           )), X &>,  "incorrect decltype deduce"); // <--- decltype(()) keeps & here
    static_assert(std::is_same_v<decltype((std::move(x))), X&&>,  "incorrect decltype deduce");
    static_assert(std::is_same_v<decltype((X{}         )), X  >,  "incorrect decltype deduce");

    // Valueness check
    static_assert(!std::is_lvalue_reference_v<decltype( x          )>, "incorrect decltype valueness");
    static_assert( std::is_lvalue_reference_v<decltype((x         ))>, "incorrect decltype valueness");
    static_assert(!std::is_lvalue_reference_v<decltype(std::move(x))>, "incorrect decltype valueness");
    static_assert(!std::is_lvalue_reference_v<decltype( X{}        )>, "incorrect decltype valueness");
    static_assert(!std::is_rvalue_reference_v<decltype( x          )>, "incorrect decltype valueness");
    static_assert(!std::is_rvalue_reference_v<decltype((x         ))>, "incorrect decltype valueness");
    static_assert( std::is_rvalue_reference_v<decltype(std::move(x))>, "incorrect decltype valueness");
    static_assert(!std::is_rvalue_reference_v<decltype( X{}        )>, "incorrect decltype valueness");
    
    print_summary("deduce by decltype", "succeeded in compile time");
}



// ******************************************** //
// *** 3 questions about type and valueness *** //
// ******************************************** //
// Given an expression, ask 4 questions :
// * what is the type of the expression?
// * what is the valueness of the expression?
// * what function signature can be used to bind the expression?
//
// decltype (expression)   = type of the expression literally
// decltype((expression))  = type of the expression considering the valueness
//
//                     
//
// with name? | 1.type | 2.valuenesss | 3.bound by  | decltype()  decltype(())
// -----------+--------+--------------+-------------+---------------------------
//   named    |   X    |   lvalue     |   f(T&)     |   X            X&    
//   named    |   X&   |   lvalue     |   f(T&)     |   X&           X&   
//   named    |   X&&  |   lvalue     |   f(T&)     |   X&&          X&  
// unnamed    |   X    |   rvalue     |   f(T&&)    |   X            X    
// unnamed    |   X&   |   lvalue     |   f(T&)     |   X&           X&   
// unnamed    |   X&&  |   rvalue     |   f(T&&)    |   X&&          X&&  
//                                                      ^--- same as column 1
//

X     named_x;
X&    named_rx  = named_x;
X&&   named_rrx = std::move(named_x);
X   unnamed_x()   { return named_x; }
X&  unnamed_rx()  { return named_x; } 
X&& unnamed_rrx() { return std::move(named_x); } 

std::uint32_t lvalue_count = 0;
std::uint32_t rvalue_count = 0;
template<typename T> void fct(T&)  { ++lvalue_count; }
template<typename T> void fct(T&&) { ++rvalue_count; }



void test_deduce_3_questions()
{
    // 1. type with decltype(), valueness is not considered
    static_assert(std::is_same_v<decltype(named_x),       X>,   "4 questions failed");
    static_assert(std::is_same_v<decltype(named_rx),      X&>,  "4 questions failed");
    static_assert(std::is_same_v<decltype(named_rrx),     X&&>, "4 questions failed");
    static_assert(std::is_same_v<decltype(unnamed_x()),   X>,   "4 questions failed");
    static_assert(std::is_same_v<decltype(unnamed_rx()),  X&>,  "4 questions failed");
    static_assert(std::is_same_v<decltype(unnamed_rrx()), X&&>, "4 questions failed");

    // 1. type with decltype (()), valueness is considered
    static_assert(std::is_same_v<decltype((named_x)),       X&>,  "4 questions failed"); // lvalue is considered
    static_assert(std::is_same_v<decltype((named_rx)),      X&>,  "4 questions failed");
    static_assert(std::is_same_v<decltype((named_rrx)),     X&>,  "4 questions failed"); // lvalue is considered
    static_assert(std::is_same_v<decltype((unnamed_x())),   X>,   "4 questions failed");
    static_assert(std::is_same_v<decltype((unnamed_rx())),  X&>,  "4 questions failed");
    static_assert(std::is_same_v<decltype((unnamed_rrx())), X&&>, "4 questions failed");

    // 2. valueness - this part is consistent with decltype(())
    static_assert( std::is_lvalue_reference_v<decltype((named_x))>,       "4 questions failed");
    static_assert( std::is_lvalue_reference_v<decltype((named_rx))>,      "4 questions failed");
    static_assert( std::is_lvalue_reference_v<decltype((named_rrx))>,     "4 questions failed"); 
    static_assert(!std::is_lvalue_reference_v<decltype((unnamed_x()))>,   "4 questions failed"); // this is non lvalue ref
    static_assert( std::is_lvalue_reference_v<decltype((unnamed_rx()))>,  "4 questions failed");
    static_assert(!std::is_lvalue_reference_v<decltype((unnamed_rrx()))>, "4 questions failed");
    
    static_assert(!std::is_rvalue_reference_v<decltype((named_x))>,       "4 questions failed");
    static_assert(!std::is_rvalue_reference_v<decltype((named_rx))>,      "4 questions failed");
    static_assert(!std::is_rvalue_reference_v<decltype((named_rrx))>,     "4 questions failed"); 
    static_assert(!std::is_rvalue_reference_v<decltype((unnamed_x()))>,   "4 questions failed"); // this is also non rvalue ref
    static_assert(!std::is_rvalue_reference_v<decltype((unnamed_rx()))>,  "4 questions failed");
    static_assert( std::is_rvalue_reference_v<decltype((unnamed_rrx()))>, "4 questions failed");

    // 3. bound by
    fct(  named_x);        assert(lvalue_count == 1 && rvalue_count == 0);
    fct(  named_rx);       assert(lvalue_count == 2 && rvalue_count == 0);
    fct(  named_rrx);      assert(lvalue_count == 3 && rvalue_count == 0);
    fct(unnamed_x());      assert(lvalue_count == 3 && rvalue_count == 1);
    fct(unnamed_rx());     assert(lvalue_count == 4 && rvalue_count == 1);
    fct(unnamed_rrx());    assert(lvalue_count == 4 && rvalue_count == 2); // <--- proved the above table is correct

    print_summary("rvalue - 3 questions", "succeeded");
}


