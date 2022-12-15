# Parallelism with threading
In operating system, thread is the smallest unit of execution which is basically a sequence of programmed instructions that is possible to run independently. On both Linux and Window operating system, it is possible to use pthread library to do this in C/C++.  
```c=
#include <stdio.h>
#include <pthread.h> ///Include thread library
void *hello(char *words){
    printf("%s\n",words);
}
int main(){
    pthread_t t1,t2;
    char *s1="Hello";
    char *s2="World";
    /// Create new threads
    int r1 = pthread_create(&t1,NULL,hello,s1); 
    int r2 = pthread_create(&t2,NULL,hello,s2);
    /// Join child threads with parent thread 
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);
    return 0;
}
```
Doing this manually is a tedious task and error prone. Therefore, OpenMP is created to do this via "#pragma" in C/C++ and "!$" Fortran. In addition, OpenMP is multi-platform, which can be used GPU as well.
