# Introduction to OpenMP
To begin with, all code are written in C/C++ in Window Operating system and compile using mingw64. Fortran is not covered as I do not enough time to learn Fortran.
## Table of content
- [Getting started](#getting-started)
- [Parallel loop](#parallel-loop)
- [Synchronization](#synchronization)
- [Private and shared variable among thread](#private-and-share-variable-among-thread)
- [Reduction clause](#reduction-clause)
- [Control the thread](#control-the-thread)
- [Task](#task)
- [SIMD](#simd)

## Getting started
Below is the simple "Hello World" program using OpenMP
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    #pragma omp parallel
    {
        std::cout<<"Hello World id: "<<omp_get_thread_num()<<"\n";;
    }
    return 0;
}
```
Then compile using g++ in cmd or Powershell
```cmd=
D:\file\openmp>g++ my_pro.cpp -fopenmp -o my_pro.exe

D:\file\openmp>my_pro.exe
Hello World id: Hello World id: 1
2
Hello World id: 3
Hello World id: 0
```
The link flag -fopenmp is to let compiler know that it must use and link OpenMP library which is integrated into mingw compiler since version 4.7. The function "omp_set_num_threads()" is to set number of thread that will be created in parallel region. The "#pragma omp parallel " is to start the parallel region, and inside the "{}" block is the place where parallel region work. The function "omp_get_thread_num()" is to get the id of the thread which is running.  
It is clearly to see that the order of output is chaostic, which is common behaviour when having multi-thread, because it is impossible to know which thread runs first, and it depends on hardware.
## Parallel loop
Let's start with simple 2 vectors addition
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    int arr[5]={1,2,3,4,5};
    int arr1[5]={1,2,3,4,5};
    int i;
    #pragma omp parallel 
    {
        for(i=0;i<5;i++){
            arr[i]+=arr1[i];
        }
    }
    for(i=0;i<5;i++){
        std::cout<<arr[i]<<" ";
    }
    return 0;
}
```
The result is
```cmd=
D:\file\openmp>g++ my_pro.cpp -fopenmp -o my_pro.exe
D:\file\openmp>my_pro.exe
5 10 15 20 25
D:\file\openmp>my_pro.exe
5 10 15 20 25
D:\file\openmp>my_pro.exe
5 10 15 20 25
```
It does not produce right result {2,4,6,8,10} as expected, because each thread has its own for-loop. To solve this problem, it is necessary to add "#pragma omp for" before for-loop statement.
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    int arr[5]={1,2,3,4,5};
    int arr1[5]={1,2,3,4,5};
    int i;
    #pragma omp parallel 
    {
        #pragma omp for
        for(i=0;i<5;i++){
            arr[i]+=arr1[i];
        }
       
    }
    for(i=0;i<5;i++){
        std::cout<<arr[i]<<" ";
    }
    return 0;
}
```
The result now is true
```cmd=
D:\file\openmp>g++ my_pro.cpp -fopenmp -o my_pro.exe
D:\file\openmp>my_pro.exe
2 4 6 8 10
D:\file\openmp>my_pro.exe
2 4 6 8 10
D:\file\openmp>my_pro.exe
2 4 6 8 10
```
It can be even more simplified like this
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    int arr[5]={1,2,3,4,5};
    int arr1[5]={1,2,3,4,5};
    int i;
    #pragma omp parallel for
    for(i=0;i<5;i++){
        arr[i]+=arr1[i];
    }
    
    for(i=0;i<5;i++){
        std::cout<<arr[i]<<" ";
    }
    return 0;
}
```
## Synchronization
The unprediction of multi-threading leads to concern of data-race of variable when running the program. Let's run a simple program to demonstrate the race condition
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    unsigned long i,k=0;
    #pragma omp parallel for
    for(i=0;i<50000;i++){
       k+=i;
    }
    std::cout<<k<<"\n";
    return 0;
}
```
The result is changing whenever the program is being run.
```cmd=
D:\file\openmp>my_pro.exe
943518738
D:\file\openmp>my_pro.exe
407536817
D:\file\openmp>my_pro.exe
470235982
D:\file\openmp>my_pro.exe
913591216
D:\file\openmp>my_pro.exe
407074778
D:\file\openmp>my_pro.exe
440325383
D:\file\openmp>my_pro.exe
340908033
D:\file\openmp>my_pro.exe
572312003
```
The reason behind this changing is because both variables i and k are shared among the thread, when one thread updates the variables i or k, other threads cannot realize the update of variables. To handle this problem, OpenMP had offered both "#pragma omp atomic" and "#pragma omp critical"

```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    unsigned long i,k=0;
    #pragma omp parallel for
    for(i=0;i<50000;i++){
       #pragma omp critical
       k+=i;
    }
    std::cout<<k<<"\n";
    return 0;
}
```
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    unsigned long i,k=0;
    #pragma omp parallel for
    for(i=0;i<50000;i++){
       #pragma omp atomic
       k+=i;
    }
    std::cout<<k<<"\n";
    return 0;
}
```
The result now is true
```cmd=
D:\file\openmp>my_pro.exe
1249975000
D:\file\openmp>my_pro.exe
1249975000
D:\file\openmp>my_pro.exe
1249975000
D:\file\openmp>my_pro.exe
1249975000
D:\file\openmp>my_pro.exe
1249975000
D:\file\openmp>my_pro.exe
1249975000
```
What is difference between "atomic" and "critical" ? Currently, it is not different much in terms of efficiency and purpose. [These answers from StackOverflow have well explained for the difference between atomic and critical in OpenMP](https://stackoverflow.com/questions/7798010/what-is-the-difference-between-atomic-and-critical-in-openmp). 

## Private and share variable among thread
Looking at variable i, it is shared between threads. In this circumstance, it is not affected much. But what if the programmers desire to determine which variable is private or shared among the thread?  

"private" is provided to create uninitialized private variable among thread whether that variable is initialized before parallel region or not.

```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    unsigned long i=100;
    #pragma omp parallel private(i)
    {   
        #pragma omp critical
        std::cout<<"private i = "<<i<<" addr: "<<&i<<"\n";
    }
    std::cout<<"i = "<<i<<"\n";
    return 0;
}
```
The result
```cmd=
D:\file\openmp>my_pro.exe
private i = 34 addr: 0x22d9fff7fc
private i = 34 addr: 0x22d9dffa1c
private i = 0 addr: 0x22d99ffb8c
private i = 520 addr: 0x22da1ff68c
i = 100
```
It is clearly to notice that the address of variable i is different among the thread. If user wants to have initialzed variable, OpenMP provides "firstprivate"
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    unsigned long i=100;
    #pragma omp parallel firstprivate(i)
    {   
        #pragma omp critical
        std::cout<<"private i = "<<i<<" addr: "<<&i<<"\n";
    }
    std::cout<<"i = "<<i<<"\n";
    return 0;
}
```
The result
```cmd=
D:\file\openmp>my_pro.exe
private i = 100 addr: 0x5bd71ff98c
private i = 100 addr: 0x5bd6fff65c
private i = 100 addr: 0x5bd6bff9dc
private i = 100 addr: 0x5bd73ff73c
i = 100
```
For the shared variables, the default set OpenMP to all variables are shared. It can be specified like this piece of code
```cpp=
int i;
#pragma omp parallel shared(i)
```
## Reduction clause
Using "atomic" and "critical" is a good solution to avoid data-race. Nonetheless, it will slow down the performance as each thread must synchronize the shared variable. To tackle this problem, OpenMP introduces "reduction". Let's change the code from [Synchronization](##Synchronization) to use "reduction"
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    unsigned long i=0,k=0;
    #pragma omp parallel for private(i) reduction(+:k)
    for(i=0;i<50000;i++){
        k+=i;
    }
    std::cout<<"k = "<<k<<"\n";
    return 0;
}
```
The result is still the same.
```cmd=
D:\file\openmp>my_pro.exe
k = 1249975000
D:\file\openmp>my_pro.exe
k = 1249975000
D:\file\openmp>my_pro.exe
k = 1249975000
D:\file\openmp>my_pro.exe
k = 1249975000
D:\file\openmp>my_pro.exe
k = 1249975000
```
The syntax of using "reduction" is "reduction(operation:list)" with "operation" is "+","-","/" and so on and list is a list of variables.

## Control the thread
So far, all OpenMP APIs we have known are doing the same share work. However, if we want to assign specific task for each thread, OpenMP provides "section" to do this.
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
   
    #pragma omp parallel 
    {
        #pragma omp sections
        {
            #pragma omp section
            std::cout<<"Hello id "<<omp_get_thread_num()<<"\n";
            #pragma omp section
            std::cout<<"Eat id "<<omp_get_thread_num()<<"\n";
            #pragma omp section
            std::cout<<"Sleep id "<<omp_get_thread_num()<<"\n";
            #pragma omp section
            std::cout<<"Drink id "<<omp_get_thread_num()<<"\n";

        }
    }
    return 0;
}
```
The result is
```cmd=
D:\file\openmp>my_pro.exe
Sleep id Hello id 1
2
Drink id 3
Eat id 0
```
Each thread only does its job in for 1 section. What if we have more sections than our number of thread? Here is the result after adding 
```cpp=
#pragma omp section
std::cout<<"Drive id "<<omp_get_thread_num()<<"\n";
```
The result is
```cmd=
D:\file\openmp>my_pro.exe
Hello id 2
Drive id 2
Drink id 3
Eat id 0
Sleep id 1
```
It is clearly that one thread has to do more than 1 section.
The program can be re-written in a shorter form:
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    #pragma omp parallel sections
    { 
        #pragma omp section
        std::cout<<"Hello id "<<omp_get_thread_num()<<"\n";
        #pragma omp section
        std::cout<<"Eat id "<<omp_get_thread_num()<<"\n";
        #pragma omp section
        std::cout<<"Sleep id "<<omp_get_thread_num()<<"\n";
        #pragma omp section
        std::cout<<"Drink id "<<omp_get_thread_num()<<"\n";
        #pragma omp section
        std::cout<<"Drive id "<<omp_get_thread_num()<<"\n";
    }
    return 0;
}
```
Another way to control the thread to execute 1 instruction per thread like "section" is to use "single"
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    #pragma omp parallel 
    {
        #pragma omp single 
        std::cout<<"Hello id "<<omp_get_thread_num()<<"\n";
        #pragma omp single 
        std::cout<<"Eat id "<<omp_get_thread_num()<<"\n";
        #pragma omp single 
        std::cout<<"Sleep id "<<omp_get_thread_num()<<"\n";
        #pragma omp single 
        std::cout<<"Drink id "<<omp_get_thread_num()<<"\n";
        #pragma omp single 
        std::cout<<"Drive id "<<omp_get_thread_num()<<"\n";
        
    }
    return 0;
}
```
The result is same as using "section"
```cmd=
D:\file\openmp>my_pro.exe
Hello id 2
Eat id 0
Sleep id 1
Drink id 0
Drive id 1
```
The difference between "single" and "section" can be found in this [StatckOverflow answer](https://stackoverflow.com/questions/23556023/difference-between-single-and-sections-directive-in-openmp)

## Task
Untill now, most of the OpenMP API so far we know only work with loop. It is very hard to deploy multi-thread on recursive code. It is possible to use "section" in OpenMP for recursion, but "section" is static which is not suitable for dynamic thing like recursion. OpenMP provide "task" to solve this problem. Let run this program to understand how task works
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
   
    #pragma omp parallel 
    {
        #pragma omp task
        std::cout<<" A ";
        #pragma omp task
        std::cout<<" B ";
        #pragma omp task
        std::cout<<" C ";
    }
    return 0;
}
```
The order of result is chaostic
```cmd=
D:\file\openmp>my_pro.exe
 A  B  C  A  B  C  A  B  C  A  B  C
D:\file\openmp>my_pro.exe
 A  B  C  A  B  C  A  B  C  A  B  C
D:\file\openmp>my_pro.exe
 B  B  C  A  B  C  A  B  C  A  C  A
D:\file\openmp>my_pro.exe
 A  B  B  C  B  C  B  A  C  A  C  A
D:\file\openmp>my_pro.exe
 B  B  C  A  B  C  A  B  C  A  C  A
```
That is the tasks can be distributed arbitrarily in thread. The solution for this is to add "#pragma omp single ".
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#include <cstdio>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    #pragma omp parallel 
    {
        #pragma omp single
        {
            #pragma omp task
            std::cout<<" A ";
            #pragma omp task
            std::cout<<" B ";
            #pragma omp task
            std::cout<<" C ";
        }
        
    }
    return 0;
}
```
The order of result is correct
```cmd=
D:\file\openmp>my_pro.exe
 A  B  C
D:\file\openmp>my_pro.exe
 A  B  C
D:\file\openmp>my_pro.exe
 A  B  C
```
However, it prints the result once not 4 times like before. This may lead to conclusion that 1 thread has to do everything because we use "single". Let check thread id to verify this.
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#include <cstdio>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
   
    #pragma omp parallel 
    {
        #pragma omp single
        {
            #pragma omp task
            std::cout<<" A id: "<<omp_get_thread_num()<<" ";
            #pragma omp task
            std::cout<<" B id: "<<omp_get_thread_num()<<" ";
            #pragma omp task
            std::cout<<" C id: "<<omp_get_thread_num()<<" ";
           
        } 
    }
    return 0;
}
```
The result shows that despite using "single" each task has its own thread.
```cmd=
D:\file\openmp>my_pro.exe
 A id:  B id: 2  C id: 1 0
D:\file\openmp>my_pro.exe
 A id: 2  C id: 1  B id: 0
D:\file\openmp>my_pro.exe
 B id: 0  A id:  C id: 1 2
D:\file\openmp>my_pro.exe
 B id:  A id: 0  C id: 1 2
D:\file\openmp>my_pro.exe
 C id:  A id:  B id: 120
```
Thus, even we use "single" the compiler still auto distributes task thread like "section". 
To wait other tasks finish before 1 specific task start, OpenMP provides "taskwait" clause.
Starting with example of computing fibonaci number using recursion and OpenMP task
```cpp=
#include <iostream>//This header file must be included
#include <omp.h>
int fib(int n)
{
    if (n < 2)
        return n;
    int a, b;
    #pragma omp task shared(a) firstprivate(n)
    a = fib(n-1);
    #pragma omp task shared(b) firstprivate(n)
    b = fib(n-2);
    #pragma omp taskwait
    return a + b;
}
int main() {
    omp_set_num_threads(4);
    #pragma omp parallel
    #pragma omp single
    std::cout<<"fib(12) = "<< fib(12)<<"\n";
    return 0;
}
```
The result is 
```cmd=
D:\file\openmp>my_pro.exe
fib(12) = 144
```
Alternatively, it is possible to use "taskgroup" to group all task and get the result at main thread. Here is the re-write of fibbonaci number using "taskgroup"
```cpp=
#include <iostream>//This header file must be included
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
    std::cout<<"fib(12) = "<< fib(12)<<"\n";
    return 0;
}
```
To distribute a loop in task like "#pragme omp parallel for", there is an API "taskloop" in OpenMP
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
   
    #pragma omp parallel 
    {
        #pragma omp single
        {
            #pragma omp taskloop 
            for(int i=0;i<6;i++)
            {
                std::cout<<" i: "<<i<<" id: "<<omp_get_thread_num()<<"\n";
            }
        }
    }
    return 0;
}
```
The result is:
```cmd=
D:\file\openmp>my_pro.exe
 i: 2 id: 3
 i: 3 id: 3
 i: 5 id: 0
 i: 4 id: 2
 i: 0 id: 1
 i: 1 id: 1
