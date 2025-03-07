#include<iostream>
#include<cassert>
#include<thread>
#include<future>
#include<promise_future.h>
#include<utility.h>


template<template<typename> typename PROMISE>
void test_promise_future_impl(const std::string& test_name)
{
    std::uint32_t num_tasks = 50;
    for(std::uint32_t n=0; n!=num_tasks; ++n)
    {
        // ******************************************* //
        // *** promise-future is 1 item SPSC queue *** //
        // ******************************************* //
        PROMISE<std::uint32_t> promise;
        auto future = promise.get_future();


        // **************** //
        // *** producer *** //
        // **************** //
        std::thread t0([&]()
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            promise.set_value(n + 123);
        });


        // ************** //
        // *** future *** //
        // ************** //
        std::thread t1([&]()
        {
            assert(future.get() == n + 123);
        });


        t0.join(); 
        t1.join(); 
    }
    print_summary(test_name, "succeeded");
}


void test_promise_future()
{
    test_promise_future_impl<alg::promise>("test alg::promise/future");
    test_promise_future_impl<std::promise>("test std::promise/future");
}
