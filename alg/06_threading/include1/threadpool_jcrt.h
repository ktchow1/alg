#pragma once
#include<iostream>
#include<vector>
#include<queue>
#include<functional>
#include<coroutine> // <--- Replace std::function-task by coroutine-task
#include<thread>
#include<mutex>
#include<condition_variable>


namespace alg
{
    class threadpool_jcrt
    {
    public:
        explicit threadpool_jcrt(std::uint32_t num_threads)
        {
            for(std::uint32_t n=0; n!=num_threads; ++n)
            {
                m_jthreads.emplace_back(std::jthread(&threadpool_jcrt::thread_fct, this, m_stop_source.get_token(), n));
            }
        }

        void stop()
        {
            m_stop_source.request_stop();
            for(auto& x:m_jthreads) x.join();
        }

    public:
        // ************************* //    
        // *** Producer of tasks *** //
        // ************************* //    
        // Ensure that task.promise().final_suspend() return std::suspend_always
        // Otherwise, crash happens when we check-done, as task does not exists.
        //
        //   if (!task.done()) ...
        //
        
        void add_task(const std::coroutine_handle<>& task)
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
        void thread_fct(std::stop_token s_token, std::uint32_t thread_id)
        {
            try
            {
                while(!m_stop_source.get_token().stop_requested())
                {
                    std::coroutine_handle<> task;
                    {
                        std::unique_lock<std::mutex> lock(m_mutex);
                        if (!m_condvar.wait(lock, s_token, [this]()
                        {
                            return !m_task_queue.empty();
                        }))
                        {
                            break;
                        }

                        task = std::move(m_task_queue.front());
                        m_task_queue.pop();
                    }
                    task();

                    // *** Unfinished coroutine *** //
                    if (!task.done()) add_task(task);
                }

                // STOP REQUESTED, ALL THREADS RUNNING, NO WAITING.
                while(!m_task_queue.empty())
                {
                    std::coroutine_handle<> task;
                    {
                        std::lock_guard<std::mutex> lock(m_mutex);
                        task = std::move(m_task_queue.front());
                        m_task_queue.pop();
                    }
                    task();

                    // *** Unfinished coroutine *** //
                    if (!task.done()) add_task(task);
                }
            }
            catch(std::exception& e)
            {
                std::cout << "\nException thrown from alg::threadpool_jcrt, thread_id " << thread_id << ", e = " << e.what() << std::flush;
            }
        }

    private:
        std::stop_source m_stop_source;           
        std::vector<std::jthread> m_jthreads;
        std::queue<std::coroutine_handle<>> m_task_queue;

    private:
        mutable std::mutex m_mutex;
        std::condition_variable_any m_condvar;
    };
}
