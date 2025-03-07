#pragma once
#include<iostream>
#include<iomanip>
#include<cstdint>
#include<vector>
#include<algorithm>
#include<functional>
#include<statistics.h>
#include<timer.h>

using namespace std::placeholders;


// ************************* //
// *** Random generators *** //
// ************************* //
inline std::string gen_random_str(std::uint32_t size, std::uint32_t alphabet_set) // 1-26
{
    std::uint32_t alphabet_set_size = 26;
    if (alphabet_set >= 1 && alphabet_set <= 26) 
    {
        alphabet_set_size = alphabet_set;
    }

    std::string ans;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        char c = 'a' + std::rand() % alphabet_set_size;
        ans.push_back(c);
    }
    return ans;
}

inline void gen_mirror_image(std::string& str, std::uint32_t centre, std::uint32_t radius)
{
    for(std::uint32_t r=0; r!=radius; ++r)
    {
        if (centre >= r && centre+r < str.size())
        {
            str[centre-r] = str[centre+r];
        }
        else break;
    }
}

inline std::string gen_random_palindrome(std::uint32_t size, std::uint32_t alphabet_set)
{
    std::uint32_t num_image = 5;
    std::uint32_t max_radius = size<3 ?  size : size/3;

    auto str = gen_random_str(size, alphabet_set);
    for(std::uint32_t n=0; n!=num_image; ++n)
    {
        std::uint32_t centre = rand() % str.size();
        std::uint32_t radius = rand() % max_radius + 1;
        gen_mirror_image(str, centre, radius);
    }
    return str;
}

template<typename T>
std::vector<T> gen_random_vec(std::uint32_t size, T min, T max)
{
    std::vector<T> ans;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        T x = min + std::rand() % (max-min);
        ans.push_back(x);
    }
    return ans;
}

template<typename T>
std::vector<T> gen_random_sorted_vec(std::uint32_t size, T min, T max)
{
    auto ans = gen_random_vec(size, min, max);
    std::sort(ans.begin(), ans.end());
    return ans;
}

template<typename T>
std::vector<T> gen_random_partial_sorted_vec(std::uint32_t size, T min, T max)
{
    auto ans = gen_random_sorted_vec(size, min, max);

    std::uint32_t N0 =        rand() % (size/2);
    std::uint32_t N1 = size - rand() % (size/2);
    for(std::uint32_t t=0; t!=N1-N0; ++t)
    {
        std::uint32_t n0 = rand() & (N1-N0) + N0; 
        std::uint32_t n1 = rand() & (N1-N0) + N0; 
        std::swap(ans[n0], ans[n1]);
    }
    return ans;
}

// Generate swapped vector [0,1,2,3,...,N-1]
inline std::vector<std::uint32_t> gen_random_swapped_order(std::uint32_t size)
{
    std::vector<std::uint32_t> ans;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        ans.push_back(n);
    }

    for(std::uint32_t n=0; n!=size; ++n)
    {
        std::uint32_t x = rand()%3;
        if (x==1 && n>=1)
        {
            std::swap(ans[n], ans[n-1]);
        }
        else if (x==2 && n>=2)
        {
            std::swap(ans[n], ans[n-1]);
            std::swap(ans[n-1], ans[n-2]);
        }
    }
    return ans;
}

// Assume size <= max-min+1
inline std::vector<std::uint32_t> gen_random_coins(std::uint32_t size, std::uint32_t min, std::uint32_t max)
{
    return gen_random_sorted_vec(size, min, max);
}

inline auto gen_random_objects(std::uint32_t size, 
                               std::uint32_t min_weight, 
                               std::uint32_t max_weight,
                               std::uint32_t min_value, 
                               std::uint32_t max_value)
{
    std::vector<std::pair<std::uint32_t,std::uint32_t>> ans;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        std::uint32_t w = min_weight + rand()% (max_weight - min_weight);
        std::uint32_t v = min_value  + rand()% (max_value  - min_value);
        ans.push_back(std::make_pair(w,v));
    }
    return ans;
}

inline auto gen_random_jobs(std::uint32_t size, 
                            std::uint32_t min_workload, 
                            std::uint32_t max_workload,
                            std::uint32_t min_profit, 
                            std::uint32_t max_profit,
                            std::uint32_t min_deadline, 
                            std::uint32_t max_deadline)
{
    auto deadlines = gen_random_sorted_vec(size, min_deadline, max_deadline);

    std::vector<std::tuple<std::uint32_t,std::uint32_t,std::uint32_t>> ans;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        std::uint32_t w = min_workload + rand()% (max_workload - min_workload);
        std::uint32_t p = min_profit   + rand()% (max_profit   - min_profit);
        std::uint32_t d = deadlines[n];
        ans.push_back(std::make_tuple(w,p,d));
    }
    return ans;
}

