***************
*** Awaitor ***
***************
Generator vs awaitor : 

1. when coroutine calls co_yield, it passes newly-created-product to 
*  generator::promise_type::yield_value() ... so that
*  generator::promise_type can save it somewhere in promise_type

2. when coroutine calls co_await, it is suspended and wait until
*  awaitable::await_resume() passes newly-created-product to coroutine
*  awaitable should have an access to newly-created-product from task<T> : via m_product_ptr



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


