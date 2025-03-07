#pragma once
#include<vector>
#include<functional>
#include<optional>

namespace alg
{
    std::optional<double> bisection(const std::function<double(double)>& fct, double x0, double x1)
    {
        // 1. check edge case (or init y in here)
        double y0 = fct(x0);
        double y1 = fct(x1);

        // 2. check solution existence
        if (y0 * y1 > 0) return std::nullopt;

        // 3. check stop condition
        while(fabs(x0 - x1) > 0.000000001)
        {
            // 4. mid point
            double xm = (x0 + x1) / 2;
            double ym = fct(xm);
            
            // 5. bisection
            if (y0 * ym > 0)
            {
                x0 = xm;
                y0 = ym;
            }
            else 
            {
                x1 = xm;
                y1 = ym;
            }
        }            

        // 6. answer
        return std::make_optional((x0 + x1) / 2);
    }


    // *********************************************************** //
    // Find target in non-monotonic increasing (or decreasing) vec
    // *********************************************************** //
    std::optional<std::uint32_t> bisection(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        // 1. check edge case 
        if (vec.size() == 0) return std::nullopt; 

        // 2. check solution existence
        if (vec.front() < target && vec.back() < target) return std::nullopt;
        if (vec.front() > target && vec.back() > target) return std::nullopt;

        // 3. check stop condition
        std::uint32_t x0 = 0;
        std::uint32_t x1 = vec.size()-1;

        while(x1 - x0 > 1) // BUG : Cannot replace by while(x0!=x1), otherwise lead to inf loop when x0+1=x1
        {
            // 4. mid point
            std::uint32_t xm = (x0 + x1) >> 1;
            
            // 5. bisection
            if      (vec[x0] <= target && target <= vec[xm]) x1 = xm;
            else if (vec[x0] >= target && target >= vec[xm]) x1 = xm;
            else                                             x0 = xm;
        }            

        // 6. answer
        if (vec[x0] == target) return std::make_optional(x0);
        if (vec[x1] == target) return std::make_optional(x1);
        return std::nullopt;
    }
    

    // ******************************************* //
    // Find peak only
    // * if vec is increasing, return std::nullopt
    // * if vec is decreasing, return std::nullopt
    // * if vec is valley,     return std::nullopt
    // * 2 equal num in a row, return std::nullopt 
    // ******************************************* //
    std::optional<std::uint32_t> peak_bisection(const std::vector<std::int32_t>& vec) 
    {
        // 1. check edge case 
        if (vec.size() < 3) return std::nullopt; 

        // 2. check solution existence
        if (vec[1] < vec[0])                       return std::nullopt; // reject falling LHS edge
        if (vec[vec.size()-1] > vec[vec.size()-2]) return std::nullopt; // reject  rising RHS edge

        // 3. check stop condition
        std::uint32_t x0 = 1;            
        std::uint32_t x1 = vec.size()-2;

        while(x1 - x0 > 1) // BUG : Cannot replace by while(x0!=x1), otherwise lead to inf loop when x0+1=x1
        {
            // 4. mid point
            std::uint32_t xm = (x0 + x1) >> 1;
            
            // 5. bisection
            if (vec[xm] > vec[xm-1])  x0 = xm;
            else                      x1 = xm;
        }            

        // 6. answer
        if (vec[x0] > vec[x0-1] && vec[x0] > vec[x0+1]) return x0;
        if (vec[x1] > vec[x1-1] && vec[x1] > vec[x1+1]) return x1;
        return std::nullopt;
    }
  

    std::optional<std::uint32_t> rotated_bisection(const std::vector<std::int32_t>& vec, std::int32_t target)
    {
        // 1. check edge case 
        if (vec.size() == 0) return std::nullopt; 

        // 2. check solution existence

        // 3. check stop condition
        std::uint32_t x0 = 0;
        std::uint32_t x1 = vec.size()-1;

        while(x1 - x0 > 1) // BUG : Cannot replace by while(x0!=x1), otherwise lead to inf loop when x0+1=x1
        {
            // 4. mid point
            std::uint32_t xm = (x0 + x1) >> 1;
            
            // 5. bisection
            if (vec[x0] <= vec[xm])
            {
                if (vec[x0] <= target && target <= vec[xm]) x1 = xm;
                else                                        x0 = xm;
            }
            else 
            {
                if (vec[xm] <= target && target <= vec[x1]) x0 = xm;
                else                                        x1 = xm;
            }
        }            

        // 6. answer
        if (vec[x0] == target) return std::make_optional(x0);
        if (vec[x1] == target) return std::make_optional(x1);
        return std::nullopt;
    } 
}

