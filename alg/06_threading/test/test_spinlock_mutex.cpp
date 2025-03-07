#include<iostream>
#include<cassert>
#include<map>

// *** alg ***//
#include<thread.h>
#include<spinlock.h>
#include<utility.h>


template<typename MUTEX>
void test_mutex(const std::string& test_name)
{
    std::uint32_t num_threads = 5;
    std::uint32_t num_tasks   = 1000;
    std::map<std::uint32_t, std::uint32_t> map; // common resource
    std::vector<std::thread> threads;
    MUTEX mutex;


    // *********************** //
    // *** spawned threads *** //
    // *********************** //
    for(std::uint32_t n=0; n!=num_threads; ++n)
    {
        threads.push_back(std::thread
        {
            [](std::uint32_t thread_id, std::uint32_t num_tasks, std::map<std::uint32_t, std::uint32_t>& map, MUTEX& mutex)
            {
                for(std::uint32_t m=0; m!=num_tasks; ++m)
                {
                    std::lock_guard<MUTEX> lock(mutex);
                    auto iter = map.find(m);
                    if (iter != map.end())
                    {
                        iter->second += thread_id+1;
                    }
                    else
                    {
                        map[m] = thread_id+1;
                    }
                }
            }, n, num_tasks, std::ref(map), std::ref(mutex)
        });
    }

    for(auto& x:threads)
    {
        x.join();
    } 


    // **************** //
    // *** checking *** //
    // **************** //
    const std::uint32_t ans = 1+2+3+4+5;

    for(std::uint32_t m=0; m!=num_tasks; ++m)
    {
        auto iter = map.find(m);
        assert(iter != map.end());
        assert(iter->second == ans);
    }
    print_summary(test_name, "succeeded");
}

void test_spinlock_mutex()
{
    test_mutex<std::mutex>     ("test std::mutex       ");
    test_mutex<alg::spinlock>  ("test alg::spinlock    ");
    test_mutex<alg::spinlock_p>("test alg::spinlock_p  ");
}