struct box // gen function needs to ensure x <= y <= z
{
    std::uint32_t m_x; 
    std::uint32_t m_y;
    std::uint32_t m_z;
};

inline auto gen_random_boxes(std::uint32_t size, 
                             std::uint32_t side_min,
                             std::uint32_t side_max) 
{
    std::vector<box> ans;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        std::uint32_t x = side_min + rand() % (side_max-side_min);
        std::uint32_t y = side_min + rand() % (side_max-side_min);
        std::uint32_t z = side_min + rand() % (side_max-side_min);
        std::uint32_t min = std::min(std::min(x, y), z);
        std::uint32_t max = std::max(std::max(x, y), z);
        std::uint32_t mid = x + y + z - min - max;
        ans.push_back({min, mid, max});
    }
    return ans;
}

struct bin_packing_problem
{
    // Assume there are 2 types of objects, A & B (unlike dynprog.doc which has 3 types)
    std::uint32_t m_num_objA;
    std::uint32_t m_num_objB;
    std::uint32_t m_size_objA;
    std::uint32_t m_size_objB;
    std::uint32_t m_size_bin;
};

inline auto gen_random_bins(std::uint32_t num_obj_min, 
                            std::uint32_t num_obj_max,
                            std::uint32_t size_obj_min,
                            std::uint32_t size_obj_max,
                            std::uint32_t size_bin_min,
                            std::uint32_t size_bin_max)
{
    bin_packing_problem ans;
    ans.m_num_objA  =  num_obj_min + rand() % ( num_obj_max -  num_obj_min);
    ans.m_num_objB  =  num_obj_min + rand() % ( num_obj_max -  num_obj_min);
    ans.m_size_objA = size_obj_min + rand() % (size_obj_max - size_obj_min);
    ans.m_size_objB = size_obj_min + rand() % (size_obj_max - size_obj_min);
    ans.m_size_bin  = size_bin_min + rand() % (size_bin_max - size_bin_min);
    return ans;
}

enum class logic : std::uint32_t
{
    OR,AND
};

struct bool_symbol
{
    bool  m_value;
    logic m_logic;
};

inline std::ostream& operator<<(std::ostream& os, const bool_symbol& x)
{
    if (x.m_logic == logic::OR) os << x.m_value << " OR ";
    else                        os << x.m_value << " AND "; 

    return os;
}

inline auto gen_random_bool_expression(std::uint32_t size)
{
    std::vector<bool_symbol> ans;
    for(std::uint32_t n=0; n!=size; ++n)
    {
        bool_symbol x;
        x.m_value = (rand()%2==0);
        x.m_logic = (rand()%2==0? logic::OR : logic::AND);
        ans.push_back(x);
    }
    return ans;
}

inline auto gen_random_piecewise_linear(std::uint32_t num_lines, std::uint32_t num_data_min, std::uint32_t num_data_max, double noise_level)
{
    std::vector<double> ans;
    double y0 = 0;
    double y1 = 0;

    for(std::uint32_t m=0; m!=num_lines; ++m)
    {
        y0 = y1;
        y1 = (rand() % 2000) / 100.0 - 10.0;

        std::uint32_t num_data = num_data_min + rand() % (num_data_max - num_data_min);
        for(std::uint32_t n=0; n!=num_data; ++n)
        {
            double e = ((rand() % 2000) / 1000.0 - 1.0) * noise_level;
            double y = y0 + (y1-y0) * ((double)n / (double)num_data) + e;
            ans.push_back(y);
        }
    }
    ans.push_back(y1);
    return ans;
}


// *************** //
// *** Compare *** //
// *************** //
template<typename CONTAINER>
bool compare(const CONTAINER& c0, const CONTAINER& c1)
{
    if (c0.size() != c1.size()) return false;

    auto i0 = c0.begin();
    auto i1 = c1.begin();
    for(; i0!=c0.end(); ++i0, ++i1)
    {
        if (*i0 != *i1) return false;
    }
    return true;
}

