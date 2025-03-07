#include<iostream>
#include<future>
#include<unordered_set>
#include<thread.h>
#include<timer.h>
#include<statistics.h> 
#include<utility.h> 


void timer_resolution()
{
    alg::statistics<std::uint64_t> stat;
    alg::timer timer;

    for(std::uint32_t n=0; n!=1000; ++n)
    {
        timer.click(); 
        timer.click(); 
        stat.add(timer.time_elapsed_in_nsec());
    }
    print_summary("time thread - resolution   ", "succeeded, percentile = " + stat.get_str());
}


void time_thread_creation()
{
    alg::statistics<std::uint64_t> stat;
    alg::timer timer;

    for(std::uint32_t n=0; n!=1000; ++n)
    {
        timer.click();
        std::thread t([&]()
        {
            timer.click();
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        });

        t.join();
        stat.add(timer.time_elapsed_in_nsec());
    }
    print_summary("time thread - thread create", "succeeded, percentile = " + stat.get_str());
}


void time_thread_creation_by_async_call()
{
    alg::statistics<std::uint64_t> stat;
    alg::timer timer;

    for(std::uint32_t n=0; n!=1000; ++n)
    {
        timer.click();
        std::future<std::uint64_t> future = std::async([&timer]()
        {
            timer.click();
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            return timer.time_elapsed_in_nsec();
        });

        stat.add(future.get()); // thread join inside
    }
    print_summary("time thread - async call   ", "succeeded, percentile = " + stat.get_str());
}


void time_thread_affinity()
{
    alg::statistics<std::uint64_t> stat;
    std::unordered_set<std::uint32_t> core_pre_affinity;
    std::unordered_set<std::uint32_t> core_post_affinity;
    alg::timer timer;

    for(std::uint32_t n=0; n!=5000; ++n)
    {
        std::thread t([&]()
        {
            for(std::uint32_t n=0; n!=20; ++n)
            {
                std::this_thread::yield();
                core_pre_affinity.insert(sched_getcpu());
            }

            timer.click();
            alg::set_this_thread_affinity(2);
            timer.click();
            stat.add(timer.time_elapsed_in_nsec());

            for(std::uint32_t n=0; n!=20; ++n)
            {
                std::this_thread::yield();
                core_post_affinity.insert(sched_getcpu());
            }
        });

        t.join();
    }

    std::stringstream ss0;
    std::stringstream ss1;
    for(const auto&x : core_pre_affinity)  ss0 << x << " ";
    for(const auto&x : core_post_affinity) ss1 << x << " ";
    print_summary("time thread - set affinity ", "succeeded, percentile = " + stat.get_str());
    print_summary("time thread - set affinity ", "core used  pre  affinity = [" + ss0.str() + "]");
    print_summary("time thread - set affinity ", "core used  post affinity = [" + ss1.str() + "]");
}


void time_thread_priority()
{
    alg::statistics<std::uint64_t> stat;
    alg::timer timer;

    for(std::uint32_t n=0; n!=1000; ++n)
    {
        std::thread t([&]()
        {
            timer.click();
            alg::set_this_thread_policy(SCHED_RR);
            timer.click();
            stat.add(timer.time_elapsed_in_nsec());
        });

        t.join();
    }
    print_summary("time thread - set priority ", "succeeded, percentile = " + stat.get_str());
}


void time_mutex_lock_and_unlock()
{
    std::mutex mutex;
    alg::statistics<std::uint64_t> stat0;
    alg::statistics<std::uint64_t> stat1;
    alg::timer timer;

    for(std::uint32_t n=0; n!=1000; ++n)
    {
        timer.click();
        mutex.lock();
        timer.click();
        stat0.add(timer.time_elapsed_in_nsec());

        timer.click();
        mutex.unlock();
        timer.click();
        stat1.add(timer.time_elapsed_in_nsec());
    }
    print_summary("time thread - mutex   lock ", "succeeded, percentile = " + stat0.get_str());
    print_summary("time thread - mutex unlock ", "succeeded, percentile = " + stat1.get_str());
}


void test_thread_timing()
{
    timer_resolution();
    time_thread_creation();
    time_thread_creation_by_async_call();
    time_thread_affinity();
    time_thread_priority();
    time_mutex_lock_and_unlock();
}


