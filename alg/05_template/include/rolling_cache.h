#pragma once
#include <array>
#include <concepts>
#include <cstdint>
#include <utility>

namespace alg 
{
    template<typename T>
    concept equality_comparable = requires(const T& x, const T& y) 
    {
        { x == y } -> std::same_as<bool>;
    };

    template<std::uint32_t N>
    concept power_of_2 = (N != 0 && (N & (N - 1)) == 0);

    template<equality_comparable T, std::uint32_t N> requires power_of_2<N>
    class rolling_cache 
    {
    public:
        struct cache_entry 
        {
            alignas(64) T m_value;
            alignas(64) std::uint32_t m_count = 0;
        };

    public:
        rolling_cache() : m_head(0)
        {
        }

        void push(std::uint32_t count, const T& value)
        {
            if (count == 0) [[unlikely]] return;

            if (m_head > 0) 
            {
                // Use bitwise AND for fast modulus mapping to buffer size
                cache_entry& prevEntry = m_buffer[(m_head - 1) & m_mask];
                if (prevEntry.m_value == value) 
                {
                    prevEntry.m_count += count;
                    return;
                }
            }

            // Logic for inserting a new cache_entry if no match is found
            cache_entry& newEntry = m_buffer[m_head & m_mask];
            newEntry.m_value = value;
            newEntry.m_count = count;
            m_head++;
        }

        template<typename... Args>
        void emplace(std::uint32_t count, Args&&... args)
        {
            if (count == 0) [[unlikely]] return;

            cache_entry& thisEntry = m_buffer[m_head & m_mask];
            thisEntry.m_value.~T(); // Explicitly destroy previous object in that slot
            new (&(thisEntry.m_value)) T{ std::forward<Args>(args)... };

            if (m_head > 0) 
            {
                cache_entry& prevEntry = m_buffer[(m_head - 1) & m_mask];
                if (prevEntry.m_value == thisEntry.m_value) 
                {
                    prevEntry.m_count += count;
                    return;
                }
            }
            thisEntry.m_count = count;
            ++m_head;
        }

        void decrement(std::uint32_t count, const T& target)
        {
            iterate_from_head([&](cache_entry& entry) 
            {
                if (entry.m_value == target && entry.m_count > 0) 
                {
                    entry.m_count = (entry.m_count > count) ? entry.m_count - count : 0;
                    return true; // stop iterating
                }
                return false; // continue iterating
            });
        }

        const cache_entry* latest()
        {
            const cache_entry* result = nullptr;
            iterate_from_head([&](const cache_entry& entry) 
            {
                if (entry.m_count > 0) 
                {
                    result = &entry;
                    return true; // stop iterating
                }
                return false; // continue iterating
            });
            return result;
        }

    private:
        template<typename Func>
        void iterate_from_head(Func&& fct) // 2 slice implementation rather than single for loop, to avoid repeated modulus
        {
            // Normally, decrement() and latest() wont be called before emplace().
            if (m_head == 0) [[unlikely]] return;

            auto nStart = static_cast<std::uint32_t>((m_head - 1) & m_mask);
            if (nStart != N - 1) 
            {
                // Slice 1: head-1 down to 0
                for (std::int32_t n = nStart; n >= 0; --n) 
                {
                    if (fct(m_buffer[n])) return;
                }

                // Just 1 slice
                if (m_head <= N) return;

                // Slice 2: N-1 down to head+1
                auto nEnd = static_cast<std::uint32_t>(m_head & m_mask);
                for (std::int32_t n = N - 1; n != nEnd; --n) 
                {
                    if (fct(m_buffer[n])) return;
                }
            } 
            else 
            {
                // Slice 1: head-1 down to 1
                for (std::uint32_t n = nStart; n >= 1; --n) 
                {
                    if (fct(m_buffer[n])) return;
                }
            }
        }

    private:
        std::array<cache_entry, N> m_buffer;

        alignas(64) static constexpr std::uint32_t m_mask = N - 1;
        alignas(64) std::uint64_t m_head;
        alignas(64) std::uint64_t m_padding; // avoid another variable sharing same cache line as m_head
    };
} 