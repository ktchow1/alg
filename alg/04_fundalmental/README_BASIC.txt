[Scope]
Scope is where an object can be accessed. Divided into :

          scope            |  classification   
---------------------------+----------------------------------------
namespace scope            |  global variable
     file scopee           |  global variable
    class scopee           |   local variable
 function scopee           |   local variable
 function prototype scopee |   local variable
    block scopee           |   local variable




[Storage duration and lifetime]
* storage duration = time duration of an object when it is allocated / deallocated in memory
*         lifetime = time duration of an object when it is constructed / destructed
* storage duration == lifetime (usually, but not necessary)  
* storage duration >  lifetime, when using "placement new"




[Automatic & static storage duration]
*    static storage duration means a variable live as long as the program
* automatic storage duration means a variable live as long as its scope
* by default, global variable has static storage duration (i.e. as main() of call stack)
* by default,  local variable has auto   storage duration (i.e.  governed by call stack), unless ...
            if local variable is declared as static,       its lifetime is extended to the program lifetime 
            if local variable is declared as thread_local, its lifetime is extended to the  thread lifetime

                       | storage duration
-----------------------+-------------------------------
      global variable  | static, i.e. program life
       local variable  |   auto, i.e. scope
static local variable  | static, i.e. program life
thread local variable  |               thread life




[Linkage]
* Linkage is about whether a global variable can be accessed by other tranlation unit.
* Linkage does not involve local variable.
* Global variable (in namespace scope or file scope) 
- can    be external linked to other translation units when declared "extern" in header
- cannot be external linked to other translation units when declared "static"




[Static keyword]
Static keyword has several overload meaning :
* static member variable inside class            = member belong to class, not object
* static  local variable inside function         = sticky variable value
* static global variable inside namespace / file = do not allow extern, support internal linkage only




[Singleton]
singleton implemented by static global : one instance for all threads
singleton implemented by  thread_local : one instance for each thread <--- useful for per_thread_timer, per_thread_logger etc ...

