#pragma once
#include<shared_mutex>


namespace alg
{
    class shared_mutex
    {
    public:
        inline void lock()
        {
            m_global_mutex.lock();
        }

        inline void unlock()
        {
            m_global_mutex.unlock();
        }

        inline void lock_shared()
        {
            std::lock_guard<std::mutex> lock0(m_reader_mutex);
            ++m_reader;
            if (m_reader == 1) lock();
        }

        inline void unlock_shared()
        {
            std::lock_guard<std::mutex> lock0(m_reader_mutex);
            --m_reader;
            if (m_reader == 0) unlock();
        }

    private:
        std::uint32_t m_reader = 0;
        std::mutex    m_global_mutex;
        std::mutex    m_reader_mutex;
    };
}
