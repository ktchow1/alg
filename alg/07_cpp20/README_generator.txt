*****************
*** Generator ***
*****************
4 roles : 
* generator object         = RAII of coroutine frame handler
* coroutine frame handler  = just a pointer
* coroutine frame          = manager of coroutine
* generator::promise_type  = config  of coroutine
                             factory of generator object
                             governs suspension policy of coroutine
                             governs suspension policy of coroutine caller
                             storage of newly created product, pass it from coroutine to coroutine caller

                                  pointing to
       coroutine frame handle  ----------------->  coroutine frame (in heap)
           ^                                           |
           |                                           |
           |                                           |
  contains |                                           | contains
           |                                           |
           |                                           |
           |                 create by factory         v
       generator object  <-----------------------  generator::promise_type 
           ^                                           ^
           .                                           .
           .                                           .
  contains .                                           . ables call promise_type::yield_value() via co_yield
           .                                           .
           .                                           .
           .                 co_yield product          .
       coroutine caller  <~~~~~~~~~~~~~~~~~~~~~~~  coroutine
      (consumer)         ~~~~~~~~~~~~~~~~~~~~~~~> (producer)  
                             handle.resume()





*************
*** Model *** 
*************
Producer consumer model
* coroutine        is the producer of product
* coroutine caller is the consumer of product, our test_coroutine_xxx() are coroutine caller
* product is stored inside promise_type


What is the flow of program?
* coroutine        yield cooperatively to coroutine caller  on co_yield / co_await
* coroutine caller yield cooperatively to coroutine         on std::coroutine_handle<promise_type>::operator()  <--+-- these 2 are equivalent
                                                            or std::coroutine_handle<promise_type>::resume()    <--+

What is the flow of product?
* product created by coroutine 
* product is passed from "coroutine co_yield"  to generator::promise_type::yield_value() 
* product is saved inside generator::promise_type generator offers get-function
* coroutine caller can then consumer product  via generator's      get-function





**********************
*** Under the hood ***
**********************
How is it implemented under the hood?
[1] When coroutine caller invokes coroutine :
-   allocate coroutine frame in heap
-   create                       generator::promise_type  inside      the coroutine frame
-   create std::coroutine_handle<generator::promise_type> pointing to the coroutine frame
-   invoke promise_type::get_return_object() to return a generator / future / task (which is an API to manage coroutine) to coroutine caller
 
    pseudo_code()
    {
        auto* frame   = allocate_coroutine_frame();
        auto& promise = *(new (&frame->promise_storage) promise_type());
        std::coroutine_handle<promise_type> handle = std::coroutine_handle<promise_type>::from_promise(promise);
        generator gen = promise.get_return_object();
    }

[2] When coroutine invokes co_yield x (where x is new product produced) :
-   invoke  promise_type::yield_value(x), we will then save it somewhere in promise_type   
-   suspend coroutine at co_yield x, program will then jump to coroutine caller's previously yield location





*******************************
*** More about promise_type ***
*******************************
Member functions of promise_type : 
* return type of promise_type::yield_value()     governs the suspension policy of producer 
* return type of promise_type::initial_suspend() governs the suspension policy of consumer

Hence the uses of promise_type::yield_value() :
* define generator's suspension policy 
* define how the product is saved inside generator::promise_type
* define how the product can be accessed by coroutine caller 
* when coroutine "co_yield product", compiler will call promise_type::yield_value(), which should save product somewhere in promise_type
- promise_type::yield_value() is similar to std::promise::set_value()
- promise_type::yield_value() can either by value or by reference

Relation between coroutine_handle and promise_type ?  
* we can get promise from coroutine_handle : auto& promise = handle.promise()                    
* we can get coroutine_handle from promise : std::coroutine_handle<promise_type>::from_promise()





******************************
*** Coroutine requirements ***
******************************
Class and function with free-to-choose name :
* generator
* generator::bool()
* generator::get_product()

Class and function with standard name :
* generator::promise_type 
* generator::promise_type::get_return_object()
* generator::promise_type::yield_value()
* generator::promise_type::initial_suspend()
* generator::promise_type::  final_suspend()
* generator::promise_type::unhandled_exception()




