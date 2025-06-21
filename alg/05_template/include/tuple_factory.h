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
//   auto   x = i;                // instantiate new instance, then deep copy 
//   auto&  y = i;                // reference to existing instance, bind to lvalue onlyA
//   auto&& z = i;                // reference to existing instance, bind to both lvalue and ...
//   auto&& w = std::move(i);     //                                              rvalue
//

namespace toy_example
{
    std::uint32_t lvalue_impl_count = 0;
    std::uint32_t rvalue_impl_count = 0;

    template<typename T> 
    struct wrapper // This is not an accurate mimic of tuple, as the return type of tuple<T> is T&, not T.
    {
        decltype(auto) get() 
        {
            return static_cast<T>(m); 
        }

        T m;
    };

    template<typename T>
    wrapper<std::decay_t<T>> make_wrapper_by_copying(T&& x) // mimic std::make_tuple
    {
        return wrapper<std::decay_t<T>>(std::forward<T>(x));
    }

    template<typename T>
    wrapper<T&> make_wrapper_by_lvalue_reference(T& x) // mimic std::tie
    {
        return wrapper<T&>(x);
    }

    template<typename T>
    wrapper<T&&> make_wrapper_by_perfect_forwarding_reference(T&& x) // mimic std::forward_as_tuple
    {
        return wrapper<T&&>(std::forward<T>(x));
    }
}



// Now generalize scaler wrapper into tuple, we have : 


// * std::make_tuple           takes deep copy of element
// * std::tie                  takes lvalue reference to element 
// * std::forward_as_tuple     takes universal reference to element




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
  
