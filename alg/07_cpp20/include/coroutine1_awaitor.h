#pragma once
#include <iostream>
#include <exception>
#include <coroutine>
#include <coroutine0_generator.h>


namespace alg
{
    template<typename T, typename U, bool DEBUG = true>
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
            task<T,U> get_return_object()  
            {
                debug<DEBUG>("promise_type::get_return_object");
                return task<T,U>
                { 
                    std::coroutine_handle<promise_type>::from_promise(*this) 
                };
            }   

            // Return type governs suspension policy of coroutine caller
            std::suspend_never initial_suspend()          { debug<DEBUG>("promise_type::initial_suspend"); return {}; }
            std::suspend_never   final_suspend() noexcept { debug<DEBUG>("promise_type::final_suspend");   return {}; }
            void unhandled_exception()                    { }

            // The product
            T data_T; 
            U data_U;
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
        T& get_product_T_ref()
        {
            debug<DEBUG>("task::get_product_by_ref");
            return m_handle.promise().data_T;
        }

        U& get_product_U_ref()
        {
            debug<DEBUG>("task::get_product_by_ref");
            return m_handle.promise().data_U;
        }

        void push_product()
        {
            m_handle(); // yield to coroutine
        }
    };

    template<typename T, typename U>
    struct awaitable
    {
        awaitable() : data_T_ptr(nullptr), data_U_ptr(nullptr)
        {
            std::cout << "\nawaitable::awaitable"; 
        }

        bool await_ready() const noexcept { return false; }
        bool await_suspend(std::coroutine_handle<typename task<T,U>::promise_type> handle) // <--- need to add promise_type inside handle
        {
            std::cout << "\nawaitable::await_suspend";
            data_T_ptr = &(handle.promise().data_T);   
            data_U_ptr = &(handle.promise().data_U);   
            return true; 
        }
        std::pair<T*,U*> await_resume() const noexcept
        { 
            std::cout << "\nawaitable::await_resume";
            return std::make_pair(data_T_ptr, data_U_ptr);
        }

    private:
        // Keep data-pointer instead of handle-pointer
        T* data_T_ptr;
        U* data_U_ptr;
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


// ******************** // 
// *** Experiment 1 *** //
// ******************** // 
struct future1 
{
    struct promise_type 
    {
        promise_type()                                {   std::cout << "\npromise::promise";   }
        future1 get_return_object()                   {   std::cout << "\npromise::ret_obj";  
                                                          return future1
                                                          { 
                                                              std::coroutine_handle<promise_type>::from_promise(*this) 
                                                          };
                                                      }   
        void unhandled_exception()                    {   }
        std::suspend_never initial_suspend()          {   std::cout << "\npromise::initial"; return {};   }
        std::suspend_never   final_suspend() noexcept {   std::cout << "\npromise::final";   return {};   }
    };

    explicit future1(std::coroutine_handle<promise_type> h_) : h(h_) 
    {
        std::cout << "\nfuture::future"; 
    } 

    // *** Conversion operator *** //
    operator std::coroutine_handle<promise_type>() const 
    {
        std::cout << "\nfuture::convert_to_handle"; 
        return h;
    } 

    std::coroutine_handle<promise_type> h;
};

[[nodiscard]] future1 coroutine1()
{
    for(std::uint32_t n=0;; ++n) 
    {
        co_await std::suspend_always{};
        std::cout << "\ncoroutine::iteration " << n << " with thread_id = " << std::this_thread::get_id();
    }
}



    
