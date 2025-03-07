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
                if (iter->second < n-sub)
                {
                    sub = sub + 1;
                    ans = std::max(ans, sub);
                    iter->second = n;
                }
                else
                {
                    sub = n - iter->second;
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


    // ********************************************************************************************************* //
    // Even palindrome can be converted into odd palindrome by
    // * inserting char '_' in front of input string
    // * inserting char '_' at end of input string
    // * inserting char '_' in between any characters in input string
    // Now only consider odd palindrome-in-palindrome. 
    //
    // Given parent-palindrome-radius = 5, there are 3 cases :                       0123456789012
    // case A1. when LHS sub-palindrome is completely inside parent-palindrome, like M*ABA.T.ABA*N
    // case A2. when LHS sub-palindrome   barely lies inside parent-palindrome, like M.ABA.T.ABA.T 
    // case A3. when LHS sub-palindrome     extrudes outside parent-palindrome, like T.ABA.T.ABA.N
    //
    // we can copy LHS-palindrome-radius to RHS-palindrome-radius for position [8-11] : 
    //
    // A1. M*ABA.T.ABA*N     when 9 + rad[3] < 6 + rad[6] 
    //      001005~~x~~      x = rad[9]   = rad[3]   = 1        no further growing
    //
    // A2. M.ABA.T.ABA.T     when 9 + rad[3] = 6 + rad[6] 
    //      002005~~y~~      y = rad[9]   = rad[3]   = 2      with further growing and become parent-palindrome
    //
    // A3. T.ABA.T.ABA.N     when 9 + rad[3] > 6 + rad[6] 
    //      003005~~z~~      z = rad[9]   = 11 - 9   = 2        no further growing, N wont be T, or rad[6] = 6
    //                       ~ = rad[6+n] = rad[6-n] = 0        no further growing, where n = 1,2,4,5
    // ********************************************************************************************************* //
    //
    std::uint32_t longest_odd_palindrome_substr(const std::string& str) 
    {
        std::vector<uint32_t> radii(str.size(), 0);
        std::uint32_t parent_centre = 0;
        std::uint32_t parent_radius = 0;

        for(std::uint32_t n=0; n!=str.size(); ++n) 
        {
            std::uint32_t r;

            // ************************************************ //
            // *** case A : str[n] inside parent-palindrome *** //
            // ************************************************ //
            if (n <= parent_centre + parent_radius) 
            {
                std::uint32_t n_image = parent_centre - (n - parent_centre); // ensure n > parent_centre

                // case A1  
                if (n + radii[n_image] < parent_centre + parent_radius)
                {
                    radii[n] = radii[n_image];
                    continue;
                }
                // case A2 
                else if (n + radii[n_image] == parent_centre + parent_radius)
                {
                    radii[n] = radii[n_image];
                    r = radii[n];
                }
                // case A3 
                else
                {
                    radii[n] = parent_centre + parent_radius - n; // i.e. RHS sub-palindrome cannot exceed parent-palindrome
                    continue;
                }
            }
            // ************************************************* //
            // *** case B : str[n] outside parent-palindrome *** //
            // ************************************************* //
            else 
            {
                r = 0; 
            }

            // ************************************************ //
            // *** Growing of radius for case A2 and case B *** //
            // ************************************************ //
            ++r;
            for(; n>=r && n+r<str.size(); ++r)
            {
                if (str[n-r] == str[n+r])
                {
                    radii[n] = r;
                }
                else break;
            }   

            parent_centre = n;
            parent_radius = radii[n];
        }
       
        auto iter = std::max_element(radii.begin(), radii.end()); 
        return 2 * (*iter) + 1;
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



