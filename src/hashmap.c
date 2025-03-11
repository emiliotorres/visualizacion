/*
  TITLE:
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-03-04 11:08 emilio on emilio-XPS-15-9570>
*/


#include "hashmap.h"


void free_hashmap_intern(HashMap* map){
  if(map == NULL) return;
  for(size_t i =0; i< map->length; i++){

    free_key_intern(&map->key[i]);
    free_key_intern(&map->value[i]); 
  }
  if(map->key) R_Free(map->key);
  if(map->value) R_Free(map->value);
  if(map->ht) free_hashtable(map->ht);
  map->length = 0;
  map->size =0;
  map->closed = 0;
  //R_Free(map);
  return;

}

void free_hashmap(HashMap* map){
  free_hashmap_intern(map);
  R_Free(map);
  return;

}



HashMap* create_hashmap(int length ){

  HashMap* map = R_Calloc(1, HashMap);
  map->closed = 0;
  map->length = length;
  map->size = 0;
  map->key = R_Calloc(map->length, Key);
  map->value = R_Calloc(map->length, Key);
  map->ht = NULL; // Initialize hash table

  return map;
}


Key* vector_r_to_key(SEXP vector_s) {
  /*
   * Converts an R vector to a Key struct.
   *
   * Args:
   *   r_vector: An SEXP representing an R integer vector.
   *
   * Returns:
   *   A pointer to a dynamically allocated Key struct containing the data
   *   from the R vector.  Returns NULL if the input is not an integer vector.
   *   The caller is responsible for freeing the allocated memory.
   */


  size_t size = LENGTH(vector_s);
  Key* key = R_Calloc(1,Key); // Allocate memory for the Key struct
  key->type = TYPEOF(vector_s);


  switch(key->type){
  case LGL:
  case INT: {
    key->size = size;
    key->x =  R_Calloc(key->size,int);
    memcpy(key->x,INTEGER(vector_s), key->size*sizeof(int));
    break;
  }
  case DOUBLE: {

    key->size = 2*size;
    key->x = R_Calloc(key->size,int);
    union uno tpv;
    OMP_SIMD
      for (R_xlen_t j = 0; j < size; ++j) {
        tpv.d = REAL(vector_s)[j];
        key->x[2*j] = tpv.u[0];
        key->x[2*j+1] = tpv.u[1];
      }

    break;
  }
  case STR: {

    size_t total_length = 0;
    OMP_SIMD
      for (R_xlen_t j = 0; j < size; ++j) {
        const char* str = CHAR(STRING_ELT(vector_s, j)); // Get the i-th string
        total_length += strlen(str) + 1;
      }
    // Step 2: Calculate padding to make the length a multiple of sizeof(int)
    size_t padding = (sizeof(int) - (total_length % sizeof(int))) % sizeof(int);
    size_t padded_length = total_length + padding;
    char* concatenated = R_Calloc(padded_length,char);
    // Step 4: Concatenate all strings into the buffer
    size_t offset = 0;
    OMP_SIMD
      for (int j = 0; j < size; j++) {
        const char* str = CHAR(STRING_ELT(vector_s,  j));
        size_t len = strlen(str);
        memcpy(concatenated + offset, str, len);
        offset += len;
        memset(concatenated + offset, 0, 1);
        offset += 1;

      }
    // Step 5: Add padding (null characters)
    memset(concatenated + offset, 0, padding);
    // Step 6: Convert the padded string to a vector of integers
    size_t num_ints = padded_length / sizeof(int);
    key->size = num_ints;
    key->x =  R_Calloc(key->size,int);
    memcpy(key->x,concatenated,num_ints*sizeof(int));
    R_Free(concatenated);

    break;
  }
  default:{
    free_key(key);
	// Rf_warning("Type of x (%s) not suppported.", Rf_type2char(TYPEOF(vector_s)));

  }

  }


  return key;
}


