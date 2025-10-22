********************
*** All problems ***
********************

Subproblems in 1D array
1. all problem in "02_dynprog_vec"   (see chap 02)
2. L shape block                     (not implemented)
3. Bell number                       (not implemented)


Subproblems in 2D matrix
1. longest common subseq             (iteration in matrix vs recursion)
2. connecting bridge                 (iteration in matrix vs recursion)
3. edit distance                     (iteration in matrix vs recursion)
4. parenthesis                       (iteration in matrix)
5. coin challenge                    (iteration in matrix)
6. shortest distance - 2 travellers  (not implemented)
7. shortest distance - bitonic tour  (not implemented)
8. piecewise linear fitting          (iteration in matrix)
9. bin packing                       (iteration in matrix) <--- it is defined in dp_matrix_and_graph.h, but better classified as this group


Subproblems in graph
1. Viterbi algorithm                 (not implemented)
2. assembly line                     (not implemented)
3. retrospective trader              (not implemented)
4. coin change - count               (iteration in matrix)
5. coin change - min                 (iteration in matrix vs iteration with region grow)
6. knapsack                          (iteration in matrix vs iteration with region grow)
7. job schedule                      (iteration in matrix vs iteration with region grow)
8. equal partition                   (iteration in matrix vs iteration with region grow)
9. box stacking                      (iteration in matrix vs iteration with region grow)





****************************
*** Subproblems in graph ***
****************************

Concepts              coin change        knapsack           job schedule       equal partition        box stacking 
----------------------------------------------------------------------------------------------------------------------------------------------
1.             x      coin value         obj weight         task workload      num                    box width & length
2.             y      1                  obj value          task profit        num                    box height 
3.       param p      coin num           obj num            task done (0/1)    num picked (0/1)       x/y/z/none as height
4.       state s      s[N-1] = sum(x[n], p[n], for n=[0,N-1])                                         s[N-1] = width/length of last picked box
5.       value v      v[N-1] = sum(y[n], p[n], for n=[0,N-1])                                         v[N-1] = sum  (height of all picked boxes)        
6.  constraint 
     coin change      min v under constraint  s[N-1] == target        
        knapsack      max v under constraint  s[N-1] <= weight_limit 
    job schedule      max v under constraints s[n]   <= deadline[n] for all n=[0,N-1]
    eq partition      max v under constraint  s = v  <= sum(num)/2
    box stacking      max v under constraint  width[n] >= width[n-1], length[n] >= length[n-1]


Approaches 
1. recursion in state_graph
2. recursion in state_matrix
3. iteration in state_graph          implemented as region_growing with a std::unordered_map & std::queue
4. iteration in state_matrix         implemented as 2-D scanning inside a alg::matrix





***************
*** Remarks ***
***************

1. How to apply equality or inequality constraint to state? Like :
     equality constraint : total_coin_value    == target
   inequality constraint : total_object_weight <= limit 
*  for iteration in graph,  return graph.find(target)        for == target
*  for iteration in graph,  search graph for optimal         for <= limit
*  for iteration in matrix, return mat(N-1,M)                for == target
*  for iteration in matrix, search mat(N-1,:) for optimal    for <= limit

2. How to apply constraint that is specific for each object?
*  for iteration in graph,  it is applied to s (after updating s = s_prev + ...)
*  for iteration in matrix, it is applied to m (in the inner nested-for-loop)

3. How to apply constraint that 
   each object is either picked once or none ...
   each object is considered in pre-defined order ? Like job schedule, box stacking.
*  for iteration in graph,  it is applied by introducing "m_next_allowed_xxx" in state 
*  for iteration in matrix, it is applied by linking mat(n,m) with mat(n-1,m-x[n]), instead of with mat(n,m-x[n])





