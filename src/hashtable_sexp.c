/*
  TITLE: 
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-05-25 21:18 emilio on emilio-XPS-15-9570>
*/

#include <stdint.h>    // for uint64_t rather than unsigned long long, intptr_t
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

// From kit.h
#define SEXPPTR_RO(x) ((const SEXP *)DATAPTR_RO(x))  // STRING_ELT and VECTOR_ELT
#define N_ISNAN(x, y) (!ISNAN(x) && !ISNAN(y))
#define B_IsNA(x, y) (R_IsNA(x) && R_IsNA(y))
#define B_IsNaN(x, y) (R_IsNaN(x) && R_IsNaN(y))
#define B_ISNAN(x, y) (ISNAN(x) && ISNAN(y))
#define REQUAL(x, y)											\
  (N_ISNAN(x, y) ? (x == y) : (B_IsNA(x, y) || B_IsNaN(x, y)))

const char class_hashtable[] ="etm_HashTable";

// Finalizer to free HashTable and its h array
void finalize_hashtable_SEXP(SEXP ptr) {
  if (!R_ExternalPtrAddr(ptr)) return;
  HashTable* ht = (HashTable*) R_ExternalPtrAddr(ptr);
  free_hashtable(ht);
  R_ClearExternalPtr(ptr); // Invalidate the pointer
}

// Function to create HashTable and return external pointer
SEXP create_hashtable_SEXP(SEXP x_s, SEXP first_s) {
  if (!Rf_isInteger(x_s))
	Rf_error("Input 'x_s' must be an integer vector. It is %s", Rf_type2char(TYPEOF(x_s)));

  if(!Rf_isLogical(first_s) || LENGTH(first_s)!=1){
	Rf_error("Input 'first_s' must be an logical scalar. It is %s", Rf_type2char(TYPEOF(first_s)));
  }
  
  int n = LENGTH(x_s);
  int *x = INTEGER(x_s);
  int first = INTEGER(first_s)[0];
    
  // Create the hashtable using the external function.
  HashTable* ht = create_hashtable(x, n,first);

  int protecti=0;
  SEXP ptr = PROTECT(R_MakeExternalPtr(ht, R_NilValue, R_NilValue));protecti++;
  R_RegisterCFinalizerEx(ptr, finalize_hashtable_SEXP, TRUE);

  SEXP class_name = PROTECT(Rf_mkString(class_hashtable));protecti++;
  setAttrib(ptr, R_ClassSymbol, class_name);
  
  UNPROTECT(protecti);
  return ptr;
  
}

  





// ============================================================





SEXP create_matrix_hashtable_SEXP(SEXP x_s) {
  // Check that the object is an integer matrix.
  if (!Rf_isInteger(x_s))
	Rf_error("Input 'x_s' must be an integer matrix. It is %s", Rf_type2char(TYPEOF(x_s)));
    
  // Retrieve the dimensions from the "dim" attribute.
  SEXP dim_s = getAttrib(x_s, R_DimSymbol);
  if (dim_s == R_NilValue || LENGTH(dim_s) != 2)
	Rf_error("Input 'x_s' must be a 2-dimensional integer matrix.");
    
  int *dims = INTEGER(dim_s);
  int nrows = dims[0];
  int ncols = dims[1];
    
  // Retrieve the integer data from the matrix.
  int *x = INTEGER(x_s);
    
  // Create the hashtable using the external matrix function.
  HashTable* ht = create_matrix_hashtable(x, nrows, ncols);

  int protecti=0;
  SEXP ptr = PROTECT(R_MakeExternalPtr(ht, R_NilValue, R_NilValue));protecti++;
  R_RegisterCFinalizerEx(ptr, finalize_hashtable_SEXP, TRUE);

  SEXP class_name = PROTECT(Rf_mkString(class_hashtable));protecti++;
  setAttrib(ptr, R_ClassSymbol, class_name);
  
  UNPROTECT(protecti);
  return ptr;
  
}

HashTable* get_from_ptr_hashtable(SEXP ht_ptr){
  if(!Rf_inherits(ht_ptr, class_hashtable)) {
	//Rf_PrintValue(ht_ptr);
	SEXP class_attr = Rf_getAttrib(ht_ptr, R_ClassSymbol);
	if (class_attr != R_NilValue) {
	  R_xlen_t n = XLENGTH(class_attr);
	  Rprintf("X has the following classes: ");
	  for (int i = 0; i < n; i++) {
        Rprintf("%s ", CHAR(STRING_ELT(class_attr, i)));
	  }
	  Rprintf("\n");
	}
	Rf_error("Invalid HashTable pointer. X must be of class '%s'. Try to create it with the function 'create_ht(x)'.", class_hashtable);
  }
  HashTable* ht = (HashTable*) R_ExternalPtrAddr(ht_ptr);
  if (!ht) Rf_error("Invalid HashTable pointer");

  return ht;
  
}


