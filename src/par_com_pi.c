//Program computes Pi number in parallel using OpenMP APIs
#include <omp.h>
#include <stdio.h>
#include <time.h>
#define NUM_THREADS 8
static long num_steps = 1000000000;
double step;
int main(int argc, const char* argv[]) {
    int i;
    clock_t start =clock();
    double x=0,pi=0, sum=0,
    step = 1.0/(double)num_steps;
    omp_set_num_threads(NUM_THREADS);
    #pragma omp parallel for  private (i,x) reduction(+:sum)
    for(i=1;i<=num_steps;i++){
        x = (i-0.5)*step;
        sum = sum + 4.0/(1.0+x*x);
    }
    pi=sum/num_steps;
    printf("pi = %6.12f\n",pi);
	clock_t end=clock();
    double time=(double)(end-start)/CLOCKS_PER_SEC;
    printf("\nTime: %lf\n",time);
    return 0;
}