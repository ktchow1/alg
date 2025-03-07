#pragma once
#include<sstream>
#include<iomanip>
#include<cmath>
#include<set>


namespace alg
{
    template<typename T> 
    concept accumulatable = requires(T x)
    {
        {x+x} -> std::convertible_to<T>;
    };

    template<typename T> 
    concept multiplicable = requires(T x)
    {
        {x*x} -> std::convertible_to<T>;
    };

    template<typename T> 
    concept divisible = requires(T x)
    {
        {x / std::declval<std::uint32_t>()} -> std::convertible_to<T>;
    };
}

namespace alg
{
    template<typename T> 
    concept statistical = accumulatable<T> && 
                          multiplicable<T> && 
                          divisible<T>;


    template<statistical T>
    class statistics
    {
    public:
        statistics() : count(0), sum{0}, sum_sq{0}, values{}
        {
        }
        ~statistics() = default;
        statistics(const statistics<T>&) = default;
        statistics(statistics<T>&&) = default;
        statistics& operator=(const statistics<T>&) = default;
        statistics& operator=(statistics<T>&&) = default;

    public:
        void add(const T& x)
        {
            ++count;
            sum += x;
            sum_sq += x*x;
            values.insert(x);
        }

        auto get_count() const
        {
            return count;
        }

        T get_mean() const
        {
            if (count==0) return 0;
            return sum/count;
        }

        T get_stddev() const
        {
            if (count==0) return 0;
            return std::sqrt(sum_sq/count - (sum/count)*(sum/count));
        }

        T get_percentile(double r) const
        {
            std::uint32_t N = (values.size()-1) * r;

            auto iter = values.begin();
            for(std::uint32_t n=0; n!=N; ++n) ++iter;
            return *iter; 
        }

        statistics& operator+=(const statistics& rhs)
        {
            count  += rhs.count;
            sum    += rhs.sum;
            sum_sq += rhs.sum_sq;
            values.insert(rhs.values.begin(), rhs.values.end());
            return *this;
        }

    public:
        std::string get_string() const 
        {
            std::stringstream ss;
            ss << "\ncount   : " << get_count();
            if (get_count() > 0)
            {
                ss << "\naverage = " << get_mean();
                ss << "\nstd dev = " << get_stddev();
                ss << "\npercent = ";
                for(auto x : {0.001, 0.01, 0.05, 0.10, 0.25, 0.50, 0.75, 0.90, 0.95, 0.99, 0.999})
                {
                    ss << x*100 << "% " << get_percentile(x) << "| ";
                }
            }
            return ss.str();
        }

        std::string get_str() const 
        {
            std::stringstream ss;
            ss << "|";
            if (get_count() > 0)
            {
                for(auto x : {0.001, 0.01, 0.10, 0.25, 0.50, 0.75, 0.90, 0.99, 0.999})
                {
                    ss << x*100 << "% " << get_percentile(x) << "| ";
                }
            }
            return ss.str();
        }

    private:
        std::uint32_t count;
        T sum;
        T sum_sq;
        std::multiset<T> values;
    };
}
