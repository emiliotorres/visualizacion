// Emilio Torres Manzanera
// University of Oviedo
// Time-stamp: <2025-01-15 19:39 emilio on emilio-despacho>
// ============================================================

#include "matchmatrix.h"

// ============================================================
// ============================================================



SEXP get_first_element_of_list_of_integers(SEXP x_s) {
  /* int nth = Rf_asInteger(nthreads_s); */
  /* nth = (nth < 1 )? max_thread: (nth > max_thread ? max_thread : (nth < min_thread ? min_thread : nth)); */
  
  //Rprintf("Number of cores %d\n",nth);
  R_xlen_t n= Rf_xlength(x_s);
  int protecti=0;
  SEXP result_s = PROTECT(Rf_allocVector(INTSXP,n));protecti++;
  int *restrict result =INTEGER(result_s);

  static double errorinthiselement = -1;
  const char *errormessage = "There is at least one NULL element in this list. For instance, the %0.0lfth element of this list of vectors of integers is NULL. I do not know how to get the first integer of a NULL vector. To remove NULL elements of a list use 'Filter(Negate(is.null), x)'.";
  
  //OMP_PARALLEL_FOR(nth)
  OMP_SIMD
    for(R_xlen_t i = 0; i < n; ++i){
      if (Rf_length(VECTOR_ELT(x_s, i))) {
        result[i] = INTEGER(VECTOR_ELT(x_s, i))[0];
      } else {
		result[i]=0;
		errorinthiselement = i;

		/* if(nth == 1) { */
		/*   UNPROTECT(protecti); */
		/*   Rf_error(errormessage,errorinthiselement+1); */
		/* } */
		
				//UNPROTECT(protecti);
		// Rf_warning("List in %d position is NULL.",i+1);
      }
    };

  if(errorinthiselement> -1) {
	UNPROTECT(protecti);
	Rf_error(errormessage,errorinthiselement+1);
  }
  UNPROTECT(protecti);
  return result_s;
}



// from pos:1,...,n in R to idx:0,1,2,..n-1 in C:
SEXP convert_R_position_to_C_idx(SEXP s_pos, SEXP s_length) {
  if(TYPEOF(s_pos) != INTSXP) {
    Rf_error("POS vector is not valid. It is '%s' (integer?).",
             Rf_type2char(TYPEOF(s_pos)));
  }

  if(TYPEOF(s_length) != INTSXP){
    Rf_error("LENGTH is not valid. It is '%s' (integer?).",
             Rf_type2char(TYPEOF(s_length)));
  }
  if(LENGTH(s_length) != 1) {
    Rf_error("LENGTH is not a scalar. Its length is '%d' (1?).",LENGTH(s_length));
  }
  int length=INTEGER(s_length)[0];
  if(length < 0) {
    Rf_error("LENGTH is negative.  '%d' (>=0?).",length);
  }
  int *pos=INTEGER(s_pos);
  int lengthpos=LENGTH(s_pos);
  int protecti=0;
  SEXP s_idx =  PROTECT(Rf_allocVector(INTSXP, lengthpos));protecti++;
  int *idx = INTEGER(s_idx);
  for(R_len_t k = 0; k < lengthpos; k++) {
    if( (pos[k]==NA_INTEGER) || (pos[k] < 1) || (pos[k] > length)){
      UNPROTECT(protecti);
	  if(pos[k]==NA_INTEGER) {
		Rf_error("POS at '%d' is NA_integer_.",k+1);
	  } else {
		Rf_error("POS at '%d' is %d (>=1 or <= '%d'?).",k+1,pos[k],length);
	  }
    }
    idx[k] = pos[k]-1;
  }
  UNPROTECT(protecti);
  return s_idx;
}





// ============================================================
SEXP get_number_of_processors_available() {
  return Rf_ScalarInteger(max_thread);
}  


// ============================================================

SEXP convert_vector_Rrows_to_Crows(SEXP x_s, SEXP Rrows_s){
  int protecti=0;
  if(Rf_isNull(Rrows_s)){
	SEXP result_s = PROTECT(Rf_allocVector(INTSXP,Rf_nrows(x_s)));protecti++;
	int *restrict result=INTEGER(result_s);
	R_xlen_t nrows = Rf_nrows(x_s);
	OMP_SIMD
	  for(R_xlen_t i = 0 ; i!=nrows; ++i){
		result[i] = i;
	  }
	UNPROTECT(protecti);
	return result_s;
  }
  switch(TYPEOF(Rrows_s)){
  case LGLSXP: {
	R_xlen_t nrows = Rf_nrows(x_s);
	if(Rf_xlength(Rrows_s) != nrows){
	  Rf_error("ROWS is a logical vector with length '%ld', whereas X has '%ld' rows. The length of the logical filter must be '%ld'.",
			   Rf_xlength(Rrows_s),nrows,nrows);
	}
	SEXP result_s = PROTECT(Rf_allocVector(INTSXP,nrows));protecti++;
	int *restrict result=INTEGER(result_s);
	const int *rows=LOGICAL(Rrows_s);
	R_xlen_t count=0;
	for(R_xlen_t i = 0; i!= nrows; ++i){
		if(rows[i]==TRUE){
		result[count++] = i;
		}
	}
	SETLENGTH(result_s,count);
	UNPROTECT(protecti);
	return result_s;
  };
  case INTSXP: {
	R_xlen_t nrows = Rf_nrows(x_s);
	int *rows=INTEGER(Rrows_s);
	int lengthrows=LENGTH(Rrows_s);
	SEXP result_s =  PROTECT(Rf_allocVector(INTSXP, lengthrows));protecti++;
	int *result = INTEGER(result_s);
	for(R_len_t k = 0; k < lengthrows; k++) {
	  if( (rows[k]==NA_INTEGER) || (rows[k] < 1) || (rows[k] > nrows)){
		UNPROTECT(protecti);
		if(rows[k]==NA_INTEGER) {
		  Rf_error("ROW at '%d' is NA_integer_.",k+1);
		} else {
		  Rf_error("ROW at '%d' is %d (>=1 or <= '%ld'?).",k+1,rows[k],nrows);
		}
	  }
	  result[k] = rows[k]-1;
	}
	UNPROTECT(protecti);
	return result_s;
	break;
  }
  default:{
	UNPROTECT(protecti);
	Rf_error("ROWS must be 'NULL', logical or integer vector, not  '%s'.", Rf_type2char(TYPEOF(Rrows_s)));
  }
}

  return R_NilValue;
  
}

/* Local Variables: */
/* ispell-local-dictionary: "british"  */
/* End: */
