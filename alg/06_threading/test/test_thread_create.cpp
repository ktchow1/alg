#include<iostream>
#include<mutex>
#include<thread.h>
#include<utility.h>


namespace test
{
    void task_fct(std::uint32_t x, const std::string& y, const std::vector<bool>& z)
    {
        // do something
    }

    class task
    {
    public:
        void fct(std::uint32_t x, const std::string& y, const std::vector<bool>& z)
        {
            // do something
        }

        void operator()(std::uint32_t x, const std::string& y, const std::vector<bool>& z)
        {
            // do something
        }
    };

    struct task_arg
    {
        std::uint32_t      x;    
        std::string        y;
        std::vector<bool>  z;
    };

    void* task_pfct(void* arg)
    {
        task_arg* p_arg = reinterpret_cast<task_arg*>(arg);

        // do something
        return nullptr;
    }
}


void create_thread()
{
    std::vector<std::thread> threads;
    test::task_arg arg;

    // ************** //
    // *** Create *** //
    // ************** //
    threads.emplace_back(test::task_fct, arg.x, std::cref(arg.y), std::cref(arg.z));
    threads.emplace_back(test::task_fct, arg.x, std::cref(arg.y), std::cref(arg.z));
    threads.emplace_back(test::task_fct, arg.x, std::cref(arg.y), std::cref(arg.z));

    // ************ //
    // *** Join *** //
    // ************ //
    for(auto& x:threads) 
    {
        x.join();        
    }
    print_summary("create -  thread", "succeeded");
}

void create_jthread()
{
    {
        std::vector<std::jthread> jthreads;
        test::task_arg arg;

        // ************** //
        // *** Create *** //
        // ************** //
        jthreads.emplace_back(test::task_fct, arg.x, std::cref(arg.y), std::cref(arg.z));
        jthreads.emplace_back(test::task_fct, arg.x, std::cref(arg.y), std::cref(arg.z));
        jthreads.emplace_back(test::task_fct, arg.x, std::cref(arg.y), std::cref(arg.z));

        // ***************** //
        // *** Auto join *** //
        // ***************** //
    }
    print_summary("create - jthread", "succeeded");
}

void create_pthread()
{
    constexpr std::uint32_t num_threads = 3;
    pthread_t threads[num_threads];
    test::task_arg arg;

    // ************** //
    // *** Create *** //
    // ************** //
    // arg 0 = pthread object
    // arg 1 = pthread_attr_t which is NULL for default
    // arg 2 = function taking void* returning void*
    // arg 3 = function argument
    pthread_create(threads+0, NULL, test::task_pfct, &arg);
    pthread_create(threads+1, NULL, test::task_pfct, &arg);
    pthread_create(threads+2, NULL, test::task_pfct, &arg);

    // ************ //
    // *** Join *** //
    // ************ //
    for(std::uint32_t n=0; n!=num_threads; ++n) 
    {
        pthread_join(threads[n], NULL);        
    }
    print_summary("create - pthread", "succeeded");
}

void test_thread_create()
{
    create_thread();
    create_jthread();
    create_pthread();
}
