//Program for matrix multiplication in parallel using OpenMP
#include<iostream>
#include<vector>
#include<omp.h>
#include<chrono>
const int MAX=1000;
const int num_thr=50;
void print_mat(const std::vector<std::vector<int>>&A){
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            std::cout<<A[i][j]<<" ";
        }
        std::cout<<"\n";
    }
    std::cout<<"\n";
}
void multiply(const std::vector<std::vector<int>>&A,const std::vector<std::vector<int>>&B, std::vector<std::vector<int>>&C){
    
    int i,j,k;
    omp_set_num_threads(num_thr);
    #pragma omp parallel for private(i,j,k) shared(A,B,C)
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
int main(){
    std::vector<std::vector<int>>vecA(MAX,std::vector<int>(MAX)),
    vecB(MAX,std::vector<int>(MAX)),
    vecC(MAX,std::vector<int>(MAX));
    int a=1;
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            vecA[i][j]=a;
            vecB[i][j]=a;
            a++;
            
        }
    }
    /*std::cout<<"Matrix A:\n";
    print_mat(vecA);
    std::cout<<"Matrix B:\n";
    print_mat(vecB);*/
    auto start=std::chrono::high_resolution_clock::now();
    multiply(vecA,vecB,vecC);
    auto end=std::chrono::high_resolution_clock::now();
    auto diff=std::chrono::duration_cast<std::chrono::milliseconds>(end-start);
    
   // print_mat(vecC);
    std::cout<<"Execution time: "<<diff.count()<<" ms\n";
    return 0;
}