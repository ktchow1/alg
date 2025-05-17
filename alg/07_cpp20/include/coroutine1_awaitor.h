#pragma once
#include <iostream>
#include <exception>
#include <coroutine>


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


// ******************** // 
// *** Experiment 2 *** //
// ******************** // 
// T = data from caller to coroutine
// U = data from coroutine to caller
template<typename T, typename U>
struct future2 
{
    struct promise_type 
    {
        promise_type()                                {   std::cout << "\npromise::promise";   } 
        future2<T,U> get_return_object()              {   std::cout << "\npromise::ret_obj"; 
                                                          return future2<T,U>
                                                          { 
                                                              std::coroutine_handle<promise_type>::from_promise(*this) 
                                                          };
                                                      }   
        void unhandled_exception()                    {   }
        std::suspend_never initial_suspend()          {   std::cout << "\npromise::initial"; return {};   }
        std::suspend_never   final_suspend() noexcept {   std::cout << "\npromise::final";   return {};   }

        T data_T; // <--- new stuff (as compared to previous)
        U data_U; // <--- new stuff (as compared to previous)
    };

    explicit future2(std::coroutine_handle<promise_type> h_) : h(h_) 
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

template<typename T, typename U>
struct awaitable2
{
    awaitable2() : data_T_ptr(nullptr), data_U_ptr(nullptr)
    {
        std::cout << "\nawait::await"; 
    }

    bool await_ready() const noexcept { return false; }
    bool await_suspend(std::coroutine_handle<typename future2<T,U>::promise_type> h) // <--- need to add promise_type inside handle
    {
        std::cout << "\nawait::suspend(handle)";
        data_T_ptr = &(h.promise().data_T);   
        data_U_ptr = &(h.promise().data_U);   
        return true; 
    }
    std::pair<T*,U*> await_resume() const noexcept
    { 
        return std::make_pair(data_T_ptr, data_U_ptr);
    }

    // Keep data-pointer instead of handle-pointer
    T* data_T_ptr;
    U* data_U_ptr;
}; 

template<typename T, typename U>
[[nodiscard]] future2<T,U> coroutine2()
{

    for(std::uint32_t n=0;; ++n) 
    {
        // Read t from caller to coroutine
        // Send u from coroutine to caller
        auto [T_ptr,U_ptr] = co_await awaitable2<T,U>{}; // wait for caller until t is ready
        U_ptr->a = T_ptr->a;
        U_ptr->b = T_ptr->a + T_ptr->n;
        U_ptr->c = T_ptr->a + T_ptr->n * 2;
        co_await std::suspend_always{}; // u is ready and wait for caller

        std::cout << "\ncoroutine::iteration " << n << ", " << *T_ptr << ", " << *U_ptr;
    }
}

// Sample POD
struct pod_T 
{
    char a;
    std::uint16_t n;
};

struct pod_U 
{
    char a;
    char b;
    char c;
};

inline std::ostream& operator<<(std::ostream& os, const pod_T& t)
{
    os << "T = " << t.a << "_" << t.n;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const pod_U& u)
{
    os << "U = " << u.a << u.b << u.c;
    return os;
}

    
