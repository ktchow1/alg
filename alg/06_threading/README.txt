Why spinlock is non-lockfree algo while ring buffer is lockfree algo?




This is spinlock : 

    while(lock.test_and_set()) {}
    winning_thread_blocking_other_with_its_critical_section();
    lock.release();




This is ring buffer : 

    while(CAS())
    {
        winning_thread_proceed_without_blocking();
    }




Definition of lockfree is : 
* threads-losing-the-race are not blocked indefinitely
* threads-losing-the-race are not gated by winning thread

For spinlock :
* if winning thread dies
* if winning thread dominates CPU
* if winning thread runs infinite large job
* other threads will be blocked
* other threads are dependent on winning_thread_blocking_other_with_its_critical_section()

For ring buffer :
* there is no critical session
* contention happens on 1 atomic instruction CAS 
* other threads do not depend on winning_thread_proceed_without_blocking()

Hence, for lockfree-ness classification :
* spinlock is non lockfree in nature
* though spinlock can be made as fast as lockfree algo (with small critical section)



