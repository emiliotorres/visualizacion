/*
  TITLE:
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-03-03 23:23 emilio on emilio-XPS-15-9570>
*/


#include "sparsematrix.h"


const char class_sparsematrix[] ="etm_SparseMatrix";

SparseMatrix* get_from_ptr_sparsematrix(SEXP sm_ptr){
  if(!Rf_inherits(sm_ptr, class_sparsematrix)) {
    //Rf_PrintValue(ht_ptr);
    SEXP class_attr = Rf_getAttrib(sm_ptr, R_ClassSymbol);
    if (class_attr != R_NilValue) {
      R_xlen_t n = XLENGTH(class_attr);
      Rprintf("X has the following classes: ");
      for (int i = 0; i < n; i++) {
        Rprintf("%s ", CHAR(STRING_ELT(class_attr, i)));
      }
      Rprintf("\n");
    }
    Rf_error("Invalid SparseMatrix pointer. X must be of class '%s'. Try to create it with the function 'create_sm(x)'.", class_sparsematrix);
  }
  SparseMatrix* m = (SparseMatrix*) R_ExternalPtrAddr(sm_ptr);
  if (!m) Rf_error("Invalid SparseMatrix pointer");

  return m;

}

void finalize_sparsematrix_SEXP(SEXP sm_ptr) {
  if (!R_ExternalPtrAddr(sm_ptr)) return;
  SparseMatrix* m =  get_from_ptr_sparsematrix(sm_ptr);
  free_sparsematrix(m);
  R_ClearExternalPtr(sm_ptr);
}

SEXP create_sparsematrix_SEXP(SEXP nrows_s, SEXP ncols_s) {

  if (!Rf_isInteger(nrows_s) || LENGTH(nrows_s) != 1)
    Rf_error("nrows must be an integer scalar.");
  if (!Rf_isInteger(ncols_s) || LENGTH(ncols_s) != 1)
    Rf_error("ncols must be an integer scalar.");

  int nrows = INTEGER(nrows_s)[0];
  int ncols = INTEGER(ncols_s)[0];


  SparseMatrix* m = create_sparsematrix(nrows,ncols);

  int protecti=0;
  SEXP ptr = PROTECT(R_MakeExternalPtr(m, R_NilValue, R_NilValue));protecti++;
  R_RegisterCFinalizerEx(ptr, finalize_sparsematrix_SEXP, TRUE);

  SEXP class_name = PROTECT(Rf_mkString(class_sparsematrix));protecti++;
  setAttrib(ptr, R_ClassSymbol, class_name);


  UNPROTECT(protecti);
  return ptr;
}


SEXP size_sparsematrix_SEXP(SEXP sm_ptr){

  SparseMatrix* sm = get_from_ptr_sparsematrix(sm_ptr);

  return Rf_ScalarInteger(size_sparsematrix(sm));
}


// Wrapper for insert_element_sparsematrix
SEXP insert_elements_sparsematrix_SEXP(SEXP rowsR_s, SEXP colsR_s, SEXP x_s,  SEXP sm_ptr) {

  SparseMatrix* m= get_from_ptr_sparsematrix(sm_ptr);


  if (!Rf_isInteger(rowsR_s) || !Rf_isInteger(colsR_s))
    Rf_error("ROW_S (%s) and COL_S (%s) must be integer vectors.",Rf_type2char(TYPEOF(rowsR_s)),Rf_type2char(TYPEOF(colsR_s)));

  int n = LENGTH(rowsR_s);
  if(n != LENGTH(colsR_s)){
    Rf_error("ROW_S (%d) and COL_S (%d) must have the same length.",n,LENGTH(colsR_s));
  }

  if (!Rf_isInteger(x_s))
    Rf_error("X_S must be an integer vector of length %d.",n);
  if(LENGTH(x_s)!=n)
    Rf_error("ROWS_S (%d), COLS_S (%d) and X_S (%d) must have the same length.",n,n,LENGTH(x_s));

  int* rowsR = INTEGER(rowsR_s);
  int* colsR = INTEGER(colsR_s);

  int nrows = m->nrows;
  int ncols = m->ncols;


  for(int i =0; i< n; i++){
    if(rowsR[i]< 1 || rowsR[i]> nrows){
      Rf_error("ROW %d  is %d. It must be between 1 and the number of rows %d.",i+1,rowsR[i],nrows);
    }

    if(colsR[i]< 1 || colsR[i]> ncols){
      Rf_error("COL %d is %d. It must be between 1 and the number of columns %d.",i+1,colsR[i],ncols);
    }
  }

  int* x = INTEGER(x_s);

  for(int i = 0; i<n; i++){
    insert_sparsematrix( rowsR[i]-1, colsR[i]-1, x[i], m);
  }

  return Rf_ScalarInteger( size_sparsematrix(m));

}


