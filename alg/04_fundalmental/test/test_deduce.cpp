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
// For example : 
// In alg::bind and 
//    alg::bound_function_object
// when we dont know the return type,
// and  we only know the return expression.   
//
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

    print_summary("deduce by decltype", "succeeded in compile time");
}
