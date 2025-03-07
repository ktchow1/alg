#include<iostream>
#include<sstream>
#include<cassert>
#include<vector>
#include<atomic>
#include<thread>
#include<semaphore>
#include<utility.h>


void test_semaphore()
{
    std::uint32_t num_threads = 12;
    std::vector<std::thread> threads;
    std::vector<std::uint32_t> work_done(num_threads, 0);
    std::stop_source source;

    // ***************** //
    // *** semaphore *** // 
    // ***************** //
    //                      +-------------------- maximum count of semaphore
    //                      v            v------- initial count of semaphore
    std::counting_semaphore<5> semaphore{5};
    std::atomic<std::int32_t> cur_count = 0;
    std::atomic<std::int32_t> min_count = 5;
    std::atomic<std::int32_t> max_count = 0;


    // *********************** //
    // *** spawned threads *** //
    // *********************** //
    for(std::uint32_t n=0; n!=num_threads; ++n)
    {
        threads.push_back(std::thread
        {
            [&](std::uint32_t& work_done)
            {
                auto token = source.get_token();
                while(!token.stop_requested())
                {
                    semaphore.acquire();
                    cur_count.fetch_add(1);
                    {
                        // Remark : replace with fetch_min / fetch_max in c++26
                        std::int32_t tp0 = min_count.load();
                        std::int32_t min = std::min(tp0, cur_count.load()); 
                        min_count.compare_exchange_strong(tp0, min);

                        std::int32_t tp1 = max_count.load();
                        std::int32_t max = std::max(tp1, cur_count.load()); 
                        max_count.compare_exchange_strong(tp1, max);
                    }

                    ++work_done;
                    std::this_thread::sleep_for(std::chrono::microseconds(200 + rand() % 500));

                    cur_count.fetch_sub(1);
                    semaphore.release();
                    {
                        std::int32_t tp0 = min_count.load();
                        std::int32_t min = std::min(tp0, cur_count.load()); 
                        min_count.compare_exchange_strong(tp0, min);

                        std::int32_t tp1 = max_count.load();
                        std::int32_t max = std::max(tp1, cur_count.load()); 
                        max_count.compare_exchange_strong(tp1, max);
                    }
                }
            }, std::ref(work_done[n])
        }); 
    }


    // ******************* //
    // *** main thread *** //
    // ******************* //
    std::this_thread::sleep_for(std::chrono::milliseconds(800));
    source.request_stop();
    for(auto& x:threads)
    {
        x.join();
    }

    
    // **************** //
    // *** checking *** //
    // **************** //
    assert(min_count.load() == 0);
    assert(max_count.load() == 5);
    assert(cur_count.load() == 0);

    std::stringstream ss;
    ss << "[";
    for(std::uint32_t n=0; n!=num_threads; ++n)
    {        
        assert(work_done[n] > 50); // 50 is a loose checking, should be around 800msec / 800usec 
        ss << work_done[n] << " ";
    }
    ss << "]";
    print_summary("test std::counting_semaphore", "succeeded, work_done = " + ss.str());
}

