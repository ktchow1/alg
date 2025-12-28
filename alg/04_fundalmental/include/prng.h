#pragma once
#include <concepts>
#include <random>

namespace alg 
{
    class pseudo_rand_num_gen
    {
    public:
        explicit pseudo_rand_num_gen(std::uint64_t seed) : m_seed(seed)
        {
            m_engine.seed(m_seed);
        }

        inline auto next() 
        { 
            return m_engine(); 
        }

        std::size_t next_uniform(std::size_t size)
        {
            std::uniform_int_distribution<std::size_t> distribution(std::size_t{ 0 }, size - 1);
            return distribution(m_engine);
        }

        template<std::integral T>
        T next_uniform(T min, T max) // min max inclusive
        {
            if (min > max) [[unlikely]] { std::swap(min, max); }
            std::uniform_int_distribution<T> distribution(min, max);
            return distribution(m_engine);
        }

        template<std::floating_point T>
        T next_uniform(T min, T max)
        {
            if (min > max) [[unlikely]] { std::swap(min, max); }
            std::uniform_real_distribution<T> distribution(min, max);
            return distribution(m_engine);
        }

    private:
        std::uint64_t m_seed;
        std::mt19937_64 m_engine; // Standard 64-bit Mersenne Twister
    };
}