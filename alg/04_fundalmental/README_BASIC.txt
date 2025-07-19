[Space domain]
scope = where an object can be accessed
* namespace scope
* file scope
* class scope
* function scope
* function prototype scope
* block scope




[Time domain]
storage duration = time duration of an object when its memory is allocated / deallocated
lifetime         = time duration of an object when its constructor / destructor is called

* storage duration == lifetime (often, but not necessary)  
* storage duration >= lifetime, when using "placement new"

for   auto local variable, storage duration is defined by its scope
for       global variable, storage duration is extended from its scope to program lifetime
for static local variable, storage duration is extended from its scope to program lifetime
for thread local variable, storage duration is extended from its scope to  thread lifetime  




[Singleton]
singleton implemented by static local : one instance for all threads
singleton implemented by global local : one instance for each thread

