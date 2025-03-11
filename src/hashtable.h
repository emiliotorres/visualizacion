#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <R.h> // gcc -g -O2 -Wall -pipe -pedantic -I/usr/share/R/include -fpic  -fopenmp -c hashtable.c -o hashtable.o


#include <stdint.h> // uint32_t
#include <limits.h>

#ifdef _OPENMP
#include <omp.h>
#define omp_enabled true
#define max_thread omp_get_num_procs()
#define min_thread 1
#define OMP_PARALLEL_FOR(nth) _Pragma("omp parallel for num_threads(nth)")
#define OMP_SIMD _Pragma("omp simd")
#else
#define omp_enabled false
#define max_thread 1
#define min_thread 1
#define omp_get_thread_num() 0
#define OMP_PARALLEL_FOR(n)
#define OMP_SIMD
#endif


#define HASH(key, K) (3141592653U * (unsigned int)(key) >> (32 - (K))) 

#define HASH_INT1(key,K) (HASH((key),(K)))



/* Hash macro for TWO integer keys */
#define HASH_INT2(a, b, K) (                                            \
                             (((unsigned int)(a) * 0x9e3779b9u) ^       \
                              ((unsigned int)(b) * 0x85ebca6bu)) >> (32 - (K)) \
                             )

/* Hash macro for THREE integer keys */
#define HASH_INT3(a, b, c, K) (                                         \
                                                                        (((unsigned int)(a) * 0x9e3779b9u) ^ \
                                                                         ((unsigned int)(b) * 0x85ebca6bu) ^ \
                                                                         ((unsigned int)(c) * 0xc2b2ae35u)) >> (32 - (K)) \
                                                                        )

/* Hash macro for FOUR integer keys */
#define HASH_INT4(a, b, c, d, K) (                                      \
                                                                        (((unsigned int)(a) * 0x9e3779b9u) ^ \
                                                                         ((unsigned int)(b) * 0x85ebca6bu) ^ \
                                                                         ((unsigned int)(c) * 0xc2b2ae35u) ^ \
                                                                         ((unsigned int)(d) * 0x27d4eb2fu)) >> (32 - (K)) \
                                                                        )


union uno {
  double d;
  unsigned int u[2];
};


typedef enum {
  CHAR  = 9,    /* "scalar" string type (internal only)*/
  LGL   = 10,   /* logical vectors */
  INT   = 13,   /* integer vectors */
  DOUBLE    = 14,   /* real variables */
  CPL   = 15,   /* complex variables */
  STR   = 16   /* string vectors */
} KeyType;


typedef struct {
  KeyType type;
  size_t size;
  int* x;
} Key;

typedef struct {
  int* h; // Array of positions of x: x[h[id] - 1] = x[i], i=0,1,2,..,size
  int M; // Size of the hash table h. h[id], id=0,1,2,...,M
  int size; // Number of different values of x. The first position (plus 1) of each different value is stored in h.
  int K; // The number of bits to shift, K=log2(M). It is used in HASH function.
} HashTable;




void free_key_intern(Key* key);
void free_key(Key* key);
void free_key_vector(Key* key, int n); // Key* key = R_Calloc(n,Key);
void copy_from_to_key(const Key* from, Key* to);
void print_key(const Key* key);



void copy_from_to_hashtable(HashTable* from, HashTable** to);
void free_hashtable(HashTable* ht);
void free_hashtable_intern(HashTable* ht);

HashTable* create_hashtable(int* x, int n,int first);
HashTable* create_matrix_hashtable(int* x, int nrows, int ncols);
HashTable* create_key_hashtable(Key* key, int n, int first);

int find_position_hashtable(int value, int* x,  HashTable* ht, int notfound );
int find_position_matrix_hashtable(int* value, int* x, int nrows, int ncols, HashTable* ht, int notfound);
int find_position_key_hashtable(const Key* key, const Key* keys, HashTable* ht, int notfound);

void find_positions_for_R_hashtable(int* values, int n, int* result, int* x, HashTable* ht, int notfound);

int* get_positions_hashtable(HashTable* ht, int* n);

void print_hashtable(HashTable* ht);


uint32_t hash_for_table_uchar(const unsigned char *key, size_t length, unsigned int k);
uint32_t hash_for_table_int(const int *key, size_t length, unsigned int k);

#endif
