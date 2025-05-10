#include<iostream>
#include<iomanip>
#include<cassert>
#include<cmath>
#include<bitset>
#include<arithmetic.h>
#include<utility.h>


void test_bignum()
{
    // ******************** //
    // *** General case *** //
    // ******************** //
    std::uint32_t trial = 10000;
    std::uint32_t error0 = 0;   
    std::uint32_t error1 = 0;  

    for(std::uint32_t t=0; t!=trial; ++t)
    {
        std::uint64_t a = rand();
        std::uint64_t b = rand();
        std::uint64_t c = a + b;
        auto x = std::to_string(a);
        auto y = std::to_string(b);
        auto z = alg::bignum_add(x,y); 
        if (std::to_string(c) != z) ++error0;
    }
  
    for(std::uint32_t t=0; t!=10000; ++t)
    {
        std::uint64_t a = rand();
        std::uint64_t b = rand();
        std::uint64_t c = a * b;
        auto x = std::to_string(a);
        auto y = std::to_string(b);
        auto z = alg::bignum_multiply(x,y); 
        if (std::to_string(c) != z) ++error1;
    }
    print_summary("bignum addition", error0, trial);
    print_summary("bignum multiply", error1, trial);


    // ***************** //
    // *** Edge case *** //
    // ***************** //
    assert(alg::bignum_add("1234",    "0") == "1234");
    assert(alg::bignum_add(   "0", "1234") == "1234");
    assert(alg::bignum_add(   "0",    "0") ==    "0");
      
    assert(alg::bignum_scale("1234", 3) == "1234000");
    assert(alg::bignum_scale("1234", 0) ==    "1234");
    assert(alg::bignum_scale(   "0", 3) ==       "0");
    assert(alg::bignum_scale(   "0", 0) ==       "0");

    assert(alg::bignum_multiply("1234",    "0") == "0");
    assert(alg::bignum_multiply(   "0", "1234") == "0");
    assert(alg::bignum_multiply(   "0",    "0") == "0");   
    print_summary("bignum edge case", "succeeded");
}


void test_reverse_digits()
{
    std::uint32_t trial = 10000;
    std::uint32_t error = 0;   

    for(std::uint32_t t=0; t!=trial; ++t)
    {
        std::uint64_t x = rand();
        std::string   s = std::to_string(x); std::reverse(s.begin(), s.end());
        std::uint64_t y = std::stol(s); 
        std::uint64_t z = alg::reverse_digits(x);
        if (y != z) ++error;
    }
    print_summary("reverse_digits", error, trial);
}


void test_count_num_of_one_bits()
{
    std::uint32_t trial = 10000;
    std::uint32_t error = 0;   

    for(std::uint32_t t=0; t!=trial; ++t)
    {
        std::uint64_t x = rand();
        std::uint64_t y = 0;

        std::stringstream ss;
        ss << std::bitset<64>{x};
        for(const auto& c:ss.str()) { if (c=='1') ++y; }

        std::uint64_t z = alg::count_num_of_one_bits(x);
        if (y != z) ++error;
    }
    print_summary("count_num_of_one_bits", error, trial);
}


void test_multiply()
{
    std::uint32_t trial = 10000;
    std::uint32_t error = 0;   

    for(std::uint32_t t=0; t!=trial; ++t)
    {
        std::uint64_t n = rand();
        std::uint64_t x = rand();
        std::uint64_t y = n * x; 
        std::uint64_t z = alg::multiply(n,x);
        if (y != z) ++error;
    }
    print_summary("multiply", error, trial);
}


void test_power()
{
    std::uint32_t trial = 500;
    std::uint32_t error = 0;   

    for(std::uint32_t t=0; t!=trial; ++t)
    {
        std::uint64_t n = rand() % 12;
        std::uint64_t x = rand() % 12;
        std::uint64_t y = pow(n,x); 
        std::uint64_t z = alg::power(n,x);
        if (y != z) ++error;
    }
    print_summary("power", error, trial);
}


void test_divide()
{
    std::uint32_t trial = 10000;
    std::uint32_t error = 0;   

    for(std::uint32_t t=0; t!=trial; ++t)
    {
        std::uint64_t n = rand() % 1000000 + 1000000;
        std::uint64_t x = rand() % 100000 + 1;
        std::uint64_t y = n/x; 
        std::uint64_t z = alg::divide(n,x);
        if (y != z) ++error;
    }
    print_summary("divide", error, trial);
}


void test_arithmetic()
{
    test_bignum();
    test_reverse_digits();
    test_count_num_of_one_bits();
    test_multiply();
    test_power();
    test_divide();
}
