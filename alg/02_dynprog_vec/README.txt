Two points diff (1+2)
* max profit
* count target prfit
* count target abs prfit

Subseq (5+6)
* max subseq sum
* max subseq prd
* max non contiguous subseq sum
* max non contiguous subseq sum (strictly)
* max non contiguous subseq sum (strictly, puzzle game)
* count target subseq sum
* count target divisible subseq sum
* count less than target subseq sum (or equal to)
* count less than target subseq prd (or equal to)
* longest target subseq sum
* longest non contiguous increasing subseq sum









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
6         72        >    3.6     >    3         +2         (3,6} (6}
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


	 
