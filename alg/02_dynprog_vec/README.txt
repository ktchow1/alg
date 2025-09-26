***************************************************************
*** Explanation of count_less_than_target_subseq_prd<false> ***
***************************************************************
Find PROD < 20
                    (double)     (int)     
       prev_cum     curr_cum     curr_cum    increase in
                    / target     / target    subseq num    subseq
-------------------------------------------------------------------------
4          4              -           -         +1         {4}   
3         12              -           -         +2         {4,3} {3}
6         72        >    3.6     >    3         +2         (3,6} (6}
7        504        >   25.2     >   25         +1         {7}
2       1008        >   50.4     >   50         +2         {7,2} {2}
5       5040        >  252.0     >  252         +2         {2,5} {5}
4      20160        > 1008.0     > 1008         +1         {4}


Since prev_cum is integer, 

(int) prev_cum > (double) curr_cum / (int) target           is always equivalent to ...
(int) prev_cum >    (int) curr_cum / (int) target 

and we search for > by upper_bound. Therefore, count_less_than_target_subseq_prd<false> is implemented as : 

m_hist.upper_bound(curr_cum / target)


	 

**************************************************************
*** Explanation of count_less_than_target_subseq_prd<true> ***
**************************************************************
Find PROD <= 20
                    (double)     (int)     
       prev_cum     curr_cum     curr_cum    increase in
                    / target     / target    subseq num    subseq
-------------------------------------------------------------------------
4          4              -           -         +1         {4}   
3         12              -           -         +2         {4,3} {3}
6         72        >    3.6     >    3         +2         (3,6} (6}
7        504        >   25.2     >   25         +1         {7}
2       1008        >   50.4     >   50         +2         {7,2} {2}
5       5040        >  252.0     >  252         +2         {2,5} {5}
4      20160        > 1008.0     > 1008         +1         {4}


Since prev_cum is integer, 

(int) prev_cum > (double) curr_cum / (int) target           is always equivalent to ...
(int) prev_cum >    (int) curr_cum / (int) target 

and we search for > by upper_bound. Therefore, count_less_than_target_subseq_prd<false> is implemented as : 

m_hist.upper_bound(curr_cum / target)


	 
