#include<cassert>

#include<ctd.h>
#include<matrix.h>
#include<utility.h>


template<typename T>
bool is_vec_iota(const std::vector<T>& vec)
{
    T expected = 0;
    for(const auto& x:vec)
    {
        if (x!=expected) return false;
        ++expected;
    }
    return true;
}


void test_ctd_spiral_traverse()
{
    // case 1 : even y, even x
    {
        alg::matrix<std::uint32_t> mat(4,4);
        mat.set_row(0, { 0, 1, 2, 3});
        mat.set_row(1, {11,12,13, 4});
        mat.set_row(2, {10,15,14, 5});
        mat.set_row(3, { 9, 8, 7, 6});

        auto ans = ctd::spiral_traverse(mat);
        assert(ans.size() == mat.size_y() * mat.size_x());
        assert(is_vec_iota(ans));
    }
    // case 2 : even y, odd x
    {
        alg::matrix<std::uint32_t> mat(4,5);
        mat.set_row(0, { 0, 1, 2, 3, 4});
        mat.set_row(1, {13,14,15,16, 5});
        mat.set_row(2, {12,19,18,17, 6});
        mat.set_row(3, {11,10, 9, 8, 7});

        auto ans = ctd::spiral_traverse(mat);
        assert(ans.size() == mat.size_y() * mat.size_x());
        assert(is_vec_iota(ans));
    }
    // case 3 : odd y, odd x
    {
        alg::matrix<std::uint32_t> mat(7,5);
        mat.set_row(0, { 0, 1, 2, 3, 4});
        mat.set_row(1, {19,20,21,22, 5});
        mat.set_row(2, {18,31,32,23, 6});
        mat.set_row(3, {17,30,33,24, 7});
        mat.set_row(4, {16,29,34,25, 8});
        mat.set_row(5, {15,28,27,26, 9});
        mat.set_row(6, {14,13,12,11,10});

        auto ans = ctd::spiral_traverse(mat);
        assert(ans.size() == mat.size_y() * mat.size_x());
        assert(is_vec_iota(ans));
    }
    // case 4 : odd y, even x
    {
        alg::matrix<std::uint32_t> mat(7,4);
        mat.set_row(0, { 0, 1, 2, 3});
        mat.set_row(1, {17,18,19, 4});
        mat.set_row(2, {16,27,20, 5});
        mat.set_row(3, {15,26,21, 6});
        mat.set_row(4, {14,25,22, 7});
        mat.set_row(5, {13,24,23, 8});
        mat.set_row(6, {12,11,10, 9});

        auto ans = ctd::spiral_traverse(mat);
        assert(ans.size() == mat.size_y() * mat.size_x());
        assert(is_vec_iota(ans));
    }
    print_summary("ctd : spiral traverse", "succeeded");
}


void test_ctd_K_merge()
{
    std::uint32_t T    = 500;
    std::uint32_t K    = 10;
    std::uint32_t size = 100;
    std::uint32_t min  = 0;
    std::uint32_t max  = 1000;

    for(std::uint32_t t=0; t!=T; ++t)
    {
        std::vector<std::vector<std::uint32_t>> data;
        std::vector<std::pair<std::vector<std::uint32_t>::iterator,
                              std::vector<std::uint32_t>::iterator>> data_ranges;

        for(std::uint32_t k=0; k!=K; ++k)
        {
            auto vec = gen_random_sorted_vec<std::uint32_t>(size, min, max);
            data.push_back(std::move(vec));
            data_ranges.push_back
            (
                std::make_pair(data.back().begin(),
                               data.back().end())
            );
        }

        std::vector<std::uint32_t> ans0;
        std::vector<std::uint32_t> ans1;
        std::vector<std::uint32_t> ans2;
        ctd::K_merge_pairwise   (data_ranges, std::back_inserter(ans0)); 
        ctd::K_merge_all_at_once(data_ranges, std::back_inserter(ans1)); 
        ctd::K_merge_all_in_one (data_ranges, std::back_inserter(ans2)); 

        assert(ans0 == ans2);
        assert(ans1 == ans2);
    }
    print_summary("ctd : K merge", "succeeded");
}


void test_ctd_assignment_problem()
{
    std::uint32_t num_trial = 200;
    std::uint32_t size = 8;
    std::uint32_t min  = 0;
    std::uint32_t max  = 100;

    benchmark<1>("ctd : assignment problem",           
                 std::bind(gen_random_mat<std::uint32_t>, size, size, min, max), 
                 std::bind(ctd::assignment_by_exhaustive_iteration, _1),     
                 std::bind(ctd::assignment_by_exhaustive_recursive, _1),
                 num_trial); 
}


void test_ctd()
{
    test_ctd_spiral_traverse();
    test_ctd_K_merge();
    test_ctd_assignment_problem();
}

