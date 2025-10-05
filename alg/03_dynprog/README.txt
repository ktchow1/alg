Subproblems in 1D array (3)
* all problem in "02_dynprog_vec"   (see chap 02)
* L shape block                     (not implemented)
* Bell number                       (not implemented)





Subproblems in 2D matrix (8)
* longest common subseq 
* connecting bridge                 (same implementation)
* edit distance
* parenthesis
* coin challenge
* shortest distance - 2 travellers  (not implemented)
* shortest distance - bitonic tour  (not implemented)
* piecewise linear fitting

- implementation 1 : recursion in matrix, boundary case when vec size == 0
- implementation 2 : iteration in matrix, boundary case when vec size == 1





Subproblems in graph (10)
* Viterbi algorithm                 (not implemented)
* assembly line                     (not implemented)
* retrospective trader              (not implemented)
* coin change - min 
* coin change - count
* knapsack
* job schedule
* equal partition
* box stacking
* bin packing

- implementation 1 : use 2 unordered_map, std::move one to another
- implementation 2 : use 1 unordered_map, region grow with a stack / queue

