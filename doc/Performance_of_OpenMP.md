# Performace of OpenMP
In this section, I will compare time complexity using sequential process, OpenMP on program: Matrix multiplication .
## Serial matrix multiplication
Here is the piece of code of matrix multiplication sequentially.
```cpp=
void multiply(const std::vector<std::vector<int>>&A,const std::vector<std::vector<int>>&B, std::vector<std::vector<int>>&C){
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            C[i][j]=0;
            for(int k=0;k<MAX;k++){
                C[i][j]+=(A[i][k]*B[k][i]);
            }
        }
    }
}
```

## Parallel matrix multiplication
Here is the piece of code of matrix multiplication using OpenMP APIs.
```cpp=
void multiply(const std::vector<std::vector<int>>&A,const std::vector<std::vector<int>>&B, std::vector<std::vector<int>>&C){
    int i,j,k;
    omp_set_num_threads(num_thr);
    #pragma omp parallel for private(i,j,k)
    for( i=0;i<MAX;i++){
        for( j=0;j<MAX;j++){
            int res=0;
            #pragma omp simd reduction(+:res)
            for( k=0;k<MAX;k++){
                res+=(A[i][k]*B[k][i]);
            }
            C[i][j]=res;
        }
    }
}
```
## Performance comparison


| Max | Serial  | OpenMP  | -O3 optimization | OpenMP + -O3 opimization |
| --- | ------- | ------- | ---------------- | ------------------------ |
500 | 2121 ms | 1345 ms | 378 ms           | 188 ms|
|1000|20537 ms|10899 ms |8521 ms|4699 ms|

Max: size of Matrix
In the parallel code, the number of threading is 8. From the table, the most efficent way to reduce time complexity is to combine both OpenMP multi-thread and vectorize the code. Although the speed of OpenMP multi-thread is twice as fast as serial computation, it is still slower than vectorize the code.

It is possible to assign more thread to reduce time complexity. However, my CPU only contains 4 cores, increasing the number of thread does not make much difference


| Number of thread | OpenMP    | OpenMP + -O3 optimization |
| ---------------- | --------- | ------------------------- |
|    8              |   11150 ms        | 4515 ms                          |
|     10             | 10860 ms          |  4472 ms                         |
|       50          |  10337 ms|        4687 ms          |



