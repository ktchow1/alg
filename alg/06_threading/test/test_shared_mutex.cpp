#include<iostream>
#include<cassert>

// *** alg ***//
#include<thread.h>
#include<shared_mutex.h>
#include<utility.h>


// ******************************* //
// Main thread is the only writer. 
// Multi threads as readers.
// ******************************* //
template<typename SHARED_MUTEX>
void test_shared_mutex(const std::string& test_name)
{
    std::uint32_t num_readers = 5;
    std::uint32_t num_tasks   = 100; 
    std::uint32_t live_data   = 0;    // BUG : remember to set zero, otherwise work_done are incorrect
    std::vector<std::uint32_t> work_done(num_readers, 0);
    std::vector<std::thread> threads;
    std::stop_source source;
    SHARED_MUTEX shared_mutex;


    // ********************** //
    // *** reader threads *** //
    // ********************** //
    for(std::uint32_t n=0; n!=num_readers; ++n)
    {
        threads.push_back(std::thread
        {
            [token = source.get_token()](const std::uint32_t&  live_data, 
                                               std::uint32_t&  work_done, 
                                               SHARED_MUTEX&   shared_mutex)
            {
                std::uint32_t prev_data = 0;
                while(!token.stop_requested())
                {
                    {
                        std::shared_lock<SHARED_MUTEX> lock(shared_mutex);
                        if (live_data != prev_data) 
                        {
                            work_done += live_data;
                            prev_data  = live_data;
                        }
                    }
                    std::this_thread::yield();
                }
            }, std::cref(live_data), std::ref(work_done[n]), std::ref(shared_mutex)
        });
    }

    // ********************* //
    // *** writer thread *** //
    // ********************* //
    std::uint32_t ans       = 0;
    std::uint32_t prev_data = 0;
    for(std::uint32_t m=0; m!=num_tasks; ++m)
    {
        std::uint32_t data = rand() % 1000 + 10;
        if (data != prev_data)
        {
            std::lock_guard<SHARED_MUTEX> lock(shared_mutex);
            live_data = data;
            prev_data = data;    
            ans      += data;
        }

        // wait for all readers to consume live_data, then proceed to produce next
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    source.request_stop();
    for(auto& x:threads) 
    {
        x.join();
    }


    // **************** //
    // *** checking *** //
    // **************** //
    std::stringstream ss;
    ss << "writer = " << ans << ", ";
    ss << "reader = [";

    for(std::uint32_t n=0; n!=num_readers; ++n)
    {
        assert(work_done[n] == ans);
        ss << work_done[n] << " ";
    }
    ss << "]";
    print_summary(test_name, "succeeded, " + ss.str());
}


void test_shared_mutex()
{
    test_shared_mutex<std::shared_mutex>("test std::shared_mutex");
    test_shared_mutex<alg::shared_mutex>("test alg::shared_mutex");
}
