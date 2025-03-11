#ifndef HASHMAP_H
#define HASHMAP_H

#include <Rinternals.h> // SEXP
#ifdef nrows // Rinternals.h define this!
#undef nrows
#endif
#ifdef ncols
#undef ncols
#endif
#ifdef match // Rinternals.h define this! Problems with omp.h
#undef match
#endif

#include "hashtable.h"

typedef struct  {
  int closed; // Closed =1: the size and  hash table ht are updated. Otherwise closed=0, you must close it before working.
  size_t length; // Length of key and value. it must be size <= length
  size_t size; // Number of valid elements. This number is only valid if it is closed. size = ht->size.
  Key* key; // Valid elements: key[i], i=0,..,size-1.  The rest key[size,...,length-1] are duplicated or NULLs.
  Key* value; // Valid elements: value[i] i=0,...,size-1
  HashTable* ht; // hash table of the valid keys key[0,...,size-1].
} HashMap;

void free_hashmap(HashMap* map);
HashMap* create_hashmap(int length );
void close_hashmap(HashMap* map);
int check_key_hashmap(const Key* key,  HashMap* map, int* pos);
int get_hashmap(const Key* key,  HashMap* map, Key* value);
int insert_hashmap(const Key* key, const Key* value, HashMap* map);
void print_hashmap(const HashMap* map, int full);
int size_hashmap(HashMap* map);

SEXP create_hashmap_SEXP(SEXP length_s);
HashMap* get_from_ptr_hashmap(SEXP map_ptr);
SEXP key_to_r(Key* key);
Key* vector_r_to_key(SEXP vector_s);
SEXP length_hashmap_SEXP(SEXP map_ptr);

#endif
