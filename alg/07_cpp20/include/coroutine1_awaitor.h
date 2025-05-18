#pragma once
#include <iostream>
#include <exception>
#include <coroutine>
#include <coroutine0_generator.h>


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
            promise_type() 
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

            // Return type governs suspension policy of coroutine caller
            std::suspend_never initial_suspend()          { debug<DEBUG>("promise_type::initial_suspend"); return {}; }
            std::suspend_never   final_suspend() noexcept { debug<DEBUG>("promise_type::final_suspend");   return {}; }
            void unhandled_exception()                    { }

            // The product
            T m_product; 
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
        void set_product(const T& product)
        {
            debug<DEBUG>("task::set_product");
            m_handle.promise().m_product = product;
            m_handle(); // yield to coroutine
        }
    };


    // ********************************************** //
    // *** Data transfer from task<T> to co_await *** //
    // ********************************************** //
    template<typename T, bool DEBUG>
    class awaitable
    {
    public:
        awaitable() : m_product_ptr(nullptr), m_num_awaits(0)
        {
            debug<DEBUG>("awaitable::awaitable");
        }

        bool await_ready() const noexcept 
        { 
            debug<DEBUG>("awaitable::await_ready");
            return false; 
        }

        // Connection between task and awaitable, coroutine handle will be passed to awaitable on "co_await"
        bool await_suspend(std::coroutine_handle<typename task<T,DEBUG>::promise_type> handle) 
        {
            debug<DEBUG>("awaitable::await_suspend");
            ++m_num_awaits;
            m_product_ptr = &(handle.promise().m_product);   
            return true; 
        }

        // Return product produced by coroutiner caller to coroutine on co_await
        T* await_resume() const noexcept
        { 
            debug<DEBUG>("awaitable::await_resume");
            return m_product_ptr;
        }


    public:
        std::uint32_t get_num_awaits() const  
        {
            return m_num_awaits;
        }


    private:
        T* m_product_ptr;

        // Other states
        std::uint32_t m_num_awaits; 
    }; 
}







// ******************** // 
// *** Experiment 0 *** //
// ******************** // 
struct future0 
{
    struct promise_type // This is promise_type, NOT promise.
    {
        promise_type()                                {  std::cout << "\npromise::promise";             }
        future0 get_return_object()                   {  std::cout << "\npromise::ret_obj"; return {};  }
        void unhandled_exception()                    {  }
        std::suspend_never initial_suspend()          {  std::cout << "\npromise::initial"; return {};  } 
        std::suspend_never   final_suspend() noexcept {  std::cout << "\npromise::final";   return {};  }
    };
    
    future0()
    {
        std::cout << "\nfuture::future"; 
    }
};

struct awaitable0
{
    explicit awaitable0(std::coroutine_handle<>* h_ptr_) : h_ptr(h_ptr_)
    {
        std::cout << "\nawait::await"; 
    }

    // No printing for constexpr fct.
    bool await_ready()  const noexcept            { std::cout << "\nawait::ready";     return false; } 
    void await_suspend(std::coroutine_handle<> h) { std::cout << "\nawait::suspend(h)";  *h_ptr = h; }
    void await_resume() const noexcept            { std::cout << "\nawait::resume";                  }

    std::coroutine_handle<>* h_ptr;
};

future0 coroutine0(std::coroutine_handle<>* h_ptr)
{
    awaitable0 a{ h_ptr };
    for(std::uint32_t n=0;; ++n) 
    {
        co_await a;
        std::cout << "\ncoroutine::iteration " << n << " with thread_id = " << std::this_thread::get_id(); 
    }
}


    
