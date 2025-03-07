#include <iostream>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <exception>

// ****************** //
// *** Question 3 *** //
// ****************** //
namespace interview {
struct A 
{
    int x{0};
    int get()       { std::cout << "\nA"; return x++; }
    int get() const { std::cout << "\nB"; return x;   }
};

void test_apt3() 
{
    A a;
    auto x = a.get();
    auto y = a.get();
    std::cout << "\n" << x << "," << y;
}

// ****************** //
// *** Question 4 *** //
// ****************** //
void f1() 
{
    double x = 1.2 / 8.0;
    double y = 1.2 * (32.0 / 256.0);
    if (x == y) std::cout << "\nf1: Yes they are equal";
    else        std::cout << "\nf1: No they are not";
}

void f2() 
{
    double x = 1.2 / 14.0;
    double y = 1.2 * (32.0 / 448.0); // error happens when (32.0/448.0) cannot exactly represent 1/14 in double precision
    if (x == y) std::cout << "\nf2: Yes they are equal";
    else        std::cout << "\nf2: No they are not";
}

void f3() 
{
    double x = 1.2 / 15.0;
    double y = (32.0 / 480.0) * 1.2;
    if (x == y) std::cout << "\nf3: Yes they are equal";
    else        std::cout << "\nf3: No they are not";
}

void test_apt4() 
{
    f1();
    f2();
    f3();
}

// ****************** //
// *** Question 5 *** //
// ****************** //
std::mutex mutexA;
std::mutex mutexB;

void threadA()
{
    std::cout << "\nthreadA : " << std::this_thread::get_id() << std::flush;
    mutexA.lock();
//  throw std::runtime_error("Exception in ThreadA");
//  mutexB.lock();
    mutexA.unlock();
//  mutexB.unlock(); 
}

void threadB()
{
    std::cout << "\nthreadB : " << std::this_thread::get_id() << std::flush;
    mutexB.lock();
//  throw std::runtime_error("Exception in ThreadB");
//  mutexA.lock();
    mutexB.unlock();
//  mutexA.unlock(); 
}

void test_apt5()
{
    // Can a thread catch the exception thrown by another thread?
    // Answer : No, since exception trigger stack unwind, 
    //          and each thread has its own stack
    //          hence the above is considered as exception-not-caught, terminated ...
    try
    {
        std::thread tA(threadA);
        std::thread tB(threadB);
        std::cout << "\nmain thread : " << std::this_thread::get_id() << " running";
        tA.join();
        tB.join();
    } 
    catch (const std::exception& e) 
    {
        std::cout << "\nmain thread : " << std::this_thread::get_id() << " : " << e.what() << std::flush;
    }
}
}
