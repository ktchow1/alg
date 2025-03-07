#pragma once
#include<iostream>
#include<cstdint>
#include<cmath>
#include<vector>
#include<list>
#include<algorithm>

// from alg
#include<matrix.h>
#include<utility.h>


// ***************************** //
// *** Longest common subseq *** //
// ***************************** //
// 1. Various matrix representation
// matrix in "dp_matrix_and_graph.h"  = subproblem size  vs state
// matrix in "dp_matrix_only.h"       = subproblem size  vs subproblem size
// matrix in LCS    and edit dist     = subproblem str0  vs subproblem str1
// matrix in bool() and coin game     = subproblem begin vs subproblem end of the same str
//
// 2. Various matrix scanning
// matrix in LCS    and edit dist : horizontal scan, from top to bottom
// matric in bool() and coin game :   diagonal scan, from middle to upper-right
//
namespace alg
{
    std::uint32_t longest_common_subseq_recursive(const std::string& str0, const std::string& str1)
    {
        if (str0.size()==0 || str1.size()==0) return 0;

        std::string trim_str0(str0); 
        std::string trim_str1(str1);
        trim_str0.erase(trim_str0.begin());
        trim_str1.erase(trim_str1.begin());

        if (str0[0] == str1[0]) 
        {
            return longest_common_subseq_recursive(trim_str0, trim_str1) + 1;
        }
        else
        {
            return std::max(longest_common_subseq_recursive(trim_str0, str1), 
                            longest_common_subseq_recursive(str0, trim_str1));
        }
    }

    std::uint32_t longest_common_subseq_iterative(const std::string& str0, const std::string& str1)
    {
        alg::matrix<std::uint32_t> mat(str1.size(), str0.size(), 0);
        mat(0,0) = (str0[0]==str1[0]);

        for(std::uint32_t m=1; m!=str0.size(); ++m)
        {
            if (str0[m]==str1[0]) mat(0,m) = 1;
            else                  mat(0,m) = mat(0,m-1);
        }

        for(std::uint32_t n=1; n!=str1.size(); ++n)
        {
            if (str0[0]==str1[n]) mat(n,0) = 1;
            else                  mat(n,0) = mat(n-1,0);
        }

        for(std::uint32_t n=1; n!=str1.size(); ++n)
        {
            for(std::uint32_t m=1; m!=str0.size(); ++m)
            {
                if (str0[m]==str1[n])
                {
                    mat(n,m) = mat(n-1,m-1) + 1;
                }
                else
                {
                    mat(n,m) = std::max(mat(n-1,m), mat(n,m-1));
                }
            }
        }
        return mat(mat.size_y()-1, mat.size_x()-1);
    }
}


// ********************* //
// *** Edit distance *** //
// ********************* //
// Recursion of edit distance is slower than recursion of LCS because of 1-to-3 in recursion equation.
//
namespace alg
{
    std::uint32_t edit_distance_recursive(const std::string& str0, const std::string& str1)
    {
        if (str0.size()==0 && str1.size()==0) return 0;
        if (str0.size()!=0 && str1.size()==0) return str0.size();
        if (str0.size()==0 && str1.size()!=0) return str1.size();

        std::string trim_str0(str0); 
        std::string trim_str1(str1);
        trim_str0.erase(trim_str0.begin());
        trim_str1.erase(trim_str1.begin());

        if (str0[0] == str1[0]) 
        {
            return edit_distance_recursive(trim_str0, trim_str1);
        }
        else
        {
            return std::min(edit_distance_recursive(trim_str0,      str1) + 1, // trim_str0 becomes str1, plus DEL
                   std::min(edit_distance_recursive(     str0, trim_str1) + 1, // str0 becomes trim_str1, plus ADD
                            edit_distance_recursive(trim_str0, trim_str1) + 1));
        }
    }

    std::uint32_t edit_distance_iterative(const std::string& str0, const std::string& str1)
    {
        alg::matrix<std::uint32_t> mat(str1.size(), str0.size(), alg::inf<std::uint32_t>);
        if (str0[0]==str1[0]) mat(0,0) = 0;
        else                  mat(0,0) = 1;

        for(std::uint32_t m=1; m!=str0.size(); ++m)
        {
            if (str0[m]==str1[0]) mat(0,m) = m;
            else                  mat(0,m) = mat(0,m-1) + 1;
        }

        for(std::uint32_t n=1; n!=str1.size(); ++n)
        {
            if (str0[0]==str1[n]) mat(n,0) = n;
            else                  mat(n,0) = mat(n-1,0) + 1;
        }

        for(std::uint32_t n=1; n!=str1.size(); ++n)
        {
            for(std::uint32_t m=1; m!=str0.size(); ++m)
            {
                if (str0[m]==str1[n])
                {
                    mat(n,m) = mat(n-1,m-1);
                }
                else
                {
                    mat(n,m) = std::min(mat(n-1,m) + 1, 
                               std::min(mat(n,m-1) + 1,
                                        mat(n-1,m-1) + 1));
                }
            }
        }
        return mat(mat.size_y()-1, mat.size_x()-1);
    }
}


