#include "hashtable.h"


/* xxHash constants */
const uint32_t PRIME1 = 2654435761U;
const uint32_t PRIME2 = 2246822519U;
const uint32_t PRIME3 = 3266489917U;
const uint32_t PRIME4 = 668265263U;
const uint32_t PRIME5 = 374761393U;


/**
 * Hashes the input key using an xxHash-inspired algorithm, optimized for speed and hash tables.
 * Returns a 32-bit hash value, which can be reduced to k bits for a hash table of size 2^k.
 * Optimized for keys of length 3 * sizeof(int) (e.g., 12 bytes), but supports any length.
 *
 * @param key    Pointer to the input key (byte array).
 * @param length Length of the input key in bytes.
 * @param seed   Seed value for the hash (use a constant for consistent hashing).
 * @return       32-bit hash value.
 */
uint32_t xx_hash_uchar(const unsigned char *key, size_t length, uint32_t seed) {
  if (key == NULL || length == 0) {
	return 0;
  }



  uint32_t hash;

  /* Fast path for keys of length 3 * sizeof(int) (e.g., 12 bytes) */
  if (length == 3 * sizeof(int)) {
	uint32_t v1 = seed + PRIME1 + PRIME2;
	uint32_t v2 = seed + PRIME2;
	uint32_t v3 = seed;

	/* Read three 4-byte blocks (handles alignment issues via memcpy) */
	uint32_t k1, k2, k3;
	memcpy(&k1, key, sizeof(uint32_t));
	memcpy(&k2, key + 4, sizeof(uint32_t));
	memcpy(&k3, key + 8, sizeof(uint32_t));

	/* Mix each block into the accumulators */
	v1 += k1 * PRIME2;
	v1 = (v1 << 13) | (v1 >> 19);
	v1 *= PRIME1;

	v2 += k2 * PRIME2;
	v2 = (v2 << 13) | (v2 >> 19);
	v2 *= PRIME1;

	v3 += k3 * PRIME2;
	v3 = (v3 << 13) | (v3 >> 19);
	v3 *= PRIME1;

	/* Combine accumulators */
	hash = (v1 << 1) | (v1 >> 31);
	hash += (v2 << 7) | (v2 >> 25);
	hash += (v3 << 12) | (v3 >> 20);
  }
  /* General case for other lengths */
  else if (length >= 16) {
	uint32_t v1 = seed + PRIME1 + PRIME2;
	uint32_t v2 = seed + PRIME2;
	uint32_t v3 = seed;
	uint32_t v4 = seed - PRIME1;

	/* Process the key in 16-byte blocks */
	const size_t block_size = 16;
	size_t num_blocks = length / block_size;
	const unsigned char *blocks = key;

	OMP_SIMD
	  for (size_t i = 0; i < num_blocks; i++) {
		uint32_t k1, k2, k3, k4;
		memcpy(&k1, blocks, sizeof(uint32_t));
		memcpy(&k2, blocks + 4, sizeof(uint32_t));
		memcpy(&k3, blocks + 8, sizeof(uint32_t));
		memcpy(&k4, blocks + 12, sizeof(uint32_t));
		blocks += block_size;

		v1 += k1 * PRIME2;
		v1 = (v1 << 13) | (v1 >> 19);
		v1 *= PRIME1;

		v2 += k2 * PRIME2;
		v2 = (v2 << 13) | (v2 >> 19);
		v2 *= PRIME1;

		v3 += k3 * PRIME2;
		v3 = (v3 << 13) | (v3 >> 19);
		v3 *= PRIME1;

		v4 += k4 * PRIME2;
		v4 = (v4 << 13) | (v4 >> 19);
		v4 *= PRIME1;
	  }

	hash = (v1 << 1) | (v1 >> 31);
	hash += (v2 << 7) | (v2 >> 25);
	hash += (v3 << 12) | (v3 >> 20);
	hash += (v4 << 18) | (v4 >> 14);
  } else {
	hash = seed + PRIME5;
  }

  /* Add length to the hash */
  hash += (uint32_t)length;

  /* Process any remaining bytes (tail) */
  const unsigned char *tail = key + (length & ~(size_t)15); /* Align to 16-byte boundary */
  size_t remaining_bytes = length % 16;

  while (remaining_bytes >= 4) {
	uint32_t k;
	memcpy(&k, tail, sizeof(uint32_t));
	hash += k * PRIME3;
	hash = (hash << 17) | (hash >> 15);
	hash *= PRIME4;
	tail += 4;
	remaining_bytes -= 4;
  }

  while (remaining_bytes > 0) {
	hash += (*tail) * PRIME5;
	hash = (hash << 11) | (hash >> 21);
	hash *= PRIME1;
	tail++;
	remaining_bytes--;
  }

  /* Finalization mix to ensure good distribution */
  hash ^= hash >> 15;
  hash *= PRIME2;
  hash ^= hash >> 13;
  hash *= PRIME3;
  hash ^= hash >> 16;

  return hash;
}

