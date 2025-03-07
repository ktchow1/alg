#include<iostream>
#include<cstdint>
#include<cassert>
#include<thread>
#include<vector>
#include<exception>
#include<utility.h>


// *************************************************************************** //
// [Interview question]
// Why move semantics function are usually declared "noexcept", but ...
// why copy semantics function are usually NOT declared so ?
//
// Answer : 
// 1. Move semantics is transfer ownership of resources without deep copying 
//    or allocating, it is low cost & low risk, hence usually "noexcept".
//
// 2. In absence of "noexcept" in std::vector<T>::push_back(T&&)
//    it will fallback to copy in std::vector<T>::push_back(const T&)
//    which increases unexpected time and risk without acknowledging caller
// *************************************************************************** //


namespace test
{
    std::vector<std::uint32_t> invoke_seq;

    struct ex_A
    {
        ex_A() { invoke_seq.push_back(11); }
       ~ex_A() { invoke_seq.push_back(21); }
    };

    struct ex_B
    {
        ex_B() { invoke_seq.push_back(12); }
       ~ex_B() { invoke_seq.push_back(22); }
    };

    struct ex_C
    {
        ex_C() { invoke_seq.push_back(13); throw std::runtime_error{"throw from ex_c"}; }
       ~ex_C() { invoke_seq.push_back(23); }
    };

    struct ex_base
    {
        ex_base() 
        { 
            invoke_seq.push_back(10); 
        }

       ~ex_base() 
        { 
            invoke_seq.push_back(20); 
        }
    };

    struct ex_derive0 : public ex_base
    {
        ex_derive0() : m_a{}, m_b{} { invoke_seq.push_back(14); throw std::runtime_error{"throw from ex_derive0"};  }
       ~ex_derive0()                { invoke_seq.push_back(24); }

        ex_A m_a;
        ex_B m_b;
    };

    struct ex_derive1 : public ex_base
    {
        ex_derive1() : m_a{}, m_b{} { invoke_seq.push_back(14); }
       ~ex_derive1()                { invoke_seq.push_back(24); }

        ex_A m_a;
        ex_B m_b;
        ex_C m_c;
    };

    void rethrow_exception0()
    {
        try 
        {
            ex_derive1 derive;
        }
        catch(...)
        {
            invoke_seq.push_back(30);            
            throw;
        } 
    }

    void rethrow_exception1()
    {
        try 
        {
            ex_derive1 derive;
        }
        catch(...) 
        {
            invoke_seq.push_back(40);            
            std::rethrow_exception(std::current_exception());
        }
    }

    // *************************************************************************** //
    // Each thread has its own execution context, a thread cannot catch exception 
    // thrown from another thread, unless we do exception propagation using :
    //
    // - std::exception_ptr 
    // - std::current_exception
    //
    // There is no direct conversion between std::exception and std::exception_ptr
    // we need to throw one of them, catch it as another type.
    // *************************************************************************** //
    void propagate_exception(std::exception_ptr& ex_ptr)
    {
        try 
        {
            ex_derive1 derive;
        }
        catch(...)
        {
            invoke_seq.push_back(50);
            ex_ptr = std::current_exception();
        }
    }
}


void test_exception_in_constructor()
{
    try
    {
        test::ex_derive0 derive;
    }
    catch(std::exception& ex)
    {
        assert(test::invoke_seq.size() == 7);
        assert(test::invoke_seq[0] == 10);
        assert(test::invoke_seq[1] == 11);
        assert(test::invoke_seq[2] == 12);
        assert(test::invoke_seq[3] == 14); // ~ex_derive0() is NOT called
        assert(test::invoke_seq[4] == 22);
        assert(test::invoke_seq[5] == 21);
        assert(test::invoke_seq[6] == 20);

        // If exception is thrown from ex_derive0 body, 
        // resources allocated inside ex_derive0 body 
        // have to be released explicitly before throwing.
    }
    test::invoke_seq.clear();

    try
    {
        test::ex_derive1 derive;
    }
    catch(std::exception& ex)
    {
        assert(test::invoke_seq.size() == 7);
        assert(test::invoke_seq[0] == 10);
        assert(test::invoke_seq[1] == 11);
        assert(test::invoke_seq[2] == 12);
        assert(test::invoke_seq[3] == 13); // ~ex_C() is NOT called
        assert(test::invoke_seq[4] == 22);
        assert(test::invoke_seq[5] == 21);
        assert(test::invoke_seq[6] == 20); 

        // If exception is thrown from member body, 
        // resources allocated inside member body 
        // have to be released explicitly before throwing.
    }
    test::invoke_seq.clear();

    print_summary("exception in constructor", "succeeded");
}


void test_exception_rethrow()
{
    // **************** //
    // *** Method 0 *** //
    // **************** //
    try
    {
        test::rethrow_exception0();
    }
    catch(std::exception& ex)
    {
        assert(test::invoke_seq.size() == 8);
        assert(test::invoke_seq[0] == 10);
        assert(test::invoke_seq[1] == 11);
        assert(test::invoke_seq[2] == 12);
        assert(test::invoke_seq[3] == 13);
        assert(test::invoke_seq[4] == 22);
        assert(test::invoke_seq[5] == 21);
        assert(test::invoke_seq[6] == 20); 
        assert(test::invoke_seq[7] == 30); 
    }
    test::invoke_seq.clear();

    // **************** //
    // *** Method 1 *** //
    // **************** //
    try
    {
        test::rethrow_exception1();
    }
    catch(std::exception& ex)
    {
        assert(test::invoke_seq.size() == 8);
        assert(test::invoke_seq[0] == 10);
        assert(test::invoke_seq[1] == 11);
        assert(test::invoke_seq[2] == 12);
        assert(test::invoke_seq[3] == 13);
        assert(test::invoke_seq[4] == 22);
        assert(test::invoke_seq[5] == 21);
        assert(test::invoke_seq[6] == 20); 
        assert(test::invoke_seq[7] == 40); 
    }
    test::invoke_seq.clear();

    print_summary("exception rethrow", "succeeded");
}


void test_exception_propagation()
{
    std::exception_ptr ex_ptr;
    std::thread t(test::propagate_exception, std::ref(ex_ptr));
    // BUG : do NOT miss std::ref, otherwise compile ERROR "thread arguments must be invocable after conversion to rvalue"

    t.join();
    if (ex_ptr)
    {
        test::invoke_seq.push_back(51);
    }

    assert(test::invoke_seq.size() == 9);
    assert(test::invoke_seq[0] == 10);
    assert(test::invoke_seq[1] == 11);
    assert(test::invoke_seq[2] == 12);
    assert(test::invoke_seq[3] == 13);
    assert(test::invoke_seq[4] == 22);
    assert(test::invoke_seq[5] == 21);
    assert(test::invoke_seq[6] == 20); 
    assert(test::invoke_seq[7] == 50); 
    assert(test::invoke_seq[8] == 51); 

    print_summary("exception propagation", "succeeded");
}


void test_exception()
{
    test_exception_in_constructor();
    test_exception_rethrow();
    test_exception_propagation();
}


