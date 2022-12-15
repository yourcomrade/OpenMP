#include <iostream>
#include <omp.h>
int fib(int n)
{
    if (n < 2)
        return n;
    int a, b;
    #pragma omp taskgroup
    {
        #pragma omp task shared(a) firstprivate(n)
        a = fib(n-1);
        #pragma omp task shared(b) firstprivate(n)
        b = fib(n-2);
    }
    
    return a + b;
}
int main() {
    omp_set_num_threads(4);
    #pragma omp parallel
    #pragma omp single
    std::cout<<"fib(10) = "<< fib(12)<<"\n";
    return 0;
}