/**
 * Wrapper function to hash a key and reduce the hash to k bits for a hash table of size 2^k.
 *
 * @param key    Pointer to the input key (byte array).
 * @param length Length of the input key in bytes.
 * @param k      Number of bits for the hash table size (1 <= k <= 32).
 * @return       Hash value in the range [0, 2^k - 1], or 0 if input is invalid.
 */
uint32_t hash_for_table_uchar(const unsigned char *key, size_t length, unsigned int k) {
  /* Validate input parameters */
  if (key == NULL || length == 0 || k == 0 || k > 32) {
	return 0;
  }

  /* Use a fixed seed for consistent hashing */
  const uint32_t SEED = 0x9747b28c; /* Arbitrary seed value */

  /* Compute the full 32-bit hash */
  uint32_t hash = xx_hash_uchar(key, length, SEED);

  /* Reduce to k bits by taking the bottom k bits (better distribution for xxHash) */
  return hash & ((1U << k) - 1);
}


#include <stddef.h> /* For size_t */
#include <stdint.h> /* For uint32_t */



/**
 * Hashes the input key (an array of int) using an xxHash-inspired algorithm, optimized for speed and hash tables.
 * Returns a 32-bit hash value, which can be reduced to k bits for a hash table of size 2^k.
 * Optimized for keys of length 3 int elements, but supports any length.
 *
 * @param key    Pointer to the input key (array of int).
 * @param length Number of int elements in the input key.
 * @param seed   Seed value for the hash (use a constant for consistent hashing).
 * @return       32-bit hash value.
 */
