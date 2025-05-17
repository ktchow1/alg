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
           .                                           .
       coroutine caller                            coroutine
      (consumer)                                  (producer)  





Producer consumer model
* coroutine        is the producer of product
* coroutine caller is the consumer of product, our test_coroutine_xxx() are coroutine caller
* coroutine        (producer) push new product into generator::promise_type via co_yield     <--- analogous to std::promise
* coroutine caller (consumer) pop  new product from generator               via get function <--- analogous to std::future
* hence in some occasions, generator is called future





Call sequence for generator : 
1. generator::promise_type::promise_type()
2. generator::promise_type::get_return_object()
3. generator::generator()
4. generator::promise_type::initial_suspend()

Call sequence for task : 
1. task::promise_type::promise_type()
2. task::promise_type::get_return_object()
3. task::generator()
4. task::promise_type::initial_suspend()
5. awaitable::awaitable()
6. awaitable::initial_suspend()





What is the flow of program?
* coroutine        yield cooperatively to coroutine caller  on co_yield / co_await
* coroutine caller yield cooperatively to coroutine         on std::coroutine_handle<promise_type>::operator()  <--+-- these 2 are equivalent
                                                            or std::coroutine_handle<promise_type>::resume()    <--+
                                                            or std::coroutine_handle<promise_type>'s construction (if initial_suspend returns ...)
* therefore :
- coroutine is just one thread, doing things concurrently (not in parallel)
- coroutine is cooperative scheduling, not preemptive scheduling 
- coroutine is non-blocking, it returns to coroutine caller with generator  (hence async)





What is the flow of product?
* product created by coroutine 
* product is passed from "coroutine co_yield"  to generator::promise_type::yield_value() 
* product is saved inside generator::promise_type generator offers get-function
* coroutine caller can then consumer product  via generator's      get-function





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























