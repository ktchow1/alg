#include<atomic>


// ************************* //
// Promise and future is :
// 1. publication pattern 
// 2. single cell spsc-queue
// ************************* //
namespace alg
{
    template<typename T>
    struct future
    {
        future(const T& x, const std::atomic<bool>& flag) : m_publication(x), m_flag(flag) 
        {
        }

        const T& get() const
        {
            while(!m_flag.load(std::memory_order_acquire));
            return m_publication;
        }

        const T& m_publication;
        const std::atomic<bool>& m_flag;
    };


    template<typename T>
    struct promise
    {
        promise() : m_flag(false)
        {
        }

        future<T> get_future() const
        {
            return { m_publication, m_flag };
        }

        void set_value(const T& x)
        {
            m_publication = x;
            m_flag.store(true, std::memory_order_release);
        }

        T m_publication;
        std::atomic<bool> m_flag;
    };
}
