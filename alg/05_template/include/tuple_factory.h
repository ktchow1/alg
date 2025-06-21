#pragma once
#include<tuple>


// Why do we need 3 similar tuple factory 
// * std::make_tuple()
// * std::tie()
// * std::forward_as_tuple()
//
//
// In short, it is about simulating : 
//  
//   int i = 123;
//   auto   x = i;              // instantiate new instance, then deep copy                     <--- simulated by std::make_tuple
//   auto&  y = i;              // reference to existing instance, bind to lvalue only          <--- simulated by std::tie
//   auto&& z = i;              // reference to existing instance, bind to both lvalue and ...  <--- simulated by std::forward_as_tuple
//   auto&& w = std::move(i);   //                                              rvalue
//
//
// Therefore :
// * std::make_tuple()       = creating std::tuple<X, ...>                      <--- newly created tuple owns a copy of values 
// * std::tie()              = creating std::tuple<X&, ...>                     <--- newly created tuple has a reference to orig lvalue variables
// * std::forward_as_tuple() = creating std::tuple<X&, ...>   for lvalue input  <--- newly created tuple has a reference to orig lvalue & rvalue variales
//                          or creating std::tuple<X&&, ....> for rvalue input       Hence, std::forward_as_tuple is a like a tuple-version of std::forward.
//
//
// According to C++20 standard, std::get<N> is implemented in a way, s.t. that it gives the following type for tuple's element :
//
// tuple type          |   std::get<0>(tuple) type
// --------------------+--------------------------------
//   named tuple<X>    |        X&
//   named tuple<X&>   |        X&       
//   named tuple<X&&>  |        X&
// unnamed tuple<X>    |        X&&
// unnamed tuple<X&>   |        X&
// unnamed tuple<X&&>  |        X&&
//



namespace alg
{
//  template<typename...Ts>
//  constexpr std::tuple<Ts...> make_tuple(const Ts&...ts) // incorrect implementation as it cannot handle rvalue input
//  {
//      return std::tuple<Ts...> {ts...}; 
//  } 
  
    template<typename...Ts>
    constexpr std::tuple<std::decay_t<Ts>...> make_tuple(Ts&&...ts) // may throw as it constructs new Ts 
    {
        return std::tuple<std::decay_t<Ts>...> {std::forward<Ts>(ts)...}; 
    }  

    template<typename...Ts>
    constexpr std::tuple<Ts&...> tie(Ts&...ts) noexcept
    {
        return std::tuple<Ts&...> {ts...}; 
    }

    template<typename...Ts>
    constexpr std::tuple<Ts&&...> forward_as_tuple(Ts&&...ts) noexcept
    {
        return std::tuple<Ts&&...> {std::forward<Ts>(ts)...}; 
    }
}


// Difference between structural binding and std::tie :
//
// * structural binding declares new variable, while std::tie binds existing variables
// * structural binding works for tuple/array, while std::tie works for std::tuple only
// * structural binding can be 
//
//   assigned by value     : auto  [x,y,z] = fct();
//   assigned by reference : auto& [x,y,z] = fct(); 
//
//   for latter case, we can modify the source values inside fct() via x,y,z
//
  
