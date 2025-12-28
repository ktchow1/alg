#pragma once
#include <atomic>
#include <chrono>
#include <cstdint>

#if defined(_MSC_VER)
    #include <intrin.h>
#endif

namespace alg
{

// ------------------------------------------------------------
// rdtsc
// ------------------------------------------------------------
inline std::uint64_t rdtsc() noexcept
{
#if defined(_MSC_VER)
    return __rdtsc();
#elif defined(__x86_64__) || defined(__i386__)
    std::uint32_t lo, hi;
    __asm__ volatile("rdtsc" : "=a"(lo), "=d"(hi));
    return (static_cast<std::uint64_t>(hi) << 32) | lo;
#else
    #error "RDTSC not supported"
#endif
}

// ------------------------------------------------------------
// Incrementally calibrated TSC timer
// ------------------------------------------------------------
class RdtscTimer
{
public:
    using clock = std::chrono::steady_clock;

    // Call once at startup (logger thread)
    static void init() noexcept
    {
        last_cycles_ = rdtsc();
        last_time_   = clock::now();
    }

    // Hot path
    static inline std::uint64_t now_cycles() noexcept
    {
        return rdtsc();
    }

    // Logger / background thread
    static inline std::chrono::nanoseconds
    to_nanoseconds(std::uint64_t cycles) noexcept
    {
        const double ns_per_cycle =
            ns_per_cycle_.load(std::memory_order_relaxed);

        return std::chrono::nanoseconds(
            static_cast<std::uint64_t>(cycles * ns_per_cycle)
        );
    }

    // Call periodically (logger thread)
    static void recalibrate() noexcept
    {
        const auto now_time   = clock::now();
        const auto now_cycles = rdtsc();

        const auto dt_ns =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                now_time - last_time_
            ).count();

        const std::uint64_t dt_cycles =
            now_cycles - last_cycles_;

        // Guard against very small intervals
        if (dt_cycles > 0 && dt_ns > 0)
        {
            const double new_ns_per_cycle =
                static_cast<double>(dt_ns) /
                static_cast<double>(dt_cycles);

            ns_per_cycle_.store(
                new_ns_per_cycle,
                std::memory_order_relaxed
            );
        }

        last_time_   = now_time;
        last_cycles_ = now_cycles;
    }

private:
    static inline std::atomic<double> ns_per_cycle_ { 0.0 };

    static inline std::uint64_t last_cycles_;
    static inline clock::time_point last_time_;
};
}