uint32_t xx_hash_int(const int *key, size_t length, uint32_t seed) {
  if (key == NULL || length == 0) {
	return 0;
  }



  uint32_t hash;

  /* Fast path for keys of length 1 int element */
  if (length == 1) {
    hash = seed + PRIME5;
    hash += (uint32_t)key[0] * PRIME3;
    hash = (hash << 17) | (hash >> 15);
    hash *= PRIME4;
  }
  /* Fast path for keys of length 2 int elements */
  else if (length == 2) {
    uint32_t v1 = seed + PRIME1 + PRIME2;
    uint32_t v2 = seed + PRIME2;

    uint32_t k1 = (uint32_t)key[0];
    uint32_t k2 = (uint32_t)key[1];

    v1 += k1 * PRIME2;
    v1 = (v1 << 13) | (v1 >> 19);
    v1 *= PRIME1;

    v2 += k2 * PRIME2;
    v2 = (v2 << 13) | (v2 >> 19);
    v2 *= PRIME1;

    hash = (v1 << 1) | (v1 >> 31);
    hash += (v2 << 7) | (v2 >> 25);
  } else if (length == 3) { /* Fast path for keys of length 3 int elements */
	uint32_t v1 = seed + PRIME1 + PRIME2;
	uint32_t v2 = seed + PRIME2;
	uint32_t v3 = seed;

	/* Read the three int elements directly (no memcpy needed, as int* is aligned) */
	uint32_t k1 = (uint32_t)key[0];
	uint32_t k2 = (uint32_t)key[1];
	uint32_t k3 = (uint32_t)key[2];

	/* Mix each block into the accumulators */
	v1 += k1 * PRIME2;
	v1 = (v1 << 13) | (v1 >> 19);
	v1 *= PRIME1;

	v2 += k2 * PRIME2;
	v2 = (v2 << 13) | (v2 >> 19);
	v2 *= PRIME1;

	v3 += k3 * PRIME2;
	v3 = (v3 << 13) | (v3 >> 19);
	v3 *= PRIME1;

	/* Combine accumulators */
	hash = (v1 << 1) | (v1 >> 31);
	hash += (v2 << 7) | (v2 >> 25);
	hash += (v3 << 12) | (v3 >> 20);
  }
  /* General case for other lengths */
  else if (length >= 4) {
	uint32_t v1 = seed + PRIME1 + PRIME2;
	uint32_t v2 = seed + PRIME2;
	uint32_t v3 = seed;
	uint32_t v4 = seed - PRIME1;

	/* Process the key in blocks of 4 int elements */
	size_t num_blocks = length / 4;
	const int *blocks = key;

	OMP_SIMD
	  for (size_t i = 0; i < num_blocks; i++) {
		uint32_t k1 = (uint32_t)blocks[0];
		uint32_t k2 = (uint32_t)blocks[1];
		uint32_t k3 = (uint32_t)blocks[2];
		uint32_t k4 = (uint32_t)blocks[3];
		blocks += 4;

		v1 += k1 * PRIME2;
		v1 = (v1 << 13) | (v1 >> 19);
		v1 *= PRIME1;

		v2 += k2 * PRIME2;
		v2 = (v2 << 13) | (v2 >> 19);
		v2 *= PRIME1;

		v3 += k3 * PRIME2;
		v3 = (v3 << 13) | (v3 >> 19);
		v3 *= PRIME1;

		v4 += k4 * PRIME2;
		v4 = (v4 << 13) | (v4 >> 19);
		v4 *= PRIME1;
	  }

	hash = (v1 << 1) | (v1 >> 31);
	hash += (v2 << 7) | (v2 >> 25);
	hash += (v3 << 12) | (v3 >> 20);
	hash += (v4 << 18) | (v4 >> 14);
  } else {
	hash = seed + PRIME5;
  }

  /* Add length to the hash */
  hash += (uint32_t)length;

  /* Process any remaining int elements (tail) */
  const int *tail = key + (length & ~(size_t)3); /* Align to 4-int boundary */
  size_t remaining_elements = length % 4;

  while (remaining_elements > 0) {
	uint32_t k = (uint32_t)(*tail);
	hash += k * PRIME3;
	hash = (hash << 17) | (hash >> 15);
	hash *= PRIME4;
	tail++;
	remaining_elements--;
  }

  /* Finalization mix to ensure good distribution */
  hash ^= hash >> 15;
  hash *= PRIME2;
  hash ^= hash >> 13;
  hash *= PRIME3;
  hash ^= hash >> 16;

  return hash;
}

/**
 * Wrapper function to hash a key and reduce the hash to k bits for a hash table of size 2^k.
 *
 * @param key    Pointer to the input key (array of int).
 * @param length Number of int elements in the input key.
 * @param k      Number of bits for the hash table size (1 <= k <= 32).
 * @return       Hash value in the range [0, 2^k - 1], or 0 if input is invalid.
 */
uint32_t hash_for_table_int(const int *key, size_t lengthkey, unsigned int K) {
  /* Validate input parameters */
  if (key == NULL || lengthkey == 0 || K == 0 || K > 32) {
	return 0;
  }

  switch(lengthkey){
  case 1: {
	return HASH_INT1(key[0],K);
  }
  case 2: {
	return HASH_INT2(key[0] , key[1], K);
  }
  case 3: {
	return HASH_INT3(key[0], key[1], key[2], K);
  }
  case 4: {
	return HASH_INT4(key[0], key[1] , key[2] , key[3], K);
  }
  default: {
	
  }
  }

  /* Use a fixed seed for consistent hashing */
  const uint32_t SEED = 0x9747b28c; /* Arbitrary seed value */

  /* Compute the full 32-bit hash */
  uint32_t hash = xx_hash_int(key, lengthkey, SEED);

  /* Reduce to k bits by taking the bottom k bits (better distribution for xxHash) */
  return hash & ((1U << K) - 1);
}


