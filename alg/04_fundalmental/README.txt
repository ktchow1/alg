// ************ //
// *** auto *** //
// ************ //
Get type of assignment statement, ignoring const and &, unless specified in auto

If T is typeof assignment statement :
* for auto                              type = reference_collapse<      const_reference_trim<T>   >
* for const auto&                       type = reference_collapse<const const_reference_trim<T>  &>
* for auto&                             type = reference_collapse<            reference_trim<T>  &>
* for auto&& and lvalue assignment      type = reference_collapse<            reference_trim<T>& &&>
* for auto&& and rvalue assignment      type = reference_collapse<            reference_trim<T>  &&>  

  
  

// **************** //
// *** decltype *** //
// **************** //
decltype (expression)  : get literal type of expression 
decltype((expression)) : get literal type of expression plus consideration of valuenss (and context)

If T is typeof assignment statement :
* for simple  expression                type = T 
* for complex  lvalue expression        type = reference_collapse<T & >  
* for complex  xvalue expression        type = reference_collapse<T &&> 
* for complex prvalue expression        type = reference_collapse<T   > 

Given an expression, ask 3 questions :
1. what is valueness of the expression?
2. what is type      of the expression? 
3. what function signature can be used to bind the expression?
                   
then 
* decltype (expression) and 
* decltype((expression)) respectively are : 

1.valuenesss    | 2.type of    | example        | decltype        decltype        | 3.hence   
  of expression |   expression |                | (expression)    ((expression))  |   bound by         
----------------+--------------+----------------+---------------------------------+-------------
  lvalue        |   X          | named_x        | X               X   + &  = X&   |   f(T&)      
  lvalue        |   X&         | named_x        | X&              X&  + &  = X&   |   f(T&)     
  lvalue        |   X&&        | named_x        | X&&             X&& + &  = X&   |   f(T&)     
  xvalue        |   X          | moved(named_x) | X&&  (why?)     X&& + && = X&&  |   f(T&&)    
  xvalue        |   X&         | moved(named_x) | X&&  (why?)     X&& + && = X&&  |   f(T&&)     
  xvalue        |   X&&        | moved(named_x) | X&&  (why?)     X&& + && = X&&  |   f(T&&)    
 prvalue        |   X          | unnamed_x()    | X               X        = X    |   f(T&&)    
 prvalue        |   X&         | unnamed_x()    | X&              X&       = X&   |   f(T&)     
 prvalue        |   X&&        | unnamed_x()    | X&&             X&&      = X&&  |   f(T&&)    

Why xvalue expression all gives X&& in decltype()? 
This is because, std::move() is static_cast<X&&>.




// ****************************************** //
// *** decltype (rules for member access) *** //
// ****************************************** //
Valueness of an expresssion that access a reference member follows these logics : 
- if the object is lvalue, then the expression is lvalue, regardless the member type
- if the object is rvalue, then the expression is xvalue, if the member is non-reference type
                                the expression is lvalue, if the member is reference type (no matter M& or M&&)


valueness    member | example     | valueness       decltype       decltype
of obj x     type   |             | of expression   (expression)   ((expression))
--------------------+-------------+------------------------------------------------
 lvalue      M      |       x.m   | lvalue          M              M   + &  = M&     
 lvalue      M&     |       x.m   | lvalue          M&             M&  + &  = M&
 lvalue      M&&    |       x.m   | lvalue          M&&            M&& + &  = M&      
 xvalue      M      | move(x).m   | xvalue          M              M   + && = M&&
 xvalue      M&     | move(x).m   | lvalue          M&             M&  + &  = M&
 xvalue      M&&    | move(x).m   | lvalue          M&&            M&& + &  = M&
prvalue      M      |     X{}.m   | xvalue          M              M   + && = M&&
prvalue      M&     |     X{}.m   | lvalue          M&             M&  + &  = M&
prvalue      M&&    |     X{}.m   | lvalue          M&&            M&& + &  = M&
                                                    ^--- same as col 2 




// ******************************************** //
// *** decltype (rules for std::get() tuple *** //
// ******************************************** //
Please refer to test_tuple_factory.cpp for type deduction of std::get<N>(tuple).




// ********************** //
// *** decltype(auto) *** //
// ********************** //
When to use decltype(auto)? When we want to use decltype deduction instead of auto : 

   decltype(expression) obj = expression;                
   decltype(expression) fct() 
   { 
       ... 
       return expression; 
   }  

simplified respectively as :

   decltype(auto) obj = expression;
   decltype(auto) fct() 
   { 
       ... 
       return expression; 
   } 

i.e. if we want to preserve const, reference ..., use decltype(auto)
     if we want to remove   const, reference ..., use auto
  