// Function to access M from R
SEXP get_M_hashtable_SEXP(SEXP ht_ptr) {
  HashTable* ht = get_from_ptr_hashtable(ht_ptr);
  
  return Rf_ScalarInteger(ht->M);
}

SEXP get_size_hashtable_SEXP(SEXP ht_ptr) {
  HashTable* ht = get_from_ptr_hashtable(ht_ptr);
  
  return Rf_ScalarInteger(ht->size);
}



SEXP find_position_hashtable_SEXP(SEXP value_s, SEXP x_s, SEXP ht_ptr, SEXP notfound_s) {
  // Validate that x_s is an integer vector.

  HashTable* ht = get_from_ptr_hashtable(ht_ptr);

  
  
  if (!Rf_isInteger(value_s))
	Rf_error("Input 'value_s' must be an integer vector. It is %s", Rf_type2char(TYPEOF(value_s)));

  
  if (!Rf_isInteger(x_s))
	Rf_error("Input 'x_s' must be an integer vector. It is %s", Rf_type2char(TYPEOF(x_s)));

  
  // Validate that notfound_s is an integer scalar.
  if (!Rf_isInteger(notfound_s) || LENGTH(notfound_s) != 1)
	Rf_error("Input 'notfound_s' must be an integer scalar. It is %s", Rf_type2char(TYPEOF(notfound_s)));

  int* value =INTEGER(value_s);
  int n = LENGTH(value_s);
  int* x = INTEGER(x_s);
  
  
  int notfound = INTEGER(notfound_s)[0];
  if( notfound>  0 && notfound <= n){
	Rf_warning("Value of not found (%d) may be in conflict with the research. Try a number < 1 or >%d (the length of the vector).\n",notfound,n);
  }
 
    
  // Allocate an integer vector for the result.
  SEXP result_s = PROTECT(allocVector(INTSXP, n));
  int* result = INTEGER(result_s);
    
     
  // Loop over all elements of x_s, calling find_position_hashtable for each.

  find_positions_for_R_hashtable(value, n,  result, x,  ht, notfound);
  
  /* for (int i = 0; i < n; i++) { */
  /* 	int posC = find_position_hashtable(value[i], x,ht, -1); */
  /* 	switch(posC) { */
  /*   case -1: */
  /* 	  result_ptr[i] = notfound; */
  /* 	  break; */
  /*   default: */
  /* 	  result_ptr[i] = posC + 1; */
  /* 	  break; */
  /* 	}  */
  /* } */

  //free_hashtable(ht);
  UNPROTECT(1);
  return result_s;
}


