#pragma once
#include<chrono>
#include<ctime>     // for timespec
#include<iomanip>   // for std::put_time
#include<sstream>


// Common time classes
// 1. std::tm     is STL-standard, low precision POD timepoint (up to seconds)      <--- POD of integers
// 2. std::time_t is STL-standard, low precision int timepoint (up to seconds)      <--- one integer
// 3. timespec is posix-standard, high precision int timepoint (up to nanoseconds)  <--- two integers
// 4. std::chrono::time_point,    high precision     timepoint 
// 5. alg::time_pod,              high precision POD timepoint
//
//
// There 5 main concepts in std::chrono :
// 1. std::chrono::duration - years, months, days, hours, ...
// 2. std::chrono::timepoint
// 3. std::chrono::system_clock
// 4. std::chrono::year_month_day, year, month, day <--- represent specific date, this is NOT a std::chrono::timepoint, but constructible from the latter
// 5. std::chrono::hh_mm_ss                         <--- represent time of day,   this is NOT a std::chrono::duration,  but convertible to the latter
//
//
// Note :
// 1. years is duration, year is specific year
//    months is duration, month is specific month
// 2. time_point + duration   = time_point
//    time_point - time_point = duration
//    duration   + duration   = duration
//
//    
//                                           std::mktime
//                                           <-----------         
//    std::time() -------------> std::time_t -----------> std::tm ------> human readable std::string
//                                 ^         std::gmtime                 (yyyy-mm-dd hh:mm:ss.xxxyyyzzz)
//                                 |         std::localtime
//                                 |
//                                 |
//                                 |
//                                 |                                           
//    clock_gettime() ---------> timespec
//                                 ^  
//                                 | 
//                                 |
//                                 |
//                                 |
//                                 v 
//    std::chrono::              std::chrono::                            std::chrono::
//    system_clock::now() -----> time_point<CLOCK,DURATION> <-----------> duration<REP,RATIO> 
//                                 |^ 
//                                 ||
//                  std::chrono::  || std::chrono::
//                  year_month_day || sys_days = (timepoint with day-resolution)
//                  hh_mm_ss       ||
//                                 v|  
//    manually set time -------> time_pod ------------------------------> human readable std::string
//                                                                       (yyyy-mm-dd hh:mm:ss.xxxyyyzzz)

namespace alg
{
    struct time_pod
    {
        // signed 64 bit is used to be consistent with that in year_month_day and hh_mm_ss
        std::int64_t year;
        std::int64_t month;
        std::int64_t day;
        std::int64_t hours;
        std::int64_t minutes;
        std::int64_t seconds;
        std::int64_t nanoseconds;

        bool operator==(const time_pod& rhs) const
        {
            if (year    != rhs.year)    return false;
            if (month   != rhs.month)   return false;
            if (day     != rhs.day)     return false;
            if (hours   != rhs.hours)   return false;
            if (minutes != rhs.minutes) return false;
            if (seconds != rhs.seconds) return false;
            return nanoseconds == rhs.nanoseconds;
        }
    };

    using time_point = std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>;


    // ************************* //
    // *** Among std::chrono *** //
    // ************************* //
    template<typename CLOCK>
    constexpr auto to_duration(const std::chrono::time_point<CLOCK, std::chrono::nanoseconds>& tp)
    {
        return tp.time_since_epoch();
    }

    template<typename CLOCK, typename DURATION>
    constexpr auto to_time_point(const DURATION& dur)
    {
        return std::chrono::time_point<CLOCK, std::chrono::nanoseconds>{ std::chrono::duration_cast<std::chrono::nanoseconds>(dur) };
    }


    // **************************************** //
    // *** Betweem std::chrono and timespec *** //
    // **************************************** //
    constexpr auto to_duration(const timespec& ts)
    {
        return std::chrono::seconds{ts.tv_sec} + 
               std::chrono::nanoseconds{ts.tv_nsec}; 
    }

    template<typename CLOCK>
    constexpr auto to_time_point(const timespec& ts)
    {
        auto dur = std::chrono::seconds(ts.tv_sec) + 
                   std::chrono::nanoseconds(ts.tv_nsec);

        return std::chrono::time_point<CLOCK, std::chrono::nanoseconds>{dur};
    }
    
    inline std::uint64_t to_nanosec(const timespec& ts)
    {
        std::uint64_t ns = ts.tv_sec * 1e9 + ts.tv_nsec;
        return ns;
    }

    template<typename DURATION>
    constexpr auto to_timespec(const DURATION& dur)
    {
        auto dur_secs = std::chrono::duration_cast<std::chrono::seconds>(dur);
        auto dur_nsecs = std::chrono::duration_cast<std::chrono::nanoseconds>(dur);

        dur_nsecs = dur_nsecs - dur_secs;                     // Please don't do the following, there will be 100ns precision error. 
        return timespec{dur_secs.count(), dur_nsecs.count()}; // dur_nsecs.count() - dur_secs.count() * 1e9
    }

