#include <cassert>
#include <concepts>
#include <thread>

#include <coroutine0_generator.h>
#include <coroutine1_awaitor.h>
#include <coroutine2_pipeline.h>
#include <utility.h>


struct pod
{
    std::uint32_t y;
    std::uint32_t m;
    std::uint32_t d;
};

std::ostream& operator<<(std::ostream& os, const pod& date)
{
    os << date.y << "-"
       << date.m << "-"
       << date.d;
    return os;
}


// **************************************** //
// *** Step by step - Generator pattern *** //
// **************************************** //
// producer  = alg::generator 
// coonsumer = test function
//
template<bool DEBUG>
alg::generator<pod,DEBUG> coroutine_to_produce(std::uint32_t y,
                                               std::uint32_t m,
                                               std::uint32_t d, 
                                               std::uint32_t N)
{
    pod data{y,m,d};
    for(std::uint32_t n=0; n!=N; ++n) 
    {
        ++data.m;
        if (data.m == 13)
        {
            data.m = 1;
            ++data.y;
        }
        if      (data.m == 2)  data.d = std::min(data.d, (std::uint32_t)28);
        else if (data.m == 4 ||
                 data.m == 6 ||
                 data.m == 9 ||
                 data.m == 11) data.d = std::min(data.d, (std::uint32_t)30);
        co_yield data;
    }
}





void test_coroutine_step_by_step_generator() 
{
    std::cout << "\n======================================";
    alg::generator<pod,true> g2 = coroutine_to_produce< true>(2022, 5, 31, 4);

    while(g2)
    {
        const auto& date = g2.get_product();
        std::cout << "\ncaller consumes ---> num_yields = " << g2.get_num_yields() << ", date = " << date;
    }
    std::cout << "\n======================================";
}



// ************************************** //
// *** Step by step - Awaitor pattern *** //
// ************************************** //
// producer  = alg::generator 
// coonsumer = test function
//

template<bool DEBUG>
[[nodiscard]] alg::task<pod,DEBUG> coroutine_to_consume()
{
    alg::awaitable<pod,DEBUG> awaitable{}; 
    for(std::uint32_t n=0; ; ++n) // infinity loop, its producer's call to end the loop
    {
        const auto& date = co_await awaitable; 
        std::cout << "\ncoroutine consumes ---> num_awaits = " << awaitable.get_num_awaits() << ", date = " << date;
    }
}

void test_coroutine_step_by_step_awaitor()
{
    std::cout << "\n======================================";
    alg::task<pod,true> task = coroutine_to_consume<true>();

    for(std::uint32_t n=0; n<4; ++n) 
    {
        pod date{2030, 1+n, 15};
        task.set_product(date);
    }
    std::cout << "\n======================================";
}


// ***************************************** //
// *** Complete test - Generator pattern *** //
// ***************************************** //
void test_coroutine_complete_test_generator() 
{
    alg::generator<pod,false> g0 = coroutine_to_produce<false>(2021, 10, 31, 5);
    {
        assert(g0);
        const auto& t = g0.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert(g0.get_num_yields() == 1 && t.y == 2021 && t.m == 11 && t.d == 30);
    }{ 
        assert(g0);
        const auto& t = g0.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert(g0.get_num_yields() == 2 && t.y == 2021 && t.m == 12 && t.d == 30);
    }{ 
        assert(g0);
        const auto& t = g0.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert(g0.get_num_yields() == 3 && t.y == 2022 && t.m == 1 && t.d == 30);
    }{ 
        assert(g0);
        const auto& t = g0.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert(g0.get_num_yields() == 4 && t.y == 2022 && t.m == 2 && t.d == 28);
    }{ 
        assert(g0);
        const auto& t = g0.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert(g0.get_num_yields() == 5 && t.y == 2022 && t.m == 3 && t.d == 28);
    }{ 
        assert(g0);
        const auto& t = g0.get_product(); // <--- caller yields here, coroutine run to end and return (without produces), caller then resumes
        assert(g0.get_num_yields() == 5 && t.y == 2022 && t.m == 3 && t.d == 28);
    }

    alg::generator<pod,false> g1 = coroutine_to_produce<false>(2022, 5, 31, 3);
    {
        assert(g1);
        const auto& t = g1.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert(g1.get_num_yields() == 1 && t.y == 2022 && t.m == 6 && t.d == 30);
    }{ 
        assert(g1);
        const auto& t = g1.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert(g1.get_num_yields() == 2 && t.y == 2022 && t.m == 7 && t.d == 30);
    }{ 
        assert(g1);
        const auto& t = g1.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert(g1.get_num_yields() == 3 && t.y == 2022 && t.m == 8 && t.d == 30);
    }{ 
        assert(g1);
        const auto& t = g1.get_product(); // <--- caller yields here, coroutine run to end and return (without produces), caller then resumes
        assert(g1.get_num_yields() == 3 && t.y == 2022 && t.m == 8 && t.d == 30);
    }
    print_summary("coroutine - generator", "succeeded");
}



// *************************************** //
// *** Complete test - Awaitor pattern *** //
// *************************************** //
void test_coroutine_complete_test_awaitor() 
{
}

















// **************************************** //
// *** Producer consumer with coroutine *** //
// **************************************** //
// X = data from caller to coroutine
// Y = data from coroutine to caller
  
struct pod_X
{
    char a;
    std::uint16_t n;
};

struct pod_Y
{
    char a;
    char b;
    char c;
};

inline std::ostream& operator<<(std::ostream& os, const pod_X& t)
{
    os << "T = " << t.a << "_" << t.n;
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const pod_Y& u)
{
    os << "U = " << u.a << u.b << u.c;
    return os;
}

[[nodiscard]] alg::future<pod_X, pod_Y> coroutine_pc()
{
    // Two suspensions in each loop
    while(true) 
    {
        const pod_X& t = co_await alg::awaitable_pc<pod_X, pod_Y>{};

        pod_Y u;
        u.a = t.a;
        u.b = t.a + t.n;
        u.c = t.a + t.n * 2;

        std::cout << "\ncoroutine : input " << t << ", output " << u;
        co_yield u;
    }
}
void test_coroutine_pc()
{
    auto f = coroutine_pc();
    for(std::uint16_t n=0; n!=10; ++n) 
    {
        pod_X t{static_cast<char>('a'+n), n};

        f.send_T_to_coroutine(t);
        pod_Y u = f.next_U_from_coroutine();

        std::cout << "\ncaller : input " << t << ", output " << u;
    }
    std::cout << "\n\n";
}


// ************ //
// *** Test *** //
// ************ //
void test_coroutine()
{
    test_coroutine_step_by_step_generator();   std::cout << "\n"; 
    test_coroutine_step_by_step_awaitor();     std::cout << "\n";

    test_coroutine_complete_test_generator();
//  test_coroutine_complete_test_awaitor();
//  test_coroutine_pc();
}
