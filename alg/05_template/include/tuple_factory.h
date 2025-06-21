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
//   auto   x = i;              // instantiate new instance, then deep copy                     <--- simulated by std::make_tupl
//   auto&  y = i;              // reference to existing instance, bind to lvalue only          <--- simulated by std::tie
//   auto&& z = i;              // reference to existing instance, bind to both lvalue and ...  <--- simulated by std::forward_as_tuple
//   auto&& w = std::move(i);   //                                              rvalue
//
//
// std::get<N> is implemented in a way, s.t. that it gives the following type for tuple's element :
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
//  constexpr std::tuple<Ts...> make_tuple(const Ts&...ts) // incorrect implementation
//  {
//      return std::tuple<Ts...> {ts...}; 
//  } 
  
    template<typename...Ts>
    constexpr std::tuple<std::decay_t<Ts>...> make_tuple(Ts&&...ts) // if we need to modify ts, use std::ref
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
//   assigned by value     : auto  [x,y,z] = fct();
//   assigned by reference : auto& [x,y,z] = fct(); 
//   for latter case, we can modify the source values inside fct() via x,y,z
//
  