SEXP find_position_matrix_hashtable_SEXP(SEXP value_s, SEXP x_s, SEXP ht_ptr, SEXP notfound_s) {

  HashTable* ht = get_from_ptr_hashtable(ht_ptr);
 
  
  if((Rf_isMatrix(value_s) && !Rf_isMatrix(x_s) ) ||
	 (!Rf_isMatrix(value_s) && Rf_isMatrix(x_s)
	  )){
	Rf_error("Input 'value_s' (is it matrix? %d) and 'x_s' (is it matrix? %d) must be both matrices (both 1) or not (both 0).",Rf_isMatrix(value_s),Rf_isMatrix(x_s));
  }
  
  if (!Rf_isInteger(value_s))
	Rf_error("Input 'value_s' must be an integer matrix. It is %s", Rf_type2char(TYPEOF(value_s)));
    
  // Retrieve the "dim" attribute and ensure x_s has two dimensions.
  SEXP dim_s = getAttrib(value_s, R_DimSymbol);
  if (dim_s == R_NilValue || LENGTH(dim_s) != 2)
	Rf_error("Input 'value_s' must be a 2-dimensional integer matrix.");

  
  if (!Rf_isInteger(x_s))
	Rf_error("Input 'x_s' must be an integer matrix. It is %s", Rf_type2char(TYPEOF(x_s)));
    
  // Retrieve the "dim" attribute and ensure x_s has two dimensions.
  SEXP dimx_s = getAttrib(x_s, R_DimSymbol);
  if (dimx_s == R_NilValue || LENGTH(dimx_s) != 2)
	Rf_error("Input 'x_s' must be a 2-dimensional integer matrix.");

  int* value=INTEGER(value_s);
  int *dims = INTEGER(dim_s);
  int nrows = dims[0];
  int ncols = dims[1];
  if(ncols != INTEGER(dimx_s)[1]){
	Rf_error("Input 'value_s' must be matrix with %d columns, as the number of columns of x, but it has %d columns.",INTEGER(dimx_s)[1],ncols);
  }
  
  
    
  // Validate that notfound_s is an integer scalar.
  if (!Rf_isInteger(notfound_s) || LENGTH(notfound_s) != 1)
	Rf_error("Input 'notfound_s' must be an integer scalar. It is %s", Rf_type2char(TYPEOF(notfound_s)));
    
  // Retrieve the integer data from x_s and its total length.
  int* x = INTEGER(x_s);
     
  // Convert the list object to a HashTable pointer.
     
  int notfound = INTEGER(notfound_s)[0];
  if( notfound>  0 && notfound <= LENGTH(x_s)){
	Rf_warning("Value of not found (%d) may be a confusing value. Try a number lesser than 1 or greater than %d (the length of the vector).\n",notfound,LENGTH(x_s));
  }

  int nrowsx = INTEGER(dimx_s)[0];
  // int ncolsx = INTEGER(dimx_s)[1];
  /* for(int i= 0; i< nrowsx;i++){ */
  /* 	Rprintf("\n"); */
  /* 	  for(int j=0; j< ncolsx; j++){ */
  /* 		Rprintf("%d ",x[i + j*nrowsx]); */
  /* 	  } */
  /* } */
  /* Rprintf("\n"); */
  
  // Allocate an integer vector for the result.
  int protecti=0;
  SEXP result = PROTECT(allocVector(INTSXP, nrows));protecti++;
  int* result_ptr = INTEGER(result);
    
  // Loop over all elements in the matrix (stored as a vector) and find positions.
  for (int i = 0; i < nrows; i++) {
	//Rprintf("Value %d\n",i);
	int* val = R_Calloc(ncols,int);
	for(int j= 0; j< ncols;j++){
	  val[j] = value[ i + j*nrows];
	}
	int posC = find_position_matrix_hashtable(val, x, nrowsx,ncols,ht, -1);
	if(posC == -1) {
	  result_ptr[i] = notfound;
	} else {
	  result_ptr[i] = posC+1;
	}
	R_Free(val);
  }

  //free_hashtable(ht);
    
  UNPROTECT(protecti);
  return result;
}



SEXP get_positions_hashtable_SEXP(SEXP ht_ptr){

  HashTable* ht = get_from_ptr_hashtable(ht_ptr);
    
  int n=0;
  int* pos=get_positions_hashtable( ht, &n);
 

  int protecti=0;
  SEXP result_s = PROTECT(allocVector(INTSXP, n));protecti++;
  int* result = INTEGER(result_s);
  for(int i=0; i<n;i++){
	result[i] = pos[i]+1;
  }

  R_Free(pos);
  //free_hashtable(ht);
    
  UNPROTECT(protecti);
  return result_s;
}

// ============================================================


char* convert_vectorstring_to_charint(SEXP vector_s, int *num_ints){
  
  size_t size =LENGTH(vector_s);
  size_t total_length = 0;
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
  *num_ints = padded_length / sizeof(int);

  return concatenated;

}

// ============================================================

