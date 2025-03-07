#include <iostream>
#include <limits>
#include <vector>
#include <algorithm>

namespace interview {
// Expected answer : 863 + 383 + 343 + 959 + 767 = 3315;
std::vector<std::vector<int>> A = 
{
    {7 ,53,183,439,863},
    {497,383,563,79,973},
    {287,63,343,169,583},
    {627,343,773,959,943},
    {767,473,103,699,303}
};

std::vector<std::vector<int>> B = 
{
    {7, 53, 183, 439, 863, 497, 383, 563, 79, 973, 287, 63, 343, 169, 583},
    {627, 343, 773, 959, 943, 767, 473, 103, 699, 303, 957, 703, 583, 639, 913},
    {447, 283, 463, 29, 23, 487, 463, 993, 119, 883, 327, 493, 423, 159, 743},
    {217, 623, 3, 399, 853, 407, 103, 983, 89, 463, 290, 516, 212, 462, 350},
    {960, 376, 682, 962, 300, 780, 486, 502, 912, 800, 250, 346, 172, 812, 350},
    {870, 456, 192, 162, 593, 473, 915, 45, 989, 873, 823, 965, 425, 329, 803},
    {973, 965, 905, 919, 133, 673, 665, 235, 509, 613, 673, 815, 165, 992, 326},
    {322, 148, 972, 962, 286, 255, 941, 541, 265, 323, 925, 281, 601, 95, 973},
    {445, 721, 11, 525, 473, 65, 511, 164, 138, 672, 18, 428, 154, 448, 848},
    {414, 456, 310, 312, 798, 104, 566, 520, 302, 248, 694, 976, 430, 392, 198},
    {184, 829, 373, 181, 631, 101, 969, 613, 840, 740, 778, 458, 284, 760, 390},
    {821, 461, 843, 513, 17, 901, 711, 993, 293, 157, 274, 94, 192, 156, 574},
    {34, 124, 4, 878, 450, 476, 712, 914, 838, 669, 875, 299, 823, 329, 699},
    {815, 559, 813, 459, 522, 788, 168, 586, 966, 232, 308, 833, 251, 631, 107},
    {813, 883, 451, 509, 615, 77, 281, 613, 459, 205, 380, 274, 302, 35, 805}
};


// Recursive implementation 
// * given N*N matrix
//   complexity f(N) = O(N*f(N-1)) = O(N!)
// * with optimization
//   it may speed up a little bit, but still very slow
  
int upper_bound_of_rook_sum(const std::vector<std::vector<int>>& mat)
{
    int bound = 0;
    for(const auto& row:mat)
    {
        bound += *std::max_element(row.begin(), row.end());
    }
    return bound;
}

int max_rook_sum_recursive(const std::vector<std::vector<int>>& mat, bool with_optimization)
{   
    auto size_y = mat.size();
    if (size_y == 0) return 0;

    auto size_x = mat[0].size();
    for(const auto& row : mat)
    {
        if (row.size() != size_x) return 0;
    }

    // Boundary case
    if (mat.size()==1) return mat[0][0];

    // General case
    int ans = std::numeric_limits<int>::min();

    for(int y=0; y!=size_y; ++y)
    {
        int tmp = mat[y][size_x-1];

        std::vector<std::vector<int>> sub_mat;
        for(int yy=0; yy!=size_y; ++yy)
        {
            if (yy!=y)
            {
                auto row = mat[yy]; // copy is needed, mat is immutable
                row.pop_back();
                sub_mat.push_back(std::move(row));
            }
        }

        if (with_optimization)
        {
            int bound = upper_bound_of_rook_sum(sub_mat);
            if (ans >= tmp + bound) continue; 
        }
        int sub_max = max_rook_sum_recursive(sub_mat, with_optimization);
        if (ans < tmp + sub_max) ans = tmp + sub_max;
    }
    return ans;
}

void test_citadel_max_rook_sum() 
{
    std::cout << "\nRook sum is " << max_rook_sum_recursive(A, false);
    std::cout << "\nRook sum is " << max_rook_sum_recursive(A, true);
//  std::cout << "\nRook sum is " << max_rook_sum_recursive(B, false);
//  std::cout << "\nRook sum is " << max_rook_sum_recursive(B, true);
}
}
