More about std::reference_wrapper and std::optional



*************** //
*** Summary *** //
*************** //
                                  | T*          T&          ref<T>      opt<T>      opt<ref<T>> opt<T&>   | vector<T> 
----------------------------------+-----------------------------------------------------------------------+-----------
represent null?                   | yes         no          no          yes         yes         ERROR     | yes     
def-constructible                 | yes         no          no          yes         yes         ERROR     | yes      
     copyable                     | yes         yes         yes         yes         yes         ERROR     | yes        
   assignable                     | yes         yes         yes         yes         yes         ERROR     | yes        
re-assignable (re-bindable)       | yes         no          yes         ---         yes         ERROR     | ---    
used as vector element type       | yes         no          yes         yes         yes         ERROR     | yes         
----------------------------------+-----------------------------------------------------------------------+-----------
require T to be def-constructible | no          no          no          no          no          ERROR     | no       
require T to be copyable          | no          no          no          yes *       yes *       ERROR     | yes * 
require T to be assignable        | no          no          no          yes *       yes *       ERROR     | yes *



[Remark]
1.      copyable means copy-constructor exists
      assignable means copy-assignment  exists
   re-assignable means copy-assignment  exists AND operator= means rebinding referenced variable, not dereferencing

   This subtle difference between :
*     assignability vs 
*  re-assignability (a.k.a. re-bindability)
-  arise due to the auto-dereference of c++ reference : 

   T&   t = x;
        t = y;           // means dereference & assign value to referenced variable 
   auto r = std::ref(x);
        r = std::ref(y); // means rebind r to y, the new referenced variable 

2. The row "used as vector element type" means whether we can construct the following :

   std::vector<T*>
   std::vector<T&>                          <--- no, because T& is non copyable
   std::vector<std::reference_wrapper<T>>
   std::vector<std::optional<T>>
   std::vector<std::optional<std::reference_wrapper<T>>>

   The answer is yes, iff the element is copyable and assignable.

3. Reference wrapper should support inheritance. That is : 

   alg::reference_wrapper<BASE> points to 
   alg::reference_wrapper<DERIVED>

4. There are different ways to dereference the wrappers: 
*  for std::tuple,             use std::get<>()
*  for std::variant,           use std::get<>()
*  for std::reference_wrapper, use std::reference_wrapper<>::get()
*  for std::optional,          use std::optional<>::operator*() 

5. "yes *" means "it is a yes, if it involves copying and assignment"



***************** //
*** Objective *** //
***************** //
1. disadvantage of T* is not safe
2. disadvantage of T& is not assignable                                <--- hence we have std::reference_wrapper<T>
3. disadvantage of std::reference_wrapper<T> is no null-representation <--- hence we have std::optional<std::reference_wrapper<T>>
4. std::optional<T&> cannot be compiled, because there is ambiguity

   std::uint32_t x = 12;
   std::uint32_t y = 23;
   std::optional<std::uint32_t&> opt = x;
   opt = y; <--- what does that mean? 
                 1. re-assigning opt to y ?
                 2. dereference x and assign with y ?

   To understand this, we need to know the difference between :
   1. std::optional<T>::operator=(const T&) <--- destruct    the underlying T, and construct new one 
   2. std::optional<T>::operator*()         <--- dereference the underlying T, and can then be modified by user

