#include<iostream>
#include<cassert>
#include<cstdint>
#include<literal.h>
#include<utility.h>



void test_literal()
{
    auto x0 = 123_u8;
    static_assert(std::is_same_v<decltype(x0), std::uint8_t>, "literal operator failed for u8");

    auto x1 = 123_u16;
    static_assert(std::is_same_v<decltype(x1), std::uint16_t>, "literal operator failed for u16");

    auto x2 = 123_u32;
    static_assert(std::is_same_v<decltype(x2), std::uint32_t>, "literal operator failed for u32");

    auto x3 = 123_u64;
    static_assert(std::is_same_v<decltype(x3), std::uint64_t>, "literal operator failed for u64");

    auto x4 = 123_i8;
    static_assert(std::is_same_v<decltype(x4), std::int8_t>, "literal operator failed for i8");

    auto x5 = 123_i16;
    static_assert(std::is_same_v<decltype(x5), std::int16_t>, "literal operator failed for i16");

    auto x6 = 123_i32;
    static_assert(std::is_same_v<decltype(x6), std::int32_t>, "literal operator failed for i32");

    auto x7 = 123_i64;
    static_assert(std::is_same_v<decltype(x7), std::int64_t>, "literal operator failed for i64");

    print_summary("literal operator", "succeeded in compile time");
}
