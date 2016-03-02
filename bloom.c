#include "bloom.h"
index_t hash1(bloom_filter_t *B, key_t k){
    return k*91 % B->size;
}
index_t hash2(bloom_filter_t *B, key_t k){
    return k*83 % B->size;
}
index_t hash_add(bloom_filter_t *B, key_t k, index_t n){
    return (hash1(B,k) + n * hash2(B,k))%(B->size);
}
void bloom_init(bloom_filter_t *B, index_t size_in_bits){
    B->size = size_in_bits;
    B->count = 0;
    if (B->size%64 == 0){
        B->table = (index_t *) malloc(sizeof(index_t)*(B->size)/64);
    }
    else {
        B->table = (index_t *) malloc(sizeof(index_t)*((B->size)/64 + 1));
    }
    /*setting every bit in the table to zero*/
    index_t i;
    for(i=0; i<sizeof(B->table)/sizeof(index_t); i++){
        (B->table)[i] = 0;
    }
}
void bloom_destroy(bloom_filter_t *B){
    free(B->table);
}
void set_bit(bloom_filter_t *B, index_t i){
    index_t val = B->table[i >>6];
    index_t idx_bit = i % 64;
    index_t shift = 1;
    val = val | (shift << idx_bit);
    B->table[i >> 6] = val;
}
index_t get_bit(bloom_filter_t *B, index_t i){
    index_t val = B->table[i >> 6];
    index_t idx_bit = i % 64;
    index_t shift = 1;
    index_t mask = shift << idx_bit;
    index_t bit = val & mask;
    return bit >> idx_bit;
}

int bloom_check(bloom_filter_t *B, key_t k){
    int check = 0;
    for (int i=0; i<N_HASHES; i++) {
        index_t bit_val = hash_add(B, k, i);
        check += get_bit(B, bit_val);
    }
    if (check == N_HASHES) {
        return 1;
    }
    return 0;
}
void bloom_add(bloom_filter_t *B, key_t k){
    int i;
    for (i=0; i<N_HASHES; i++) {
        index_t bit_val = hash_add(B, k, i);
        set_bit(B, bit_val);
    }  
    (B->count)++;

}



int *randarray(){
    int i;
    int *n = malloc(sizeof(int)*100);
    for (i = 0; i < 100; i++){
        n[i] = rand()%100000; 
    }
    return n;
}

void check_coll(int *r1, int *r2){
    index_t size = 1000;
    bloom_filter_t* k = (bloom_filter_t*) malloc(sizeof(bloom_filter_t));
    bloom_init(k, size);
    printf("Running overlap test\n");
    for(int i; i < 100; i++){
        bloom_add(k, r1[i]);
    }
    int count = 0;
    for(int i = 0; i < size; i++){
        count += get_bit(k,i);
    }
    printf("Bit count: %d\n", count);
    int collisions = 0;
    for (int i=0; i<100; i++) {
        collisions = collisions + bloom_check(k,r2[i]);
    }
    printf("Collisions: %d\n", collisions);
}
void print_hash_val(){
    index_t size = 100;
    index_t count = 0;
    bloom_filter_t* b2 = (bloom_filter_t*) malloc(sizeof(bloom_filter_t));
    bloom_init(b2, size);
    printf("0 - Hash 1: %llu, Hash 2: %llu\n", hash1(b2, 0), hash2(b2, 0));
    printf("1 - Hash 1: %llu, Hash 2: %llu\n", hash1(b2, 1), hash2(b2, 1));
    printf("2 - Hash 1: %llu, Hash 2: %llu\n", hash1(b2, 2), hash2(b2, 2));
    printf("3 - Hash 1: %llu, Hash 2: %llu\n", hash1(b2, 3), hash2(b2, 3));
    printf("13 - Hash 1: %llu, Hash 2: %llu\n", hash1(b2, 13), hash2(b2, 13));
    printf("97 - Hash 1: %llu, Hash 2: %llu\n", hash1(b2, 97), hash2(b2, 97));
    bloom_destroy(b2);
}
void tests(){
    /*Initializing bloom filter*/
    index_t size = 1000;
    index_t count = 0;
    bloom_filter_t* b = (bloom_filter_t*) malloc(sizeof(bloom_filter_t));
    bloom_init(b, size);
    
    print_hash_val();

    /*Adding integers*/
    for(int i = 0; i < 70; i++){
        bloom_add(b, i);
    }
    
    for(int i = 0; i < size; i++){
        count += get_bit(b,i);
    }
    printf("Bit count: %llu\n", count);
    
    int *rand1 = randarray();
	int *rand2 = randarray();
    check_coll(rand1, rand2);
    
    bloom_destroy(b);
    free(b);
    }
int main(){
    tests();
}
