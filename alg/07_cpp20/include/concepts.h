#include<concepts>
#include<functional>
#include<traits.h>


namespace alg
{
   template<typename C>
   concept container =
   requires(C x)
   {
       typename C::value_type;
       typename C::iterator;
       typename C::const_iterator;
       x.begin();
       x.end();
   };
 
   template<typename T>
   concept incrementable =
   requires(T x)
   {
       ++x;
   };

   template<typename T>
   concept decrementable =
   requires(T x)
   {
       --x;
   };

   template<typename T>
   concept hashable = 
   requires(T x)
   {
       { std::hash<T>{}(x) } -> std::same_as<std::size_t>;
   };

   template<typename T, typename U>
   concept non_commutative_addable = 
   requires(T x0, U x1)
   {
       { x0 + x1 } -> std::convertible_to<T>;
   };

   template<typename T, typename U>
   concept commutative_addable = 
   requires(T x0, U x1)
   {
       { x0 + x1 } -> std::convertible_to<T>;
   } 
   ||
   requires(U x1, T x0)
   {
       { x1 + x0 } -> std::convertible_to<U>;
   };

   template<typename D, typename B>
   concept derived_from = alg::is_base_of_v<B,D> && 
                          alg::is_convertible_v<const D*, const B*>; 

   template<typename F, typename...ARGS>
   concept invocable =
   requires(F&& fct, ARGS&&...args)
   {
       std::invoke(std::forward<F>(fct), std::forward<ARGS>(args)...);
   }; 

   template<typename F, typename...ARGS>
   concept invocable2 =
   requires(F&& fct, ARGS&&...args) // why not use this simpler implementation?
   {
       fct(std::forward<ARGS>(args)...);
   }; 
}



