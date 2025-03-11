/*
  TITLE: 
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-03-05 17:58 emilio on emilio-XPS-15-9570>
*/

#include <Rinternals.h> // SEXP
#ifdef nrows // Rinternals.h define this!
#undef nrows
#endif
#ifdef ncols
#undef ncols
#endif


#include "tictoc.h"

SEXP tic_SEXP(){
  tic();
	return Rf_ScalarInteger(0);
}

SEXP toc_SEXP(){
   return Rf_ScalarReal(toc());
}

SEXP tic_named_SEXP(SEXP x_s){
  if(TYPEOF(x_s) != STRSXP || LENGTH(x_s)!=1){
	Rf_error("X (%s) must be a character of length 1.",Rf_type2char(TYPEOF(x_s)));
  }
  return Rf_ScalarInteger(tic_named( CHAR(Rf_asChar(x_s))));
}

SEXP toc_timer_SEXP(SEXP x_s){
  if(TYPEOF(x_s) != INTSXP || LENGTH(x_s)!=1){
	Rf_error("X (%s) must be an integer of length 1.",Rf_type2char(TYPEOF(x_s)));
  }
   return Rf_ScalarReal(toc_timer(INTEGER(x_s)[0]));
}

SEXP toc_print_timer_SEXP(SEXP x_s){
  if(TYPEOF(x_s) != INTSXP || LENGTH(x_s)!=1){
	Rf_error("X (%s) must be an integer of length 1.",Rf_type2char(TYPEOF(x_s)));
  }
  toc_print_timer(INTEGER(x_s)[0]);
  return Rf_ScalarInteger(1);
}

/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
