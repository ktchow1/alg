#include <cassert>
#include <concepts>
#include <thread>

#include <coroutine0_generator.h>
#include <coroutine1_awaitor.h>
#include <coroutine2_pc.h>
#include <utility.h>


// ***************** //
// *** Generator *** //
// ***************** //
struct pod
{
    std::uint32_t y;
    std::uint32_t m;
    std::uint32_t d;
};

// Producer : Monthly payment date generator
inline alg::generator<pod> coroutine(std::uint32_t y,
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

// Consumer
void test_coroutine_generator() 
{
    alg::generator<pod> g0 = coroutine(2021, 10, 31, 5);
    alg::generator<pod> g1 = coroutine(2022,  5, 31, 3);

    // ***************** //
    // *** Example 1 *** //
    // ***************** //
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
    // ***************** //
    // *** Example 2 *** //
    // ***************** //
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



// *************** //
// *** Awaitor *** //
// *************** //
void test_coroutine_awaitor()
{
    std::cout << "\ncaller with main thread_id = " << std::this_thread::get_id();
    std::cout << "\n";


    // *** Experiment 0 *** //
    std::coroutine_handle<> h0;
    std::cout << "\nExperiment 0";
    std::cout << "\n--------------------";
    coroutine0(&h0);
    std::cout << "\n--------------------";
    for(int i=0; i<8; ++i)
    {
        std::cout << "\ncaller invokes handle"; 
        h0();
    }
    std::cout << "\n\n";

  
    // *** Experiment 1 *** //
//  std::cout << "\nExperiment 1";
//  std::cout << "\n--------------------";
//  std::coroutine_handle<> h1 = coroutine1();
//  std::cout << "\n--------------------";
//  for(int i=0; i<8; ++i) 
//  {
//      std::cout << "\ncaller invokes handle";
//      h1();
//  }
//  std::cout << "\n\n";


    // *** Experiment 2 *** //
    std::cout << "\nExperiment 2";
    std::cout << "\n--------------------";
    std::coroutine_handle<future2<pod_T,pod_U>::promise_type> h2 = coroutine2<pod_T,pod_U>();
    auto& p2 = h2.promise();
    std::cout << "\n--------------------";
    for(int i=0; i<8; ++i) 
    {
        p2.data_T.a = static_cast<char>('A' + i);
        p2.data_T.n = i;

        std::cout << "\ncaller inputs " << p2.data_T;
        h2();
        std::cout << "\ncaller outputs " << p2.data_U;
        h2();
    }
    std::cout << "\n\n";

  
    
    // *** Explicit destroy handle in heap *** //
    h0.destroy();
//  h1.destroy();
    h2.destroy();
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
        const pod_X& t = co_await alg::awaitable<pod_X, pod_Y>{};

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
    test_coroutine_generator();
    test_coroutine_awaitor();
//  test_coroutine_pc();
}