template<typename T, typename VIEW0, typename VIEW1> // Todo : T can be deduced from VIEW0 & VIEW1
bool compare(VIEW0& v0, VIEW1& v1)
{
    std::vector<T> vec0; 
    std::vector<T> vec1; 
    for(const auto& x:v0) vec0.push_back(x);
    for(const auto& x:v1) vec1.push_back(x);
    return compare(vec0, vec1);
}


// **************** //
// *** Printing *** //
// **************** //
template<typename CONTAINER>
void print(const std::string& header, const CONTAINER& c)
{
    std::uint32_t n=0;
    std::uint32_t N=c.size();

    std::cout << header << "["; 
    for(const auto& x:c)
    {
        if (n!=N) std::cout << x << ",";
        else      std::cout << x << "]";
        ++n;
    }
}

template<typename OUTPUT>
void print_one_case(const std::string& test_name, 
                    OUTPUT ans0, 
                    OUTPUT ans1, 
                    std::uint32_t error,
                    std::uint32_t trial, 
                    bool flag, 
                    const std::string& str = "")
{
    std::cout << "\n" << test_name
              << " : ans0 = " << ans0 
              <<  ", ans1 = " << ans1 
              <<  ", error rate = " << error
              <<  "/" << trial
              <<  " " << (flag? "OK":"ERROR") << str << std::flush;
}

inline void print_summary(const std::string& test_name, const std::string& status)
{
    std::cout << "\n" << std::setw(75) << std::left << test_name 
                      << " " << status
                      << std::flush;
}
  
inline void print_summary(const std::string& test_name, std::uint32_t error, std::uint32_t trial)
{
    std::cout << "\n" << std::setw(75) << std::left << test_name 
                      << " error rate = " << error << "/" << trial
                      << std::flush;
}
  
inline void print_summary_return_cursor(const std::string& test_name, std::uint32_t error, std::uint32_t trial, std::uint64_t time0, std::uint64_t time1)
{
    std::cout << "\r" << std::setw(75) << std::left << test_name 
                      << " error rate = " << error << "/" << trial 
                      << ", time = " << time0 << "/" << time1 
                      << std::flush;
}

inline void print_progress(const std::string& test_name, std::uint32_t t, std::uint32_t trial)
{
    if (t == 0)
    {
        std::cout << "\r" << std::setw(75) << std::left << test_name 
                          << " running "
                          << std::flush;
    }
    if (trial >= 20) 
    {
        if (t % (trial/20) == 0) std::cout << "." << std::flush; 
    }   
    else
    {
        std::cout << "." << std::flush; 
    }
}


// *************************** //
// *** Test loop functions *** //
// *************************** //
template<std::uint32_t GEN_NUM, typename GEN_FUNCTION, typename ALG_FUNCTION, typename BMK_FUNCTION>
void benchmark(const std::string&  test_name,
               const GEN_FUNCTION& gen_function, // nullary
               const ALG_FUNCTION& alg_function, // nary = unary
               const BMK_FUNCTION& bmk_function, // nary = unary
               std::uint32_t       trial, 
               bool                print_each_case = false) 
{
    alg::timer timer0;
    alg::timer timer1;
    alg::statistics<std::uint64_t> stat0;
    alg::statistics<std::uint64_t> stat1;
    std::uint32_t error = 0;

    std::cout << "\n";
    for(std::uint32_t t=0; t!=trial; ++t)
    {
        if constexpr (GEN_NUM == 1)
        {
            auto data = gen_function();

            timer0.click();
            auto ans0 = alg_function(data);
            timer0.click();

            timer1.click();
            auto ans1 = bmk_function(data); 
            timer1.click();

            if (ans0 != ans1) ++error;
            if (print_each_case) 
            {
                print_one_case(test_name, ans0, ans1, error, trial, ans0 == ans1);
            }
            else 
            {
                print_progress(test_name, t, trial);
            }
        }
        else
        {
            auto data0 = gen_function();
            auto data1 = gen_function();

            timer0.click();
            auto ans0 = alg_function(data0, data1);
            timer0.click();

            timer1.click();
            auto ans1 = bmk_function(data0, data1); 
            timer1.click();

            if (ans0 != ans1) ++error;
            if (print_each_case) 
            {
                print_one_case(test_name, ans0, ans1, error, trial, ans0 == ans1);
            }
            else 
            {
                print_progress(test_name, t, trial);   
            }
        }

        stat0.add(timer0.time_elapsed_in_nsec());
        stat1.add(timer1.time_elapsed_in_nsec());
    }
    print_summary_return_cursor(test_name, error, trial, stat0.get_mean(), stat1.get_mean());
}
