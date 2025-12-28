#include <cassert>
#include <concepts>
#include <thread>

#include <coroutine0_generator.h>
#include <coroutine1_awaitor.h>
#include <coroutine2_awaitor_pc.h>
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



// *********************************** //
// *** Generator pattern - example *** //
// *********************************** //
// producer = alg::generator (co_yield to push product)
// consumer = test function 
//
template<bool DEBUG>
alg::generator<pod,DEBUG> coroutine_to_produce(std::uint32_t y, 
                                               std::uint32_t m,
                                               std::uint32_t d, // start date
                                               std::uint32_t N) // num of days
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

void run_coroutine_caller_to_consume() 
{
    std::cout << "\n=========================================================================================";
    alg::generator<pod,true> generator = coroutine_to_produce< true>(2022, 5, 31, 4);

    while(generator)
    {
        const auto& date = generator.get_product();
        std::cout << "\ncaller consumes ---> num_yields = " 
                  << generator.get_num_yields() << ", date = " << date;
    }
    std::cout << "\n=========================================================================================";
}



// ********************************* //
// *** Awaitor pattern - example *** //
// ********************************* //
// consumer = alg::task (co_await to pull product)
// producer = test function
//
[[nodiscard]] 
alg::task<pod,true> coroutine_to_consume()
{
    alg::awaitable<pod,true> awaitable{}; 
    while(awaitable)
    {
        const auto& date = co_await awaitable; 
        std::cout << "\ncoroutine consumes ---> num_awaits = " 
                  << awaitable.get_num_awaits() << ", date = " << date << " = "
                  << awaitable.get_product();
    }
}

void run_coroutine_caller_to_produce()
{
    std::cout << "\n=========================================================================================";
    alg::task<pod,true> task = coroutine_to_consume();

    for(std::uint32_t n=0; n<4; ++n) 
    {
        pod date{2030, 1+n, 15};
        task.set_product(date);
    }
    std::cout << "\n=========================================================================================";
}



// ************************************* //
// *** Generator pattern - full test *** //
// ************************************* //
// assert is done in coroutine caller
//
void run_coroutine_caller_to_consume_full_test() 
{
    alg::generator<pod,false> generator0 = coroutine_to_produce<false>(2021, 10, 31, 5);
    {
        assert(generator0);
        const auto& date = generator0.get_product(); // <--- caller yields, coroutine produces and co_yield, caller resumes
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
        const auto& date = generator0.get_product(); // <--- caller yields, coroutine run to end without produces, caller resumes
        assert((generator0.get_num_yields() == 5 && date == pod{2022, 3, 28}));
    }
    assert(!generator0);

    alg::generator<pod,false> generator1 = coroutine_to_produce<false>(2022, 5, 31, 3);
    {
        assert(generator1);
        const auto& date = generator1.get_product(); // <--- caller yields, coroutine produces and co_yield, caller resumes
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
        const auto& date = generator1.get_product(); // <--- caller yields, coroutine run to end without produces, caller resumes
        assert((generator1.get_num_yields() == 3 && date == pod{2022, 8, 30}));
    }
    assert(!generator1);
    print_summary("coroutine - generator full test", "succeeded");
}



// *********************************** //
// *** Awaitor pattern - full test *** //
// *********************************** //
// assert is done in coroutine, hence :
// * we cannot reuse coroutine_to_consume()
// * we need to  use coroutine_to_consume_with_assert()
//
[[nodiscard]] 
alg::task<pod,false> coroutine_to_consume_with_assert() 
{
    alg::awaitable<pod,false> awaitable{}; 
    {
        assert(awaitable);
        const auto& date = co_await awaitable; 
        assert((awaitable.get_num_awaits() == 1));
        assert((awaitable.get_product() == pod{2030, 1, 15}));
    }{
        assert(awaitable);
        const auto& date = co_await awaitable; 
        assert((awaitable.get_num_awaits() == 2));
        assert((awaitable.get_product() == pod{2030, 2, 15}));
    }{
        assert(awaitable);
        const auto& date = co_await awaitable; 
        assert((awaitable.get_num_awaits() == 3));
        assert((awaitable.get_product() == pod{2030, 3, 15}));
    }{
        assert(awaitable);
        const auto& date = co_await awaitable; 
        assert((awaitable.get_num_awaits() == 4));
        assert((awaitable.get_product() == pod{2030, 4, 15}));
    }{
        assert(awaitable);                     // <--- awaitable here, as handle still exists
        const auto& date = co_await awaitable; // <--- yields to caller, caller runs to the end, task<T> out of scope
        assert(false);                         // <--- caller never yields again, hence this line cannot be reached
    }
}

void run_coroutine_caller_to_produce_full_test() 
{
    alg::task<pod,false> task = coroutine_to_consume_with_assert();

    for(std::uint32_t n=0; n<4; ++n) 
    {
        pod date{2030, 1+n, 15};
        task.set_product(date);
    }
    print_summary("coroutine - awaitor full test", "succeeded");
}



// *************************************** //
// *** Producer & consumer - full test *** //
// *************************************** //
[[nodiscard]] 
alg::generator_pc<pod, std::string> coroutine_pc()
{
    // Two suspensions in each loop
    // * co_await T from coroutine caller
    // * co_yield U to   coroutine caller
    //
    while(true) 
    {
        const pod& date = co_await alg::awaitable_pc<pod, std::string>{};

        std::string str(std::to_string(date.y));
        str.append("-");
        if (date.m < 10) str.append("0");
        str.append(std::to_string(date.m));
        str.append("-");
        if (date.d < 10) str.append("0");
        str.append(std::to_string(date.d));

        co_yield str;
    }
}

void run_producer_consumer_full_test()
{
    auto generator = coroutine_pc();
    {
        generator.set_product_T(pod{2016, 7, 12});
        const auto& str = generator.get_product_U();
        assert(str == "2016-07-12");
    }{
        generator.set_product_T(pod{2017, 8, 11});
        const auto& str = generator.get_product_U();
        assert(str == "2017-08-11");
    }{
        generator.set_product_T(pod{2018, 9, 10});
        const auto& str = generator.get_product_U();
        assert(str == "2018-09-10");
    }{
        generator.set_product_T(pod{2019, 10, 9});
        const auto& str = generator.get_product_U();
        assert(str == "2019-10-09");
    }{
        generator.set_product_T(pod{2020, 11, 8});
        const auto& str = generator.get_product_U();
        assert(str == "2020-11-08");
    }{
        generator.set_product_T(pod{2021, 12, 7});
        const auto& str = generator.get_product_U();
        assert(str == "2021-12-07");
    }
    print_summary("coroutine - producer-consumer full test", "succeeded");
}



// ************ //
// *** Test *** //
// ************ //
void test_coroutine()
{
    run_coroutine_caller_to_consume_full_test();
    run_coroutine_caller_to_produce_full_test();
    run_producer_consumer_full_test();            std::cout << "\n";
    run_coroutine_caller_to_consume();            std::cout << "\n"; 
    run_coroutine_caller_to_produce();            std::cout << "\n";
}





