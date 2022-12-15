//Program for matrix multiplication in serial
#include<iostream>
#include<vector>
#include<chrono>
const int MAX=1000;
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
    
    for(int i=0;i<MAX;i++){
        for(int j=0;j<MAX;j++){
            C[i][j]=0;
            for(int k=0;k<MAX;k++){
                C[i][j]+=(A[i][k]*B[k][i]);
            }
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