```

## SIMD

Single instruction, multiple data or SIMD for short is the type of parallelism exploting data parallelism instead of concurrency or multi-threading. For example, in vector addition program, instead of taking each number from each vector to sum up, it will be faster if we take 2 vectors and sum them at the same time. This image from Wikipedia illustrates clearly.
![SIMD image](https://upload.wikimedia.org/wikipedia/commons/thumb/9/9b/SIMD_cpu_diagram1.svg/800px-SIMD_cpu_diagram1.svg.png)
Intel, AMD, IBM, ARM and other CPU manufacturers has their own architecture of SIMD. Currently, I experiment on my laptop which has Intel Core i7 3rd generation inside. This CPU supports SIMD and uses XMM registers (part of SSE instruction) or YMM registers (part of AVX instruction) for SIMD. In OpenMP, the enabling of simd instruction requires some optimization flags of compiler to vectorise the loop. 
Starting with simple 2 vector addition program.
```cpp=
#include <omp.h> //This header file must be included
#include <iostream>
#include <vector>
const int MAX=10;
#define NUM_THREAD 4
int main(){
    omp_set_num_threads(NUM_THREAD);
    std::vector<int>A(MAX),B(MAX),C(MAX);
    for(int i=0;i<MAX;i++){
        A[i]=i+1;
        B[i]=i+1;
    }
    #pragma omp simd
    for(int i=0;i<MAX;i++){
        C[i]=(A[i]+B[i]);
    }
    for(int i=0;i<MAX;i++){
        std::cout<<C[i]<<" ";
    }
    return 0;
}
```
It is necessary to compile the program using flags -O2(or -O3) for optimization, -fopt-info-vec-optimized(to know which part is vectorized), -fopenmp, -fopenmp-simd, -march=native(to enable full CPU optimization), because OpenMP is not guarantee that it can vectorize the loop itself without the assistance from compiler. 

```cmd=
D:\file\openmp>g++ -O2 -march=native -fopt-info-vec-optimized -fopenmp -fopen-simd my_pro.cpp -o my_pro.exe
my_pro.cpp:16:18: optimized: loop vectorized using 16 byte vectors
my_pro.cpp:16:18: optimized: loop vectorized using 8 byte vectors
my_pro.cpp:10:18: optimized: loop vectorized using 8 byte vectors
c:\w64devkit\lib\gcc\x86_64-w64-mingw32\12.2.0\include\c++\bits\stl_vector.h:100:16: optimized: basic block part vectorized using 32 byte vectors
c:\w64devkit\lib\gcc\x86_64-w64-mingw32\12.2.0\include\c++\bits\stl_vector.h:100:16: optimized: basic block part vectorized using 32 byte vectors
```
The information reveals which part of code is vectorized. 
The result is correct.
```cmd=
D:\file\openmp>my_pro.exe
2 4 6 8 10 12 14 16 18 20
```
Here is the piece of assembly code of vectorized part using gdb
```
0x00007ff70ad839ff <+175>:   lea    0x28(%rax),%rsi
0x00007ff70ad83a03 <+179>:   mov    %rax,0x60(%rsp)
0x00007ff70ad83a08 <+184>:   mov    %rax,%rbx
0x00007ff70ad83a0b <+187>:   vmovq  0x75f5(%rip),%xmm1        # 0x7ff70ad8b008
0x00007ff70ad83a13 <+195>:   vmovq  0x75f5(%rip),%xmm3        # 0x7ff70ad8b010
0x00007ff70ad83a1b <+203>:   mov    %rsi,0x70(%rsp)
0x00007ff70ad83a20 <+208>:   vpxor  %xmm0,%xmm0,%xmm0
0x00007ff70ad83a24 <+212>:   vmovq  0x75ec(%rip),%xmm2        # 0x7ff70ad8b018
0x00007ff70ad83a2c <+220>:   vmovdqu %xmm0,0x10(%rax)
0x00007ff70ad83a31 <+225>:   movq   $0x0,0x20(%rax)
0x00007ff70ad83a39 <+233>:   xor    %eax,%eax
0x00007ff70ad83a3b <+235>:   mov    %rsi,0x68(%rsp)
0x00007ff70ad83a40 <+240>:   vmovdqa %xmm1,%xmm0
0x00007ff70ad83a44 <+244>:   vpaddd %xmm3,%xmm1,%xmm1
0x00007ff70ad83a48 <+248>:   vpaddd %xmm2,%xmm0,%xmm0
0x00007ff70ad83a4c <+252>:   vmovq  %xmm0,0x0(%rbp,%rax,1)
0x00007ff70ad83a52 <+258>:   vmovq  %xmm0,(%rdi,%rax,1)
```
It is clearly to see that XMM registers are used which indicates that the program indeed using SIMD instructions. 