SEXP key_to_r(Key* key) {
  /*
   * Converts a Key struct back to an R vector.
   *
   * Args:
   *   key: A pointer to a Key struct.
   *
   * Returns:
   *   An SEXP representing an R vector containing the data from the Key struct.
   *   Returns NULL if the input Key struct is invalid.
   */

  if (key == NULL) {
    return R_NilValue;
  }


  SEXP vector_s;

  int protecti=0;
  switch (key->type) {
  case LGL:
  case INT:{
    vector_s=PROTECT(Rf_allocVector(INTSXP, key->size));protecti++;
    memcpy(INTEGER(vector_s),key->x,key->size*sizeof(int));
    break;
  }
  case DOUBLE:{
    vector_s=PROTECT(Rf_allocVector(REALSXP,key->size/2));protecti++;
    memcpy(REAL(vector_s),key->x,key->size*sizeof(int));
    break;
  }
  case STR:{

    // Calculate the total length of the concatenated string
    size_t total_length = key->size * sizeof(int);
    char* concatenated = (char*)key->x;

    // Split the concatenated string into individual strings
    size_t num_strings = 0;
    size_t offset = 0;
    while (offset < total_length) {
      size_t len = strlen(concatenated + offset);
      if (len == 0) break;
      num_strings++;
      offset += len + 1; // Move to the next string
    }

    // Allocate an R character vector
    vector_s = PROTECT(allocVector(STRSXP, num_strings));protecti++;
    offset = 0;
    for (size_t i = 0; i < num_strings; i++) {
      size_t len = strlen(concatenated + offset);
      SET_STRING_ELT(vector_s, i, mkChar(concatenated + offset));
      offset += len + 1;
    }

    break;
  }
  default:
    //Rf_warning("Unsupported Key type.");
    return R_NilValue;
  }

  UNPROTECT(protecti);
  return vector_s;
}






void print_hashmap(const HashMap* map, int full) {
  if (map == NULL) {
    Rprintf("HashMap object: NULL\n");
    return;
  }
  Rprintf("HashMap object:\n");
  Rprintf("  Closed: %d\n", map->closed);
  Rprintf("  Length: %d\n", map->length);
  Rprintf("  Size: %d\n", map->size);
  if(full){
    Rprintf("  Key-Value pairs:\n");
    for (int i = 0; i < map->size; i++) {
      if (map->key[i].x != NULL) {
        Rprintf("    Key %d: ",i);
        print_key(&map->key[i]);

        Rprintf("    Value %d: ",i);
        print_key(&map->value[i]);

      }
    }
    Rprintf("  ");
    print_hashtable(map->ht);
  }
}


// ============================================================

void close_hashmap(HashMap* map){
  if(map->closed) return;

  HashTable* newht = create_key_hashtable(map->key, map->size, 0); // Reverse order. The last duplicated elements are the most important.

  //hmm, aquí copio todo para que esté en los primeros puestos. Pero no me convence mucho.
  // No sería mejor meter un vector de posiciones?
  // Vale, paso de optimizar.


  int newsize=0;
  int* positions = get_positions_hashtable( newht, &newsize); // positions of no duplicated keys. (in reverse order)
  free_hashtable(newht);

  // Put that keys in the first places [0,...,newsize-1]
  Key* newkey = R_Calloc(newsize,Key);
  Key* newvalue = R_Calloc(newsize,Key);
  OMP_SIMD
    for(int i=0; i<newsize; i++){
      int pos = positions[i];
      copy_from_to_key( &map->key[pos], &newkey[i] );
      copy_from_to_key( &map->value[pos], &newvalue[i] );
    }
  OMP_SIMD
    for(int i=0; i<newsize; i++){
      copy_from_to_key( &newkey[i], &map->key[i]  );
      copy_from_to_key( &newvalue[i], &map->value[i]  );
    }


  free_key_vector(newkey,newsize);
  free_key_vector(newvalue,newsize);
  R_Free(positions);

  // Valid keys are at the begining of key.
  // Update the hashtable.
  map->size = newsize;
  HashTable* newht2 = create_key_hashtable(map->key, map->size, 0);
  free_hashtable_intern(map->ht);
  copy_from_to_hashtable(newht2, &map->ht);
  free_hashtable(newht2);

  map->closed =1;
}


int check_key_hashmap(const Key* key, HashMap* map, int* pos){

  if(!map->closed) close_hashmap(map);
  if(map->ht == NULL) return 0;
  int notfound = -1;
  *pos = find_position_key_hashtable(key, map->key,  map->ht, notfound);
  if(*pos == notfound){
    return 0;
  }

  return 1;
}

int get_hashmap(const Key* key,  HashMap* map, Key* value){
  int pos;
  int check =  check_key_hashmap( key, map, &pos);
  if(check) {
    copy_from_to_key(&map->value[pos], value);
    return 1;
  } else {
    return 0;
  }
}

Key* Get_key_by_position_hashmap(int pos, HashMap* map){
  if(!map->closed) close_hashmap(map);
  return &map->key[pos];
}

Key* Get_value_by_position_hashmap(int pos, HashMap* map){
  if(!map->closed) close_hashmap(map);
  return &map->value[pos];
}

int size_hashmap(HashMap* map){
  close_hashmap(map);
  return map->size;
}

