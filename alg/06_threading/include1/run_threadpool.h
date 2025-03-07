#pragma once
#include<iostream>
#include<functional>

// *** alg *** //
#include<run_mpmcq.h>
#include<threadpool_sync.h>
#include<threadpool.h>
#include<threadpool_j.h>
#include<threadpool_jcrt.h>
#include<utility.h>


// ******************************** //
// *** Test for threadpool_sync *** //
// ******************************** //
namespace alg
{
    template<template<typename> typename QUEUE>
    void run_threadpool_sync(const std::string& test_name, 
                             std::uint32_t num_threads, 
                             std::uint32_t num_tasks, 
                             std::uint32_t delay_between_tasks_in_us)
    {
        // ******************* //
        // *** Main thread *** //
        // ******************* //
        std::vector<task_output> task_outputs(num_tasks);
        {
            alg::threadpool_sync<task_spec, QUEUE> pool(num_threads);
            for(std::uint32_t n=0; n!=num_tasks; ++n)
            {
                while(!pool.emplace_task(task_outputs[n])) 
                {
                    std::this_thread::sleep_for(std::chrono::microseconds(1));
                }
                std::this_thread::sleep_for(std::chrono::microseconds(delay_between_tasks_in_us));
            }
            pool.stop();
        }
        // threadpool_sync is destructed, joins all threads, task_outputs are ready
          

        // **************** //
        // *** Checking *** //
        // **************** //
        std::string comment = task_check(num_threads, task_outputs);
        print_summary(test_name, "succeeded, " + comment);
    }
}

  
// ************************************************************ //
// *** Test for threadpool / threadpool_j / threadpool_jcrt *** //
// ************************************************************ //
namespace alg
{
    template<typename POOL>
    void run_threadpool_condvar(const std::string& test_name, 
                                std::uint32_t num_threads, 
                                std::uint32_t num_tasks, 
                                std::uint32_t delay_between_tasks_in_us)
    {
        // ******************* //
        // *** Main thread *** //
        // ******************* //
        std::vector<task_output> task_outputs(num_tasks);
        {
            POOL pool(num_threads);
            for(std::uint32_t n=0; n!=num_tasks; ++n)
            {
                task_spec task{task_outputs[n]};
                pool.add_task(task); // add_task always succeed, no retry needed

                std::this_thread::sleep_for(std::chrono::microseconds(delay_between_tasks_in_us));
            }
            pool.stop();
        }
          

        // **************** //
        // *** Checking *** //
        // **************** //
        std::string comment = task_check(num_threads, task_outputs);
        print_summary(test_name, "succeeded, " + comment);
    }
}
