#pragma once
#include<cstdint>


namespace alg_literals
{
    inline constexpr std::uint8_t operator"" _u8(unsigned long long value)
    {
        return static_cast<std::uint8_t>(value);
    }

    inline constexpr std::uint16_t operator"" _u16(unsigned long long value)
    {
        return static_cast<std::uint16_t>(value);
    }

    inline constexpr std::uint32_t operator"" _u32(unsigned long long value)
    {
        return static_cast<std::uint32_t>(value);
    }

    inline constexpr std::uint64_t operator"" _u64(unsigned long long value)
    {
        return static_cast<std::uint64_t>(value);
    }

    inline constexpr std::int8_t operator"" _i8(unsigned long long value)
    {
        return static_cast<std::int8_t>(value);
    }

    inline constexpr std::int16_t operator"" _i16(unsigned long long value)
    {
        return static_cast<std::int16_t>(value);
    }

    inline constexpr std::int32_t operator"" _i32(unsigned long long value)
    {
        return static_cast<std::int32_t>(value);
    }

    inline constexpr std::int64_t operator"" _i64(unsigned long long value)
    {
        return static_cast<std::int64_t>(value);
    }
}

using namespace alg_literals; // <--- user can use the literal operator without namespace



// Literal operators for numbers must take either these forms 
//
// constexpr T operator"" _suffix(unsigned long long); 
// constexpr T operator"" _suffix(long double);         
// 
// These are not allowed : 
//
// constexpr T operator"" _suffix(unsigned long); 
// constexpr T operator"" _suffix(  signed long long); 
// constexpr T operator"" _suffix(int); 
// constexpr T operator"" _suffix(auto); 
