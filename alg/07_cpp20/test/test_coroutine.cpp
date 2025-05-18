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

bool operator==(const pod& lhs, const pod& rhs)
{
    return lhs.y == rhs.y &&
           lhs.m == rhs.m &&
           lhs.d == rhs.d;
}

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
// producer = alg::generator (co_yield to push product)
// consumer = test function 
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
        if (data.m == 2)  
        {
            data.d = std::min(data.d, (std::uint32_t)28);
        }
        else if (data.m == 4 || data.m == 6 || data.m == 9 || data.m == 11) 
        {
            data.d = std::min(data.d, (std::uint32_t)30);
        }
        co_yield data;
    }
}

void test_coroutine_step_by_step_generator() 
{
    std::cout << "\n======================================";
    alg::generator<pod,true> generator = coroutine_to_produce< true>(2022, 5, 31, 4);

    while(generator)
    {
        const auto& date = generator.get_product();
        std::cout << "\ncaller consumes ---> num_yields = " << generator.get_num_yields() << ", date = " << date;
    }
    std::cout << "\n======================================";
}



// ************************************** //
// *** Step by step - Awaitor pattern *** //
// ************************************** //
// consumer = alg::task (co_await to pull product)
// producer = test function
//
[[nodiscard]] alg::task<pod,true> coroutine_to_consume()
{
    alg::awaitable<pod,true> awaitable{}; 
    while(true) // <--- This is inf loop, how can prog end? See remark 1
    {
        const auto& date = co_await awaitable; 
        std::cout << "\ncoroutine consumes ---> num_awaits = " << awaitable.get_num_awaits() << ", date = " << date;
    }
}

[[nodiscard]] alg::task<pod,false> coroutine_to_consume_with_assert() // <--- Todo
{
    alg::awaitable<pod,false> awaitable{}; 
    {
        const auto& date = co_await awaitable; 
    }
}

void test_coroutine_step_by_step_awaitor()
{
    std::cout << "\n======================================";
    alg::task<pod,true> task = coroutine_to_consume();

    for(std::uint32_t n=0; n<4; ++n) 
    {
        pod date{2030, 1+n, 15};
        task.set_product(date);
    }
    std::cout << "\n======================================";
    
    // Remark 1 :
    // When caller resumes execution (right after coroutine finished the last production),
    // then caller runs to the end of test function, task runs out of scope, destroy coroutine by RAII. 
}



// ***************************************** //
// *** Complete test - Generator pattern *** //
// ***************************************** //
// assert is done in coroutine caller
//
void test_coroutine_complete_test_generator() 
{
    alg::generator<pod,false> generator0 = coroutine_to_produce<false>(2021, 10, 31, 5);
    {
        assert(generator0);
        const auto& date = generator0.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert((generator0.get_num_yields() == 1 && date == pod{2021, 11, 30}));
    }{ 
        assert(generator0);
        const auto& date = generator0.get_product(); // <--- same as above
        assert((generator0.get_num_yields() == 2 && date == pod{2021, 12, 30}));
    }{ 
        assert(generator0);
        const auto& date = generator0.get_product(); // <--- same as above
        assert((generator0.get_num_yields() == 3 && date == pod{2022, 1, 30}));
    }{ 
        assert(generator0);
        const auto& date = generator0.get_product(); // <--- same as above
        assert((generator0.get_num_yields() == 4 && date == pod{2022, 2, 28}));
    }{ 
        assert(generator0);
        const auto& date = generator0.get_product(); // <--- same as above
        assert((generator0.get_num_yields() == 5 && date == pod{2022, 3, 28}));
    }{ 
        assert(generator0);
        const auto& date = generator0.get_product(); // <--- caller yields here, coroutine run to end and return (without produces), caller then resumes
        assert((generator0.get_num_yields() == 5 && date == pod{2022, 3, 28}));
    }
    assert(!generator0);

    alg::generator<pod,false> generator1 = coroutine_to_produce<false>(2022, 5, 31, 3);
    {
        assert(generator1);
        const auto& date = generator1.get_product(); // <--- caller yields here, coroutine produces and co_yield, caller then resumes
        assert((generator1.get_num_yields() == 1 && date == pod{2022, 6, 30}));
    }{ 
        assert(generator1);
        const auto& date = generator1.get_product(); // <--- same as above
        assert((generator1.get_num_yields() == 2 && date == pod{2022, 7, 30}));
    }{ 
        assert(generator1);
        const auto& date = generator1.get_product(); // <--- same as above
        assert((generator1.get_num_yields() == 3 && date == pod{2022, 8, 30}));
    }{ 
        assert(generator1);
        const auto& date = generator1.get_product(); // <--- caller yields here, coroutine run to end and return (without produces), caller then resumes
        assert((generator1.get_num_yields() == 3 && date == pod{2022, 8, 30}));
    }
    assert(!generator1);
    print_summary("coroutine - generator", "succeeded");
}



// *************************************** //
// *** Complete test - Awaitor pattern *** //
// *************************************** //
// assert is done in coroutine, hence :
// * we cannot reuse coroutine_to_consume()
// * we need another coroutine_to_consume_with_assert()
//
void test_coroutine_complete_test_awaitor() 
{
/*  alg::task<pod,false> task0 = coroutine_to_consume_with_assert();
    {
        pod date{2030, 1, 15};
        task0.set_product(date);
    } */
    print_summary("coroutine - awaitor", "succeeded");
}













// ************ //
// *** Test *** //
// ************ //
void test_coroutine()
{
    test_coroutine_step_by_step_generator();   std::cout << "\n"; 
    test_coroutine_step_by_step_awaitor();     std::cout << "\n";
//  test_coroutine_step_by_step_pipeline();    std::cout << "\n";

    test_coroutine_complete_test_generator();
    test_coroutine_complete_test_awaitor();
}





// ********************************** //
// *** Producer consumer pipeline *** //
// ********************************** //
// Coroutine that acts as producer and consumer
//
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
void test_coroutine_step_by_step_pipeline()
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


