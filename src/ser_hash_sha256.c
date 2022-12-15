///File of implementing sha256 in c
#include<stdio.h>
#include<string.h>
#include<stdint.h>
#include<stdlib.h>
#include<time.h>
#define NUM 512///Constant number for pre-processing: 512 bits
#define max_len_str 1024
#define MOD(a,b) (a&(b-1)) ///Modulo of a and b or r=a%b Note, b must be power of 2
#define ROR(a,b) ((a)>>(b))|((a)<<(32-b )) ///Right rotate a by b-bits for 32-bits type
#define pow32 4924967296
typedef struct block{
    unsigned char input[max_len_str+1];
    uint64_t len_input;
   
    uint32_t*data;//data to process
    uint32_t num_block;

    uint32_t h0,h1,h2,h3,h4,h5,h6,h7;
    uint32_t mes_sche[64];

}
block;
const uint32_t k[64]={
        0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
        0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
        0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
        0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
        0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
        0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
        0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
        0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};
const uint32_t h[8]={
        0x6a09e667,
        0xbb67ae85,
        0x3c6ef372,
        0xa54ff53a,
        0x510e527f,
        0x9b05688c,
        0x1f83d9ab,
        0x5be0cd19
};
/// @brief This function is pre-processing the input 
/// @param my_hash 
void pre_pro(block*my_hash){
    my_hash->len_input=strlen(my_hash->input);
    uint64_t ori_len= strlen(my_hash->input);
    char padding=MOD(my_hash->len_input+1,4);
    uint16_t len=(my_hash->len_input+1)/4;
    my_hash->input[my_hash->len_input]=0x80;//Padding 1 bit at the end of input
    int num=1;//Number of chunk
    int bit_len=my_hash->len_input*8+64;
    while(bit_len>(512*num)){
        num++;//Find number of chunk size of 512 bits
    }
    int num_arr=512*num/32;//Calculate number of element with size of 32-bits for number of chunk
    uint32_t *new_arr=(uint32_t*)malloc(num_arr*sizeof(uint32_t));
    if(new_arr==NULL){
        perror("\nCannot create new block");
        return ;
    }
    my_hash->num_block=num_arr;
    
    //Last 2 members must contain number indicating length of input
    uint32_t try=((ori_len)*8);
    uint32_t try1=(((ori_len)*8)>>32);
   
    new_arr[num_arr-1]=try;
    new_arr[num_arr-2]=try1;
    int j=0;
    int i=0;
    for(;i<=ori_len;i+=4){
        
        uint32_t a=my_hash->input[i];
        uint32_t b=my_hash->input[i+1];
        uint32_t c=my_hash->input[i+2];
        uint32_t d=my_hash->input[i+3];
        new_arr[j]=(a<<24)|(b<<16)|(c<<8)|d;
        j++;
        
    }
    switch (padding)
    {
    case 1:
        new_arr[j]=((my_hash->input[i])<<24);
        break;
    case 2:
        new_arr[j]=((my_hash->input[i])<<24)|((my_hash->input[i+1])<<16);
        break;
    case 3:
        new_arr[j]=((my_hash->input[i])<<24)|((my_hash->input[i+1])<<16)|((my_hash->input[i+2]<<8));
        break;
    default:
        break;
    }
    my_hash->data=new_arr;


    my_hash->h0=h[0];
    my_hash->h1=h[1];
    my_hash->h2=h[2];
    my_hash->h3=h[3];
    my_hash->h4=h[4];
    my_hash->h5=h[5];
    my_hash->h6=h[6];
    my_hash->h7=h[7];
}
void process(block*my_hash){
    for(int i=16;i<64;i++){
        uint32_t wi_15=my_hash->mes_sche[i-15];
        uint32_t wi_2=my_hash->mes_sche[i-2];
        uint32_t s0=(ROR(wi_15,7))^(ROR(wi_15,18))^((wi_15)>>3);
        uint32_t s1=(ROR(wi_2,17))^(ROR(wi_2,19))^((wi_2)>>10);
        uint32_t w1=my_hash->mes_sche[i-16];
        uint32_t w2=my_hash->mes_sche[i-7];
        my_hash->mes_sche[i]=w2+s0+w1+s1;
        
    }
}
void compress(block*my_hash){
    uint32_t a,b,c,d,e,f,g,h;
    a=my_hash->h0;
    b=my_hash->h1;
    c=my_hash->h2;
    d=my_hash->h3;
    e=my_hash->h4;
    f=my_hash->h5;
    g=my_hash->h6;
    h=my_hash->h7;
    for(int i=0;i<64;i++){
        uint32_t s1=(ROR(e,6))^(ROR(e,11))^(ROR(e,25));
        uint32_t ch=((e)&(f))^((~(e))&(g));
        uint32_t temp1=(h)+(s1)+(ch)+(k[i])+(my_hash->mes_sche[i]);
        uint32_t s0=(ROR(a,2))^(ROR(a,13))^(ROR(a,22));
        uint32_t maj=((a)&(b))^((a)&(c))^((b)&(c));
        uint32_t temp2=s0+maj;
        
        h=g;
        g=f;
        f=e;
        e=d+temp1;
        d=c;
        c=b;
        b=a;
        a=temp1+temp2;
    }
    (my_hash->h0)+=(a);
    (my_hash->h1)+=(b);
    (my_hash->h2)+=(c);
    (my_hash->h3)+=(d);
    (my_hash->h4)+=(e);
    (my_hash->h5)+=(f);
    (my_hash->h6)+=(g);
    (my_hash->h7)+=(h);


}
void cal_hash(block*my_hash){

    int loop=my_hash->num_block/16;
    int k=0;
    for(int i=0;i<loop;i++) {
        for (int j = 0; j < 64; j++) {
            if (j <= 15) {
                my_hash->mes_sche[j] = my_hash->data[k];
                k++;
            } else {
                my_hash->mes_sche[j] = 0;
            }
        }
        process(my_hash);
        compress(my_hash);
    }

}
void display(block*my_hash){
    printf("\nReturn hash: ");
    printf("%x",my_hash->h0);
    printf("%x",my_hash->h1);
    printf("%x",my_hash->h2);
    printf("%x",my_hash->h3);
    printf("%x",my_hash->h4);
    printf("%x",my_hash->h5);
    printf("%x",my_hash->h6);
    printf("%x",my_hash->h7);

}
int main(void){
    block my_hash;
    clock_t s=clock();
    for(uint32_t i=0;i<4204967;i++){
        memset(my_hash.input,0,max_len_str);
        for(int i=0;i<300;i++){
        my_hash.input[i]='a';
        }
        pre_pro(&my_hash);
        cal_hash(&my_hash);
        free(my_hash.data);
    }
    clock_t e=clock();
    double duration=(e-s)/CLOCKS_PER_SEC;
    printf("\nduration: %lf",duration);

    return 0;
}