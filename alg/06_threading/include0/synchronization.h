#pragma once
#include<mutex>
#include<condition_variable>
#include<future>
#include<array>

// For posix-thread lib
#include<stdint.h>    
#include<unistd.h>    
#include<semaphore.h> 
#include<linux/futex.h> 
#include<sys/syscall.h> // for SYS_futex
#include<sys/time.h> 


namespace alg
{
    template<typename T> 
    concept sync_primitive = requires(T x)
    {
        x.wait();
        x.notify();
    };
}


// ********************************* //
// *** Implementation with futex *** //
// ********************************* //
namespace alg
{
    class sync_futex
    {
    public:
        // This thread is blocked when m_futex == m_blocking_value.
        sync_futex() : m_blocking_value(0)
        {
        }

       ~sync_futex() = default;
        sync_futex(const sync_futex&) = delete;
        sync_futex(sync_futex&&) = default;
        sync_futex& operator=(const sync_futex&) = delete;
        sync_futex& operator=(sync_futex&&) = default;

    public: 
        void wait()
        {
            syscall(SYS_futex, &m_futex, FUTEX_WAIT, m_blocking_value, NULL, NULL, 0); 
            ++m_blocking_value;
        }

        void notify()
        {
            m_futex.fetch_add(1);
            syscall(SYS_futex, &m_futex, FUTEX_WAKE, 1, NULL, NULL, 0); 
        }

    private:
        std::atomic<std::int32_t> m_futex;
        std::int32_t m_blocking_value;
    };
}


// ******************************************************** //
// *** Implementation with std::mutex (single consumer) *** //
// ******************************************************** //
namespace alg
{
    class sync_mutex
    {
    public:
        sync_mutex() : m_mutex()
        {
            m_mutex.lock();
        }

       ~sync_mutex() 
        {
            m_mutex.unlock();
        }

        sync_mutex(const sync_mutex&) = delete;
        sync_mutex(sync_mutex&&) = default;
        sync_mutex& operator=(const sync_mutex&) = delete;
        sync_mutex& operator=(sync_mutex&&) = default;

    public:
        inline void wait()
        {
            m_mutex.lock(); 
        }

        inline void notify()
        {
            m_mutex.unlock(); 
        }

    private:
        std::mutex m_mutex;
    };
}


// *********************************************************** //
// *** Implementation with pthread mutex (single consumer) *** //
// *********************************************************** //
namespace alg
{
    class sync_pmutex 
    {
    public:
        sync_pmutex()
        {
            pthread_mutex_init(&m_mutex, NULL);
            pthread_mutex_lock(&m_mutex);
        }

       ~sync_pmutex() 
        {
            pthread_mutex_unlock(&m_mutex);
            pthread_mutex_destroy(&m_mutex);
        }

        sync_pmutex(const sync_pmutex&) = delete;
        sync_pmutex(sync_pmutex&&) = default;
        sync_pmutex& operator=(const sync_pmutex&) = delete;
        sync_pmutex& operator=(sync_pmutex&&) = default;

    public:
        inline void wait()
        {
            pthread_mutex_lock(&m_mutex);
        }

        inline void notify()
        {
            pthread_mutex_unlock(&m_mutex);
        }

    private:
        pthread_mutex_t m_mutex;
    };
}


// ******************************************************** //
// *** Implementation with pthread mutex (Hans W. Barz) *** //
// ******************************************************* //
// 1. count is protected by cs_mutex, hence inc/dec are atomic
// 2. waiting threads on P are waked either by :
// -- other threads completing P and count N->N-1, where N>1
// -- other threads completing V and count 0->1
// ******************************************************* //
namespace alg
{
    class sync_HansBarz  
    {
    public:
        sync_HansBarz() : m_count(0)
        {
            pthread_mutex_init(&m_cs_mutex, NULL);
            pthread_mutex_init(&m_pv_mutex, NULL);
            pthread_mutex_lock(&m_pv_mutex);
        }

       ~sync_HansBarz() 
        {
            pthread_mutex_unlock (&m_pv_mutex);
            pthread_mutex_destroy(&m_pv_mutex);
            pthread_mutex_destroy(&m_cs_mutex);
        }

        sync_HansBarz(const sync_HansBarz&) = delete;
        sync_HansBarz(sync_HansBarz&&) = default;
        sync_HansBarz& operator=(const sync_HansBarz&) = delete;
        sync_HansBarz& operator=(sync_HansBarz&&) = default;

    public:
        inline void wait()
        {
            pthread_mutex_lock(&m_pv_mutex); // wait ... 
            pthread_mutex_lock(&m_cs_mutex);
            --m_count;
            if (m_count > 0)
            {
                pthread_mutex_unlock(&m_pv_mutex);
            }
            pthread_mutex_unlock(&m_cs_mutex);
        }

        inline void notify()
        {
            pthread_mutex_lock(&m_cs_mutex);
            ++m_count;
            if (m_count == 1)
            {
                pthread_mutex_unlock(&m_pv_mutex); // notify ... 
            }
            pthread_mutex_unlock(&m_cs_mutex);
        }

