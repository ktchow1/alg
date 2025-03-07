#include<iostream>
#include<cassert>
#include<vector>
#include<thread>
#include<mutex>
#include<atomic>
#include<singleton.h>
#include<utility.h>


namespace test
{
    struct A
    {
        std::uint32_t m_x;
        std::uint32_t m_y;
        std::uint32_t m_z;
    };
}

template<template<typename> typename SINGLETON>
void test_singleton_method(const std::string& test_name)
{    
    SINGLETON<test::A>::get_instance().m_x = 111;
    SINGLETON<test::A>::get_instance().m_y = 222;
    SINGLETON<test::A>::get_instance().m_z = 333;
    const std::uint32_t* px = &SINGLETON<test::A>::get_instance().m_x;
    const std::uint32_t* py = &SINGLETON<test::A>::get_instance().m_y;
    const std::uint32_t* pz = &SINGLETON<test::A>::get_instance().m_z;
    std::vector<std::thread> threads;
  

    // *********************** //
    // *** spawned threads *** //
    // *********************** //
    for(std::uint32_t n=0; n!=4; ++n)
    {
        threads.push_back(std::thread
        {
            [&]()
            {
                const std::uint32_t* px0 = &SINGLETON<test::A>::get_instance().m_x;
                const std::uint32_t* py0 = &SINGLETON<test::A>::get_instance().m_y;
                const std::uint32_t* pz0 = &SINGLETON<test::A>::get_instance().m_z;
                assert( px0 == px);
                assert( py0 == py);
                assert( pz0 == pz); 
                assert(*px0 == 111);
                assert(*py0 == 222);
                assert(*pz0 == 333); 
            }
        }); 
    }
  
    for(auto& x:threads)
    {
        x.join();
    } 
    print_summary(test_name, "succeeded");
}

void test_singleton()
{
    test_singleton_method<alg::singleton0>("test alg::singleton0");
    test_singleton_method<alg::singleton1>("test alg::singleton1");
    test_singleton_method<alg::singleton2>("test alg::singleton2");
    test_singleton_method<alg::singleton3>("test alg::singleton3");
}
