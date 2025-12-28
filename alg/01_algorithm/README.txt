Bisection can be done by :
* 2 cases in each iteration <--- need to run for all log(N) iterations
* 3 cases in each iteration <--- allow early stop, but each iteration is slower (it behaves like binary tree)



That is (given increasing vec) :

if      (vec[mid] < target) lower = mid;  
else                        upper = mid;

if      (vec[mid] < target) lower = mid; 
else if (vec[mid] > target) upper = mid;
else                        return  mid;



Lets compare the efficiency between 2-case-bisection and 3-case-bisection. Suppose :
T2 = time for each iteration in 2-case-bisection
T3 = time for each iteration in 3-case-bisection

T3 > T2, due to :
* more comparisons
* more complex branch prediction (cache miss)

Given N = number of elements in vec :
* number of iteration for 2-case-bisection = log(N)
* number of iteration for 3-case-bisection 
= 1/N * 1 +                                <--- number of nodes = 1, each does 1 iteration
  2/N * 2 +                                <--- number of nodes = 2, each does 2 iterations
  4/N * 3 +                                <--- number of nodes = 3, each does 3 iterations
  8/N * 4 +
 16/N * 5 + ... + 2^(M-1)/N * M            where 1 + 2 + 4 + 8 + ... + 2^(M-1) = N
                                           i.e.                        2^ M    = N
                                           --->                           M    = log(N)


   (M-1) * 2^M + 1
= -----------------   by ChatGPT
           2^M - 1

  (logN-1) * N + 1
= ----------------- 
             N - 1

-> logN as N increases


Total time for 2-case bisection = T2 * log(N)
Total time for 3-case bisection = T3 * ... see above expression 
                                ~ T3 * log(N) as N increases

In conclusion, 2-case-bisection is faster.
