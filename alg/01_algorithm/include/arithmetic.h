#pragma once
#include<iostream>
#include<cstdint>
#include<string>
#include<vector>


namespace alg 
{
    inline std::uint32_t get_n_LSD(const std::string& x, std::uint32_t n) // n_lsd = n-th least significant digit
    {
        return (std::uint32_t)(x[x.size()-1-n] - '0');
    }

    inline void add_before_MSD(std::string& x, std::uint32_t i) // msd = most significant digit
    {
        x.insert(x.begin(), (char)(i + '0'));
    }

    inline bool is_zero(const std::string& x)
    {
        return x.size() == 1 && x[0] == '0'; // BUG : compare to '0', NOT to 0
    }


    // *********** //
    // *** ADD *** //
    // *********** //
    inline std::string bignum_add(const std::string& x, const std::string& y)
    {
        std::string ans;

        std::uint32_t n = 0;
        std::uint32_t c = 0; // carry
        while(n < x.size() && n < y.size())
        {
            std::uint32_t s = get_n_LSD(x,n) + get_n_LSD(y,n) + c;
            c = s / 10;
            s = s % 10;
            add_before_MSD(ans,s);
            ++n;
        }
        while(n < x.size())
        {
            std::uint32_t s = get_n_LSD(x,n) + c;
            c = s / 10;
            s = s % 10;
            add_before_MSD(ans,s);
            ++n;
        }
        while(n < y.size())
        {
            std::uint32_t s = get_n_LSD(y,n) + c;
            c = s / 10;
            s = s % 10;
            add_before_MSD(ans,s);
            ++n;
        }
        if (c > 0) add_before_MSD(ans,c);
        return ans;
    }


    // **************** //
    // *** MULTIPLY *** //
    // **************** //
    inline std::string bignum_scale(const std::string& x, std::uint32_t num_of_digits) 
    {
        auto ans = x;
        if (!is_zero(x))
        {
            ans.append(std::string(num_of_digits, '0'));
        }
        return ans;
    }

    inline std::string bignum_multiply(const std::string& x, const std::string& y)
    {
        std::string ans{"0"};
        if (is_zero(x)) return ans;
        if (is_zero(y)) return ans;

        for(std::uint32_t n=0; n!=y.size(); ++n)
        {
            std::string tmp; // tmp = (whole x) * (n-th LSD of y)

            std::uint32_t c = 0;
            for(std::uint32_t m=0; m!=x.size(); ++m)
            {
                std::uint32_t p = get_n_LSD(y,n) * get_n_LSD(x,m) + c;
                c = p / 10;
                p = p % 10;
                add_before_MSD(tmp,p);
            }
            if (c > 0) 
            {
                add_before_MSD(tmp,c);
            }

            tmp = bignum_scale(tmp, n);
            ans = bignum_add(ans, tmp);
        }
        return ans;
    }
}


// ******************************* //
// *** Bits / digits algorithm *** // 
// ******************************* //
namespace alg 
{
    inline std::uint64_t reverse_digits(std::uint64_t x)
    {
        std::uint64_t z = 0;
        while(x > 0)
        {
            z = z * 10 + x % 10;
            x = x / 10;
        }
        return z;
    }

    inline std::uint64_t count_num_of_one_bits(std::uint64_t x)
    {
        std::uint64_t z = 0;
        while(x > 0)
        {
            z = z + x % 2;
            x = x >> 1;
        }
        return z;
    }

    inline std::uint64_t multiply(std::uint64_t n, std::uint64_t x) // output = n * x
    {
        std::uint64_t z = 0;
        while(x > 0)
        {
            if (x %2 == 1) z = z + n; // (n*1)*LSB[0] + (n*2)*LSB[1] + (n*4)*LSB[2] + (n*8)*LSB[3] + ...
            x = x / 2;
            n = n << 1;
        }
        return z;
    }

    inline std::uint64_t power(std::uint64_t n, std::uint64_t x) // output = n^x
    {
        std::uint64_t z = 1;
        while(x > 0)
        {
            if (x % 2 == 1) z = z * n; // (n^1)^LSB[0] * (n^2)^LSB[1] * (n^4)^LSB[2] * (n^8)^LSB[3] * ...
            x = x / 2;
            n = n * n;
        }
        return z;
    }

    inline std::uint64_t divide(std::uint64_t n, std::uint64_t x) // output = n / x
    {
        if (x == 0) return 0; // divided by zero

        // ***************************************************** //
        // *** Max scaled_x = x*2^M, such that scaled_x <= n *** //
        // ***************************************************** //
        std::uint64_t scaled_x = x;
        std::uint64_t M = 0;
        while(scaled_x <= n)
        {
            scaled_x = (scaled_x << 1);
            ++M;
        }
        if (M > 0)
        {
            scaled_x = (scaled_x >> 1);
            --M;
        }
        else return 0;

        // ********************* //
        // *** Update answer *** //
        // ********************* //
        std::uint64_t z = 0;
        for(std::uint32_t m=0; m!=M+1; ++m) // BUG : M+1 instead of M
        {
            if (n >= scaled_x)
            {
                z = (z << 1) + 1;
                n = n - scaled_x;
                scaled_x = (scaled_x >> 1);
            }
            else
            {
                z = (z << 1);
                scaled_x = (scaled_x >> 1);
            }
        }
        return z;
    }

}

