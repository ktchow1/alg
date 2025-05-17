When c++ compiler comes across function that contains one of the following keywords :
* co_yield    (for producer ?)
* co_await    (for consumer ?)
* co_return
> that function is considered as coroutine.





When a function is considered as coroutine
1. coroutine frame is created in heap (hidden from user), it contains :
*  coroutine states
*  "promise_type"


* promise is a config of coroutine / definition of how coroutine behave
* coroutine management is done by a coroutine frame in heap 
* coroutine frame is encapsulated by task / generator / future for safety (like RAII)
* task / generator / future is an api for outside world to manage coroutine
* task / generator / future have a pointer to coroutine frame called std::coroutine_handle<>
* promise has to provide a factory for task as a part of coroutine config

co return T = task 
co yield T  = generator (or producer)
co await T  = awaitor   (or consumer .... not too accurate, better all awaitor, as it may not consume)




