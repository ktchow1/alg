Inaccurate terminology :
* valueness      should be called value-catergory
* literal-type   should be called declared-type





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
When accessing a member, which may be 
*     reference type member or 
* non reference type member

then we need to apply 2 sets of rules 
* rules to propagate the valueness of object to valueness of expression
* rules to propagate the const/ref of object to const/ref of member

then we can decide final type of decltype(()) using the 
* propagated valueness of expression AND
* propagated type of member
  using ordinary logic in decltype(()) and reference collapsing





[Rule set 1] Valueness of an expresssion that access a member is propagated by : 
* if the object is lvalue, then the expression is lvalue, regardless the member is reference type of NOT
* if the object is rvalue, then the expression is xvalue, if member is non-reference type 
                                the expression is lvalue, if member is     reference type

[Rule set 1, putting in another way]
* if member is non-reference type, expression is always lvalue
* if member is     reference type, expression is always xvalue if object is rvalue, lvalue otherwise


valueness    member | example     | valueness       decltype       decltype
of obj x     type   |             | of expression   (expression)   ((expression))
--------------------+-------------+------------------------------------------------
 lvalue      M      |       x.m   | lvalue          M              M   + &  = M&     
 lvalue      M&     |       x.m   | lvalue          M&             M&  + &  = M&
 lvalue      M&&    |       x.m   | lvalue          M&&            M&& + &  = M&      
 xvalue      M      | move(x).m   | xvalue          M              M   + &  = M&& 
 xvalue      M&     | move(x).m   | lvalue          M&             M&  + &  = M&
 xvalue      M&&    | move(x).m   | lvalue          M&&            M&& + &  = M&
prvalue      M      |     X{}.m   | xvalue          M              M   + && = M&&
prvalue      M&     |     X{}.m   | lvalue          M&             M&  + &  = M&
prvalue      M&&    |     X{}.m   | lvalue          M&&            M&& + &  = M&
                                                    ^
                                                    +--- same as col 2 




[Rule set 2] Constness and reference of object may propagate to member (in col 2) using following logic :
* if member is non-reference type,    propagate all constness AND reference of object to member 
* if member is     reference type, no propagation happens

Therefore after propagation with rule set 2, member type may be different from decltype(expression) :
* column 2                    will be updated with addition const/ref from object
* column decltype(expression) will remain the literal type of expression





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
  
