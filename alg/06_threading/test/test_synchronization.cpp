#include<iostream>
#include<thread.h>
#include<synchronization.h>
#include<statistics.h>
#include<timer.h>
#include<utility.h>


namespace test
{
    template<typename SYNC>
    void sync_test(const std::string& test_name, std::uint32_t N, std::uint32_t us)
    {
        SYNC sync;
        std::atomic<std::uint32_t> ready = 0;


        // ******************* //
        // *** Measurement *** //
        // ******************* //
        alg::statistics<std::uint64_t> stat{};
        alg::timer timer;


        // **************** //
        // *** Consumer *** //
        // **************** //
        std::thread consumer
        (
            [&]()
            {
                alg::set_this_thread_affinity(2);
                alg::set_this_thread_priority();
                for(std::uint32_t n=0; n!=N; ++n)
                {
                    ready.fetch_add(1); // <--- sync point
                    sync.wait();        // <--- consumer is blocked

                    // stop timer
                    timer.click();
                    stat.add(timer.time_elapsed_in_nsec());
                }
            }
        );


        // **************** //
        // *** Producer *** //
        // **************** //
        {
            alg::set_this_thread_affinity(4);
            alg::set_this_thread_priority();
            for(std::uint32_t n=0; n!=N; ++n)
            {
                while(ready.load()!=n+1) 
                {
                    std::this_thread::yield(); // <--- sync point (yield allows consumer to fetch-add)
                }
                std::this_thread::sleep_for(std::chrono::microseconds(us)); // <--- wait until consumer is blocked

                // start timer
                timer.click();
                sync.notify();
            }
        }

        consumer.join();
        print_summary(test_name, "succeeded, percentile = " + stat.get_str());
    }
}


void test_synchronization()
{
    constexpr std::uint32_t N  = 1000; // must be compile time const for sync_promfut
    constexpr std::uint32_t us =  300;

    // ***************** //
    // *** 9 methods *** //
    // ***************** //
    test::sync_test<alg::sync_futex>     ("synchronization 0 with sync_futex",      N, us);
    test::sync_test<alg::sync_mutex>     ("synchronization 1 with sync_mutex",      N, us);
    test::sync_test<alg::sync_pmutex>    ("synchronization 2 with sync_pmutex",     N, us);
    test::sync_test<alg::sync_HansBarz>  ("synchronization 3 with sync_HansBarz",   N, us);
    test::sync_test<alg::sync_semaphore> ("synchronization 4 with sync_semaphore",  N, us);
    test::sync_test<alg::sync_psemaphore>("synchronization 5 with sync_psemaphore", N, us);
    test::sync_test<alg::sync_condvar>   ("synchronization 6 with sync_condvar",    N, us);
    test::sync_test<alg::sync_promfut<N>>("synchronization 7 with sync_promfut",    N, us);
    test::sync_test<alg::sync_atomic>    ("synchronization 8 with sync_atomic",     N, us);
}
