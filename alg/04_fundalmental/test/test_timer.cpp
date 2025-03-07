#include<iostream>
#include<cassert>
#include<thread>
#include<timer.h>
#include<utility.h>


// ******************************************* //
// Do NOT add more than std::chrono::days(1)
// adding std::chrono::months(1) or
// adding std::chrono::years(1) 
// will result in unexpected day & time shift.
// ******************************************* //
// Do NOT floor more than std::chrono::days(1)
// flooring std::chrono::months(1) or
// flooring std::chrono::years(1)
// will result in unexpected day & time shift.
// ******************************************* //

bool convert_and_compare(const alg::time_point& tp, 
                         const alg::time_pod& pod) // expected answer
{
    timespec        ts0 = alg::to_timespec(tp);
    alg::time_point tp0 = alg::to_time_point<std::chrono::system_clock>(ts0);
    alg::time_pod  pod0 = alg::to_time_pod(tp0);
    alg::time_point tp1 = alg::to_time_point<std::chrono::system_clock>(pod0);
     
    bool ans = (pod == pod0 && tp0 == tp1);
    if (!ans) 
    {
        std::cout << "\nctest_time ERROR : "
                  << "pod="  << alg::to_string(pod)       << ", "
                  << "ts0="  << alg::to_string(ts0)       << ", "
                  << "tp0="  << alg::to_string(tp0, true) << ", "
                  << "pod0=" << alg::to_string(pod0)      << ", "
                  << "tp1="  << alg::to_string(tp1, true) << std::flush;
    }
    return ans;
}


