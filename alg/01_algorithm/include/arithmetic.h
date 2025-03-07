#pragma once
#include<iostream>
#include<cstdint>
#include<string>
#include<vector>


namespace alg 
{
    class bignum
    {
    public:
        bignum() = default;

        explicit bignum(const std::string& str)
        {
            for(std::uint32_t n=str.size(); n!=0; --n)
            {
                if (str[n-1] >= '0' &&
                    str[n-1] <= '9') { m_digits.push_back((std::uint32_t)str[n-1] - '0'); }
                else                 { m_digits.clear();   break;                         }
            }
            trim_zeros();
        }

        auto to_string() const noexcept
        {
            std::string str;
            for(std::uint32_t n=m_digits.size(); n!=0; --n)
            {
                str.push_back((char)(m_digits[n-1] + '0'));
            }
            return str;
        }   

    public:
        void push_back(std::uint32_t n) 
        {
            m_digits.push_back(n);
        }

        std::uint32_t size() const noexcept
        {
            return m_digits.size();
        }

        std::uint32_t operator[](std::uint32_t n) const noexcept
        {
            return m_digits[n];
        }

    private:
        void trim_zeros()
        {
            while(m_digits.size() > 1 && m_digits.back() == 0) m_digits.pop_back();
        }

    private:
        std::vector<std::uint32_t> m_digits; // from LSB to MSB
    };


    inline bignum operator+(const bignum& x, const bignum& y)
    {
        bignum ans;

        std::uint32_t n = 0;
        std::uint32_t c = 0; // carry
        while(n < x.size() && n < y.size())
        {
            std::uint32_t s = x[n] + y[n] + c;
            c = s / 10;
            s = s % 10;
            ans.push_back(s);
            ++n;
        }
        while(n < x.size())
        {
            std::uint32_t s = x[n] + c;
            c = s / 10;
            s = s % 10;
            ans.push_back(s);
            ++n;
        }
        while(n < y.size())
        {
            std::uint32_t s = y[n] + c;
            c = s / 10;
            s = s % 10;
            ans.push_back(s);
            ++n;
        }
        if (c > 0) ans.push_back(c);
        return ans;
    }

    inline bignum operator<<(const bignum& x, std::uint32_t num_of_digits) 
    {
        if (x.size() == 1 && x[0] == 0) return x;
        bignum ans;

        for(std::uint32_t n=0; n!=num_of_digits; ++n) ans.push_back(0);
        for(std::uint32_t n=0; n!=x.size();      ++n) ans.push_back(x[n]);
        return ans;
    }

    inline bignum operator*(const bignum& x, const bignum& y)
    {
        if (x.size() == 1 && x[0] == 0) return x;
        if (y.size() == 1 && y[0] == 0) return y;
        bignum ans{"0"};

        for(std::uint32_t n=0; n!=y.size(); ++n)
        {
            bignum tmp;

            std::uint32_t c = 0;
            for(std::uint32_t m=0; m!=x.size(); ++m)
            {
                std::uint32_t p = y[n] * x[m] + c;
                c = p / 10;
                p = p % 10;
                tmp.push_back(p);
            }
            if (c > 0) 
            {
                tmp.push_back(c);
            }
            tmp = tmp << n;
            ans = ans + tmp;
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
            z = z*10 + x%10;
            x = x/10;
        }
        return z;
    }

    inline std::uint64_t count_num_of_one_bits(std::uint64_t x)
    {
        std::uint64_t z = 0;
        while(x > 0)
        {
            z = z + x%2;
            x = x/2;
        }
        return z;
    }

    inline std::uint64_t multiply(std::uint64_t n, std::uint64_t x) // output = n * x
    {
        std::uint64_t z = 0;
        while(x > 0)
        {
            if (x%2 == 1) z = z + n;
            x = x/2;
            n = n*2;
        }
        return z;
    }

    inline std::uint64_t power(std::uint64_t n, std::uint64_t x) // output = n^x
    {
        std::uint64_t z = 1;
        while(x > 0)
        {
            if (x%2 == 1) z = z * n;
            x = x/2;
            n = n*n;
        }
        return z;
    }

    inline std::uint64_t divide(std::uint64_t n, std::uint64_t x) // output = n / x
    {
        // ***************************************** //
        // *** Max sx = x*2^M, such that sx <= n *** //
        // ***************************************** //
        std::uint64_t sx = x;
        std::uint64_t M  = 0;
        while(sx <= n)
        {
            sx = sx * 2;
            ++M;
        }
        if (M > 0)
        {
            sx = sx / 2;
            --M;
        }

        // ********************* //
        // *** Update answer *** //
        // ********************* //
        std::uint64_t z = 0;
        for(std::uint32_t m=0; m!=M+1; ++m) // BUG : M+1 instead of M
        {
            if (n >= sx)
            {
                z = z*2 + 1;
                n = n - sx;
                sx = sx / 2;
            }
            else
            {
                z = z*2;
                sx = sx / 2;
            }
        }
        return z;
    }

}