// *************************** //
// *** Boolean parenthesis *** //
// *************************** //
// Expression = input[0].first input[0].second 
//              input[1].first input[1].second
//              ...
//              input[N-2].first input[N-2].second
//              input[N-1].first                   <--- The last logic is don't care.
//
// Must fulfill :
// f(0,N-1) + g(0,N-1) = tree_permutation(N-1).size()
// 
namespace alg
{
    std::uint32_t bool_parenthesis_iterative(const std::vector<bool_symbol>& input) 
    {
        std::uint32_t N = input.size();
        alg::matrix<std::uint32_t> f(N,N,0); 
        alg::matrix<std::uint32_t> g(N,N,0);

        // Main diagonal
        for(std::uint32_t n=0; n!=N; ++n)
        {
            f(n,n) =  input[n].m_value;
            g(n,n) = !input[n].m_value;
        }

        // Iterate from main diagonal to UR
        for(std::uint32_t diag=1; diag!=N; ++diag) // shift of diagonal = diag
        {
            for(std::uint32_t n=0; n!=N-diag; ++n) // length of diagonal = N-diag
            {
                std::uint32_t m = n+diag;

                // hori_dist between diagonal & (n,m) = m-n
                // vert_dist between diagonal & (n,m) = m-n
                for(std::uint32_t k=0; k!=m-n ; ++k)
                {
                //  if (input[  k].m_logic == logic::OR) // BUG
                    if (input[n+k].m_logic == logic::OR)
                    {
                        f(n,m) += f(n,n+k) * f(n+k+1,m) +
                                  f(n,n+k) * g(n+k+1,m) +
                                  g(n,n+k) * f(n+k+1,m);
                        g(n,m) += g(n,n+k) * g(n+k+1,m); 
                    }
                    else
                    {
                        f(n,m) += f(n,n+k) * f(n+k+1,m);
                        g(n,m) += g(n,n+k) * g(n+k+1,m) + 
                                  g(n,n+k) * f(n+k+1,m) +
                                  f(n,n+k) * g(n+k+1,m);
                    }
                } 
            }
        }
        return f(0,N-1); 
    }

    // ************************* //
    // *** Exhaustive search *** //
    // ************************* //
    // What is tree_permutation?
    // * tree_permutation is a node-sequence, when branched in order, defines a tree topology, with leaves come first and root at last (post-ordered)
    // * tree_permutation is different from std::permutation, multiple std::permutation may refer to the same tree_permutation
    // * tree_permutation when applied to bool expression, should be evaluated from leaves to root 
    //
    auto tree_permutations_impl(auto iter_begin, auto iter_end)
    {
        std::vector<std::vector<std::uint32_t>> ans;
        if (iter_begin == iter_end) 
        {
            ans.push_back(std::vector<std::uint32_t>{});
            return ans;
        }

        for(auto iter=iter_begin; iter!=iter_end; ++iter)
        {
            auto tmp0 = tree_permutations_impl(iter_begin, iter);
            auto tmp1 = tree_permutations_impl(iter+1, iter_end);
            for(const auto x0:tmp0)
            {
                for(const auto x1:tmp1)
                {
                    std::vector<std::uint32_t> perm;
                    perm.insert(perm.end(), x0.begin(), x0.end());
                    perm.insert(perm.end(), x1.begin(), x1.end());
                    perm.push_back(*iter); // BUG : parent comes last, as evaluation starts from leaves
                    ans.push_back(perm);
                }
            }
        }
        return ans;
    }
    
    auto tree_permutations(std::uint32_t num_nodes) // num of nodes in tree = num of logic operators
    {
        std::vector<std::uint32_t> indices;
        for(std::uint32_t n=0; n!=num_nodes; ++n)
        {
            indices.push_back(n);            
        }
        return tree_permutations_impl(indices.begin(), indices.end());
    }

    bool evaluate(const std::vector<bool_symbol>& input, const std::vector<std::uint32_t>& perm)
    {
        // ************************************************* //
        // *** Convert input into list and list-iterator *** //
        // ************************************************* //
        std::list<bool_symbol> list;
        std::vector<std::list<bool_symbol>::iterator> list_iters;

        for(const auto& x:input)
        {
            list.push_back(x);
        }
        for(const auto& n:perm)
        {
            auto list_iter = list.begin();
            std::advance(list_iter, n);
            list_iters.push_back(list_iter);
        }

        // ****************** //
        // *** Evaluation *** //
        // ****************** //
        for(const auto& this_iter : list_iters)
        {
            auto next_iter = this_iter;
            ++next_iter;

            if  (this_iter->m_logic == logic::OR)
                 next_iter->m_value = this_iter->m_value || next_iter->m_value;
            else next_iter->m_value = this_iter->m_value && next_iter->m_value;
            list.erase(this_iter);
        }

        // ************** //
        // *** Result *** //
        // ************** //
        if (list.size() != 1)
        {
            throw std::runtime_error("boolean parenthesis evaluation - final list size not equal to 1");
        }
        return list.front().m_value;
    }