// Initialize hash table
// x[i], i=0,1,2,...,n-1
// Store the first/last ocurrence
HashTable* create_hashtable(int* x, int n,int first) {
  if(n<1) {
	// Rf_warning("create_hashtable: You are trying to create a hash table with %d elements (n>0).\n",n);
    return NULL;
  }

  HashTable* ht = R_Calloc(1,HashTable);

  const size_t n2 = 2U * (size_t)n;
  size_t M = 256;
  int K = 8;
  while (M < n2) {
    M *= 2;
    K++;
  }

  ht->M = M;
  ht->K = K; // K=log2(M)
  uint32_t id = 0;
  size_t count=0;
  ht->h = R_Calloc(M, int); // Table to save the hash values, table has size M

  if(first){ // First element
    OMP_SIMD
      for (int i = 0; i <n; i++) {
		
		//id = hash_for_table_int(&x[i],1,K);

		//	id = HASH(x[i], ht->K);
		id = HASH_INT1(x[i],K);
        while(ht->h[id]) {
          if(x[ht->h[id]-1] == x[i]) goto ibl;
          if(++id >= M) id = 0;
        }
        ht->h[id] = i + 1;
        count++;
      ibl:;
      }
  } else { // Last element
    OMP_SIMD
      for (int i = n-1; i > -1; i--) {
		//id = HASH(x[i], ht->K);
		id = HASH_INT1(x[i],K);
		//id = hash_for_table_int(&x[i],1,K);
		while(ht->h[id]) {
          if(x[ht->h[id]-1] == x[i]) goto ibl_last;
          if(++id >= M) id = 0;
        }
        ht->h[id] = i + 1;
        count++;
      ibl_last:;
      }
  }
  ht->size = count;
  return ht;
}


// Initialize hash table
HashTable* create_matrix_hashtable(int* x, int nrows, int ncols) {
  if(nrows<1 || ncols <1) {
    //Rf_warning("create_hashtable: You are trying to create a hash table of a matrix with %d rows and %d cols (nrows>0, ncols>0).\n",nrows, ncols);
    return NULL;
  }

  int n= nrows;
  HashTable* ht = R_Calloc(1,HashTable);

  const size_t n2 = 2U * (size_t)n;
  size_t M = 256;
  int K = 8;
  while (M < n2) {
    M *= 2;
    K++;
  }

  ht->M = M;
  ht->K = K;
  uint32_t id = 0;
  size_t count=0;
  ht->h = R_Calloc(M, int); // Table to save the hash values, table has size M
  int key[ncols];
  const int lengthkey =ncols;
  if(lengthkey <= 4){
  OMP_SIMD
    for (int i = 0; i < n; i++) {
      //unsigned key = 0;
      for (int j = 0; j < ncols; ++j) {
        int k= i + j*nrows;
		key[j] = x[k];
	  }
	  id=0;
	  switch(lengthkey){
	  case 1: {
		id= HASH_INT1(key[0],K);
		break;
	  }
	  case 2: {
		id= HASH_INT2(key[0] , key[1], K);
		break;
	  }
	  case 3: {
		id= HASH_INT3(key[0], key[1], key[2], K);
		break;
	  }
	  case 4: {
		id= HASH_INT4(key[0], key[1] , key[2] , key[3], K);
		break;
	  }
	  default: {
		//id = hash_for_table_int(key,lengthkey,K);
	  }
	  }	  
      while (ht->h[id]) { // While this slot is occupied
        // Check if the row of this slot is equal to current row.
        for (int j = 0; j < ncols; ++j) {
          if (x[ (ht->h[id] - 1) + j * nrows] != x[i + j * nrows]) {
            goto labelmi1; // Rows are different.
          }
        }
        goto ibl; // Rows are equal. Go to to another row.
      labelmi1:; // Rows are different.
        if(++id >= ht->M) id = 0;
      } // This slot is empty. And rows are different.
      ht->h[id] = i + 1;
      count++;
    ibl:; // Rows are equal. Do nothing.
    }
  } else { // lengthkey>4
	OMP_SIMD
	for (int i = 0; i < n; i++) {
      //unsigned key = 0;
      for (int j = 0; j < ncols; ++j) {
        int k= i + j*nrows;
		key[j] = x[k];
	  }
	  id=0;
	  id = hash_for_table_int(key,lengthkey,K);
	  
	    
      while (ht->h[id]) { // While this slot is occupied
        // Check if the row of this slot is equal to current row.
        for (int j = 0; j < ncols; ++j) {
          if (x[ (ht->h[id] - 1) + j * nrows] != x[i + j * nrows]) {
            goto labelmi1_4; // Rows are different.
          }
        }
        goto ibl_4; // Rows are equal. Go to to another row.
      labelmi1_4:; // Rows are different.
        if(++id >= ht->M) id = 0;
      } // This slot is empty. And rows are different.
      ht->h[id] = i + 1;
      count++;
    ibl_4:; // Rows are equal. Do nothing.
    }
	
  }
  
  ht->size = count;
  return ht;
}




