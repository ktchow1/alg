#pragma once
#include<iostream>
#include<array>
#include<atomic>
#include<thread>
#include<optional>


namespace alg
{
    template<typename T, std::uint32_t N = 1024,
             typename CONSTRAINT = std::enable_if_t< std::is_move_constructible<T>::value && (N&(N-1))==0 >> 

    class lockfree_queue final
    {
    public:
        using value_type = T;
        using this_type = lockfree_queue<T,N,CONSTRAINT>;


    public:
        struct cell
        {
            cell() noexcept : value{}, state{false} 
            {
            }

            ~cell() = default;
            cell(const cell&) = delete;
            cell(cell&&) = delete;
            cell& operator=(const cell&) = delete;
            cell& operator=(cell&&) = delete;
            
            alignas(64) T value;
            std::atomic<std::uint32_t> state;
        };


    public:
        lockfree_queue() noexcept : m_array{}, m_next_write(0), m_next_read(0)
        {
            for(std::uint32_t n=0; n!=N; ++n)
            {
                m_array[n].state.store(n); 
                // empty  cell when m_array[n%N].state == n
                // filled cell when m_array[n%N].state == n+1
            }
        }

        ~lockfree_queue() = default;
        lockfree_queue(const this_type&) = delete;
        lockfree_queue(this_type&&) = delete;
        this_type& operator=(const this_type&) = delete;
        this_type& operator=(this_type&&) = delete;


    public:
        // ********************** //
        // *** Implementation *** //
        // ********************** //
        template<typename... ARGS> 
        bool emplace(ARGS&&... args) noexcept;
        std::optional<T> pop() noexcept;

        // ************************************** //
        // *** Interface compatiable to boost *** //
        // ************************************** //
        bool push(const T& item) noexcept; 
        bool pop(T& item) noexcept;


    public:
        std::uint32_t peek_size() const noexcept; // Just an estimation 
        template<template<typename> typename C> void deep_copy(C<T>&output) const noexcept;
        

    private:
        static constexpr std::uint32_t size = N;
        static constexpr std::uint32_t mask = N-1;
        std::array<cell, size> m_array;
        alignas(64) std::atomic<std::uint32_t> m_next_write;
        alignas(64) std::atomic<std::uint32_t> m_next_read;
        alignas(64) std::uint32_t m_padding;
        // padding is added to avoid other threads sharing same cacheline as last member
    };


    template<typename T>
    using lockfree_queue_long = lockfree_queue<T, 1024>;

    template<typename T>
    using lockfree_queue_short = lockfree_queue<T, 4>;
    
    template<typename Q> 
    constexpr bool is_lockfree = false;

    template<typename T, std::uint32_t N> 
    constexpr bool is_lockfree<lockfree_queue<T,N>> = true;
}


// ********************** //
// *** Implementation *** //
// ********************** //
namespace alg
{
    template<typename T, std::uint32_t N, typename CONSTRAINT> 
    template<typename... ARGS> 
    bool lockfree_queue<T, N, CONSTRAINT>::emplace(ARGS&&... args) noexcept 
    {
        while(true)
        {
            auto n = m_next_write.load(std::memory_order_acquire);
            auto m = n & mask;
            auto s = m_array[m].state.load(std::memory_order_acquire);

            if (s < n) 
            {
                return false; // overproduction
            }
            else if (s == n)
            {
                if (m_next_write.compare_exchange_strong(n,n+1,std::memory_order_relaxed))
                {
                    new (&(m_array[m].value)) T{std::forward<ARGS>(args)...}; 
                    m_array[m].state.store(n+1, std::memory_order_release);
                    return true;
                }
                else
                {
                    continue; // retry on losing PP race (winner completed CAS)
                }
            }
            else 
            {
                continue; // retry on losing PP race (winner completed CAS and state)
            }
        }
    }

    template<typename T, std::uint32_t N, typename CONSTRAINT> 
    std::optional<T> lockfree_queue<T, N, CONSTRAINT>::pop() noexcept
    {
        while(true)
        {
            auto n  = m_next_read.load(std::memory_order_acquire);
            auto n1 = n+1;
            auto m  = n & mask;
            auto s  = m_array[m].state.load(std::memory_order_acquire);

            if (s < n1) 
            {
                return std::nullopt; // overconsumption
            }
            else if (s == n1)
            {
                if (m_next_read.compare_exchange_strong(n,n1,std::memory_order_relaxed))
                {
                    std::optional<T> x = std::make_optional(std::move_if_noexcept(m_array[m].value)); 
                    m_array[m].state.store(n+N, std::memory_order_release);
                    return x; 
                }
                else
                {
                    continue; // retry on losing CC race (winner completed CAS)
                }
            }
            else 
            {
                continue; // retry on losing CC race (winner completed CAS and state)
            }
        }
    }    
}


// ************************************** //
// *** Interface compatiable to boost *** //
// ************************************** //
namespace alg
{
    template<typename T, std::uint32_t N, typename CONSTRAINT> 
    bool lockfree_queue<T, N, CONSTRAINT>::push(const T& item) noexcept
    {
        return emplace(item);
    }

    template<typename T, std::uint32_t N, typename CONSTRAINT>
    bool lockfree_queue<T, N, CONSTRAINT>::pop(T& item) noexcept
    {
        auto result = pop();
        if (result)
        {
            item = std::move(*result); 
            return true;
        }
        else return false;
    }

    template<typename T, std::uint32_t N, typename CONSTRAINT>
    std::uint32_t lockfree_queue<T, N, CONSTRAINT>::peek_size() const noexcept
    {
        return m_next_write.load(std::memory_order_seq_cst) - m_next_read.load(std::memory_order_seq_cst);
    }

    template<typename T, std::uint32_t N, typename CONSTRAINT> 
    template<template<typename> typename C> 
    void lockfree_queue<T, N, CONSTRAINT>::deep_copy(C<T>&output) const noexcept 
    {
        output.clear();

        std::uint32_t nw = m_next_write.load();
        std::uint32_t nr = m_next_read.load();
        for(std::uint32_t n=nr; n!=nw; ++n)
        {
            auto m = n & mask;
            output.push_back(m_array[m].value);
        }
    }
}
