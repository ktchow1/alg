#pragma once
#include <iostream>
#include <exception>
#include <coroutine>
#include <coroutine0_generator.h>


// ****************************************************************************** //
// How to define coroutine as pr using co_await?
//
// alg::task<my_product> my_coroutine(...)
// {
//     alg::awaitable<my_product> awaitable; 
//     auto x = co_await awaitable;  
//     consume(x);
// }
//
// Remark 1 : "co_await" to wait for newly created product from coroutine caller 
// Remark 2 : "return alg::task<my_product>" is NOT needed, my_coroutine is async 
// ****************************************************************************** //
namespace alg
{
    template<typename T, bool DEBUG>
    class task 
    {
        // ********************************* //
        // *** STL coroutine requirement *** //
        // ********************************* //
    public:
        struct promise_type 
        {
            promise_type() : m_num_awaits(0)
            {
                debug<DEBUG>("promise_type::promise_type");
            }

            // Factory of task (construct task from coroutine frame handle)
            task<T,DEBUG> get_return_object()  
            {
                debug<DEBUG>("promise_type::get_return_object");
                return task<T,DEBUG>
                { 
                    std::coroutine_handle<promise_type>::from_promise(*this) 
                };
            }   

            // Suspension policy of coroutine caller governed by return type
            std::suspend_never initial_suspend()          { debug<DEBUG>("promise_type::initial_suspend"); return {}; }
            std::suspend_never   final_suspend() noexcept { debug<DEBUG>("promise_type::final_suspend");   return {}; }
            void unhandled_exception()                    { }

            // Transfer of product (from task<T> to coroutine) is done in awaitable


            // ************************** //
            // *** Product and states *** //
            // ************************** //
            T             m_product; 
            std::uint32_t m_num_awaits; 
        };


    private:
        // Pointer to coroutine frame
        std::coroutine_handle<promise_type> m_handle;


    public:
        explicit task(std::coroutine_handle<promise_type> handle) : m_handle(handle) 
        {
            debug<DEBUG>("task::task");
        } 

        // RAII of coroutine frame
       ~task()
        {
            debug<DEBUG>("task::~task");
            if (m_handle) m_handle.destroy();
        }


        // ********************************************* //
        // *** Custom functions for coroutine caller *** //
        // ********************************************* //
        void set_product_copy(const T& product)
        {
            debug<DEBUG>("task::set_product");
            m_handle.promise().m_product = product;
            m_handle(); // yield to coroutine
        }

        template<typename...ARGS>
        void set_product(ARGS&&...args) 
        {
            debug<DEBUG>("task::set_product");
            new (&m_handle.promise().m_product) T{ std::forward<ARGS>(args)... };
            m_handle(); // yield to coroutine
        }
    };
}


// ********************************************** //
// *** Data transfer from task<T> to co_await *** //
// ********************************************** //
namespace alg
{
    template<typename T, bool DEBUG>
    class awaitable // <--- we can also call it like "asyn_produce"
    {
    public:
        awaitable() 
        {
            debug<DEBUG>("awaitable::awaitable");
        }

        bool await_ready() const noexcept 
        { 
            debug<DEBUG>("awaitable::await_ready");
            return false; 
        }

        // Connection between task and awaitable, coroutine framge handle will be passed to awaitable on "co_await"
        bool await_suspend(std::coroutine_handle<typename task<T,DEBUG>::promise_type> handle) 
        {
            debug<DEBUG>("awaitable::await_suspend");
            m_handle = handle; // deep copy of light weight handle  
            m_handle.promise().m_num_awaits++;
            return true; 
        }

        // Return product produced by coroutine caller to coroutine on co_await
        const T& await_resume() const noexcept
        { 
            debug<DEBUG>("awaitable::await_resume");
            return m_handle.promise().m_product;
        }


    public:
        const T& get_product() const  
        {
            return m_handle.promise().m_product;
        }

        std::uint32_t get_num_awaits() const  
        {
            return m_handle.promise().m_num_awaits;
        }


    private:
        std::coroutine_handle<typename task<T,DEBUG>::promise_type> m_handle;
    }; 
}