int find_position_hashtable(int value, int* x,  HashTable* ht, int notfound ){
  if(ht==NULL) {
    //Rf_warning("find_position_using_hashtable: You are trying to use a NULL hashtable.\n");
    return notfound;
  }

  uint32_t id = 0;
  int ans = notfound;
  //id = hash_for_table_int(&value,1,ht->K);
  // id = HASH(value, ht->K);
  id = HASH_INT1(value,ht->K);
  int* h = ht->h;
  int M = ht->M;
  while(h[id]) {
    if(x[h[id]-1] == value) {
      ans = h[id]-1;
      goto ibl2;
    }
    if(++id >= M) id = 0;
  }
  ans = notfound;
 ibl2:;
  return ans;

}

void find_positions_for_R_hashtable(int* keys, int n, int* result, int* x, HashTable* ht, int notfound){
  if(ht==NULL) {
    //Rf_warning("find_position_using_hashtable: You are trying to use a NULL hashtable.\n");
    OMP_SIMD
      for(int i =0; i < n; i++){
        result[i] = notfound;
      }
    return;
  }
  int* h = ht->h;
  int M = ht->M;
  int K = ht->K;

  OMP_SIMD
    for (int i = 0; i != n; ++i) {
	  //uint32_t id = hash_for_table_int(&values[i],1,K);
      //uint32_t id = HASH(values[i], K);
	  uint32_t id = HASH_INT1(keys[i],K);
      while(h[id]) {
        if(x[h[id]-1] == keys[i]) {
          result[i] = ht->h[id]; // R format
          goto ibl2;
        }
        if(++id >= M) id = 0;
      }
      result[i] = notfound;
    ibl2:;
    }


}



