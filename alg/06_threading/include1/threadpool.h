#pragma once
#include<iostream>
#include<vector>
#include<queue>
#include<functional>
#include<thread>
#include<mutex>
#include<condition_variable>


namespace alg
{
    class threadpool
    {
    public:
        explicit threadpool(std::uint32_t num_threads) : m_run(true)
        {
            for(std::uint32_t n=0; n!=num_threads; ++n)
            {
                m_threads.emplace_back(std::thread(&threadpool::thread_fct, this, n));
            }
        }

        ~threadpool()
        {
            stop();
        }

    public:
        void stop()
        {
            m_run.store(false);
            m_condvar.notify_all();
            for(auto& x:m_threads)
            {
                // BUG : Need to check joinable to avoid multi-join, otherwise it throws
                if (x.joinable()) x.join();
            }
        }

    public: 
        // ************************* //    
        // *** Producer of tasks *** //
        // ************************* //    
        void add_task(const std::function<void(std::uint32_t)>& task) 
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                m_task_queue.push(task);
            }
            m_condvar.notify_one();
        }

    private:
        // ************************* //    
        // *** Consumer of tasks *** //
        // ************************* //    
        // [Remark 1]
        // BUG : Missing "m_run.load()" leads to wakeup-miss on stop-request

        void thread_fct(std::uint32_t thread_id)
        {
            try 
            {
                while(m_run.load())
                {
                    std::optional<std::function<void(std::uint32_t)>> task;
                    {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        m_condvar.wait(lock, [this]() { return !m_task_queue.empty() || !m_run.load(); }); // <--- Remark 1
                        task = pop_task();
                    }
                    if (task) 
                    {
                        (*task)(thread_id);
                    }
                }

                // STOP REQUESTED, ALL THREADS RUNNING, NO WAITING.
                while(!m_task_queue.empty())                           // <--- one queue-empty-check before mutex
                {
                    std::optional<std::function<void(std::uint32_t)>> task;
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);
                        task = pop_task();                           // <--- one queue-empty-check after mutex
                    }
                    if (task) 
                    {
                        (*task)(thread_id);
                    }
                }
            }
            catch(std::exception& e) // BUG : Need to handle exception thrown from task
            {
                std::cout << "\nException thrown from alg::threadpool, thread_id " << thread_id << ", e = " << e.what() << std::flush;
            }
        }

        // ************************************** //
        // *** Combine std::queue<T>::front() *** //
        // ***    with std::queue<T>::pop()   *** //
        // ************************************** //
        std::optional<std::function<void(std::uint32_t)>> pop_task()
        {
            if (!m_task_queue.empty())               
            {
                auto task = std::move(m_task_queue.front());
                m_task_queue.pop();
                return task;
            }
            return std::nullopt;
        }

    private:
        std::atomic<bool> m_run;
        std::vector<std::thread> m_threads;
        std::queue<std::function<void(std::uint32_t)>> m_task_queue;

    private:
        // *** Substitute for sync_primitive in alg::threadpool_sync *** //
        mutable std::mutex m_mutex;
        std::condition_variable m_condvar;
    };
}

