#pragma once
#include <iostream>
#include <exception>
#include <coroutine>
#include <utility.h>


// *********************************************************************************** //
// How to define coroutine as producer using co_yield?
//
// alg::generator<my_product> my_coroutine(...)
// {
//     my_product x = produce();
//     x.a = ...
//     x.b = ...
//     x.c = ...
//     co_yield x;  
// }
//
// Remark 1 : "co_yield" to yield newly created product to coroutiner caller
// Remark 2 : "return alg::generator<my_product>" is NOT needed, my_coroutine is async 
// *********************************************************************************** //
namespace alg 
{
    template<typename T, bool DEBUG>
    class generator 
    {
        // ********************************* //
        // *** STL coroutine requirement *** //
        // ********************************* //
    public:
        struct promise_type 
        {
            promise_type() : m_num_yields(0)
            { 
                debug<DEBUG>("promise_type::promise_type");
            } 

            // Factory of generator 
            generator<T,DEBUG> get_return_object() 
            { 
                debug<DEBUG>("promise_type::get_return_object");
                return generator<T,DEBUG>
                {
                    std::coroutine_handle<promise_type>::from_promise(*this) 
                };
            }   

            // Suspension policy of coroutine caller governed by return type
            std::suspend_always initial_suspend()          { debug<DEBUG>("promise_type::initial_suspend"); return {}; }
            std::suspend_always   final_suspend() noexcept { debug<DEBUG>("promise_type::final_suspend");   return {}; }
            void unhandled_exception()                     { }


            // ********************************************************* //
            // *** Transfer of product (from coroutine to generator) *** //
            // ********************************************************* //
            // Suspension policy of coroutine governed by return type
            //
            std::suspend_always yield_value(const T& product) 
            {
                debug<DEBUG>("promise_type::yield_value");
                m_product = product; 
                ++m_num_yields;
                return {};
            }


            // ************************** //
            // *** Product and states *** //
            // ************************** //
            T             m_product; 
            std::uint32_t m_num_yields; 
        };


    public:
        explicit generator(std::coroutine_handle<promise_type> handle) : m_handle(handle) 
        {
            debug<DEBUG>("generator::generator");
        } 

        // RAII of coroutine frame
       ~generator() 
        {
            debug<DEBUG>("generator::~generator");
            if (m_handle) m_handle.destroy(); 
        }


        // ********************************************* //
        // *** Custom functions for coroutine caller *** //
        // ********************************************* //
        operator bool() const 
        {
            debug<DEBUG>("generator::bool");
            return !m_handle.done();
        } 

        [[nodiscard]] // caller must cache, or T will be stale
        const T& get_product() const  
        {
            debug<DEBUG>("generator::get_product");
            m_handle.resume(); // <--- yield to coroutine, ask coroutine to produce
            return m_handle.promise().m_product;
        }

        std::uint32_t get_num_yields() const  
        {
            return m_handle.promise().m_num_yields;
        }


    private:
        std::coroutine_handle<promise_type> m_handle; 
    };
}

