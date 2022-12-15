# Practice parallel hash processing with OpenMP
The hash algorithm chosen here is [SHA-256](https://en.wikipedia.org/wiki/SHA-2) which is the famous hash used in Bitcoin. I attempt to generate a lot of hashes to simulate crypto mining environment. It is not possible to compute parallel SHA-256 like this [StackOverflow answer](https://stackoverflow.com/questions/24088295/can-sha1-sha-256-sha-512-be-broken-up-to-run-across-multiple-cores-threads). However, some parts of SHA-256 can be vectorized which indicates that OpenMP can be compututed parallelly partially, and there is many sources on Internet of how to apply SIMD for SHA-256. Therefore, I will apply multi-thread for generating hash and vectorize some part of code. 
It is noted that this part requiring reader to have good understanding how SHA-256 works. In the SHA-256, the part which can be using SIMD is in message schedule
```cpp=
 #pragma omp simd
    for(int i=16;i<64;i++){
        uint32_t wi_15=my_hash->mes_sche[i-15];
        uint32_t wi_2=my_hash->mes_sche[i-2];
        uint32_t s0=(ROR(wi_15,7))^(ROR(wi_15,18))^((wi_15)>>3);
        uint32_t s1=(ROR(wi_2,17))^(ROR(wi_2,19))^((wi_2)>>10);
        uint32_t w1=my_hash->mes_sche[i-16];
        uint32_t w2=my_hash->mes_sche[i-7];
        my_hash->mes_sche[i]=w2+s0+w1+s1;
        
    }
```
To compile the program here is the command I used
```cmd=

D:\file\openmp>gcc -g -O3 -march=native -fopt-info-vec-optimized -fopenmp-simd  -fopenmp par_hash_sha256.c -o par_hash_sha256.exe
par_hash_sha256.c:82:11: optimized: loop vectorized using 16 byte vectors
par_hash_sha256.c:82:11: optimized: loop vectorized using 8 byte vectors
par_hash_sha256.c:109:16: optimized: basic block part vectorized using 32 byte vectors
par_hash_sha256.c:109:16: optimized: basic block part vectorized using 32 byte vectors
par_hash_sha256.c:121:9: optimized: loop vectorized using 8 byte vectors
par_hash_sha256.c:159:18: optimized: basic block part vectorized using 32 byte vectors
par_hash_sha256.c:121:9: optimized: loop vectorized using 8 byte vectors
par_hash_sha256.c:121:9: optimized: loop vectorized using 8 byte vectors
```
It is clearly to see that compiler vectorizing some parts of code as well to speed up time for processing.
I compute 4204967 hashes and here is the result of time complexity in serial and parallel.
```cmd=
D:\file\openmp>gcc ser_hash_sha256.c -o ser_hash_sha256.exe
D:\file\openmp>ser_hash_sha256.exe
duration: 32.000000
D:\file\openmp>par_hash_sha256.exe
duration: 17.000000
```
It takes nearly 32 seconds to compute more than 4 million hashes while it takes approximatly 17 seconds using OpenMP in combination with vectorize code. Although that is impressive, it is not enough to compete with ASIC machine which can generate few Giga hashes per second. 