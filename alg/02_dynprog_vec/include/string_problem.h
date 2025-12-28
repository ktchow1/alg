#include<iostream>
#include<string>
#include<array>
#include<vector>
#include<unordered_map>
#include<algorithm>


// ******************** //
// *** Maximization *** //
// ******************** //
namespace alg
{
    std::uint32_t longest_non_duplicated_substr(const std::string& str)
    {
        if (str.size()==0) return 0;
        std::unordered_map<char, std::uint32_t> index; 

        std::int32_t sub = 0;
        std::int32_t ans = sub;
        for(std::uint32_t n=0; n!=str.size(); ++n)
        {
            if (auto iter=index.find(str[n]); iter!=index.end())
            {
                if (iter->second >= n-sub) // BUG : Dont miss this
                {
                    sub = n - iter->second;
                    ans = std::max(ans, sub);
                    iter->second = n;
                }
                else
                {
                    sub = sub + 1;
                    ans = std::max(ans, sub);
                    iter->second = n;
                }
            }
            else
            {
                sub = sub + 1;
                ans = std::max(ans, sub);
                index[str[n]] = n;
            }
        }
        return ans;
    }

    // ************************************************************** //
    // Even palindrome can be converted into odd palindrome by
    // * inserting char '_' in front of input string
    // * inserting char '_' at end of input string
    // * inserting char '_' in between any characters in input string
    //
    // Now only consider odd palindrome-in-palindrome. 
    // ************************************************************** //
    // Remark 1 : Rollback of "n" is necessary
    //            for continuing to detect RHS sub-palindrome 
    //            after failing to grow parent-palindrome, like 
    //
    //            M***ABA...T...ABA***N
    // ************************************************************** //
    std::uint32_t longest_odd_palindrome_substr(const std::string& str) 
    {
        std::uint32_t sub = 0;
        std::uint32_t ans = 0;

        for(std::uint32_t n=0; n!=str.size(); ++n)
        {
            if (n >= 2*(sub+1))
            {
                if (str[n] == str[n-2*(sub+1)])
                {
                    ++sub;
                }
                else 
                {
                    n -= sub; // Remark 1 
                    sub = 0;
                }
            }
            else
            {
                n -= sub; // Remark 1
                sub = 0;
            }

            ans = std::max(ans, sub);
        }
        return 2*ans+1;
    }
}



// *********************************************** //
// *** Maximization - benchmark implementation *** //
// *********************************************** //
namespace alg
{
    std::uint32_t longest_non_duplicated_substr_bmk(const std::string& str)
    {
        std::uint32_t ans = 0;
        for(std::uint32_t n=0; n!=str.size(); ++n) // consider substr starting with n, and reaches as far as possible 
        {
            std::array<char,26> hist;
            hist.fill(false);

            for(std::uint32_t m=n; m!=str.size(); ++m) 
            {
                if (!hist[str[m]-'a']) 
                {
                    hist[str[m]-'a'] = true;
                    ans = std::max(ans,m-n+1);
                }
                else break;
            }
        }
        return ans;
    }
    
    std::uint32_t longest_odd_palindrome_substr_bmk(const std::string& str) 
    {
        std::uint32_t ans = 0;                     // radius of palindrome
        for(std::uint32_t n=0; n!=str.size(); ++n) // centre of palindrome
        {
            // growing from centre
            for(std::uint32_t r=1; n>=r && n+r<str.size(); ++r)
            {
                if (str[n-r] == str[n+r])
                {
                    ans = std::max(ans, r);
                }
                else break;
            }
        }
        return 2 * ans + 1; // length of palindrome
    }
}



