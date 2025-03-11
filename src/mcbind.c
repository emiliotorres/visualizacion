#include "matchmatrix.h"

// ============================================================

SEXP cbind_matrix(SEXP x_s, SEXP idrowsx_s, SEXP idcolsx_s, SEXP y_s, SEXP idrowsy_s, SEXP idcolsy_s) {

  if(TYPEOF(x_s)!= TYPEOF(y_s)) {
    Rf_error("X type '%s' is different to the Y type '%s'.", Rf_type2char(TYPEOF(x_s)), Rf_type2char(TYPEOF(y_s)));
  }

  R_xlen_t nrowsx = Rf_length(idrowsx_s);
  R_xlen_t ncolsx = Rf_length(idcolsx_s);
  R_xlen_t nrowsy = Rf_length(idrowsy_s);
  R_xlen_t ncolsy = Rf_length(idcolsy_s);

  //Rprintf("ncolsy es %d\n",ncolsy);
  if(nrowsx!= nrowsy){
    Rf_error("Length of idrows of X (%ld) is different to the length of idrows of Y (%ld)",nrowsx,nrowsy);
  }

  const int *restrict rowsx = INTEGER(idrowsx_s);
  const int *restrict colsx = INTEGER(idcolsx_s);
  const int *restrict rowsy = INTEGER(idrowsy_s);
  const int *restrict colsy = INTEGER(idcolsy_s);

  int protecti=0;

  R_xlen_t dim0x = Rf_nrows(x_s);
  R_xlen_t dim0y = Rf_nrows(y_s);

  /* int nth = Rf_asInteger(nthreads_s); */
  /* nth = (nth < 1 )? max_thread: (nth > max_thread ? max_thread : (nth < min_thread ? min_thread : nth)); */

  SEXP result_s = R_NilValue;
  switch(TYPEOF(x_s)) {
  case LGLSXP : {
    result_s = PROTECT(Rf_allocMatrix(LGLSXP, nrowsx, ncolsx+ncolsy));protecti++;
    const int *restrict x = LOGICAL(x_s);
    const int *restrict y = LOGICAL(y_s);
    int *restrict result = LOGICAL(result_s);
	//OMP_PARALLEL_FOR(nth)
	OMP_SIMD
      for (R_xlen_t irow = 0; irow < nrowsx; ++irow) {
        for (R_xlen_t jcol = 0; jcol < ncolsx; ++jcol) {
          result[irow+nrowsx*jcol] = x[rowsx[irow]+ colsx[jcol]*dim0x];
        }
        for (R_xlen_t jcol = 0; jcol < ncolsy; ++jcol) {
          result[irow+nrowsx*(jcol+ncolsx)] = y[rowsy[irow]+ colsy[jcol]*dim0y];
        }
      }
  } break;
  case INTSXP : {
    result_s = PROTECT(Rf_allocMatrix(INTSXP, nrowsx, ncolsx+ncolsy));protecti++;
    const int *restrict x = INTEGER(x_s);
    const int *restrict y = INTEGER(y_s);
    int *restrict result = INTEGER(result_s);
	//    OMP_PARALLEL_FOR(nth)
	OMP_SIMD
      for (R_xlen_t irow = 0; irow < nrowsx; ++irow) {
        for (R_xlen_t jcol = 0; jcol < ncolsx; ++jcol) {
          result[irow+nrowsx*jcol] = x[rowsx[irow]+ colsx[jcol]*dim0x];
        }
        for (R_xlen_t jcol = 0; jcol < ncolsy; ++jcol) {
          result[irow+nrowsx*(jcol+ncolsx)] = y[rowsy[irow]+ colsy[jcol]*dim0y];
        }
      }
  } break;
  case REALSXP : {
    result_s = PROTECT(Rf_allocMatrix(REALSXP, nrowsx, ncolsx+ncolsy));protecti++;
    const double *restrict x = REAL(x_s);
    const double *restrict y = REAL(y_s);
    double *restrict result = REAL(result_s);
	//    OMP_PARALLEL_FOR(nth)
	OMP_SIMD
      for (R_xlen_t irow = 0; irow < nrowsx; ++irow) {
        for (R_xlen_t jcol = 0; jcol < ncolsx; ++jcol) {
          result[irow+nrowsx*jcol] = x[rowsx[irow]+ colsx[jcol]*dim0x];
        }
        for (R_xlen_t jcol = 0; jcol < ncolsy; ++jcol) {
          result[irow+nrowsx*(jcol+ncolsx)] = y[rowsy[irow]+ colsy[jcol]*dim0y];
        }
      }
  } break;
  case STRSXP : {
    result_s = PROTECT(Rf_allocMatrix(STRSXP, nrowsx, ncolsx+ncolsy));protecti++;
    const SEXP *restrict x = STRING_PTR(x_s);
    const SEXP *restrict y = STRING_PTR(y_s);
    //OMP_PARALLEL_FOR(nth)
	OMP_SIMD
      for (R_xlen_t irow = 0; irow < nrowsx; ++irow) {
        for (R_xlen_t jcol = 0; jcol < ncolsx; ++jcol) {
          SET_STRING_ELT(result_s, irow+nrowsx*jcol, x[rowsx[irow]+colsx[jcol]*dim0x]);
        }
        for (R_xlen_t jcol = 0; jcol < ncolsy; ++jcol) {
          SET_STRING_ELT(result_s, irow+nrowsx*(jcol+ncolsx), y[rowsy[irow]+colsy[jcol]*dim0y]);
        }

      }
  } break;
  default: // # nocov
    Rf_error("Type %s not supported.", Rf_type2char(TYPEOF(x_s))); // # nocov
  }


  // Colnames. Check out the names.

  SEXP nmsx_s = PROTECT(Rf_getAttrib(x_s, R_DimNamesSymbol));protecti++;
  SEXP nmsy_s = PROTECT(Rf_getAttrib(y_s, R_DimNamesSymbol));protecti++;

  SEXP colnms_s = PROTECT(Rf_allocVector(STRSXP,ncolsx+ncolsy));protecti++;
  for(R_xlen_t i = 0; i < Rf_xlength(colnms_s); i++) SET_STRING_ELT(colnms_s,i,Rf_mkChar(""));

  int count = 0;

  if (TYPEOF(nmsx_s) == VECSXP && XLENGTH(nmsx_s) >= 2 &&
      TYPEOF(VECTOR_ELT(nmsx_s, 1)) == STRSXP) {
    SEXP colnmsx_s =PROTECT(VECTOR_ELT(nmsx_s,1));protecti++;
    for(R_xlen_t i =0; i< ncolsx; i++){
      SET_STRING_ELT(colnms_s,i,STRING_ELT(colnmsx_s,colsx[i]));
      count++;
    }
  }

  if (TYPEOF(nmsy_s) == VECSXP && XLENGTH(nmsy_s) >= 2 &&
      TYPEOF(VECTOR_ELT(nmsy_s, 1)) == STRSXP) {
    SEXP colnmsy_s =PROTECT(VECTOR_ELT(nmsy_s,1));protecti++;
    for(R_xlen_t i =0; i< ncolsy; i++){
      SET_STRING_ELT(colnms_s,i+ncolsx,STRING_ELT(colnmsy_s,colsy[i]));
      count++;
    }
  }

  //Rprintf("%d",count);

  if(count) {
    SEXP dimnames_s = PROTECT(Rf_allocVector(VECSXP, 2));protecti++;
    SET_VECTOR_ELT(dimnames_s, 0, R_NilValue);
    SET_VECTOR_ELT(dimnames_s, 1, colnms_s);
    // Assign column names to the matrix
	//Rf_PrintValue(dimnames_s);
	 Rf_setAttrib(result_s, R_DimNamesSymbol, dimnames_s);
  }

  UNPROTECT(protecti);
  return result_s;
}