int find_position_matrix_hashtable(int* key, int* x, int nrows, int ncols, HashTable* ht, int notfound){
  // x is a matrix of ncols.
  // value is an array of length ncols

  
  if(ht==NULL) {
    //Rf_warning("find_position_using_hashtable: You are trying to use a NULL hashtable.\n");
    return notfound;
  }

  //Rprintf("Estoy en find_position_using_hashtable\n");
  int ans = notfound;
  int K= ht->K;
  int* h = ht->h;
  int M = ht->M;
  uint32_t id;
  const int lengthkey = ncols;

  id=0;
  switch(lengthkey){
  case 1: {
	id= HASH_INT1(key[0],K);
	break;
  }
  case 2: {
	id= HASH_INT2(key[0] , key[1], K);
	break;
  }
  case 3: {
	id= HASH_INT3(key[0], key[1], key[2], K);
	break;
  }
  case 4: {
	id= HASH_INT4(key[0], key[1] , key[2] , key[3], K);
	break;
  }
  default: {
	id = hash_for_table_int(key,lengthkey,K);
  }
  }	  

  

  //uint32_t id = hash_for_table_int(value,ncols,K);
  while (h[id]) { // While this slot is occupied
    // Check if the row of this slot is equal to current row.
    //Rprintf("\nFila %d: ",ht->h[id]-1);
    for (int j = 0; j < ncols; ++j) {
      //Rprintf("%d (%d, %d), ",x[ (ht->h[id] - 1) + j * nrows],(ht->h[id] - 1),j);
      if (x[ (h[id] - 1) + j * nrows] != key[j]) {
        goto labelmi1; // Rows are different.
      }
    }
    // Aqui son iguales;
    ans = h[id]-1;
    return ans; //goto ibl; // Rows are equal. Go to to another row.
  labelmi1:; // Rows are different.
    if(++id >= M) id = 0;
  } // This slot is empty. And rows are different.
  ans = notfound;
  //ibl:; // Rows are equal.
  return ans;


}


void free_hashtable_intern(HashTable* ht) {
  if (ht == NULL) return;

  // Free the hash table array
  if (ht->h != NULL) {
    R_Free(ht->h);
  }
  ht->size = 0;
  ht->M=0;
  ht->K=0;

}

// Free hash table memory
void free_hashtable(HashTable* ht) {
  free_hashtable_intern(ht);
  // Free the hash table structure itself
  R_Free(ht);
}


void copy_from_to_hashtable(HashTable* from, HashTable** to) {
  // If source and destination are the same, do nothing
  if (from == *to) {
    return;
  }

  // If source is NULL, free destination and set to NULL
  if (from == NULL) {
    free_hashtable(*to);
    *to = NULL;

    return;
  }

  // Allocate destination if NULL
  if (*to == NULL) {
    *to = R_Calloc(1, HashTable);
  }

  // Handle the array h
  if ((*to)->h == NULL) {
    // First allocation
    (*to)->h = R_Calloc(from->M, int);
  } else if ((*to)->M != from->M) {
    // Reallocation needed - size changed
    (*to)->h = R_Realloc((*to)->h, from->M, int);
  }
  // else - existing array is correct size, reuse it

  // Copy metadata
  (*to)->M = from->M;
  (*to)->K = from->K;
  (*to)->size = from->size;

  // Copy array data
  memcpy((*to)->h, from->h, from->M * sizeof(int));
}




int* get_positions_hashtable(HashTable* ht, int* n){

  if(ht==NULL){
    *n= 0;
    return NULL;
  }

  *n = ht->size;
  int* positions = R_Calloc(ht->size,int);
  int count = 0;
  int* h = ht->h;
  int M = ht->M;
  OMP_SIMD
    for(int i=0; i< M;i++){
      if(h[i]) positions[count++] = h[i]-1;
    }
  return positions;
}

// ============================================================
// ============================================================



void print_key(const Key* key) {
  if (key == NULL) {
	Rprintf("Key: NULL\n");
	return;
  }

    
  // Print basic key information
  Rprintf("Type: ");
    
  // Print type with descriptive text
  switch(key->type) {
  case CHAR:   Rprintf("CHAR (9)"); break;
  case LGL:    Rprintf("LGL (10)"); break;
  case INT:    Rprintf("INT (13)"); break;
  case DOUBLE: Rprintf("DOUBLE (14)"); break;
  case CPL:    Rprintf("CPL (15)"); break;
  case STR:    Rprintf("STR (16)"); break;
  default:     Rprintf("UNKNOWN (%d)", key->type); break;
  }
  

  // Print size
  Rprintf(" Size: %zu,", key->size);

  // Print array contents if exists
  if (key->x != NULL && key->size > 0) {
	Rprintf(" Data: [");
	for (size_t i = 0; i < key->size; i++) {
	  Rprintf("%d", key->x[i]);
	  if (i < key->size - 1) Rprintf(", ");
	}
	Rprintf("]\n");
  } else {
	Rprintf("  Data: NULL or empty\n");
  }
}

