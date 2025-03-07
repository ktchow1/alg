#include<threadpool_cv.h>
#include<jthreadpool_cv.h>
#include<jthreadpool_cv2.h>

/*
// ***************************** //
// *** Sample coroutine task *** //
// ***************************** //
struct sample_future
{
    struct promise_type
    {
        promise_type(){}
        sample_future get_return_object()
        {
             return sample_future { std::coroutine_handle<promise_type>::from_promise(*this) };
        }
        void unhandled_exception(){}
        std::suspend_never initial_suspend() { return {}; }
        std::suspend_always  final_suspend() { return {}; } // BUG !!! suspend on quiting, as we need to check done().
    };

    std::coroutine_handle<promise_type> h;
};

[[nodiscard]] sample_future sample_coroutine(std::uint32_t task_id)
{
    for(std::uint32_t n=0; n!=5; ++n)
    {
        co_await std::suspend_always{};
        std::cout << "\ncoroutine task_id=" << task_id << ", step=" << n;
    }
}

void test_engine_for_coroutine()
{
    jthreadpool_cv2 pool(4);
    for(std::uint32_t n=0; n!=20; ++n)
    {
        auto task = sample_coroutine(n);
        pool.add_task(task.h);

        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
    pool.stop();
} */

// ************ //
// *** Test *** //
// ************ //
void test_threadpool_cv()
{
    // ****************** //
    // *** Threadpool *** //
    // ****************** //
    std::cout << "\n[Threadpool_cv : no throw]";
    try
    {
        test_engine<threadpool_cv>(10000,10000);
    }
    catch(std::exception& e)
    {
        std::cout << "\nexception caught " << e.what() << std::flush;
    }

    std::cout << "\n\n[Threadpool_cv : throw in main]";
    try
    {
        test_engine<threadpool_cv>(12,10000);
    }
    catch(std::exception& e)
    {
        std::cout << "\nexception caught in main, " << e.what() << std::flush;
    }

    std::cout << "\n\n[Threadpool_cv : throw in task]";
    try
    {
        test_engine<threadpool_cv>(10000,12);
    }
    catch(std::exception& e)
    {
        std::cout << "\nexception caught in main, " << e.what() << std::flush;
    }

    // ******************* //
    // *** Jthreadpool *** //
    // ******************* //
    std::cout << "\n\n[Jthreadpool_cv : no throw]";
    try
    {
        test_engine<jthreadpool_cv>(10000,10000);
    }
    catch(std::exception& e)
    {
        std::cout << "\nexception caught " << e.what() << std::flush;
    }

    std::cout << "\n\n[Jthreadpool_cv : throw in main]";
    try
    {
        test_engine<jthreadpool_cv>(12,10000);
    }
    catch(std::exception& e)
    {
        std::cout << "\nexception caught in main, " << e.what() << std::flush;
    }

    std::cout << "\n\n[Jthreadpool_cv : throw in task]";
    try
    {
        test_engine<jthreadpool_cv>(10000,12);
    }
    catch(std::exception& e)
    {
        std::cout << "\nexception caught in main, " << e.what() << std::flush;
    }
/*
    // ********************************* //
    // *** Jthreadpool for coroutine *** //
    // ********************************* //
    std::cout << "\n\n[Jthreadpool_cv2 : no throw]";
    try
    {
        test_engine_for_coroutine();
    }
    catch(std::exception& e)
    {
        std::cout << "\nexception caught " << e.what() << std::flush;
    }
*/
}