SEXP get_elements_sparsematrix_SEXP(SEXP rowsR_s, SEXP colsR_s, SEXP sm_ptr, SEXP zerovalue_s) {

  SparseMatrix* m= get_from_ptr_sparsematrix(sm_ptr);

  if (!Rf_isInteger(rowsR_s) || !Rf_isInteger(colsR_s))
    Rf_error("ROW_S (%s) and COL_S (%s) must be integer vectors.",Rf_type2char(TYPEOF(rowsR_s)),Rf_type2char(TYPEOF(colsR_s)));

  int n = LENGTH(rowsR_s);
  if(n != LENGTH(colsR_s)){
    Rf_error("ROW_S (%d) and COL_S (%d) must have the same length.",n,LENGTH(colsR_s));
  }


  int* rowsR = INTEGER(rowsR_s);
  int* colsR = INTEGER(colsR_s);

  int nrows = m->nrows;
  int ncols = m->ncols;


  // Validate that notfound_s is an integer scalar.
  if (!Rf_isInteger(zerovalue_s) || LENGTH(zerovalue_s) != 1)
    Rf_error("Input 'notfound_s' must be an integer scalar. It is %s", Rf_type2char(TYPEOF(zerovalue_s)));

  int zerovalue = INTEGER(zerovalue_s)[0];

  // Allocate an integer vector for the result.
  int protecti=0;
  SEXP result_s = PROTECT(allocVector(INTSXP, n));protecti++;
  int* result = INTEGER(result_s);


  for (int i = 0; i < n; i++) {
    if(rowsR[i]< 1 || rowsR[i]> nrows){
      UNPROTECT(protecti);
      Rf_error("ROW %d is %d. It must be between 1 and %d, the number of rows.",i+1,rowsR[i],nrows);
    }
    if(colsR[i]< 1 || colsR[i]> ncols){

      UNPROTECT(protecti);
      Rf_error("COL %d is %d. It must be between 1 and %d, the number of columns.",i+1,colsR[i],ncols);
    }


    result[i]= get_sparsematrix( rowsR[i]-1, colsR[i]-1, m, zerovalue);

  }



  UNPROTECT(protecti);
  return result_s;

}

SEXP dump_long_format_sparsematrix(SEXP sm_ptr){

  SparseMatrix* m= get_from_ptr_sparsematrix(sm_ptr);
  // int nrows = m->nrows;
  //int ncols = INTEGER(ncols_s)[0];

  HashMap* map = m->map;

  int n = size_hashmap(map);
  int protecti=0;
  SEXP result_s = PROTECT(Rf_allocMatrix(INTSXP, n,3));protecti++;
  int* result = INTEGER(result_s);

  int n2 = n*2;

  for(int k=0; k< n; k++){
    //  Key* key Key* Get_key_by_position_hashmap(int pos, HashMap* map){
    int row =  map->key[k].x[0]; //    pos = row + col * (*matrix)->nrows;
	int col =  map->key[k].x[1]; //    pos = row + col * (*matrix)->nrows;
  
    result[k] = row +1;
    result[k+n] = col +1;
    result[k+n2] = map->value[k].x[0];

  }

  // Create column names
  SEXP dimnames_s = PROTECT(allocVector(VECSXP, 2));protecti++; // List for dimnames
  SEXP colnames_s = PROTECT(allocVector(STRSXP, 3));protecti++; // Character vector for column names

  // Set the column names
  SET_STRING_ELT(colnames_s, 0, mkChar("i"));
  SET_STRING_ELT(colnames_s, 1, mkChar("j"));
  SET_STRING_ELT(colnames_s, 2, mkChar("x"));

  // Assign column names to dimnames
  SET_VECTOR_ELT(dimnames_s, 1, colnames_s); // Set column names (index 1)
  SET_VECTOR_ELT(dimnames_s, 0, R_NilValue);   // Set row names to NULL (index 0)

  // Set the dimnames attribute of the matrix
  setAttrib(result_s, R_DimNamesSymbol, dimnames_s);


  UNPROTECT(protecti);
  return result_s;

}



SEXP dim_sparsematrix_SEXP(SEXP sm_ptr){

	SparseMatrix* m= get_from_ptr_sparsematrix(sm_ptr);
	int protecti = 0;
	SEXP result_s=PROTECT(Rf_allocVector(INTSXP,2));protecti++;
	int* result=INTEGER(result_s);
	result[0] = m->nrows;
	result[1] = m->ncols;

	UNPROTECT(protecti);
	return result_s;


}



/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