void print_hashtable(HashTable* ht) {
  if (ht == NULL) {
	Rprintf("HashTable: NULL\n");
	return;
  }

  // Print basic hash table information
  Rprintf("HashTable: M %d, size %d, K %d: [", ht->M,ht->size,ht->K);
  for (int i = 0; i < ht->M; i++) {
	if(ht->h[i]) {
	  Rprintf("%d (%d)", ht->h[i],i);
	if (i < ht->M - 1) Rprintf(", ");
	}
  }
  Rprintf("]\n");
}



int find_position_key_hashtable(const Key* key, const Key* keys, HashTable* ht, int notfound) {
  // Validate input
  if (ht == NULL) {
	return notfound;
  }

  int K = ht->K;
  int* h = ht->h;
  int M = ht->M;
  uint32_t id;

  // Determine hashing method based on key length
 

  // Hash the key
	switch(key->size) {
  case 1: {
	id = HASH_INT1(key->x[0], K);
	break;
  }
  case 2: {
	id = HASH_INT2(key->x[0], key->x[1], K);
	break;
  }
  case 3: {
	id = HASH_INT3(key->x[0], key->x[1], key->x[2], K);
	break;
  }
  case 4: {
	id = HASH_INT4(key->x[0], key->x[1], key->x[2], key->x[3], K);
	break;
  }
  default: {
	id = hash_for_table_int(key->x, key->size, K);
  }
  }

  // Collision resolution and key matching
  while (h[id]) { // While this slot is occupied
	// Get the index of the existing key in x
	int existing_index = h[id] - 1;

	
	// Compare key types first
	if ( keys[existing_index].type != key->type) {
	  goto next_slot_key;
	}

	// Compare key sizes
	if ( keys[existing_index].size != key->size) {
	  goto next_slot_key;
	}

	// Compare key contents // size must be equal!
	int is_match = 1;
	for (int j = 0; j < key->size; ++j) {
	  if (keys[existing_index].x[j] != key->x[j]) {
		is_match = 0;
		goto next_slot_key;
		break;
	  }
	}

	// If all comparisons match, return the index
	if (is_match) {
	  return existing_index;
	}

  next_slot_key:
	// Move to next slot
	if (++id >= M) id = 0;
  }

  // Key not found
  return notfound;
}

void free_key_intern(Key* key){
  if(key==NULL) return;
  if(key->x) R_Free(key->x);
  key->size = 0;
}
void free_key(Key* key){
  if(key==NULL) return;
  free_key_intern(key);
  R_Free(key);
  return;
}
void free_key_vector(Key* key, int n){
  if(key==NULL) return;
  for(int i =0; i<n;i++){
	free_key_intern(&key[i]);
  }
  R_Free(key);
}


