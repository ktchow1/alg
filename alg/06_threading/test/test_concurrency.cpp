#include<iostream>
#include<locked_queue.h>
#include<lockfree_queue.h>

// *** test *** //
#include<run_mpmcq.h>
#include<run_threadpool.h>

// *** boost *** // 
// #include<boost/lockfree/spsc_queue.hpp> 
// #include<boost/lockfree/queue.hpp> 



// ******************************************************************************************* //
// Here are the results for 3 producers, 3 consumers, 50000 tasks, in Yubo machine, 
// with 300ns waiting time between any two productions in producer (unit = nanosec) :
//
// percentile           | 0.1 |   1 |   5 |  10 |  25 |  50 |  75 |   90 |   95 |   99 | 99.9     
// ---------------------+-----+-----+-----+-----+-----+-----+-----+------+------+------+------
//   alg locked   queue | 141 | 142 | 146 | 151 | 176 | 197 | 546 | 1527 | 2481 | 5798 | 12ms  
//   alg lockfree queue |  83 |  88 | 116 | 117 | 119 | 122 | 126 |  128 |  130 |  135 |  151    
// boost lockfree spscq |  86 |  97 | 106 | 108 | 112 | 116 | 122 |  127 |  130 |  138 |  157    
// boost lockfree queue | 166 | 172 | 178 | 181 | 188 | 195 | 210 |  228 |  252 |  282 |  318    
// 
//
// Remarks :
// * Affinity is a must.
// * Priority (using nice value)  is a must.
// * Priority (using FIFO policy) should be avoided, it takes all resources.
// ******************************************************************************************* //
//
void test_mpmcq()
{
    std::uint32_t num_producers = 3;
    std::uint32_t num_consumers = 4;
    std::uint32_t num_tasks = 40000; // per producer

    alg::run_mpmcq<alg::mutex_locked_queue>  ("mpmcq with alg::mutex_locked_queue   ", num_producers, num_consumers, num_tasks);
    alg::run_mpmcq<alg:: spin_locked_queue>  ("mpmcq with alg:: spin_locked_queue   ", num_producers, num_consumers, num_tasks);
    alg::run_mpmcq<alg::lockfree_queue_long> ("mpmcq with alg::lockfree_queue_long  ", num_producers, num_consumers, num_tasks);
    alg::run_mpmcq<alg::lockfree_queue_short>("mpmcq with alg::lockfree_queue_short ", num_producers, num_consumers, num_tasks);
} 


// ************************************************************************************************* //
// Comparison among different threadpool : 
//
//                 | template? | thread       | task          | queue      | synchronization
// ----------------+-----------+--------------+---------------+------------+------------------------
// threadpool_sync | yes       | std:: thread | fixed T       | customized | compile determined SYNC 
// threadpool      | no        | std:: thread | std::function | std::queue | std::condition_variable 
// threadpool_j    | no        | std::jthread | std::function | std::queue | std::condition_variable 
// threadpool_jcrt | no        | std::jthread | coroutine     | std::queue | std::condition_variable 
//
// 
// 1. threadpool_sync is developed in Yubo
//    threadpool      is developed in TDMS
// 2. threadpool_sync.emplace_task() may fail, need retry
//    threadpool     .add_task() will block until success
// 3. threadpool_sync task T CANNOT throw.
//    threadpool      task std::function<> may throw.
// 4. threadpool_jcrt DOES NOT WORK yet, need to support unary-argument task in coroutine
// ************************************************************************************************* //
//
void test_threadpool()
{
    std::uint32_t num_trials = 1;
    std::uint32_t num_threads = 3;
    std::uint32_t num_tasks = 30000;
    std::uint32_t delay_between_tasks_in_us = 80;

//  num_trials = 10;
//  num_tasks = 3000;
//  delay_between_tasks_in_us = 40;

    for(std::uint32_t n=0; n!=num_trials; ++n)
    {
        alg::run_threadpool_sync   <alg::mutex_locked_queue>  ("alg::threadpool_sync with alg::mutex_locked_queue   ", num_threads, num_tasks, delay_between_tasks_in_us);
        alg::run_threadpool_sync   <alg:: spin_locked_queue>  ("alg::threadpool_sync with alg:: spin_locked_queue   ", num_threads, num_tasks, delay_between_tasks_in_us);
        alg::run_threadpool_sync   <alg::lockfree_queue_long> ("alg::threadpool_sync with alg::lockfree_queue_long  ", num_threads, num_tasks, delay_between_tasks_in_us);
        alg::run_threadpool_sync   <alg::lockfree_queue_short>("alg::threadpool_sync with alg::lockfree_queue_short ", num_threads, num_tasks, delay_between_tasks_in_us);
        alg::run_threadpool_condvar<alg::threadpool>          ("alg::threadpool      with condvar",                       num_threads, num_tasks, delay_between_tasks_in_us);
        alg::run_threadpool_condvar<alg::threadpool_j>        ("alg::threadpool_j    with condvar",                       num_threads, num_tasks, delay_between_tasks_in_us);
    }
}