int insert_hashmap(const Key* key, const Key* value, HashMap* map) {
  // Validate input
  if (map == NULL || key == NULL || value == NULL) {
    return -1;  // Error: Invalid input
  }

  if(map->closed && map->size>0) {
    // Try to find the key in the hash table
    int pos;
    int check =  check_key_hashmap( key, map, &pos);
    if(check) {
      if(map->value != NULL) {
        copy_from_to_key((Key*)value, &map->value[pos]);
        return 1;
      }
    }

  }

  map->closed = 0;
  // Find or create position for the new entry

  // We need to insert a new entry
  // Check if we need to resize
  if( map->length==0 || map->length <= map->size  ||  map->key == NULL || map->value == NULL) {

	size_t new_length = (map->length == 0) ? 8 : map->length * 2;  // Initial size or double

    new_length = (  map->size < new_length)? new_length:( map->size+1);
   
    // Allocate or reallocate key array
    if(map->key == NULL) {
      map->key = R_Calloc(new_length, Key);
	} else {
      map->key = R_Realloc(map->key,new_length,Key);
	  for(int i=map->size; i< new_length; i++){
		map->key[i].size =0;
		map->key[i].x = NULL;
    }
	}
    // Similar for value array
    if(map->value == NULL) {
      map->value = R_Calloc(new_length, Key);
    } else {
      map->value = R_Realloc(map->value,new_length,Key);
	  for(int i=map->size; i< new_length; i++){
		map->value[i].size =0;
		map->value[i].x = NULL;
	  }
    }

    map->length = new_length;  // Update size

  }

  // Find or create position for the new entry
  size_t position = map->size;
 
  // Insert the new key-value pair
  // Now insert the new pair
  // Create temporary pointers for copy_from_to_key
  Key* key_ptr = &map->key[position];
  Key* value_ptr = &map->value[position];

  copy_from_to_key(key, key_ptr);
  copy_from_to_key(value, value_ptr);
  map->size++;
 
  return 1;
}



SEXP test_key_hashtable() {
  // Create a set of keys for the hash table
  int n = 10;
  Key* keys = R_Calloc(n, Key);

  // Populate keys with varying types and sizes
  for (int i = 0; i < n; i++) {
    // Alternate between different types
    keys[i].type = INT;

    // Varying sizes
    keys[i].size = i + 1;
    keys[i].x = R_Calloc(keys[i].size, int);

    // Fill with some sample data
    for (int j = 0; j < keys[i].size; j++) {
      keys[i].x[j] = i * 10 + j;
    }
  }

  // Print original keys for verification
  Rprintf("Original Keys:\n");
  for (int i = 0; i < n; i++) {
    Rprintf("Key[%d]: Type=%d, Size=%zu, Data=[", i, keys[i].type, keys[i].size);
    for (int j = 0; j < keys[i].size; j++) {
      Rprintf("%d", keys[i].x[j]);
      if (j < keys[i].size - 1) Rprintf(", ");
    }
    Rprintf("]\n");
  }

  // Create hash table
  HashTable* ht = create_key_hashtable(keys, n,1);

  // Verify hash table creation
  if (ht == NULL) {
    Rprintf("Failed to create hash table\n");

    // Clean up
    for (int i = 0; i < n; i++) {
      if (keys[i].x) R_Free(keys[i].x);
    }
    R_Free(keys);

    return R_NilValue;
  }

  Rprintf("\nHash Table Details:\n");
  Rprintf("  Table Size: %zu\n", ht->size);
  Rprintf("  M (Table Capacity): %d\n", ht->M);
  Rprintf("  K (Hash Parameter): %d\n", ht->K);

  // Test finding keys
  Rprintf("\nTesting Key Lookup:\n");

  // Test cases
  struct {
    int size;
    int* data;
    int expected_index;
  } test_cases[] = {
    {1, (int[]){0}, 0},     // First key
    {2, (int[]){10,11}, 1}, // Second key
    {3, (int[]){20, 21,22}, 2}, // Third key
    {1, (int[]){80}, -1} // Non-existent key
  };

  int num_tests = sizeof(test_cases) / sizeof(test_cases[0]);

  for (int t = 0; t < num_tests; t++) {
    Key test_key;
    test_key.type = INT;
    test_key.size = test_cases[t].size;
    test_key.x = test_cases[t].data;

    int position = find_position_key_hashtable(&test_key, keys, ht, -1);

    Rprintf("Looking up key: %d [",test_key.type);
    for (int j = 0; j < test_key.size; j++) {
      Rprintf("%d", test_key.x[j]);
      if (j < test_key.size - 1) Rprintf(", ");
    }
    Rprintf("] - ");

    if (position != -1) {
      Rprintf("Found at index %d (Expected %d)\n",
              position, test_cases[t].expected_index);
    } else {
      Rprintf("Not Found (Expected %d)\n", test_cases[t].expected_index);
    }
  }

  // Clean up
  // Free hash table
  if (ht->h) R_Free(ht->h);
  R_Free(ht);

  // Free keys
  for (int i = 0; i < n; i++) {
    if (keys[i].x) R_Free(keys[i].x);
  }
  R_Free(keys);

  return R_NilValue;
}

/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
