#pragma once
#include <iostream>
#include <exception>
#include <coroutine>
#include <utility.h>


namespace alg
{
    template<typename T, typename U>
    class future 
    {
        // ********************************* //
        // *** STL coroutine requirement *** //
        // ********************************* //
    public:
        struct promise_type 
        {
            // Factory of task
            future<T,U> get_return_object() 
            { 
                return future<T,U>
                {
                    std::coroutine_handle<promise_type>::from_promise(*this) 
                };
            }   

            // Suspension policy of coroutine caller governed by return type
            std::suspend_never  initial_suspend()        { return {}; }
            std::suspend_never  final_suspend() noexcept { return {}; }
            void unhandled_exception(){}


            // ******************************************************* //
            // *** Transfer of product (from task<T> to coroutine) *** //
            // ******************************************************* //
            std::suspend_always yield_value(const U& u) 
            {
                data_U = u; 
                return {};
            }


            // *************** //
            // *** Product *** //
            // *************** //
            T data_T; // from caller to coroutine 
            U data_U; // from coroutine to caller 
        };

        
    public:
        explicit future(std::coroutine_handle<promise_type> h_) : h(h_) {} 
       ~future()
        {
            h.destroy();
        }

        // Mimic python : sink.send(data)
        template<typename... ARGS>
        void send_T_to_coroutine(ARGS&&... args) const // universal reference
        {
            h.promise().data_T = T{ std::forward<ARGS>(args)... };
            h.resume();
        }

        // Mimic python : data = next(source)
        const U& next_U_from_coroutine() const 
        {
            h();
            return h.promise().data_U;
        }


    private:
        std::coroutine_handle<promise_type> h;
    };
}


// *************************************** //
// *** Awaitable, a.k.a. Async produce *** //
// *************************************** //
namespace alg
{
    template<typename T, typename U>
    class awaitable_pc
    {
    public:
        awaitable_pc() : data_T_ptr(nullptr) 
        {
        }

        bool await_ready() const noexcept 
        { 
            return false; 
        }

        bool await_suspend(std::coroutine_handle<typename future<T,U>::promise_type> h) 
        {
            data_T_ptr = &(h.promise().data_T);
    //      data_U_ptr = &(h.promise().data_U); 
            return true; 
        }

        const T& await_resume() const noexcept
        { 
            return *data_T_ptr;
        }

    
    public:


    private:
        T* data_T_ptr = nullptr;
    //  U* data_U_ptr = nullptr; // As U is transmitted by co_yield, no need to keep it here.
    }; 
}
