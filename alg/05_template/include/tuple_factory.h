#pragma once
#include<tuple>


// ******************************************************** //
// *** Start from here, use std::tuple (not alg::tuple) *** //
// ******************************************************** //
// Difference among std::make_tuple / std::tie / std::forward_as_tuple :
// * std::make_tuple           takes deep copy of element
// * std::tie                  takes lvalue reference to element 
// * std::forward_as_tuple     takes universal reference to element
//
// Difference between structural binding and std::tie :
// * structural binding declares new variable, while std::tie binds existing variables
// * structural binding works for tuple/array, while std::tie works for std::tuple only
// * structural binding can be 
//   assigned by value     : auto  [x,y,z] = fct();
//   assigned by reference : auto& [x,y,z] = fct(); 
//   for latter case, we can modify the source values inside fct() via x,y,z
//
  
namespace alg
{
//  template<typename...Ts>
//  constexpr std::tuple<std::decay_t<Ts>...> make_tuple(Ts&&...ts) // if we need to modify ts, use std::ref
//  {
//      return std::tuple<std::decay_t<Ts>...> {std::forward<Ts>(ts)...}; 
//  }
  
    template<typename...Ts>
    constexpr std::tuple<Ts...> make_tuple(const Ts&...ts) // incorrect implementation
    {
        return std::tuple<Ts...> {ts...}; 
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
