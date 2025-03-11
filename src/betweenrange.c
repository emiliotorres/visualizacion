// Emilio Torres Manzanera
// University of Oviedo
// Time-stamp: <2023-12-30 02:09 emilio on emilio-XPS-15-9570>
// ============================================================

// ============================================================
// ============================================================



// ============================================================
// ============================================================

#include "matchmatrix.h"


SEXP betweenrangeoo(SEXP xoo_s, SEXP loweroo_s, SEXP upperoobylower_s) {
  // You should use gc() after calling this function.

  if(LENGTH(loweroo_s)!=LENGTH(upperoobylower_s)) {
    Rf_error("Lengths of lower and uppers limits are different: %d and %d.", LENGTH(loweroo_s),LENGTH(upperoobylower_s));
  }

  if (!Rf_isVectorAtomic(xoo_s)){
    Rf_error("Only atomic vectors X can be used.");
  }

  if (!Rf_isVectorAtomic(loweroo_s)){
    Rf_error("Only atomic vectors LOWER can be used.");
  }

  if (!Rf_isVectorAtomic(upperoobylower_s)){
    Rf_error("Only atomic vectors LOWER can be used.");
  }

  if( (TYPEOF(xoo_s)!= TYPEOF(loweroo_s)) || (TYPEOF(xoo_s) !=TYPEOF(upperoobylower_s))) {
    Rf_error("X, LOWER and UPPER must be of the same type, but they are different: %s, %s, %s.",Rf_type2char(TYPEOF(xoo_s)), Rf_type2char(TYPEOF(loweroo_s)),Rf_type2char(TYPEOF(upperoobylower_s)));
  }

  int protecti=0;

  // Rprintf("range: length of x %d, length of lower %d\n",LENGTH(x_s),LENGTH(lower_s));

  // Normally the dimension will be lower. How to increase this

  SEXP result_s=R_NilValue;
  R_xlen_t count=0;

  R_xlen_t i=0;
  R_xlen_t j=0;
  R_xlen_t nx=Rf_length(xoo_s);
  R_xlen_t nl=Rf_length(loweroo_s);

  //Rprintf("range: passed protect\n");
  switch(TYPEOF(xoo_s)) {

  case LGLSXP:
  case INTSXP:
    {
      int* x=INTEGER(xoo_s);
      int* lower=INTEGER(loweroo_s);
      int* upper=INTEGER(upperoobylower_s);
      result_s=PROTECT(Rf_allocVector(INTSXP,3*(LENGTH(xoo_s)+LENGTH(loweroo_s))));protecti++;
      int* result=INTEGER(result_s);

	  while(i < nx && j < nl ){
		//Rprintf("i: %d, x: %d, j: %d, u:%d\n",i,x[i],j,upper[j] );
		while(i< nx && x[i] < lower[j]) {++i;}
		while(j< nl && x[i] > upper[j]) {++j;}
		int jtemp = j;
		//Rprintf("jtemp %d\n",jtemp );
	    while( jtemp < nl && x[i] >= lower[jtemp] && x[i]<= upper[jtemp]) {
		  //Rprintf("i: %d, x: %d, jtemp: %d, u:%d\n",i,x[i],jtemp,upper[jtemp] );
		  R_xlen_t t = LENGTH(result_s);
		  if(count+4 > t){
			result_s=PROTECT(Rf_xlengthgets(result_s,3*count));protecti++;
			result=INTEGER(result_s);
		  }
		  result[count++] = x[i];
		  result[count++] = lower[jtemp];
		  result[count++] = upper[jtemp];
		  ++jtemp;
		}
		++i;
	  }
	 
	 	 
      break;
    }
  case REALSXP:
    {
      double* x=REAL(xoo_s);
      double* lower=REAL(loweroo_s);
      double* upper=REAL(upperoobylower_s);
      result_s=PROTECT(Rf_allocVector(REALSXP,3*(LENGTH(xoo_s)+LENGTH(loweroo_s))));protecti++;
      double* result=REAL(result_s);

	  
	  while(i < nx && j < nl ){
		while(i< nx && x[i] < lower[j]) {++i;}
		while(j< nl && x[i] > upper[j]) {++j;}
		int jtemp = j;
	    while( jtemp < nx && x[i] >= lower[jtemp] && x[i]<= upper[jtemp]) {
		  R_xlen_t t = LENGTH(result_s);
		  if(count+4 > t){
			result_s=PROTECT(Rf_xlengthgets(result_s,3*count));protecti++;
			result=REAL(result_s);
		  }
		  result[count++] = x[i];
		  result[count++] = lower[jtemp];
		  result[count++] = upper[jtemp];
		  ++jtemp;
		}
		++i;
	  }
	  
	  

	  
      /* for(R_xlen_t xi=0; xi < LENGTH(x_s); xi++){ */
      /*   for(R_xlen_t il=0; il < LENGTH(lower_s); il++) { */
      /*     if( (x[xi]>= lower[il]) && (x[xi] <= upper[il]) ){ */

      /*       R_xlen_t t = LENGTH(result_s); */
      /*       if(count+4 > t){ */
      /*         result_s=PROTECT(Rf_xlengthgets(result_s,3*count));protecti++; */
      /*         result=&INTEGER(result_s)[0]; */
      /*       } */
      /*       result[count++] = x[xi]; */
      /*       result[count++] = lower[il]; */
      /*       result[count++] = upper[il]; */
      /*     } */
      /*   } */
      /* } */
      break;
    }
  default:
	{
	  UNPROTECT(protecti);
	  Rf_error("Type '%s' is not supported.",Rf_type2char(TYPEOF(xoo_s)));
	}

  }
  //Rprintf("Vaciamos: de %d a %d\n", LENGTH(result_s),count);
  SETLENGTH(result_s,count);
  //Rprintf("range: passed setlength\n");
  UNPROTECT(protecti);
  //Rprintf("range: passed unprotect \n");
  return result_s;

}
