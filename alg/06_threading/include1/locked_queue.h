#pragma once
#include<optional>
#include<queue>
#include<mutex>
#include<spinlock.h>


namespace alg
{
    template<typename T, typename LOCK> 
    class locked_queue final
    {
    public:
        using value_type = T;
        using this_type = locked_queue<T, LOCK>;


    public:
        locked_queue() = default;
       ~locked_queue() = default;
        locked_queue(const this_type&) = delete;
        locked_queue(this_type&&) = default;
        this_type& operator=(const this_type&) = delete;
        this_type& operator=(this_type&&) = default;


    public:
        // ********************** //
        // *** Implementation *** //
        // ********************** //
        template<typename... ARGS> 
        bool emplace(ARGS&&... args) noexcept
        {
            std::lock_guard<LOCK> lock(m_impl);  
            m_queue.emplace(std::forward<ARGS>(args)...);
            return true;
        }

        std::optional<T> pop() noexcept
        {
            std::lock_guard<LOCK> lock(m_impl); 
            if (m_queue.size() == 0) return std::nullopt;
            
            T output = std::move(m_queue.front());
            m_queue.pop();
            return std::make_optional(output);
        }

        // ************************************** //
        // *** Interface compatiable to boost *** //
        // ************************************** //
        bool push(const T& item) noexcept
        {
            return emplace(item);
        }

        bool pop(T& item) noexcept
        {
            auto result = pop();
            if (result)
            {
                item = std::move(*result); 
                return true;
            }
            else return false;
        }


    public:
        std::uint32_t peek_size() const noexcept
        {
            std::lock_guard<LOCK> lock(m_impl);
            return m_queue.size();
        }
        

    private:
        std::queue<T> m_queue;
        mutable LOCK m_impl; // mutex or spinlock
    };

    template<typename T>
    using mutex_locked_queue = locked_queue<T, std::mutex>;

    template<typename T>
    using spin_locked_queue = locked_queue<T, spinlock>;
}
