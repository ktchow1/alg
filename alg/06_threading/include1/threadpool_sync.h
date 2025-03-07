#pragma once
#include<locked_queue.h>
#include<lockfree_queue.h>
#include<synchronization.h>
#include<thread.h>


// ************************************* //
// T = task 
// Q = multi-thread safe queue
// S = synchronization primitive
// ************************************* //
// S = sync_futex        work, 1.7us 
// S = sync_pmutex       deadlock
// S = sync_HansBarz     work, 1.7us 
// S = sync_psemaphore   work, 1.7us
// S = sync_condvar      work, 2.2us
// ************************************* //
namespace alg
{
    template<std::invocable<std::uint32_t> T,                     // <---- invocable with thread_id as unary arg 
             template<typename> typename Q = mutex_locked_queue,  
             sync_primitive S = sync_psemaphore>       

    class threadpool_sync
    {
    public:
        explicit threadpool_sync(std::uint32_t num_threads) : m_run(true), m_threads(), m_task_queue(), m_sync() 
        {
            for(std::uint32_t n=0; n!=num_threads; ++n)
            {
                m_threads.emplace_back(std::thread(&threadpool_sync<T,Q>::thread_fct, this, n));
            }
        }

       ~threadpool_sync()
        { 
            stop();
        }

        threadpool_sync() = delete;
        threadpool_sync(const threadpool_sync&) = delete;
        threadpool_sync(threadpool_sync&&) = default;
        threadpool_sync& operator=(const threadpool_sync&) = delete;
        threadpool_sync& operator=(threadpool_sync&&) = default;

        // ****************************************** //
        // NOT USED FOR HOME-DESKTOP. THIS IS FOR :
        // * RELEASE MODE
        // * sudo TO ALLOW CHANGE IN SCHEDULE POLICY
        // ****************************************** //
        threadpool_sync(std::uint32_t num_threads, const std::vector<std::uint32_t>& affinity) : threadpool_sync(num_threads)
        {   
            for(auto& x:m_threads) 
            {
                set_thread_affinity(x.native_handle(), affinity);
                set_thread_policy  (x.native_handle(), SCHED_RR);
            } 
        }

    public: 
        // **************************** //
        // Stop may be called 2 times : 
        // 1. once explicitly
        // 2. once inside destructor
        // **************************** //
        void stop()
        {
            m_run.store(false);
            for(const auto& x:m_threads) 
            {
                m_sync.notify();
            }
            for(auto& x:m_threads) 
            {
                if (x.joinable()) x.join();
            }
        }

    public: 
        // ************************* //    
        // *** Producer of tasks *** //
        // ************************* //    
        template<typename... ARGS>
        bool emplace_task(ARGS&&... args)
        {
            if (m_task_queue.emplace(std::forward<ARGS>(args)...))
            {
                m_sync.notify(); 
                return true;
            }
            else
            {
                return false;
            }
        }

        template<typename... ARGS>
        void emplace_task_until_success(ARGS&&... args)
        {
            while(!m_task_queue.emplace(std::forward<ARGS>(args)...));
            m_sync.notify(); 
        }

    private:
        // ************************* //    
        // *** Consumer of tasks *** //
        // ************************* //    
        void thread_fct(std::uint32_t thread_id)
        {
            // set affinity here (skipped for simplicity)
            // set priority here (skipped for simplicity)


            // ****************************************************** //
            // Decouple two checking :
            // 1. is running 
            // 2. is task queue empty 
            //
            // Adventages of decoupling :
            // 1. avoid repeated lock & unlock on peeking queue size
            // 2. avoid lost notification on stopping threadpool
            // ****************************************************** //
            while(m_run.load())
            {
                m_sync.wait();

                auto task = m_task_queue.pop();
                if (task)
                {
                    (*task)(thread_id);
                }
            }

            // STOP REQUESTED, ALL THREADS RUNNING, NO WAITING.
            while(m_task_queue.peek_size() > 0)
            {
                auto task = m_task_queue.pop();
                if (task)
                {
                    (*task)(thread_id);
                }
            }
        }

    private:
        std::atomic<bool> m_run;
        std::vector<std::thread> m_threads;
        Q<T> m_task_queue;
        S m_sync;
    };
}
