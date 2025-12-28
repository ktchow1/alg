More about std::reference_wrapper and std::optional





*************** //
*** Summary *** //
*************** //
                                  | T*          T&          ref<T>      opt<T>       opt<ref<T>>  opt<T&>   | vector<T> 
----------------------------------+-------------------------------------------------------------------------+-----------
represent null?                   | yes         no          no          yes          yes          ERROR     | yes     
def-constructible                 | yes         no          no          yes          yes          ERROR     | yes      
     copyable                     | yes         yes         yes         yes          yes          ERROR     | yes        
   assignable                     | yes         yes         yes         yes          yes          ERROR     | yes        
re-assignable (re-bindable)       | yes         no          yes         owner        yes          ERROR     | owner
used as vector element type       | yes         no          yes         yes          yes          ERROR     | yes         
----------------------------------+-------------------------------------------------------------------------+-----------
require T to be def-constructible | no          no          no          no           no           ERROR     | no       
require T to be copyable          | no          no          no          yes if ...   yes if ...   ERROR     | yes if ... 
require T to be assignable        | no          no          no          yes if ...   yes if ...   ERROR     | yes if ...



[Remark]
1. Reference wrapper does not own the variable, it only points to the variable.
   Optional owns the variable.

2. Rebinding in reference wrapper means pointing to another reference variable.
   Rebinding in optional means destructing the current variable, then construct a new one.

3.      copyable means copy-constructor exists
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

4. The row "used as vector element type" means whether we can construct the following :

   std::vector<T*>
   std::vector<T&>                          <--- no, because T& is non copyable
   std::vector<std::reference_wrapper<T>>
   std::vector<std::optional<T>>
   std::vector<std::optional<std::reference_wrapper<T>>>

   The answer is yes, iff the element is copyable and assignable.

5. Reference wrapper should support inheritance. That is : 

   alg::reference_wrapper<BASE> points to 
   alg::reference_wrapper<DERIVED>

7. There are different ways to dereference the wrappers: 
*  for std::tuple,             use std::get<>()
*  for std::variant,           use std::get<>()
*  for std::reference_wrapper, use std::reference_wrapper<>::get()
*  for std::optional,          use std::optional<>::operator*() 

8. "yes if ..." means "it is a yes, if it involves copying and assignment"





***************** //
*** Objective *** //
***************** //
1. disadvantage of T* is not safe
2. disadvantage of T& is not re-assignable                             <--- hence we have std::reference_wrapper<T>
3. disadvantage of std::reference_wrapper<T> is no null-representation <--- hence we have std::optional<std::reference_wrapper<T>>
4. but why not use std::optional<T&>? This is because std::optional<T&> cannot be compiled.





// ************************************ //
// *** Why optional<T&> is illegal? *** //
// ************************************ //
For example :

    T x{1}
    T y{2};

    std::optional<T&> op = x;
    op = y; <--- what does that mean? 
                 
1. assigning internal reference of op to y ? or ... 
2. assigning internal value     of op to y's value ?

This is controversal, and c++ compiler treats this as illegal.





// ********************** //
// *** Implementation *** //
// ********************** //
std::reference_wrapper<T> implemented as T*                     no dynamic allocation, just pointing to variable
std::optional<T> can't be implemented as T                      as it requires default-constructiblility of T
std::optional<T> can't be implemented as T*                     as it involves dynamic allocation
std::optional<T>          implemented as char[sizeof(T)]        no dynamic allocation, allow non-default-constructible T
std::optional<T>          implemented as std::aligned_storage   no dynamic allocation, allow non-default-constructible T
std::vector<T>            implemented as T*                     as it involves dynamic allocation anyway

However char[sizeof(T)] does not take care of T's alignment in various cpu, which may result in UB (undefined behaviour).