SEXP create_matrix_general_hashtable_SEXP(SEXP x_s, SEXP rowsR_s, SEXP colsR_s, SEXP first_s) {

  
  if(!Rf_isLogical(first_s) || LENGTH(first_s)!=1){
	Rf_error("Input 'first_s' must be an logical scalar. It is %s", Rf_type2char(TYPEOF(first_s)));
  }
  
  int first = INTEGER(first_s)[0];
  
   int ncols = Rf_length(colsR_s);
  R_len_t nrowsx = Rf_isMatrix(x_s) ? Rf_nrows(x_s) : Rf_length(x_s);
  R_len_t ncolsx = Rf_isMatrix(x_s) ? Rf_ncols(x_s) : 1;
  int *colsR = INTEGER(colsR_s);
  int *rowsR = INTEGER(rowsR_s);
  R_len_t nidrows = Rf_length(rowsR_s);

  int* idrows=R_Calloc(nidrows,int);
  int* idcols=R_Calloc(ncols,int);
  for(int i =0 ; i<nidrows;i++){
	if(rowsR[i] < 1 || rowsR[i]> nrowsx){
	  R_Free(idrows);
	  R_Free(idcols);
	  Rf_error("Error in Row %d. It is %d, but it must be between 1 and %d, the number of rows of X.",i+1,rowsR[i],nrowsx);
	}
	idrows[i] = rowsR[i]-1;
  }

  for(int i =0 ; i<ncols;i++){
	if(colsR[i] < 1 || colsR[i]> ncolsx){
	  R_Free(idrows);
	  R_Free(idcols);
	  Rf_error("Error in Col %d. It is %d, but it must be between 1 and %d, the number of columns of X.",i+1,colsR[i],ncolsx);
	}
	idcols[i] = colsR[i]-1;
  }

 
  

  //Rprintf("is matrix? %d, nrows = %d y nidrows es %d\n",Rf_isMatrix(x_s),nrowsx, nidrows);

  const size_t n2 = 2U * (size_t)nidrows;
  size_t M = 256;
  int K = 8;
  while (M < n2) {
    M *= 2;
    K++;
  }
  HashTable* ht=R_Calloc(1,HashTable);
  ht->M = M;
  ht->K=K;
  ht->h = R_Calloc(M,int);
	
  //  Rprintf("M %d K %d %d\n",M,K,log2(M));
  R_xlen_t count = 0;
  uint32_t id = 0;

  if(first) {
  switch (TYPEOF(x_s)) {
  case LGLSXP:
  case INTSXP: { // See kit::kit_dup.c
    int *x = INTEGER(x_s);
    // Rprintf("Empieza el hash\n");
	int row[ncols];
	OMP_SIMD
    for (R_xlen_t i = 0; i < nidrows; ++i) {
	  for (int j = 0; j < ncols; ++j) {
		R_xlen_t k= idrows[i] + idcols[j] * nrowsx;
      
		row[j] = x[k];
	  }
	  id = hash_for_table_int(row,ncols,K);
      while (ht->h[id]) { // Mientras este puesto esté ocupado
        for (R_xlen_t j = 0; j < ncols; ++j) {
          if (x[idrows[ht->h[id] - 1] + idcols[j] * nrowsx] != x[idrows[i] + idcols[j] * nrowsx]) {
            goto labelmi1;
          }
        }
        goto labelmi2; // Si son iguales vete al final
      labelmi1:;       // Si no son iguales, sigue mirando
        if(id++ >= M) id =0;
    
      } // Hemos llegado a un puesto que no está ocupado
      ht->h[id] = (int)i + 1; 
      // pans[i]++; // Anotamos un 1 en esa row
       count++; // Anotamos cuantos casos distintos llevamos */
    labelmi2:; // Son iguales
	}
    break;
  }
  case REALSXP: {
    const double *restrict x = REAL(x_s);
    union uno tpv;
	int n2 = 2*ncols;
	int row[n2];

	OMP_SIMD
    for (R_xlen_t i = 0; i < nidrows; ++i) {
	  for (R_xlen_t j = 0; j < ncols; ++j) {
        tpv.d = x[idrows[i] + idcols[j] * nrowsx];
		row[2*j] = tpv.u[0];
		row[2*j+1] = tpv.u[1];
      }
	  id = hash_for_table_int(row,n2,K);
      while (ht->h[id]) {
        for (R_xlen_t j = 0; j < ncols; ++j) {
          if (!REQUAL(x[idrows[ht->h[id] - 1] + idcols[j] * nrowsx],
                      x[idrows[i] + idcols[j] * nrowsx])) {
            goto labelmr1;
          }
        }
        goto labelmr2;
      labelmr1:;
		if(id++ >= M) id =0;
	  }
      ht->h[id] = (int)i + 1;
      // pans[i]++;
      count++;
    labelmr2:;
    }
    break;
  }
  case STRSXP: {


    //const SEXP *restrict x = STRING_PTR(x_s);
	OMP_SIMD
    for (R_xlen_t i = 0; i < nidrows; ++i) {

	  
	  SEXP temp_s = PROTECT(Rf_allocVector(STRSXP, ncols));
	  for (R_xlen_t j = 0; j < ncols; ++j) {
		SET_STRING_ELT(temp_s, j,STRING_ELT(x_s, idrows[i] + idcols[j] * nrowsx));
	  }
	  int num_ints=0;
	  char *concatenated = convert_vectorstring_to_charint(temp_s,&num_ints);
	  UNPROTECT(1);
	  
		  id = hash_for_table_int((int *)concatenated,num_ints,K);
	  R_Free(concatenated);
	  
      while (ht->h[id]) {
        for (R_xlen_t j = 0; j < ncols; ++j) {
		  // Get the string elements for comparison
		  SEXP str_elem1 = STRING_ELT(x_s, idrows[ht->h[id] - 1] + idcols[j] * nrowsx);
		  SEXP str_elem2 = STRING_ELT(x_s, idrows[i] + idcols[j] * nrowsx);
		  // Compare the strings
		  if (strcmp(CHAR(str_elem1), CHAR(str_elem2)) != 0) {
			goto labelms1; // # nocov
		  }
		  
          /* if (x[idrows[ht->h[id] - 1] + idcols[j] * nrowsx] != */
          /*     x[idrows[i] + idcols[j] * nrowsx]) { */
          /*   goto labelms1; // # nocov */
          /* } */
        }
        goto labelms2;
      labelms1:; // # nocov
		if(id++ >= M) id =0;
      }
      ht->h[id] = (int)i+ 1;
      count++;
    labelms2:;

    }
    break;
  }
  default: {
    Rf_error("Type '%s' not implemented.", Rf_type2char(TYPEOF(x_s)));
  }
  }

  } else {
	// Last element


	switch (TYPEOF(x_s)) {
	case LGLSXP:
	case INTSXP: { // See kit::kit_dup.c
	  int *x = INTEGER(x_s);
	  // Rprintf("Empieza el hash\n");
	  int row[ncols];
	  OMP_SIMD
		for (R_xlen_t i = nidrows-1; i >=0; i--) {
		  for (int j = 0; j < ncols; ++j) {
			R_xlen_t k= idrows[i] + idcols[j] * nrowsx;
      
			row[j] = x[k];
		  }
		  id = hash_for_table_int(row,ncols,K);
		  while (ht->h[id]) { // Mientras este puesto esté ocupado
			for (R_xlen_t j = 0; j < ncols; ++j) {
			  if (x[idrows[ht->h[id] - 1] + idcols[j] * nrowsx] != x[idrows[i] + idcols[j] * nrowsx]) {
				goto labelmi1_last;
			  }
			}
			goto labelmi2_last; // Si son iguales vete al final
		  labelmi1_last:;       // Si no son iguales, sigue mirando
			if(id++ >= M) id =0;
    
		  } // Hemos llegado a un puesto que no está ocupado
		  ht->h[id] = (int)i + 1; 
		  // pans[i]++; // Anotamos un 1 en esa row
		  count++; // Anotamos cuantos casos distintos llevamos */
		labelmi2_last:; // Son iguales
		}
	  break;
	}
	case REALSXP: {
	  const double *restrict x = REAL(x_s);
	  union uno tpv;
	  int n2 = 2*ncols;
	  int row[n2];

	  OMP_SIMD
		for (R_xlen_t i = nidrows-1; i >=0; i--) {
			  for (R_xlen_t j = 0; j < ncols; ++j) {
			tpv.d = x[idrows[i] + idcols[j] * nrowsx];
			row[2*j] = tpv.u[0];
			row[2*j+1] = tpv.u[1];
		  }
		  id = hash_for_table_int(row,n2,K);
		  while (ht->h[id]) {
			for (R_xlen_t j = 0; j < ncols; ++j) {
			  if (!REQUAL(x[idrows[ht->h[id] - 1] + idcols[j] * nrowsx],
						  x[idrows[i] + idcols[j] * nrowsx])) {
				goto labelmr1_last;
			  }
			}
			goto labelmr2_last;
		  labelmr1_last:;
			if(id++ >= M) id =0;
		  }
		  ht->h[id] = (int)i + 1;
		  // pans[i]++;
		  count++;
		labelmr2_last:;
		}
	  break;
	}
	case STRSXP: {
			// Iterate over each string element in x_s in reverse order

	  OMP_SIMD
		for (R_xlen_t i = nidrows - 1; i >= 0; i--) {
		  SEXP temp_s = PROTECT(Rf_allocVector(STRSXP, ncols));
		  for (R_xlen_t j = 0; j < ncols; ++j) {
            // Get the string element from x_s using the calculated index
            SEXP str_elem = STRING_ELT(x_s, idrows[i] + idcols[j] * nrowsx);
            // Set the string element in temp_s
            SET_STRING_ELT(temp_s, j, str_elem);
		  }
		  int num_ints = 0;
		  char *concatenated = convert_vectorstring_to_charint(temp_s, &num_ints);
		  UNPROTECT(1);

		  id = hash_for_table_int((int *)concatenated, num_ints, K);
		  R_Free(concatenated);

		  while (ht->h[id]) {
            for (R_xlen_t j = 0; j < ncols; ++j) {
			  // Get the string elements for comparison
			  SEXP str_elem1 = STRING_ELT(x_s, idrows[ht->h[id] - 1] + idcols[j] * nrowsx);
			  SEXP str_elem2 = STRING_ELT(x_s, idrows[i] + idcols[j] * nrowsx);
			  // Compare the strings
			  if (strcmp(CHAR(str_elem1), CHAR(str_elem2)) != 0) {
				goto labelms1_last; // # nocov
			  }
            }
            goto labelms2_last;
		  labelms1_last:; // # nocov
            if (id++ >= M) id = 0;
		  }
		  ht->h[id] = (int)i + 1;
		  count++;
		labelms2_last:;
		}
		break;
	  }

	/*   const SEXP *restrict x = STRING_PTR(x_s); */
	/*   OMP_SIMD */
	/* 	for (R_xlen_t i = nidrows-1; i >=0; i--) { */

	/* 	  SEXP temp_s = PROTECT(Rf_allocVector(STRSXP, ncols)); */
	/* 	  for (R_xlen_t j = 0; j < ncols; ++j) { */
	/* 		SET_STRING_ELT(temp_s, j,STRING_ELT(x_s, idrows[i] + idcols[j] * nrowsx)); */
	/* 	  } */
	/* 	  int num_ints=0; */
	/* 	  char *concatenated = convert_vectorstring_to_charint(temp_s,&num_ints); */
	/* 	  UNPROTECT(1); */
		  
	/* 	  id = hash_for_table_int((int *)concatenated,num_ints,K); */
	/* 	  R_Free(concatenated); */

		  
		  	  
	/* 	  while (ht->h[id]) { */
	/* 		for (R_xlen_t j = 0; j < ncols; ++j) { */
	/* 		  if (x[idrows[ht->h[id] - 1] + idcols[j] * nrowsx] != */
	/* 			  x[idrows[i] + idcols[j] * nrowsx]) { */
	/* 			goto labelms1_last; // # nocov */
	/* 		  } */
	/* 		} */
	/* 		goto labelms2_last; */
	/* 	  labelms1_last:; // # nocov */
	/* 		if(id++ >= M) id =0; */
	/* 	  } */
	/* 	  ht->h[id] = (int)i+ 1; */
	/* 	  count++; */
	/* 	labelms2_last:; */

	/* 	} */
	/*   break; */
	/* } */
	default: {
	  Rf_error("Type '%s' not implemented.", Rf_type2char(TYPEOF(x_s)));
	}
	}
	
  }

  R_Free(idrows);
  R_Free(idcols);

  ht->size = count;
  int protecti=0;
  SEXP ptr = PROTECT(R_MakeExternalPtr(ht, R_NilValue, R_NilValue));protecti++;
  R_RegisterCFinalizerEx(ptr, finalize_hashtable_SEXP, TRUE);

  SEXP class_name = PROTECT(Rf_mkString(class_hashtable));protecti++;
  setAttrib(ptr, R_ClassSymbol, class_name);
  
  UNPROTECT(protecti);
  return ptr;
  
}