    template<typename CLOCK>
    constexpr auto to_timespec(const std::chrono::time_point<CLOCK, std::chrono::nanoseconds>& tp)
    {
        auto tp_secs = std::chrono::time_point_cast<std::chrono::seconds>(tp); 
        auto dur_nsecs = tp - tp_secs;
        return timespec{tp_secs.time_since_epoch().count(), dur_nsecs.count()};
    }


    // ********************************************* /
    // *** Between std::chrono and alg::time_pod *** //
    // ********************************************* //
    template<typename CLOCK>
    constexpr auto to_time_pod(const std::chrono::time_point<CLOCK, std::chrono::nanoseconds>& tp)
    {
        using nsec = std::chrono::nanoseconds;
        std::chrono::time_point<CLOCK, nsec> tp_day { std::chrono::floor<std::chrono::days>(tp) }; // this is time_point
        std::chrono::year_month_day          ymd    { std::chrono::floor<std::chrono::days>(tp) }; // this is NOT std::chrono::time_point
        std::chrono::hh_mm_ss<nsec>          hms    { tp - tp_day };                               // this is NOT std::chrono::duration

        time_pod pod
        {
            static_cast<std:: int32_t>(ymd.year()),  // why different static cast is used for y/m/d?
            static_cast<std::uint32_t>(ymd.month()), // because y/m/d offer different conversion operator
            static_cast<std::uint32_t>(ymd.day()), 
            hms.hours().count(),
            hms.minutes().count(),
            hms.seconds().count(),
            hms.subseconds().count()
        };
        return pod;
    }

    template<typename CLOCK>
    constexpr auto to_time_point(const time_pod& pod)
    {
        auto date = std::chrono::sys_days( std::chrono::year (pod.year ) /  // remark 1 : sys_days == std::chrono::time_point<std::chrono::system_clock, std::chrono::days>
                                           std::chrono::month(pod.month) /  // remark 2 : use year, not years   
                                           std::chrono::day  (pod.day  ) ); // remark 3 : use operator/() not operator,() 

        std::chrono::time_point<CLOCK, std::chrono::nanoseconds> tp 
                  = date + std::chrono::hours      (pod.hours) 
                         + std::chrono::minutes    (pod.minutes)
                         + std::chrono::seconds    (pod.seconds)
                         + std::chrono::nanoseconds(pod.nanoseconds);
        return tp;
    }
        
    //  Please don't do the following, there will be shift in time & date.
    //  std::chrono::nanoseconds dur = std::chrono::years          { pod.year         } 
    //                               + std::chrono::months         { pod.month        } 
    //                               + std::chrono::days           { pod.day          }
    //                               + std::chrono::hours          { pod.hours        } 
    //                               + std::chrono::minutes        { pod.minutes      }
    //                               + std::chrono::seconds        { pod.seconds      }
    //                               + std::chrono::nanoseconds    { pod.nanoseconds  };
    

    // ***************** //
    // *** To string *** //
    // ***************** //
    inline std::string to_string(const timespec& ts)
    {
        std::stringstream ss;
        std::time_t tt = ts.tv_sec;             // just an integer of seconds
        std::tm     tm = *std::localtime(&tt);  // convert integer to time with calendar

        ss << std::put_time(&tm, "%F %T"); 
        ss << "." << std::setfill('0') << std::setw(9) << ts.tv_nsec;
        return ss.str();
    }

    template<typename CLOCK>
    std::string to_string(const std::chrono::time_point<CLOCK, std::chrono::nanoseconds>& tp, bool convert_to_timespec)
    {
        if (convert_to_timespec)
        {
            return to_string(to_timespec(tp));
        }
        else
        {
            return to_string(to_time_pod(tp));
        }
    }

    inline std::string to_string(const time_pod& pod)
    {
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(4) << pod.year    << "-"
           << std::setfill('0') << std::setw(2) << pod.month   << "-"
           << std::setfill('0') << std::setw(2) << pod.day     << " "
           << std::setfill('0') << std::setw(2) << pod.hours   << ":"
           << std::setfill('0') << std::setw(2) << pod.minutes << ":"
           << std::setfill('0') << std::setw(2) << pod.seconds << "."
           << std::setfill('0') << std::setw(9) << pod.nanoseconds;
        return ss.str();
    }
}

namespace alg
{
    class timer
    {
    public:
        timer() : ts0(std::chrono::system_clock::now()),
                  ts1(std::chrono::system_clock::now())
        {
        }

        inline void click()
        {
            ts0 = ts1;
            ts1 = std::chrono::system_clock::now();
        }

        inline std::chrono::nanoseconds time_elapsed() const
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(ts1-ts0);
        }
    
        inline std::uint64_t time_elapsed_in_nsec() const
        {
            return std::chrono::duration_cast<std::chrono::nanoseconds>(ts1-ts0).count();
        }

    private:
        std::chrono::time_point<std::chrono::system_clock> ts0;
        std::chrono::time_point<std::chrono::system_clock> ts1;
    };
}
