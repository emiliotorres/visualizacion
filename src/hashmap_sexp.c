/*
  TITLE: 
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-03-04 11:50 emilio on emilio-XPS-15-9570>
*/

#include "hashmap.h"


const char class_hashmap[] ="etm_HashMap";

HashMap* get_from_ptr_hashmap(SEXP map_ptr){
  if(!Rf_inherits(map_ptr, class_hashmap)) {
	//Rf_PrintValue(ht_ptr);
	SEXP class_attr = Rf_getAttrib(map_ptr, R_ClassSymbol);
	if (class_attr != R_NilValue) {
	  R_xlen_t n = XLENGTH(class_attr);
	  Rprintf("X has the following classes: ");
	  for (int i = 0; i < n; i++) {
        Rprintf("%s ", CHAR(STRING_ELT(class_attr, i)));
	  }
	  Rprintf("\n");
	}
	Rf_error("Invalid HashMap pointer. X must be of class '%s'. Try to create it with the function 'create_map(x)'.", class_hashmap);
  }
  HashMap* map = (HashMap*) R_ExternalPtrAddr(map_ptr);
  if (!map) Rf_error("Invalid HashMap pointer");

  return map;
  
}


void finalize_hashmap_SEXP(SEXP map_ptr) {
  if (!R_ExternalPtrAddr(map_ptr)) return;
  HashMap* map =  get_from_ptr_hashmap(map_ptr);
  free_hashmap(map);
  R_ClearExternalPtr(map_ptr);
}

SEXP create_hashmap_SEXP(SEXP length_s) {
   
  if (!Rf_isInteger(length_s) || LENGTH(length_s)!=1)
	Rf_error("Input 'length_s' must be an scalar vector. It is %s", Rf_type2char(TYPEOF(length_s)));

  HashMap* map = R_Calloc(1, HashMap);
  map->closed = 0;
  map->length = INTEGER(length_s)[0];
  map->size = 0;
  map->key = R_Calloc(map->length, Key);
  map->value = R_Calloc(map->length, Key);
  for(int i = 0; i< map->length; i++){
	map->key[i].size =0;
	map->value[i].size =0;
	map->key[i].x = NULL;
	map->value[i].x = NULL;
  }
  map->ht = NULL; // Initialize hash table
  int protecti=0;
  SEXP ptr = PROTECT(R_MakeExternalPtr(map, R_NilValue, R_NilValue));protecti++;
  R_RegisterCFinalizerEx(ptr, finalize_hashmap_SEXP, TRUE);
  
  SEXP class_name = PROTECT(Rf_mkString(class_hashmap));protecti++;
  setAttrib(ptr, R_ClassSymbol, class_name);

 
  UNPROTECT(protecti);
   return ptr;
}




SEXP check_key_hashmap_SEXP(SEXP key_s, SEXP map_ptr) {
  HashMap* map= get_from_ptr_hashmap(map_ptr);
  close_hashmap(map);
  
  Key* key = vector_r_to_key(key_s);
  int pos;
  int result = check_key_hashmap(key, map, &pos);
  free_key(key);
  SEXP result_sexp = PROTECT(allocVector(INTSXP, 1));
  INTEGER(result_sexp)[0] = result;
  UNPROTECT(1);
  return result_sexp;
}

SEXP get_hashmap_SEXP(SEXP key_s, SEXP map_ptr) {
  HashMap* map= get_from_ptr_hashmap(map_ptr);
 
  close_hashmap(map);
 
  Key* key = vector_r_to_key(key_s);

   Key* value = R_Calloc(1,Key);
  int result = get_hashmap(key, map, value);
  free_key(key);
  if(result == 0 ){
	free_key(value);
	return R_NilValue;
  }

  int protecti=0;
  SEXP result_s = PROTECT(key_to_r(value));protecti++;
  free_key(value);
  
  UNPROTECT(protecti);
  return result_s;
}

SEXP insert_hashmap_SEXP( SEXP key_s, SEXP value_s, SEXP map_ptr) {
  HashMap* map= get_from_ptr_hashmap(map_ptr);


  //R_print_hashmap(map,0);
  
  Key* key = vector_r_to_key(key_s);
  Key* value = vector_r_to_key(value_s);
  int result = insert_hashmap(key, value, map);

  free_key(key);
  free_key(value);
  
  SEXP result_sexp = PROTECT(allocVector(INTSXP, 1));
  INTEGER(result_sexp)[0] = result;
  UNPROTECT(1);
  return result_sexp;
}