    std::uint32_t bool_parenthesis_exhaustive(const std::vector<bool_symbol>& input) 
    {
        auto tree_perms = tree_permutations(input.size()-1);

        std::uint32_t ans = 0;
        for(const auto& tree_perm : tree_perms)
        {
            auto flag = evaluate(input, tree_perm);
            if (flag) ++ans;
        } 
        return ans;
    }
}


// ***************** //
// *** Coin game *** //
// ***************** //
namespace alg
{
    std::uint32_t coin_game_recursive(const std::vector<std::uint32_t>& coins) 
    {
        if (coins.size() == 1) return coins[0];
        if (coins.size() == 2) return std::max(coins[0], coins[1]);
        std::uint32_t N = coins.size();

        std::vector<std::uint32_t> trim_coins_20(coins.begin()+2, coins.end()  );
        std::vector<std::uint32_t> trim_coins_11(coins.begin()+1, coins.end()-1);
        std::vector<std::uint32_t> trim_coins_02(coins.begin()  , coins.end()-2);
        
        return std::max
        (
             coins[0]   + std::min(coin_game_recursive(trim_coins_20),
                                   coin_game_recursive(trim_coins_11)),
             coins[N-1] + std::min(coin_game_recursive(trim_coins_11),
                                   coin_game_recursive(trim_coins_02))
        );
    }

    std::uint32_t coin_game_iterative(const std::vector<std::uint32_t>& coins) 
    {
        std::uint32_t N = coins.size();
        alg::matrix<std::uint32_t> mat(N,N,0); 

        // Main diagonal
        for(std::uint32_t n=0; n!=N; ++n)
        {
            mat(n,n) =  coins[n];
        }

        // Adjacent diagonal
        for(std::uint32_t n=0; n!=N-1; ++n)
        {
            mat(n,n+1) =  std::max(coins[n], coins[n+1]);
        }

        // Iterate to UR
        for(std::uint32_t diag=2; diag!=N; ++diag) 
        {
            for(std::uint32_t n=0; n!=N-diag; ++n)
            {
                std::uint32_t m = n+diag;
                mat(n,m) = std::max
                (
                     coins[n] + std::min(mat(n+2,m), mat(n+1,m-1)),
                     coins[m] + std::min(mat(n+1,m-1), mat(n,m-2))
                );
            }
        }
        return mat(0,N-1); 
    }
}


// *********************************** //
// *** Piecewise linear regression *** //
// *********************************** //
// Regression of : x =    0,     1,     2,  ...
//                 y = ys[0], ys[1], ys[2], ...
//
namespace alg
{
    double sum_of_error_square(const std::vector<double>::const_iterator& begin, 
                               const std::vector<double>::const_iterator& end)
    {
        double Dx  = (double)std::distance(begin, end-1);
        double Dy  = *(end-1) - *begin;
        double ans = 0;

        for(auto iter=begin; iter!=end; ++iter)
        {
            double dx  = (double)std::distance(begin, iter);
            double dy  = *iter - *begin;
            double err = dy - Dy * (dx / Dx); 
            ans += err * err;
        }
        return ans;
    }
    
    double piecewise_linear_equation(const std::vector<double>& ys, std::uint32_t num_lines) 
    {
        std::uint32_t N = ys.size();
        std::uint32_t M = num_lines;
        if (N <= M+1) return 0; // perfect fit

        // For subproblem mat(n,m)
        // num of data  = n+1, hence n = index of last data point   in subproblem
        // num of lines = m+1, hence m = index of last line segment in subproblem
        alg::matrix<double> mat(N,M,0); 

        // Diagonal
        for(std::uint32_t m=0; m!=M; ++m)
        {
            mat(m+1,m) = 0;
        }

        // 1st column
        for(std::uint32_t n=2; n!=N; ++n)
        {
            mat(n,0) = sum_of_error_square(ys.begin(), ys.begin()+n+1);
        }

        // Iterate to UR
        for(std::uint32_t m=1; m!=M; ++m) 
        {
            for(std::uint32_t n=m+2; n!=N; ++n) // n = index of last data point in subproblem mat(n,m)
            {
                double min_err = std::numeric_limits<double>::max();
                for(std::uint32_t k=m; k!=n; ++k)
                {
                    double err = mat(k,m-1) + sum_of_error_square(ys.begin()+k, ys.begin()+n+1);
                    if (min_err > err)
                        min_err = err;
                }
                mat(n,m) = min_err;
            }
        }
        return mat(N-1,M-1); 
    }
}

