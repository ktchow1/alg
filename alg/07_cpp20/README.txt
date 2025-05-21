********************
*** Introduction ***
********************
When a function contains one of the keywords :
* co_yield  
* co_await  
* co_return
> it is considered as coroutine.





***********************
*** Flow of program ***
***********************
1. coroutine caller (a.k.a. caller) invokes coroutine, it yields to coroutine
2. coroutine will run until reaching either co_yield / co_await / co_return, it yields to caller (to the point where caller invokes coroutine)
3. coroutine caller gets a return_object, known as generator / task, through which it can resume coroutine
   coroutine caller resumes by calling :
   generator.some_resume_fct() 
        task.some_resume_fct() 
   we have to define these resume_fct() inside return_object, usually it involves calling coroutine_handle::resume()
4. coroutine resumes from the point it left off last time (i.e. last co_yield / co_await) 
   coroutine runs until next co_yield / co_await and yields to caller (to the point where caller invokes coroutine_handle::resume())
*  repeat 3 & 4


[Remark 1] 
Therefore
* coroutine is about one thread, doing things concurrently (not in parallel)
* coroutine is cooperative scheduling, not preemptive scheduling 
* coroutine is non-blocking (async), coroutine returns to coroutine caller immediately 

[Remark 2]
When we talk about suspend and resume, it is usually about coroutine, but in fact it is symmetrical : 
* suspend coroutine = resume caller
* resume caller = suspend coroutine

[Remark 3]
When coroutine is used in producer consumer model, then :

                           | case 1             | case 2  
---------------------------+--------------------|-------------------
coroutine                  | producer           | consumer    
coroutine caller           | consumer           | producer    
return_object              | generator          | task
coroutine yields by        | co_yield product   | co_await awaitable
coroutine caller yields by | generator.resume() | task.resume()     





**********************
*** Under the hood *** 
**********************
Program jump from coroutine to caller and vice versa, cannot be achieved by traditional 
function invocation in stack, instead compiler allocates a coroutine frame in heap.

1. coroutine frame (in heap) is a manager of coroutine, it stores all coroutine info :
*  program counter  
*  local variables
*  promise_type (will talk about it shortly)

2. promise_type is created inside coroutine frame, its purposes include : 
*  acts as config  of coroutine, such as suspend policy
*  acts as factory of return_object (will talk about it shortly) i.e. generator / task
*  acts as gateway to pass product produced by coroutine into generator, so that 
   caller can consume it via gennerator.get_product(), again we need to define it
   
3. coroutine_handle, which is a pointer to coroutine frame, it provides access to coroutine frame :
*  coroutine_handle::promise()    <--- provide access to promise_type
*  coroutine_handle::resume()     <--- resume coroutine from caller
*  coroutine_handle::done()       <--- check if coroutine is done from caller
*  coroutine_handle::destroy()    <--- clear coroutine frame in heap

4. return_object is a RAII of coroutine_handle, so that :
*  return_object calls coroutine_handle::destroy() in destructor
*  return_object provides API to resume / check coroutine





**************************
*** Suspend and resume ***
**************************
Suspend coroutine / resume coroutine caller
* co_yield 
* co_await 

Resume coroutine / suspend coroutine caller 
* coroutine_handle::resume() <--- prefer thie, more readable 
* coroutine_handle::operator()