HashTable* create_key_hashtable(Key* key, int n, int first) {
  // Validate input
  if (n < 1) {
	//Rf_warning("create_key_hashtable: You are trying to create a hash table with %d keys (n>0).\n", n);
	return NULL;
  }

  HashTable* ht = R_Calloc(1, HashTable);

  // Calculate hash table size
  const size_t n2 = 2U * (size_t)n;
  size_t M = 256;
  int K = 8;
  while (M < n2) {
	M *= 2;
	K++;
  }

  ht->M = M;
  ht->K = K;
  ht->h = R_Calloc(M, int);  // Table to save the hash values
    
  uint32_t id = 0;
  size_t count = 0;

  // Hashing based on the int array in each Key
  if(first){
  for (int i = 0; i < n; i++) {
	// Skip if current key has no int array
	if (key[i].x == NULL || key[i].size == 0) continue;

	id = 0;
	// Choose hashing method based on key size
	switch(key[i].size) {
	case 1: {
	  id = HASH_INT1(key[i].x[0], K);
	  break;
	}
	case 2: {
	  id = HASH_INT2(key[i].x[0], key[i].x[1], K);
	  break;
	}
	case 3: {
	  id = HASH_INT3(key[i].x[0], key[i].x[1], key[i].x[2], K);
	  break;
	}
	case 4: {
	  id = HASH_INT4(key[i].x[0], key[i].x[1], key[i].x[2], key[i].x[3], K);
	  break;
	}
	default: {
	  // For keys longer than 4, use a general hash function
	  id = hash_for_table_int(key[i].x, key[i].size, K);
	}
	}

	// Handle hash collisions
	while (ht->h[id]) {
	  // Check if the existing key is the same as current key
	  int is_same = 1;
	  Key* existing_key = &key[ht->h[id] - 1];
            
	  // Compare key types first
	  if (existing_key->type != key[i].type) {
		is_same = 0;
	  } 
	  // Then compare sizes
	  else if (existing_key->size != key[i].size) {
		is_same = 0;
	  } 
	  // Then compare actual data
	  else {
		for (size_t j = 0; j < key[i].size; j++) {
		  if (existing_key->x[j] != key[i].x[j]) {
			is_same = 0;
			break;
		  }
		}
	  }

	  if (is_same) {
		// Key already exists, break out of collision resolution
		break;
	  }

	  // Move to next slot
	  if (++id >= ht->M) id = 0;
	}

	// If slot is empty, insert the key
	if (ht->h[id] == 0) {
	  ht->h[id] = i + 1;  // Store 1-based index
	  count++;
	}
  }
  } else {

	for (int i = n-1; i > -1; i--) {
	  // Skip if current key has no int array
	  if (key[i].x == NULL || key[i].size == 0) continue;

	  id = 0;
	  // Choose hashing method based on key size
	  switch(key[i].size) {
	  case 1: {
		id = HASH_INT1(key[i].x[0], K);
		break;
	  }
	  case 2: {
		id = HASH_INT2(key[i].x[0], key[i].x[1], K);
		break;
	  }
	  case 3: {
		id = HASH_INT3(key[i].x[0], key[i].x[1], key[i].x[2], K);
		break;
	  }
	  case 4: {
		id = HASH_INT4(key[i].x[0], key[i].x[1], key[i].x[2], key[i].x[3], K);
		break;
	  }
	  default: {
		// For keys longer than 4, use a general hash function
		id = hash_for_table_int(key[i].x, key[i].size, K);
	  }
	  }

	  // Handle hash collisions
	  while (ht->h[id]) {
		// Check if the existing key is the same as current key
		int is_same = 1;
		Key* existing_key = &key[ht->h[id] - 1];
            
		// Compare key types first
		if (existing_key->type != key[i].type) {
		  is_same = 0;
		} 
		// Then compare sizes
		else if (existing_key->size != key[i].size) {
		  is_same = 0;
		} 
		// Then compare actual data
		else {
		  for (size_t j = 0; j < key[i].size; j++) {
			if (existing_key->x[j] != key[i].x[j]) {
			  is_same = 0;
			  break;
			}
		  }
		}

		if (is_same) {
		  // Key already exists, break out of collision resolution
		  break;
		}

		// Move to next slot
		if (++id >= ht->M) id = 0;
	  }

	  // If slot is empty, insert the key
	  if (ht->h[id] == 0) {
		ht->h[id] = i + 1;  // Store 1-based index
		count++;
	  }
	}
	
  }

  ht->size = count;
  return ht;
}


void copy_from_to_key(const Key* from, Key* to) {

  if(from == to) {
	return;
  }
  if (from == NULL || to == NULL || from == to)
	return;

  if(to == NULL){
	to = R_Calloc(1, Key);
  }

  
   // Copy basic properties
  to->type = from->type;
  to->size = from->size;

  // Handle 'x' array
  if (from->x != NULL) {
	to->x = R_Realloc(to->x,from->size, int);
	if (to->x == NULL) {
	  // Allocation failed: Set size to 0 to reflect invalid data
	  to->size = 0;
	  return;
	}
	memcpy(to->x, from->x, from->size * sizeof(int));
  } else {
	to->x = NULL;
  }

}





// ============================================================
// ============================================================



