#pragma once
#include <iostream>
#include <exception>
#include <coroutine>
#include <utility.h>


// ************************************ //
// Coroutine as both : 
// * consumer of product T, and ...
// * producer of product U
// ************************************ //
namespace alg
{
    template<typename T, typename U>
    class generator_pc
    {
        // ********************************* //
        // *** STL coroutine requirement *** //
        // ********************************* //
    public:
        struct promise_type 
        {
            promise_type() = default;

            // Factory of generator 
            generator_pc<T,U> get_return_object() 
            { 
                return generator_pc<T,U>
                {
                    std::coroutine_handle<promise_type>::from_promise(*this) 
                };
            }   

            // Suspension policy of coroutine caller governed by return type
            std::suspend_never  initial_suspend()          { return {}; }
            std::suspend_never    final_suspend() noexcept { return {}; }
            void unhandled_exception()                     {            }


            // ********************************************************* //
            // *** Transfer of product (from coroutine to generator) *** //
            // ********************************************************* //
            // Suspension policy of coroutine governed by return type
            //
            std::suspend_always yield_value(const U& u) 
            {
                m_product_from_coroutine = u; 
                return {};
            }


            // *************** //
            // *** Product *** //
            // *************** //
            T m_product_from_caller;
            U m_product_from_coroutine; 
        };

        
    public:
        explicit generator_pc(std::coroutine_handle<promise_type> handle) : m_handle(handle) 
        {
        } 

       ~generator_pc()
        {
            if (m_handle) m_handle.destroy();
        }


        // ********************************************* //
        // *** Custom functions for coroutine caller *** //
        // ********************************************* //
        template<typename... ARGS>
        void set_product_T(ARGS&&... args) 
        {
            m_handle.promise().m_product_from_caller = T{ std::forward<ARGS>(args)... };
            m_handle.resume();
        }

        const U& get_product_U() const 
        {
            m_handle.resume();
            return m_handle.promise().m_product_from_coroutine;
        }


    private:
        std::coroutine_handle<promise_type> m_handle;
    };
}


// ***************** //
// *** Awaitable *** //
// ***************** //
namespace alg
{
    template<typename T, typename U>
    class awaitable_pc
    {
    public:
        awaitable_pc() = default;

        bool await_ready() const noexcept 
        { 
            return false; 
        }


        // ****************************************************************** //
        // *** Transfer of coroutine_handle (from coroutine to awaitable) *** //
        // ****************************************************************** //
        bool await_suspend(std::coroutine_handle<typename generator_pc<T,U>::promise_type> handle) 
        {
            m_handle = handle;
            return true; 
        }


        // ********************************************************* // 
        // *** Transfer of product (from awaitable to coroutine) *** //
        // ********************************************************* // 
        const T& await_resume() const noexcept
        { 
            return m_handle.promise().m_product_from_caller;
        }

    
    public:
        // ********************************************* //
        // *** Custom functions for coroutine caller *** //
        // ********************************************* //
        // No need at the moment


    private:
        std::coroutine_handle<typename generator_pc<T,U>::promise_type> m_handle;
    }; 
}
