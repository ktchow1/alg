#pragma once
#include <iostream>
#include <exception>
#include <coroutine>


// *********************************************************************** //
// The following coroutine mechanics :
// * are exposed in c++ (implemented in this header file)
// * are hidden in python 
// thus python coroutine code looks simple, just like test_coroutine.cpp.
//
//
// Class and function with free-to-choose name :
// * generator
// * generator::bool()
// * generator::get_product()
//
//
// Class and function with standard name :
// * generator::promise_type 
// * generator::promise_type::get_return_object()
// * generator::promise_type::yield_value()
// * generator::promise_type::initial_suspend()
// * generator::promise_type::  final_suspend()
// * generator::promise_type::unhandled_exception()
// *********************************************************************** //
// How to use this generator in coroutine? Define like this :
//
// alg::generator<my_product> my_coroutine(...)
// {
//     my_product x = create_my_product();
//     
//     // some updates if you want
//     x.a = ...
//     x.b = ...
//     x.c = ...
//
//     co_yield x; 
// }
//
// Remark 1 : co_yield to yield the new created my_product
// Remark 2 :  no need to return alg::generator<my_product> explicitly
// *********************************************************************** //
                             
  
namespace alg 
{
    template<typename T>
    class generator 
    {
        // ********************************* //
        // *** STL coroutine requirement *** //
        // ********************************* //
    public:
        // Define the config of coroutine
        struct promise_type 
        {
            promise_type() : m_num_yields(0)
            { 
            } 

            // Factory of generator (construct generator from coroutine frame handle)
            generator<T> get_return_object() 
            { 
                return generator<T>
                {
                    std::coroutine_handle<promise_type>::from_promise(*this) 
                };
            }   

            // Return type governs suspension policy of coroutine (usually, the producer)
            std::suspend_always yield_value(const T& product)
            {
                m_product = product; 
                ++m_num_yields;
                return {};
            }

            // Return type governs suspension policy of coroutine caller (usually, the consumer)
            std::suspend_always initial_suspend()          { return {}; }
            std::suspend_always   final_suspend() noexcept { return {}; }
            void unhandled_exception()                     {            }

            // The product
            T m_product; 

            // Other states
            std::uint32_t m_num_yields; 
        };


    private:
        // Pointer to coroutine frame
        std::coroutine_handle<promise_type> m_handle; 


    public:
        explicit generator(std::coroutine_handle<promise_type> handle) : m_handle(handle) 
        {
        } 

        // RAII of coroutine frame
       ~generator() 
        {
            if (m_handle)
            {
                m_handle.destroy(); 
            }
        }


        // ********************************************* //
        // *** Custom functions for coroutine caller *** //
        // ********************************************* //
        operator bool() const 
        {
            return !m_handle.done();
        } 

        [[nodiscard]]   // <--- caller must cache new product reference everytime, otherwise it will be stale
        const T& get_product() const  
        {
            m_handle(); // <--- coroutine caller yield here, asking coroutine for next product
            return m_handle.promise().m_product;
        }

        std::uint32_t get_num_yields() const  
        {
            return m_handle.promise().m_num_yields;
        }
    };
}

