#include<iostream>
#include<cstdint>
#include<string>
#include<vector>
#include<array>
#include<tuple>
#include<unordered_map>
#include<algorithm>

namespace interview {
enum class State : std::uint32_t
{
    Open,
    Closed,
    Preopen_Auction,
    Closing_Auction,
    Intraday_Auction,
    Intraday_Close,
    NumOfState, // This is for counting the number of states above.
    Unknown
};

std::ostream& operator<<(std::ostream& os, const State& state)
{
    if      (state == State::Open)              os << "Open";
    else if (state == State::Closed)            os << "Close";
    else if (state == State::Preopen_Auction)   os << "Preopen_Auction";
    else if (state == State::Closing_Auction)   os << "Closing_Auction";
    else if (state == State::Intraday_Auction)  os << "Intraday_Auction";
    else if (state == State::Intraday_Close)    os << "Intraday_Close";
    else                                        os << "Unknown";
    return os;
}

std::uint32_t to_msec(std::uint32_t hours, std::uint32_t minutes)
{
    return (hours * 60  + minutes) * 60 * 1000;
}

class TimeTable
{
public:
    TimeTable()
    {
        table[static_cast<std::uint32_t>(State::Open)]            .push_back(std::make_tuple(to_msec( 9, 0), to_msec(12, 0), State::Preopen_Auction));
        table[static_cast<std::uint32_t>(State::Open)]            .push_back(std::make_tuple(to_msec(14, 0), to_msec(15,30), State::Intraday_Auction));
        table[static_cast<std::uint32_t>(State::Closed)]          .push_back(std::make_tuple(to_msec( 0, 0), to_msec( 8,30), State::Closed));
        table[static_cast<std::uint32_t>(State::Closed)]          .push_back(std::make_tuple(to_msec(16, 0), to_msec(24, 0), State::Closing_Auction));
        table[static_cast<std::uint32_t>(State::Preopen_Auction)] .push_back(std::make_tuple(to_msec( 8,30), to_msec( 9, 0), State::Closed));
        table[static_cast<std::uint32_t>(State::Closing_Auction)] .push_back(std::make_tuple(to_msec(15,30), to_msec(16, 0), State::Open));
        table[static_cast<std::uint32_t>(State::Intraday_Auction)].push_back(std::make_tuple(to_msec(12, 0), to_msec(12,30), State::Open));
        table[static_cast<std::uint32_t>(State::Intraday_Auction)].push_back(std::make_tuple(to_msec(13,30), to_msec(14, 0), State::Intraday_Close));
        table[static_cast<std::uint32_t>(State::Intraday_Close)]  .push_back(std::make_tuple(to_msec(12,30), to_msec(13,30), State::Intraday_Auction));
    }
    
public:
    bool verify(const State& state, std::uint32_t timestamp, const State& prev_state) const
    {
        if (state >= State::NumOfState) return false;
        
        const auto& vec = table[static_cast<std::uint32_t>(state)]; 
        for(const auto& x:vec)
        {
//          std::cout << "\n[DEBUG] " << timestamp 
//                             << " " << std::get<0>(x) 
//                             << " " << std::get<1>(x) 
//                             << " " << state 
//                             << " " << prev_state 
//                             << " " << std::get<2>(x);
            if (timestamp  >= std::get<0>(x) && 
                timestamp  <= std::get<1>(x) && 
                prev_state == std::get<2>(x)) return true;
        }
        return false;
    }

private:
    // tuple.get<0> = starting time
    // tuple.get<1> = ending time
    // tuple.get<2> = previous state
    std::array<std::vector<std::tuple<std::uint32_t, std::uint32_t, State>>, static_cast<std::uint32_t>(State::NumOfState)> table;
};

State to_state(const std::string& str)
{
    auto s = str;
    std::for_each(s.begin(), s.end(), [](char& c)
    {
        c = tolower(c);
    });
    
    if      (str == "Open")               return State::Open;
    else if (str == "Closed")             return State::Closed;
    else if (str == "Preopen_Auction")    return State::Preopen_Auction;
    else if (str == "Closing_Auction")    return State::Closing_Auction;
    else if (str == "Intraday_Auction")   return State::Intraday_Auction;
    else if (str == "Intraday_Close")     return State::Intraday_Close;
    else                                  return State::Unknown;
}

struct StateTick
{
    std::uint32_t timestamp;
    std::string stock;
    State state;
};

class Parser
{
public:
    explicit Parser(std::istream& is) : _is(is)
    {
    }
    
    bool eof() const
    {
        return _is.eof();
    }

    // Todo : replace the output by std::optional<StateTick>
    std::pair<bool, StateTick> next()
    {
        StateTick tick;
        if (_is.eof()) return std::make_pair(false, tick);
        
        char buf[1024];
        _is.getline(buf, 1024);
        std::string str(buf);
            
        auto n0 = str.find(",");
        if (n0==std::string::npos) return std::make_pair(false, tick);
        tick.timestamp = std::stoi(str.substr(0,n0));   
        
        auto n1 = str.find(",", n0+1);
        if (n1==std::string::npos) return std::make_pair(false, tick);
        if (std::stoi(str.substr(n0+1,n1-n0-1)) != 2) return std::make_pair(false, tick);
            
        auto n2 = str.find(",", n1+1);
        if (n2==std::string::npos) return std::make_pair(false, tick);
        tick.stock = str.substr(n1+1,n2-n1-1);
        tick.state = to_state(str.substr(n2+1));
        return std::make_pair(true, tick);
    }

private:
    std::istream& _is;
};

class StateManager
{
public:
    StateManager() : time_table{}
    {
    }

   ~StateManager() = default;
    StateManager(const StateManager&) = delete;
    StateManager(StateManager&&) = delete;
    StateManager& operator=(const StateManager&) = delete;
    StateManager& operator=(StateManager&&) = delete;

public:
    void new_tick(const StateTick& tick)
    {
        auto iter = states.find(tick.stock);
        if (iter == states.end())
        {
            bool flag;
            std::tie(iter, flag) = states.insert(std::make_pair(tick.stock, State::Closed));
        }
       
        if (time_table.verify(tick.state, tick.timestamp, iter->second))
        {
            std::cout << "\n"  << tick.timestamp
                      << ",2," << tick.stock
                      << ","   << iter->second  
                      << ","   << tick.state
                      << ",Success";
                      
            iter->second = tick.state;
        }
        else 
        {
            std::cout << "\n"  << tick.timestamp
                      << ",2," << tick.stock
                      << ","   << iter->second  
                      << ","   << tick.state
                      << ",Failure";
        }
    }

private: 
    TimeTable time_table;
    std::unordered_map<std::string, State> states;
};

void test_epoch_state() 
{
    Parser parser(std::cin);
    StateManager manager;
    while(!parser.eof())
    {
        auto tick = parser.next();
        if (tick.first)
        {
            manager.new_tick(tick.second);   
        }
    }    
}
}
