Two points sum (3+2+3)
* max 2 point sum-distance
* max 2 point equal-digit-sum
* max 2 point sum (non attacking rook)
* find  target 2 point sum in 1 sorted vec
* find  target 2 point sum in 2 sorted vec
* count target 2 point sum in 1 unsorted vec
* count target 3 point sum in 1 unsorted vec
* count target 4 point sum in 1 unsorted vec

Two points diff (1+2)
* max profit
* count target profit
* count target abs profit

Subseq (5+4+2)
* max subseq sum
* max subseq prd
* max non contiguous subseq sum
* max non contiguous subseq sum (strictly) 
* max non contiguous subseq sum (strictly, puzzle game)
* count target subseq sum
* count target divisible subseq sum
* count less than target subseq sum (or equal to)          1. instead of init hist, do checking 
* count less than target subseq prd (or equal to)          1. instead of init hist, do checking 2. std::floor vs std::ceil
* longest target subseq sum
* longest non contiguous increasing subseq sum

Substr 
* longest non-duplicate substr                             1. sub is needed, 2. check prev duplicate inside sub
* longest palindrome substr                                1. sub is needed, 2. check access outside str

Stack trick
* shortest unsorted subseq
* count stroke in histogram                                1. pushing new vec[n] is needed, pop all equal out
* biggest rect in histogram                                1. pushing new vec[n] is needed, pop all equal out
* trapping water

Sorting variant
* order statistic (quick sort variant)
* number of swap (bubble sort variant)







***************************************************************
*** Explanation of count_less_than_target_subseq_prd<false> ***
***************************************************************
Find PROD < 20
                    (double)     (int)     
vec    prev_cum     curr_cum     curr_cum    increase in
                    / target     / target    subseq num    subseq
-------------------------------------------------------------------------
4          4              -           -         +1         {4}   
3         12              -           -         +2         {4,3} {3}
6         72        >    3.6     >    3         +2         {3,6} {6}
7        504        >   25.2     >   25         +1         {7}
2       1008        >   50.4     >   50         +2         {7,2} {2}
5       5040        >  252.0     >  252         +2         {2,5} {5}
4      20160        > 1008.0     > 1008         +1         {4}
-------------------------------------------------------------------------
                                        total = 11

Since prev_cum is integer, 

(int) prev_cum > (double) curr_cum / (int) target           is always equivalent to ...
(int) prev_cum >    (int) curr_cum / (int) target           where > is done by upper_bound.

Therefore, count_less_than_target_subseq_prd<false> is implemented as : 

m_hist.upper_bound(curr_cum / target)



	 

**************************************************************
*** Explanation of count_less_than_target_subseq_prd<true> ***
**************************************************************
Find PROD <= 20
                    (double)     ceil(double)     
vec    prev_cum     curr_cum     curr_cum    increase in
                    / target     / target    subseq num    subseq
-------------------------------------------------------------------------
4          4              -           -         +1         {4}   
3         12              -           -         +2         {4,3} {3}
6         72       >=    3.6    >=    4         +2         (3,6} (6}
7        504       >=   25.2    >=   26         +1         {7}
2       1008       >=   50.4    >=   51         +2         {7,2} {2}
5       5040       >=  252.0    >=  252         +2         {2,5} {5}
4      20160       >= 1008.0    >= 1008         +2         {5,4} {4}   
-------------------------------------------------------------------------
                                        total = 12

Since prev_cum is integer, 

(int) prev_cum >=      (double) curr_cum / (int) target     is always equivalent to ...
(int) prev_cum >= ceil (double) curr_cum / (int) target     where >= is done by lower_bound. 

Therefore, count_less_than_target_subseq_prd<true> is implemented as : 

m_hist.lower_bound(curr_cum / target)


	 


