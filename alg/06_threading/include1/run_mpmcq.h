#pragma once
#include<iostream>
#include<cassert>
#include<numeric>

// *** alg *** //
#include<thread.h>
#include<synchronization.h>
#include<statistics.h>
#include<utility.h>


struct task_output
{
    inline void mark_done(std::uint32_t thread_id)
    {
        ++m_value;
        m_thread_id = thread_id;
    }

    inline void mark_start_time()
    {
        clock_gettime(CLOCK_MONOTONIC, &m_ts0); 
    }

    inline void mark_stop_time()
    {
        clock_gettime(CLOCK_MONOTONIC, &m_ts1); 
    }

    inline std::uint64_t nanosec_elapsed() const noexcept
    {
        return alg::to_nanosec(m_ts1) - alg::to_nanosec(m_ts0);
    }

    std::uint32_t m_value = 0;
    std::uint32_t m_thread_id = 0;
    timespec      m_ts0;
    timespec      m_ts1;
};


class task_spec
{
public:
    task_spec() = default;

    explicit task_spec(task_output& output) : m_ptr(&output)
    {
        m_ptr->mark_start_time();
    }

    inline void operator()(std::uint32_t thread_id)
    {
        m_ptr->mark_stop_time();
        m_ptr->mark_done(thread_id);
    }

private:
    task_output* m_ptr = nullptr;
};


std::string task_check(std::uint32_t num_consumers, const std::vector<task_output>& task_outputs)
{
    std::vector<std::uint32_t> task_counts(num_consumers, 0); 
    alg::statistics<std::uint64_t> stat;

    for(const auto& x:task_outputs) 
    {
        assert(x.m_value == 1);
        assert(x.m_thread_id < num_consumers);
        ++task_counts[x.m_thread_id];
        stat.add(x.nanosec_elapsed());
    }

    std::stringstream ss;
    for(std::uint32_t n=0; n!=num_consumers; ++n) 
    {
        ss << "consumer_" << n << " = " << task_counts[n] << ", ";
    }
    ss << "total_task = " << std::accumulate(task_counts.begin(), task_counts.end(), 0) << ", ";
    ss << "time = "  << stat.get_str();
    return ss.str();
}


// ****************************************** //
// *** Test for producer / consumer queue *** //
// ****************************************** //
//
//            producer          consumer
// task_spec ----------> queue ----------> task_output
//
namespace alg
{
    template<template<typename> typename QUEUE> 
    void run_mpmcq(const std::string& test_name,
                   std::uint32_t num_producers,
                   std::uint32_t num_consumers, 
                   std::uint32_t num_tasks)
    {
        QUEUE<task_spec>           queue;
        std::vector<std::thread>   producers;
        std::vector<std::thread>   consumers;
        std::vector<task_output>   task_outputs(num_producers * num_tasks); // Each element is accessed by 1 consumer, no atomic needed.
        std::atomic<std::uint32_t> consumers_ready(0);
        std::stop_source source;


        // ***************** //
        // *** Consumers *** //
        // ***************** //
        for(std::uint32_t n=0; n!=num_consumers; ++n)
        {
            consumers.push_back(std::thread([&](std::uint32_t consumer_id)
            {
                set_this_thread_affinity(consumer_id); 
                set_this_thread_priority();           // This is necessary.
            //  set_this_thread_policy(SCHED_FIFO);   // This is ok, but NOT necessary.
                task_spec task;


                // *** Step 1 : Loop until stop requested *** //
                auto token = source.get_token();
                while(!token.stop_requested())
                {
                    consumers_ready.fetch_add(1);
                    if (queue.pop(task))
                    {
                        task(consumer_id);
                    } 
                }

                // *** Step 2 : Loop until queue is clear *** //
                while(queue.pop(task))
                {
                    task(consumer_id);
                } 
            }, n));
        }
        

        // ***************** //
        // *** Producers *** //
        // ***************** //
        for(std::uint32_t n=0; n!=num_producers; ++n)
        {
            producers.push_back(std::thread([&](std::uint32_t producer_id)
            {
                set_this_thread_affinity(num_consumers + producer_id);
                set_this_thread_priority();           // This is necessary.
            //  set_this_thread_policy(SCHED_FIFO);   // This is dangerous, it takes all resources.

                while(consumers_ready.load() < num_consumers);
                for(std::uint32_t m=0; m!=num_tasks; ++m)
                {
                    task_spec task{task_outputs[num_tasks * producer_id + m]};
                    while(!queue.push(task))
                    {
                        task = task_spec{task_outputs[num_tasks * producer_id + m]}; 
                    }
                    std::this_thread::sleep_for(std::chrono::nanoseconds(300)); // This is necessary. 
                }
            }, n));
        }


        // ******************* //
        // *** Main thread *** //
        // ******************* //
        set_this_thread_affinity(num_consumers + num_producers); // Main thread doesn't interfere producer / consumer threads.

        for(auto& x:producers) x.join();
        source.request_stop();
        for(auto& x:consumers) x.join();    

        
        // **************** //
        // *** Checking *** //
        // **************** //
        std::string comment = task_check(num_consumers, task_outputs);
        print_summary(test_name, "succeeded, " + comment);
    }
}
