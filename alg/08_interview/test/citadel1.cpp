#include<iostream>
#include<cstdint>
#include<vector>
#include<list>
#include<set>
#include<iterator>

namespace interview {
template<typename OUT_ITER> 
void gen_sorted_data(OUT_ITER begin, std::uint32_t N)
{
    std::set<std::uint32_t> set;
    while(set.size()!=N)
    {
        set.insert(rand()%100);
    }

    OUT_ITER iter = begin;
    for(const auto& x:set)
    {
        *iter = x;
        ++iter;
    }
}

template<typename IN_ITER0, typename IN_ITER1, typename OUT_ITER> 
void pair_merge(IN_ITER0 begin0, IN_ITER0 end0,
                IN_ITER1 begin1, IN_ITER1 end1,
                OUT_ITER begin)
{
    static_assert(std::is_same<typename std::iterator_traits<IN_ITER0>::value_type, 
                               typename std::iterator_traits<IN_ITER1>::value_type>::value,
                               "IN_ITER0::value_type / IN_ITER1::value_type are not consistent");

    IN_ITER0 iter0 = begin0;
    IN_ITER1 iter1 = begin1;
    OUT_ITER iter  = begin;
    while(iter0!=end0 && iter1!=end1)
    {
        if      (*iter0 < *iter1) ++iter0;
        else if (*iter0 > *iter1) ++iter1;
        else
        {
            iter = *iter0;
            ++iter0;
            ++iter1;
            ++iter;
        }
    }
}

// ******************************************************************* //
// Input : a vector of N [begin, end] pairs, representing N sequences. //
// ******************************************************************* //
template<typename IN_ITER, typename OUT_ITER>
void K_merge_pairwise(const std::vector<std::pair<IN_ITER, IN_ITER>>& sequences, OUT_ITER begin)
{
    // *** Deep of first sequence *** //
    auto seq = sequences.begin();
    std::vector<typename std::iterator_traits<IN_ITER>::value_type> in(seq->first, seq->second);

    // *** Merge with next sequence *** //
    for(++seq; seq != sequences.end(); ++seq)
    {
        std::vector<typename std::iterator_traits<IN_ITER>::value_type> out;
        pair_merge(in.begin(), in.end(), seq->first, seq->second, std::back_inserter(out));

        // *** Remember to move *** //
        in = std::move(out);
    }

    // *** Final output *** //
    for(const auto& x:in)
    {
        *begin = x;
        ++begin; 
    }
}

// ************************************************************ //
// *** Generation of pair_merge (requested by Gasper Azman) *** //
// ************************************************************ //
// Normally we dont pick this implementation, its error prone, 
// I was forced to use this implementation in interview, fuck.
// ************************************************************ // 
template<typename IN_ITER, typename OUT_ITER>
void K_merge_allinone(const std::vector<std::pair<IN_ITER, IN_ITER>>& sequences, OUT_ITER begin)
{
    auto seqs = sequences;

    while(true) // loop all elements in sequences
    {
        bool match_exists = true;
        auto seq0 = seqs.begin();
        auto seq1 = seqs.begin();
        ++seq1;

        while(seq1!=seqs.end()) // loop all sequences
        {
            if (*seq0->first < *seq1->first)
            {
                // break and redo : with seq0 = first sequence
                ++(seqs.begin()->first);
                if (seqs.begin()->first == seqs.begin()->second) return;

                match_exists = false;
                break; 
            }
            else if (*seq0->first > *seq1->first) 
            {
                // continue : with same seq0 and seq1
                ++(seq1->first); 
                if (seq1->first == seq1->second) return;
            } 
            else
            {
                // continue : with next seq0 and seq1
                ++seq0;
                ++seq1;
            }
            // It is easy to confuse ++seq0 with ++(seq0.first).
            // It is easy to confuse ++seq1 with ++(seq1.first).
        }

        if (match_exists)
        {
            *begin = *(seqs.begin()->first);
            ++begin;

            // increment each sequence iterator
            for(auto& seq:seqs)
            {
                ++(seq.first);
                if (seq.first == seq.second) return;
            }
        }
    }
}

bool test_K_merge_impl()
{
    std::vector<std::uint32_t> in0; 
    std::vector<std::uint32_t> in1; 
    std::vector<std::uint32_t> in2; 
    std::vector<std::uint32_t> in3; 
    std::vector<std::uint32_t> out0;
    std::vector<std::uint32_t> out1;

    gen_sorted_data(std::back_inserter(in0), 70);
    gen_sorted_data(std::back_inserter(in1), 70);
    gen_sorted_data(std::back_inserter(in2), 70);
    gen_sorted_data(std::back_inserter(in3), 70);

    std::vector<std::pair<std::vector<std::uint32_t>::iterator, 
                          std::vector<std::uint32_t>::iterator>> sequences;

    sequences.push_back(std::make_pair(in0.begin(), in0.end()));
    sequences.push_back(std::make_pair(in1.begin(), in1.end()));
    sequences.push_back(std::make_pair(in2.begin(), in2.end()));
    sequences.push_back(std::make_pair(in3.begin(), in3.end()));
    K_merge_pairwise(sequences, std::back_inserter(out0));
    K_merge_allinone(sequences, std::back_inserter(out1));

    bool result = true;
    if (out0.size() == out1.size())
    {
        for(std::uint32_t n=0; n!=out0.size(); ++n)
        {
            if (out0[n] != out1[n]) result = false;
        }
    }
    else result = false;

    std::cout << "\nin0  : "; for(const auto& x:in0)  std::cout << x << " ";
    std::cout << "\nin1  : "; for(const auto& x:in1)  std::cout << x << " ";
    std::cout << "\nin2  : "; for(const auto& x:in2)  std::cout << x << " ";
    std::cout << "\nin3  : "; for(const auto& x:in3)  std::cout << x << " ";
    std::cout << "\nout0 : "; for(const auto& x:out0) std::cout << x << " ";
    std::cout << "\nout1 : "; for(const auto& x:out1) std::cout << x << " ";
    std::cout << "\nresult = " << result;
    std::cout << "\n";
    return result;
}

void test_citadel_K_merge()
{
    std::uint32_t correct_count = 0;
    std::uint32_t N = 1000;
    for(std::uint32_t n=0; n!=N; ++n)
    {
        if (test_K_merge_impl()) ++correct_count;
    }
    std::cout << "\ncorrect_count = " << correct_count << "/" << N;
    std::cout << "\n\n";
}
}