SEXP insert_list_hashmap_SEXP( SEXP key_s, SEXP value_s, SEXP map_ptr) {
  HashMap* map= get_from_ptr_hashmap(map_ptr);

  if (TYPEOF(key_s) != VECSXP || TYPEOF(value_s) != VECSXP || LENGTH(key_s) != LENGTH(value_s)){
	Rf_error("Key (%s) and Value (%s) must be lists of the same length.",
			 Rf_type2char(TYPEOF(key_s)),
			 Rf_type2char(TYPEOF(value_s)));
  }

  int n = LENGTH(key_s);

  

  int protecti=0;
  SEXP result_sexp = PROTECT(allocVector(INTSXP, n));protecti++;
  int* result=INTEGER(result_sexp);

   
  
  for(int i=0; i< n; i++){
	SEXP k_s = VECTOR_ELT(key_s, i);
	SEXP v_s = VECTOR_ELT(value_s, i);
	Key* key = vector_r_to_key(k_s);
	Key* value = vector_r_to_key(v_s);
	if(key == NULL){
	  UNPROTECT(protecti);
	  free_key(key);
	  free_key(value);
	  Rf_error("Key (%s) at %d is not a vector of integers, doubles or characters.", Rf_type2char(TYPEOF(k_s)), i+1);
	}
	if(value == NULL){
	  UNPROTECT(protecti);
	  free_key(key);
	  free_key(value);
	  Rf_error("Value (%s) at %d is not a vector of integers, doubles or characters.", Rf_type2char(TYPEOF(v_s)), i+1);
	}
	result[i] = insert_hashmap(key, value, map);
    
	free_key(key);
	free_key(value);

  }
  //R_print_hashmap(map,0);

 
  UNPROTECT(protecti);
  return result_sexp;
}




SEXP length_hashmap_SEXP(SEXP map_ptr){
  HashMap* map= get_from_ptr_hashmap(map_ptr);
  close_hashmap(map);
  return Rf_ScalarInteger(map->size);
}

SEXP get_key_by_position_hashmap_SEXP(SEXP posR_s, SEXP map_ptr){
  HashMap* map= get_from_ptr_hashmap(map_ptr);
  close_hashmap(map);
  return key_to_r(&map->key[INTEGER(posR_s)[0]-1]);
  }

SEXP get_value_by_position_hashmap_SEXP(SEXP posR_s, SEXP map_ptr){
  HashMap* map= get_from_ptr_hashmap(map_ptr);
  close_hashmap(map);
  return key_to_r(&map->value[INTEGER(posR_s)[0]-1]);
}

SEXP convert_vector_to_key_SEXP(SEXP vector_s) {
  Key* key = vector_r_to_key(vector_s);
  if(key==NULL) return R_NilValue;
  int protecti=0;
  SEXP result_s=PROTECT(Rf_allocVector(INTSXP,key->size + 2));protecti++;
  int* result=INTEGER(result_s);
  result[0] = key->type;
  result[1] = key->size;
  memcpy(result+2,key->x,key->size*sizeof(int));

  free_key(key);
  UNPROTECT(protecti);
  return result_s;
  
}

SEXP convert_key_to_vector_SEXP(SEXP keyvector_s) {
  if(!Rf_isVectorAtomic(keyvector_s) || LENGTH(keyvector_s)<2) {
	return R_NilValue;
  }
  int* keyvector = INTEGER(keyvector_s);
  Key* key = R_Calloc(1,Key);
  key->type = keyvector[0];
  key->size = keyvector[1];
  if(LENGTH(keyvector_s) != 2+ key->size) {
	free_key(key);
	return R_NilValue;
  }
  key->x = R_Calloc(key->size,int);
  memcpy(key->x,keyvector+2,key->size*sizeof(int));

   int protecti=0;
   SEXP result_s=PROTECT(key_to_r(key));protecti++;
    free_key(key);
  UNPROTECT(protecti);
  return result_s;
  
}


/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