    private:
        std::int32_t m_count;
        pthread_mutex_t m_cs_mutex; // for critical session protection
        pthread_mutex_t m_pv_mutex; // for signaling, i.e. p and v
    }; 
}
      

// ***************************************** //
// *** Implementation with std semaphore *** //
// ***************************************** //
namespace alg
{
    class sync_semaphore
    {
    public:
        sync_semaphore() : m_semaphore(0) // initial count is zero, i.e. consumer must wait 
        {
        }

       ~sync_semaphore() = default;
        sync_semaphore(const sync_semaphore&) = delete;
        sync_semaphore(sync_semaphore&&) = default;
        sync_semaphore& operator=(const sync_semaphore&) = delete;
        sync_semaphore& operator=(sync_semaphore&&) = default;

    public:
        inline void wait()
        {
            m_semaphore.acquire(); // if (count>0) --count; else blocked()
        }

        inline void notify()
        {
            m_semaphore.release(); // ++count
        }

    private:
        std::counting_semaphore<1> m_semaphore;
    };   
}


// ********************************************* //
// *** Implementation with pthread semaphore *** //
// ********************************************* //
namespace alg
{
    class sync_psemaphore
    {
    public:
        sync_psemaphore()
        {
            sem_init(&m_semaphore, 0, 0); 
            // arg[1] : 0 for multi-thread, 1 for multi-process
            // arg[2] : initial value
        }

       ~sync_psemaphore() 
        {
            sem_destroy(&m_semaphore);
        }

        sync_psemaphore(const sync_psemaphore&) = delete;
        sync_psemaphore(sync_psemaphore&&) = default;
        sync_psemaphore& operator=(const sync_psemaphore&) = delete;
        sync_psemaphore& operator=(sync_psemaphore&&) = default;

    public:
        inline void wait()
        {
            sem_wait(&m_semaphore);
        }

        inline void notify()
        {
           sem_post(&m_semaphore);
        }

        inline auto peek_value() 
        {
            std::int32_t x;
            sem_getvalue(&m_semaphore, &x);
            return x;
        }

    private:
        sem_t m_semaphore;
    };
}


// ********************************************** //
// *** Implementation with condition variable *** //
// ********************************************** //
namespace alg
{
    class sync_condvar
    {
    public:
        sync_condvar() : m_count(0)
        {
        }

       ~sync_condvar() = default;
        sync_condvar(const sync_condvar&) = delete;
        sync_condvar(sync_condvar&&) = default;
        sync_condvar& operator=(const sync_condvar&) = delete;
        sync_condvar& operator=(sync_condvar&&) = default;

    public: 
        void wait()
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            while(m_count == 0) m_cv.wait(lock); 
            --m_count;
        }

        void notify()
        {
            {
                std::lock_guard<std::mutex> lock(m_mutex);
                ++m_count;
            }
            m_cv.notify_one();
        }

    private:
        std::mutex m_mutex;
        std::condition_variable m_cv;
        std::uint32_t m_count; // emulate a semaphore
    };
}


// ********************************************** //
// *** Implementation with promise and future *** //
// ********************************************** //
namespace alg
{
    template<std::uint32_t N>
    class sync_promfut
    {
    public:
        sync_promfut() : m_pindex(0), m_findex(0), m_promises{}, m_futures{}
        {
            for(std::uint32_t n=0; n!=N ; ++n)
            {
                m_futures[n] = m_promises[n].get_future();
            }
        }

       ~sync_promfut() = default;
        sync_promfut(const sync_promfut&) = delete; 
        sync_promfut(sync_promfut&&) = default; 
        sync_promfut& operator=(const sync_promfut&) = delete; 
        sync_promfut& operator=(sync_promfut&&) = default; 

    public:
        void wait()
        {
            auto n = m_findex.fetch_add(1);
            if (n < N) 
            {
                m_futures[n].get();
            }
        }

        void notify()
        {
            auto n = m_pindex.fetch_add(1);
            if (n < N)
            {
                m_promises[n].set_value();
            }
        }

    private:
        std::atomic<std::uint32_t> m_pindex;
        std::atomic<std::uint32_t> m_findex;
        std::array<std::promise<void>, N> m_promises;
        std::array<std::future <void>, N> m_futures;
    };
}
    

// ********************************** //
// *** Implementation with atomic *** //
// ********************************** //
namespace alg
{
    class sync_atomic
    {
    public:
        sync_atomic() : m_flag(false)
        {
        }

       ~sync_atomic() = default;
        sync_atomic(const sync_atomic&) = delete;
        sync_atomic(sync_atomic&&) = default;
        sync_atomic& operator=(const sync_atomic&) = delete;
        sync_atomic& operator=(sync_atomic&&) = default;

    public: 
        void wait()
        {
            while(!m_flag.load(std::memory_order_acquire))
            {
                std::this_thread::yield();
            }
            m_flag.store(false, std::memory_order_release); // notify one thread only
        }

        void notify()
        {
            m_flag.store(true, std::memory_order_release);
        }

    private:
        std::atomic<bool> m_flag;
    };
}