void test_time_conversion()
{
    // **************************** //
    // *** 1. Set time manually *** //
    // **************************** //
    auto tp = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2000,1,1,12,30,0,1234567 });


    for(int n=1; n!=31; ++n, tp+=std::chrono::days(1))
    {
        assert(convert_and_compare(tp, alg::time_pod { 2000,1,n,12,30,0,1234567 }));
    }
    assert(convert_and_compare(tp, alg::time_pod { 2000,1,31,12,30,0,1234567 }));
    tp+=std::chrono::days(1);


    for(int n=1; n!=29; ++n, tp+=std::chrono::days(1)) 
    {
        assert(convert_and_compare(tp, alg::time_pod { 2000,2,n,12,30,0,1234567 }));
    }
    assert(convert_and_compare(tp, alg::time_pod { 2000,2,29,12,30,0,1234567 }));
    tp+=std::chrono::days(1);


    for(int n=1; n!=31; ++n, tp+=std::chrono::days(1)) 
    {
        assert(convert_and_compare(tp, alg::time_pod { 2000,3,n,12,30,0,1234567 }));
    }
    assert(convert_and_compare(tp, alg::time_pod { 2000,3,31,12,30,0,1234567 }));
    tp+=std::chrono::days(1);


    for(int n=1; n!=30; ++n, tp+=std::chrono::days(1))
    {
        assert(convert_and_compare(tp, alg::time_pod { 2000,4,n,12,30,0,1234567 }));
    }
    assert(convert_and_compare(tp, alg::time_pod { 2000,4,30,12,30,0,1234567 }));
    tp+=std::chrono::days(1);


    for(int n=0; n!=10; ++n, tp+=std::chrono::  hours(1)) assert(convert_and_compare(tp, alg::time_pod { 2000,5,1,12+n,30,0,1234567 }));
    for(int n=0; n!=10; ++n, tp+=std::chrono::minutes(1)) assert(convert_and_compare(tp, alg::time_pod { 2000,5,1,22,30+n,0,1234567 }));
    for(int n=0; n!=10; ++n, tp+=std::chrono::seconds(1)) assert(convert_and_compare(tp, alg::time_pod { 2000,5,1,22,40,0+n,1234567 }));
    print_summary("time - conversion of manual time", "succeeded");


    // ****************************** //
    // *** 2. Get time from clock *** //
    // ****************************** //
    for(int n=0; n!=10; ++n)
    {
        timespec ts_now; 
        clock_gettime(CLOCK_REALTIME, &ts_now);   

        alg::time_point tp_now = alg::to_time_point<std::chrono::system_clock>(ts_now);
        alg::time_pod  pod_now = alg::to_time_pod(tp_now);
        convert_and_compare(tp_now, pod_now);
    
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    for(int n=0; n!=10; ++n)
    {
        alg::time_point tp_now = std::chrono::system_clock::now();
        alg::time_pod  pod_now = alg::to_time_pod(tp_now);
        convert_and_compare(tp_now, pod_now);
    
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    print_summary("time - conversion of clock time", "succeeded");
}


void test_time_chrono()
{
    // duration cast
    {
        std::chrono::nanoseconds dur  = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,56,1234567 }).time_since_epoch();
        std::chrono::nanoseconds dur0 = std::chrono::duration_cast<std::chrono::years>  (dur);
        std::chrono::nanoseconds dur1 = std::chrono::duration_cast<std::chrono::months> (dur);
        std::chrono::nanoseconds dur2 = std::chrono::duration_cast<std::chrono::days>   (dur);
        std::chrono::nanoseconds dur3 = std::chrono::duration_cast<std::chrono::hours>  (dur);
        std::chrono::nanoseconds dur4 = std::chrono::duration_cast<std::chrono::minutes>(dur);
        std::chrono::nanoseconds dur5 = std::chrono::duration_cast<std::chrono::seconds>(dur);
        std::chrono::nanoseconds ans0 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,1,1,0,0,0,0 }).time_since_epoch();
        std::chrono::nanoseconds ans1 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,1,0,0,0,0 }).time_since_epoch();
        std::chrono::nanoseconds ans2 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,0,0,0,0 }).time_since_epoch();
        std::chrono::nanoseconds ans3 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,0,0,0 }).time_since_epoch();
        std::chrono::nanoseconds ans4 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,0,0 }).time_since_epoch();
        std::chrono::nanoseconds ans5 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,56,0 }).time_since_epoch();
        assert(dur0 != ans0); // <--- unexpected shift in day & time, why?
        assert(dur1 != ans1); // <--- unexpected shift in day & time, why?
        assert(dur2 == ans2);
        assert(dur3 == ans3);
        assert(dur4 == ans4);
        assert(dur5 == ans5);
    }

    // time_point cast
    {
        alg::time_point tp   = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,56,1234567 });
        alg::time_point tp0  = std::chrono::time_point_cast<std::chrono::years>  (tp); 
        alg::time_point tp1  = std::chrono::time_point_cast<std::chrono::months> (tp); 
        alg::time_point tp2  = std::chrono::time_point_cast<std::chrono::days>   (tp);
        alg::time_point tp3  = std::chrono::time_point_cast<std::chrono::hours>  (tp);
        alg::time_point tp4  = std::chrono::time_point_cast<std::chrono::minutes>(tp);
        alg::time_point tp5  = std::chrono::time_point_cast<std::chrono::seconds>(tp);
        alg::time_point ans0 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,1,1,0,0,0,0 });
        alg::time_point ans1 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,1,0,0,0,0 });
        alg::time_point ans2 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,0,0,0,0 });
        alg::time_point ans3 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,0,0,0 });
        alg::time_point ans4 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,0,0 });
        alg::time_point ans5 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,56,0 });
        assert(tp0 != ans0); // <--- unexpected shift in day & time, why?
        assert(tp1 != ans1); // <--- unexpected shift in day & time, why?
        assert(tp2 == ans2);
        assert(tp3 == ans3);
        assert(tp4 == ans4);
        assert(tp5 == ans5);
    }   

    // time_point floor
    {
        alg::time_point tp   = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,56,1234567 });
        alg::time_point tp0  = std::chrono::floor<std::chrono::years>  (tp);
        alg::time_point tp1  = std::chrono::floor<std::chrono::months> (tp);
        alg::time_point tp2  = std::chrono::floor<std::chrono::days>   (tp);
        alg::time_point tp3  = std::chrono::floor<std::chrono::hours>  (tp);
        alg::time_point tp4  = std::chrono::floor<std::chrono::minutes>(tp);
        alg::time_point tp5  = std::chrono::floor<std::chrono::seconds>(tp);
        alg::time_point ans0 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,1,1,0,0,0,0 });
        alg::time_point ans1 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,1,0,0,0,0 });
        alg::time_point ans2 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,0,0,0,0 });
        alg::time_point ans3 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,0,0,0 });
        alg::time_point ans4 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,0,0 });
        alg::time_point ans5 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,56,0 });
        assert(tp0 != ans0); // <--- unexpected shift in day & time, why?
        assert(tp1 != ans1); // <--- unexpected shift in day & time, why?
        assert(tp2 == ans2);
        assert(tp3 == ans3);
        assert(tp4 == ans4);
        assert(tp5 == ans5);
    }
    
    // time_point addition
    {
        alg::time_point tp   = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,56,1234567 });
        alg::time_point tp0  = tp + std::chrono::years  (3);
        alg::time_point tp1  = tp + std::chrono::months (3);
        alg::time_point tp2  = tp + std::chrono::days   (3);
        alg::time_point tp3  = tp + std::chrono::hours  (3);
        alg::time_point tp4  = tp + std::chrono::minutes(3);
        alg::time_point tp5  = tp + std::chrono::seconds(3);
        alg::time_point ans0 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2027,5,15,12,34,56,1234567 });
        alg::time_point ans1 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,8,15,12,34,56,1234567 });
        alg::time_point ans2 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,18,12,34,56,1234567 });
        alg::time_point ans3 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,15,34,56,1234567 });
        alg::time_point ans4 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,37,56,1234567 });
        alg::time_point ans5 = alg::to_time_point<std::chrono::system_clock>(alg::time_pod { 2024,5,15,12,34,59,1234567 });
        assert(tp0 != ans0); // <--- unexpected shift in day & time, why?
        assert(tp1 != ans1); // <--- unexpected shift in day & time, why?
        assert(tp2 == ans2);
        assert(tp3 == ans3);
        assert(tp4 == ans4);
        assert(tp5 == ans5);
    }
    print_summary("time - chrono", "succeeded");
}


void test_timer()
{
    test_time_conversion();
    test_time_chrono();
}
