********************
*** Introduction ***
********************
When compiler comes across function containing one of the keywords :
* co_yield  
* co_await  
* co_return
> that function is considered as coroutine.



Flow of program
1. coroutine caller (a.k.a. caller) invokes coroutine, it yields to coroutine
2. coroutine will run until reaching either co_yield / co_await / co_return, it yields to caller (to the point where caller invokes coroutine)
3. 
* caller will get a return_object, known as generator / task, it proceeds until caller yields to coroutine by :  
- generator.some_mem_fct() 
-      task.some_mem_fct()



When a function is considered as coroutine, coroutine frame is created in heap (hidden from user), it contains :
*  coroutine states
*  "promise_type"




Relation among those concepts
* promise is a config of coroutine / definition of how coroutine behave
* coroutine management is done by a coroutine frame in heap 
* coroutine frame is encapsulated by task / generator / future for safety (like RAII)
* task / generator / future is an api for outside world to manage coroutine
* task / generator / future have a pointer to coroutine frame called std::coroutine_handle<>
* promise has to provide a factory for task as a part of coroutine config







suspend coroutine / resume coroutine caller
* co_yield 
* co_await 

suspend coroutine caller / resume coroutine
* handle.resume() <--- prefer thie, more readable 
* handle()

we usually talk in the perspective of coroutine, that is : 
* suspend on co_*** operator
* resume  on handle.resume() 


















These are similar : 

when coroutine calls co_yield, it passes newly-created-product to 
* generator::promise_type::yield_value() ... so that
* generator::promise_type can save it somewhere in promise_type

when coroutine calls co_await, it is suspended and wait until
* awaitable::await_resume() passes newly-created-product to coroutine
* awaitable should have an access to newly-created-product from task<T> : via m_product_ptr



The product passing mechanism :
* is     implemented from co_yield to generator<T>
* is NOT implemented from task<T> to co_await, compiler generates calls to :
  awaitable::await_ready
  awaitable::await_suspend
  awaitable::await_resume
  developer must implement these methods to define how data is transferred



This is one reason why :
* coroutine as generator is simpler 
* coroutine as awaitor is more complicaited






T is yieldable
T is not awaitable <--- need another awaitable, hence comews the asymmetry
