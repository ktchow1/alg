#pragma once
#include<atomic>
#include<thread>


namespace alg
{
    // ***************************** //
    // *** Using pthread library *** //
    // ***************************** //
    class spinlock_p final
    {
    public:
        spinlock_p()
        {
            pthread_spin_init(&m_impl, PTHREAD_PROCESS_PRIVATE); // for threads inside same process only
        }

        ~spinlock_p()
        {
            pthread_spin_destroy(&m_impl);
        }

        spinlock_p(const spinlock_p&) = delete;
        spinlock_p(spinlock_p&&) = delete;
        spinlock_p& operator=(const spinlock_p&) = delete;
        spinlock_p& operator=(spinlock_p&&) = delete;

    public:
        inline void lock() noexcept
        {
            pthread_spin_lock(&m_impl);
        }

        inline void unlock() noexcept
        {
            pthread_spin_unlock(&m_impl);
        }

    private:
        pthread_spinlock_t m_impl;
    };


    // ********************************* //
    // *** Using std::thread library *** //
    // ********************************* //
    class spinlock final
    {
    public:
        spinlock() = default;
        ~spinlock() = default;

        spinlock(const spinlock&) = delete;
        spinlock(spinlock&&) = delete;
        spinlock& operator=(const spinlock&) = delete;
        spinlock& operator=(spinlock&&) = delete;

    public:
        inline void lock() noexcept
        {
            while(m_flag.test_and_set(std::memory_order_acquire))
            {
                std::this_thread::yield();
            }
        }

        inline void unlock() noexcept
        {
            m_flag.clear(std::memory_order_release);
        }

    private:
        std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
    };
}
