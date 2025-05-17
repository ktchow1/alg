

These are similar : 

when coroutine calls co_yield, it passes newly-created-product to 
* generator::promise_type::yield_value() ... so that
* generator::promise_type can save it somewhere in promise_type

when coroutine calls co_await, it is suspended and wait until
* awaitable::await_resume() passes newly-created-product to coroutine
* awaitable should have an access to newly-created-product from task<T> : via m_product_ptr

The product passing mechanism :
* is     implemented from co_yield to generator<T>
* is NOT implemented from task<T> to co_await, need manually creat awaitable to execute it

This is one reason why :
* coroutine as generator is simpler 
* coroutine as awaitor is more complicaited
