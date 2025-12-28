#include<iostream>
#include<string>
#include<stack>
#include<vector>
#include<unordered_map>


namespace apt
{
    class brainfuck
    { 
    public:
        //  num of data cell = data_num
        // size of data cell = 1 byte
        explicit brainfuck(std::uint32_t data_num) : m_data_num(data_num)
        {
        }

    public:
        bool compile(const std::string& code) 
        {
            m_code = code;
            m_brackets.clear();

            // match brackets
            std::stack<std::uint32_t> stack;
            for(std::uint32_t n=0; n!=code.size(); ++n)
            {
                if (code[n] == '[')
                {
                    stack.push(n);

                }
                else if (code[n] == ']')
                {
                    if (stack.empty()) return false; // excessive ']'

                    m_brackets[stack.top()] = n;
                    m_brackets[n] = stack.top();
                    stack.pop();
                }
            }
            if (!stack.empty()) return false; // excessive '['

            // compilation OK
            return true; 
        }
    
        std::string run(const std::string& input)
        {
            // Initialize data space 
            std::vector<std::uint8_t> data(m_data_num, 0); 
            std::uint32_t dp = 0; // dp = data pointer
            std::uint32_t ip = 0; // ip = input pointer

            // Main 
            std::string output;
            for(std::uint32_t n=0; n!=m_code.size(); ++n) // n = program counter
            {
                switch(m_code[n])
                {
                    case '>':
                        if  (dp == m_data_num-1) 
                             dp = 0; // overflow
                        else dp = dp+1;
                        break;

                    case '<':
                        if  (dp == 0) 
                             dp = m_data_num-1; // underflow
                        else dp = dp-1;
                        break;

                    case '+':
                        ++data[dp];
                        break;

                    case '-':
                        --data[dp];
                        break;

                    case ',':
                        if (ip < input.size()) 
                        {
                            data[dp] = static_cast<std::uint8_t>(input[ip]); 
                            ++ip;
                        }
                        else
                        {
                            data[dp] = 0; // EOF
                        }
                        break;

                    case '.':
                        output.push_back(static_cast<char>(data[dp]));
                        break;

                    case '[':
                        if (data[dp] == 0)
                        {
                            auto   iter = m_brackets.find(n); // BUG : find(n), NOT find(dp)
                            assert(iter!= m_brackets.end());

                            n = iter->second; // ++n in for-loop will take program counter to the instruction after ]
                        }
                        break;

                    case ']':
                        if (data[dp] != 0)
                        {
                            auto   iter = m_brackets.find(n); // BUG : find(n), NOT find(dp)
                            assert(iter!= m_brackets.end());

                            n = iter->second; // ++n in for-loop will take program counter to the instruction after [
                        }
                        break;

                    // ************* //
                    // *** Debug *** //
                    // ************* //
                    case '*': 
                        std::cout << "\nbreakpoint " << n << ", dp = " << dp << ", data = ";
                        for(const auto& x:data) 
                        {
                            std::cout << static_cast<std::uint32_t>(static_cast<unsigned char>(x)) << ", ";
                        }
                        break;

                    default: 
                        break;
                }
            }
            return output;
        }

    private:
        std::uint32_t m_data_num;
        std::string   m_code;
        std::unordered_map<std::uint32_t, std::uint32_t> m_brackets;
    };  
}