// For each value (a matrix or vector) return the first match on y[idrowsy, ], giving the position of the row of y (in R format)
// idrows,idcols are C format
SEXP find_position_matrix_general_hashtable_SEXP(SEXP value_s, SEXP z_s, SEXP rowszR_s, SEXP colszR_s, SEXP ht_ptr, SEXP notfound_s) {

  HashTable* ht = get_from_ptr_hashtable(ht_ptr);
    
  if (TYPEOF(value_s)!=TYPEOF(z_s))
	Rf_error("Input 'value_s' (%s) and 'z_s' (%s) must be of the same type.", Rf_type2char(TYPEOF(value_s)),Rf_type2char(TYPEOF(z_s)));

  if(TYPEOF(notfound_s)!=INTSXP){
	Rf_error("NOTFOUND must be integer, not '%s'.", Rf_type2char(TYPEOF(notfound_s)));
  }

  /* int nth = Rf_asInteger(nthreads_s); */
  /* nth = (nth < 1 )? max_thread: (nth > max_thread ? max_thread : (nth < min_thread ? min_thread : nth));  */

  int protecti=0;


  R_len_t nidcolsx = Rf_length(colszR_s);
  R_len_t nidrowsx = Rf_length(rowszR_s);
  R_len_t nrowsx = Rf_isMatrix(z_s) ? Rf_nrows(z_s) : Rf_length(z_s);
  R_len_t ncolsx = Rf_isMatrix(z_s) ? Rf_nrows(z_s) : 1;
  R_len_t ncols = Rf_isMatrix(value_s) ? Rf_ncols(value_s) : 1;
  R_len_t nrows = Rf_isMatrix(value_s) ? Rf_nrows(value_s) : Rf_length(value_s);

  if((Rf_isMatrix(value_s) && !Rf_isMatrix(z_s) ) ||
	  (!Rf_isMatrix(value_s) && Rf_isMatrix(z_s)
	   )){
	Rf_error("Input 'value_s' (is it matrix? %d) and 'z_s' (is it matrix? %d) must be both matrices (both 1) or not (both 0).",Rf_isMatrix(value_s),Rf_isMatrix(z_s));
	}
  if(nidcolsx!= ncols){
	  Rf_error("Number of columns of 'value_s' (%d) and of 'x_s' (%d) are different.",ncols, nidcolsx);
  }
  if(!Rf_isMatrix(z_s) && nidcolsx!=1){
	Rf_error("X is not matrix but you are trying to select %d columns.",nidcolsx);
  }
  if(!Rf_isMatrix(z_s) && INTEGER(colszR_s)[0]!=1){
	Rf_error("X is not matrix but you are trying to select the %d column. You must select the column 1.",INTEGER(colszR_s)[0]);
  }


  int* colsRx =INTEGER(colszR_s);
  int* rowsRx =INTEGER(rowszR_s);
  

  int* idrowsx =R_Calloc(nidrowsx,int);
  int* idcolsx =R_Calloc(nidcolsx,int);
  
 
  for(int i =0 ; i<nidrowsx;i++){
	if(rowsRx[i] < 1 || rowsRx[i]> nrowsx){
	  R_Free(idrowsx);
	  R_Free(idcolsx);
	  Rf_error("Error in Row %d. It is %d, but it must be between 1 and %d, the number of rows of X.",i+1,rowsRx[i],nrowsx);
	}
	idrowsx[i] = rowsRx[i]-1;
  }

  for(int i =0 ; i<ncols;i++){
	if(colsRx[i] < 1 || colsRx[i]> ncolsx){
	  R_Free(idrowsx);
	  R_Free(idcolsx);
	  Rf_error("Error in Col %d. It is %d, but it must be between 1 and %d, the number of columns of X.",i+1,colsRx[i],ncolsx);
	}
	idcolsx[i] = colsRx[i]-1;
  }
    
     
  // Validate that notfound_s is an integer scalar.
  if (!Rf_isInteger(notfound_s) || LENGTH(notfound_s) != 1)
	Rf_error("Input 'notfound_s' must be an integer scalar. It is %s", Rf_type2char(TYPEOF(notfound_s)));

  int notfound=Rf_asInteger(notfound_s);
 

  SEXP result_s = PROTECT(Rf_allocVector(INTSXP,nrows));protecti++;
  int* result=INTEGER(result_s);

  uint32_t id = 0;
 

  switch (TYPEOF(z_s)) {
  case LGLSXP:
  case INTSXP: {
    const  int* restrict x = INTEGER(z_s);
	const int* restrict v= INTEGER(value_s);
	int row[ncols];
	int K = ht->K;
	OMP_SIMD
	for (R_xlen_t i = 0; i < nrows; ++i) {
			for (R_xlen_t j = 0; j < ncols; ++j) {
		  R_xlen_t k= i + j * nrows;
		  row[j] = v[k];
		 		}
			id = hash_for_table_int(row,ncols,K);
		while (ht->h[id]) { // Mientras este puesto esté ocupado
		  for (R_xlen_t j = 0; j < ncols; ++j) {
			if (x[idrowsx[ht->h[id] - 1] + idcolsx[j] * nrowsx] != v[i + j * nrows]) {
			  goto labelmi1;
			}
		  }
		  goto labelmi2; // Si son iguales vete al final
		labelmi1:;       // Si no son iguales, sigue mirando
		  if(++id >= ht->M) id = 0;
		} // Hemos llegado a un puesto que no está ocupado
		result[i] =notfound;
		continue;
	  labelmi2:; // Son iguales
		result[i] = ht->h[id]; // R format
	
	  }
    break;
  }
  case REALSXP: {
	const double *restrict x = REAL(z_s);
	const double *restrict v = REAL(value_s);
	union uno tpv;
	int n2 = 2*ncols;
	int row[n2];
	int K = ht->K;

	//OMP_PARALLEL_FOR(nth)
	OMP_SIMD
      for (R_xlen_t i = 0; i < nrows; ++i) {
               for (R_xlen_t j = 0; j < ncols; ++j) {
          tpv.d = v[i + j * nrows];
		  row[2*j] = tpv.u[0];
		  row[2*j+1] = tpv.u[1];
		}
			   id = hash_for_table_int(row,n2,K);
       
        while (ht->h[id]) {
          for (R_xlen_t j = 0; j < ncols; ++j) {
            if (!REQUAL(x[idrowsx[ht->h[id] - 1] +  idcolsx[j]* nrowsx],
                        v[i + j * nrows])) {
              goto labelmr1;
            }
          }
          goto labelmr2;
        labelmr1:;
		  if(++id >= ht->M) id = 0;
		}
		result[i] =notfound;
		continue;
	  labelmr2:;
		result[i] = ht->h[id]; // R format
	  }
	  
	break;
  }
  /* case STRSXP: { */
  /* 	const SEXP *restrict x= STRING_PTR(z_s); */
  /* 	const SEXP *restrict v = STRING_PTR(value_s); */
  /* 	int K=ht->K; */

  /* 	OMP_SIMD */
  /*     for (R_xlen_t i = 0; i < nrows; ++i) { */

  /* 		SEXP temp_s = PROTECT(Rf_allocVector(STRSXP, ncols)); */
  /* 		for (R_xlen_t j = 0; j < ncols; ++j) { */
  /* 		  SET_STRING_ELT(temp_s, j,STRING_ELT(value_s, i + j * nrows)); */
  /* 		} */
  /* 		int num_ints=0; */
  /* 		char *concatenated = convert_vectorstring_to_charint(temp_s,&num_ints); */
  /* 		UNPROTECT(1); */
  /* 		id = hash_for_table_int((int *)concatenated,num_ints,K); */
  /* 		R_Free(concatenated); */

		
		
  /* 		while (ht->h[id]) { */
  /*         for (R_xlen_t j = 0; j < ncols; ++j) { */
  /*           if (x[idrowsx[ht->h[id] - 1] + idcolsx[j] * nrowsx] != */
  /*               v[i + j * nrows]) { */
  /*             goto labelms1; // # nocov */
  /*           } */
  /*         } */
  /*         goto labelms2; */
  /*       labelms1:; // # nocov */
  /* 		  if(++id >= ht->M) id = 0; */
  /*       } */
  /* 		result[i] =notfound; */
  /* 		continue; */
  /* 	  labelms2:; */
  /* 		result[i] = ht->h[id]; // Rformat */
  /*     } */
  /* 	break; */
  /* } */

  case STRSXP: {
    // Get pointers to the character data using API-compliant methods
    //const char **restrict x = (const char **)STRING_ELT(z_s, 0);
    //const char **restrict v = (const char **)STRING_ELT(value_s, 0);
    int K = ht->K;

	OMP_SIMD
    for (R_xlen_t i = 0; i < nrows; ++i) {
	  SEXP temp_s = PROTECT(Rf_allocVector(STRSXP, ncols));
	  for (R_xlen_t j = 0; j < ncols; ++j) {
		// Get the string element from value_s using the calculated index
		SEXP str_elem = STRING_ELT(value_s, i + j * nrows);
		// Set the string element in temp_s
		SET_STRING_ELT(temp_s, j, str_elem);
	  }
	  int num_ints = 0;
	  char *concatenated = convert_vectorstring_to_charint(temp_s, &num_ints);
	  UNPROTECT(1);

	  id = hash_for_table_int((int *)concatenated, num_ints, K);
	  R_Free(concatenated);

	  while (ht->h[id]) {
		for (R_xlen_t j = 0; j < ncols; ++j) {
		  // Get the string elements for comparison
		  SEXP str_elem1 = STRING_ELT(z_s, idrowsx[ht->h[id] - 1] + idcolsx[j] * nrowsx);
		  SEXP str_elem2 = STRING_ELT(value_s, i + j * nrows);
		  // Compare the strings
		  if (strcmp(CHAR(str_elem1), CHAR(str_elem2)) != 0) {
			goto labelms1; // # nocov
		  }
		}
		goto labelms2;
	  labelms1:; // # nocov
		if (++id >= ht->M) id = 0;
	  }
	  result[i] = notfound;
	  continue;
    labelms2:;
	  result[i] = ht->h[id]; // Rformat
    }
    break;
  }

	
  default: {
    Rf_error("Type '%s' not implemented.", Rf_type2char(TYPEOF(z_s)));
  }
  }

  R_Free(idrowsx);
  R_Free(idcolsx);


  UNPROTECT(protecti);
  return result_s;
}



/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
