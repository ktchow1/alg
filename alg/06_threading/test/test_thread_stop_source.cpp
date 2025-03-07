#include<iostream>
#include<cassert>
#include<vector>
#include<thread>
#include<functional> // for std::placeholders
#include<utility.h>


// ****************************************************************************** //
// Thread functor for std::thread is nullary.
// Thread functor for std::jthread is either nullary or unary (with stop source).
// ****************************************************************************** //
void stop_source_thread()
{
    std::uint32_t num_threads = 5;
    std::vector<std::uint32_t> work_done(num_threads,0);
    std::vector<std::thread> threads;
    std::stop_source source;


    // *********************** //
    // *** spawned threads *** //
    // *********************** //
    for(std::uint32_t n=0; n!=num_threads; ++n)
    {
        threads.push_back(std::thread
        {
            [token = source.get_token()](std::uint32_t thread_id, std::uint32_t& work)
            {
                while(!token.stop_requested())
                {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10 + thread_id));
                    ++work;
                }
            }, n, std::ref(work_done[n]) // BUG : dont forget std::ref, since this is implicit std::bind
        }); 
    }


    // ******************* //
    // *** main thread *** //
    // ******************* //
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    source.request_stop();
    for(auto& x:threads)
    {
        x.join();
    }

    
    // **************** //
    // *** checking *** //
    // **************** //
    std::stringstream ss;
    for(std::uint32_t n=0; n!=num_threads; ++n)
    {
        assert(work_done[n] > 5);
        ss  << work_done[n] << " ";
    }
    print_summary("stop source -  thread", "succeeded, work_done = " + ss.str());
}

  
void stop_source_jthread()
{
    std::uint32_t num_threads = 5;
    std::vector<std::uint32_t> work_done(num_threads,0);
    {
        std::vector<std::jthread> jthreads;
    //  std::stop_source source; 


        // *********************** //
        // *** spawned threads *** //
        // *********************** //
        for(std::uint32_t n=0; n!=num_threads; ++n)
        {
            jthreads.push_back(std::jthread
            {
                [](std::stop_token token, std::uint32_t thread_id, std::uint32_t& work)
                {
                    while(!token.stop_requested())
                    {
                        std::this_thread::sleep_for(std::chrono::milliseconds(10 + thread_id));
                        ++work;
                    }
                }, n, std::ref(work_done[n])
            }); 
        }


        // ******************* //
        // *** main thread *** //
        // ******************* //
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //  source.request_stop();
    } 


    // **************** //
    // *** checking *** //
    // **************** //
    std::stringstream ss;
    for(std::uint32_t n=0; n!=num_threads; ++n)
    {
        assert(work_done[n] > 5);
        ss  << work_done[n] << " ";
    }
    print_summary("stop source - jthread", "succeeded, work_done = " + ss.str());
} 


void test_thread_stop_source()
{
    stop_source_thread();
    stop_source_jthread();
}
