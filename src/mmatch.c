#include "matchmatrix.h"
#include <math.h> // log2
//#define M_LOG2E 1.44269504088896340736 // log2(e)   log(x) * M_LOG2E;



// ============================================================
// ============================================================


SEXP SetHashTable(SEXP x_s,  SEXP idrows_s, SEXP idcols_s,  SEXP h){

  int protecti=0;
  // Create a character vector for attribute name
  SEXP attrName_s = PROTECT(Rf_mkString("match_matrix"));protecti++;
  /* Construct named result list from variables containing the results */
  const char *names[] = {"idcols", "idrows","h",""}; /* note the null string */
  SEXP res = PROTECT(Rf_mkNamed(VECSXP, names));protecti++;  /* list of length 4 */
  SET_VECTOR_ELT(res, 0, idcols_s);
  SET_VECTOR_ELT(res, 1, idrows_s);
  SET_VECTOR_ELT(res, 2, h);
  Rf_setAttrib(x_s, attrName_s, res);

  UNPROTECT(protecti);
  return res;

}


SEXP get_hash_table_attached(SEXP x_s, SEXP idrows_s, SEXP idcols_s){
  int protecti=0;

  int *idcols = INTEGER(idcols_s);
  int *idrows = INTEGER(idrows_s);
  R_len_t ncols = Rf_length(idcols_s);
  R_len_t nrows = Rf_length(idrows_s);

  // Create a character vector for attribute name
  SEXP attrName_s = PROTECT(Rf_mkString("match_matrix"));protecti++;
  /* Construct named result list from variables containing the results */
  SEXP attached_s = Rf_getAttrib(x_s, attrName_s);
  if(Rf_length(attached_s)!=3) {
	UNPROTECT(protecti);
	return R_NilValue;
  }
  //Rprintf("cols\n");
  SEXP element_s = VECTOR_ELT(attached_s,0);
  int* element=INTEGER(element_s);
  if (Rf_length(element_s)  != ncols || memcmp(element,idcols,sizeof(element[0])*ncols)!=0){
	//Rprintf("elemn %d cols %d\n",Rf_length(element_s), ncols);
	UNPROTECT(protecti);
	return R_NilValue;
  }
 
  element_s = VECTOR_ELT(attached_s,1);
  element=INTEGER(element_s);
  if (Rf_length(element_s)  != nrows || memcmp(element,idrows,sizeof(element[0])*nrows)!=0){
	UNPROTECT(protecti);
	return R_NilValue;
  }
  
  //  Rprintf("Attached table!\n");
  UNPROTECT(protecti);
  return VECTOR_ELT(attached_s,2);
  
}

// ============================================================
// ============================================================

// Return a vector with 0 and the (first) position of idrows where a distint element of x appear.
// that is x[rows[res[res>0]],] are the unique elements.
SEXP create_hash_table(SEXP x_s, SEXP idrows_s, SEXP idcols_s) {

  

  int protecti = 0;
  // If the table exists in the attached properies.
  SEXP h_s =PROTECT(get_hash_table_attached(x_s, idrows_s, idcols_s));protecti++;
  if(h_s!=R_NilValue){
	UNPROTECT(protecti);
	return h_s;
  }

  
  int ncols = Rf_length(idcols_s);
  R_len_t nrowsx = Rf_isMatrix(x_s) ? Rf_nrows(x_s) : Rf_length(x_s);
  int *idcols = INTEGER(idcols_s);
  int *idrows = INTEGER(idrows_s);
  R_len_t nidrows = Rf_length(idrows_s);

  // Rprintf("Now we check the size\n");

  const size_t n2 = 2U * (size_t)nidrows;
  size_t M = 256;
  int K = 8;
  while (M < n2) {
    M *= 2;
    K++;
  }
  //  Rprintf("M %d K %d %d\n",M,K,log2(M));
  R_xlen_t count = 0;
  int *restrict h = (int *)Calloc(M, int);
  // int *restrict pans = (int *)Calloc(nrowsx, int);
  /* int *restrict groupfirstposition = (int *)Calloc(nidrows, int); */
  /* int *restrict sizeofthegroup = (int *)Calloc(nidrows, int); */
  size_t id = 0;

  switch (TYPEOF(x_s)) {
  case LGLSXP:
  case INTSXP: { // See kit::kit_dup.c
    int *x = INTEGER(x_s);
    // Rprintf("Empieza el hash\n");
    for (R_xlen_t i = 0; i < nidrows; ++i) {
      R_xlen_t key = 0;
      for (R_xlen_t j = 0; j < ncols; ++j) {
        R_xlen_t k= idrows[i] + idcols[j] * nrowsx;
        key ^= HASH(((x[k] == NA_INTEGER)
                     ? 0
                     : x[k]),
                    K) *
          97 * (j + 1);
      }
      id = HASH(key, K);
      while (h[id]) { // Mientras este puesto esté ocupado
        for (R_xlen_t j = 0; j < ncols; ++j) {
          if (x[idrows[h[id] - 1] + idcols[j] * nrowsx] != x[idrows[i] + idcols[j] * nrowsx]) {
            goto labelmi1;
          }
        }
        goto labelmi2; // Si son iguales vete al final
      labelmi1:;       // Si no son iguales, sigue mirando
        id++;
        id %= M;
      } // Hemos llegado a un puesto que no está ocupado
      h[id] = (int)i + 1; // In R format. For id=0 is the NA_integer
      // pans[i]++; // Anotamos un 1 en esa row
      /*   count++; // Anotamos cuantos casos distintos llevamos */
    labelmi2:; // Son iguales
      /*            // Archivamos la position idx de la primera fila de este grupo */
      /*   groupfirstposition[i] = h[id] - 1; // In C format */
      /*   sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de */
      // cada grupo
      // Rprintf("%d register. %d sizeofthisgrupo %d\n",i,h[id]-1,sizeofthegroup[h[id]-1]);
    }
    break;
  }
  case REALSXP: {
    const double *restrict x = REAL(x_s);
    union uno tpv;
    for (R_xlen_t i = 0; i < nidrows; ++i) {
      R_xlen_t key = 0;
      for (R_xlen_t j = 0; j < ncols; ++j) {
        tpv.d = x[idrows[i] + idcols[j] * nrowsx];
        key ^= HASH(tpv.u[0] + tpv.u[1], K) * 97 * (j + 1);
      }
      tpv.d = key;
      id = HASH(tpv.u[0] + tpv.u[1], K);
      while (h[id]) {
        for (R_xlen_t j = 0; j < ncols; ++j) {
          if (!REQUAL(x[idrows[h[id] - 1] + idcols[j] * nrowsx],
                      x[idrows[i] + idcols[j] * nrowsx])) {
            goto labelmr1;
          }
        }
        goto labelmr2;
      labelmr1:;
        id++;
        id %= M;
      }
      h[id] = (int)i + 1;
      // pans[i]++;
      count++;
    labelmr2:;
    }
    break;
  }
  case STRSXP: {
    const SEXP *restrict x = STRING_PTR(x_s);
    for (R_xlen_t i = 0; i < nidrows; ++i) {
      R_xlen_t key = 0;
      for (R_xlen_t j = 0; j < ncols; ++j) {
        key ^= HASH(((intptr_t)x[idrows[i] + idcols[j] * nrowsx] & 0xffffffff), K) *
          97 * (j + 1);
      }
      id = HASH(key, K);
      while (h[id]) {
        for (R_xlen_t j = 0; j < ncols; ++j) {
          if (x[idrows[h[id] - 1] + idcols[j] * nrowsx] !=
              x[idrows[i] + idcols[j] * nrowsx]) {
            goto labelms1; // # nocov
          }
        }
        goto labelms2;
      labelms1:; // # nocov
        id++;
        id %= M; // # nocov
      }
      h[id] = (int)i+ 1;
      count++;
    labelms2:;

    }
    break;
  }
  default: {
    Rf_error("Type '%s' not implemented.", Rf_type2char(TYPEOF(x_s)));
  }
  }

  // Rprintf("Empieza la lista de vectores: tama->o de la lista %d\n", count);

  // Una lista de vectores, cada elemento son las positions de cada grupo
  SEXP result_s = PROTECT(Rf_allocVector(INTSXP, M));protecti++;
  int* result = INTEGER(result_s);
  memcpy(result, h, M*sizeof(result[0]));

  Free(h);

  SetHashTable( x_s,  idrows_s, idcols_s,  result_s);


  UNPROTECT(protecti);

  return result_s;
}


// ============================================================

// For each x[idrowx, ] return the first match on y[idrowsy, ], giving the position of the row of y (in R format)

SEXP match_matrix(SEXP x_s, SEXP idrowsx_s, SEXP idcolsx_s, SEXP y_s, SEXP idrowsy_s, SEXP idcolsy_s, SEXP  nomatch_s ) {

  if(LENGTH(idcolsx_s)!= LENGTH(idcolsy_s)){
    Rf_error("Number of columns are diferent: %d and %d.",LENGTH(idcolsx_s), LENGTH(idcolsy_s));
  }
  if(TYPEOF(x_s)!= TYPEOF(y_s)) {
    Rf_error("Type of X and Y are not equal: '%s', '%s'.", Rf_type2char(TYPEOF(x_s)), Rf_type2char(TYPEOF(y_s)));
  }

  if(TYPEOF(nomatch_s)!=INTSXP){
	Rf_error("NOMATCH must be integer, not '%s'.", Rf_type2char(TYPEOF(nomatch_s)));
  }

  /* int nth = Rf_asInteger(nthreads_s); */
  /* nth = (nth < 1 )? max_thread: (nth > max_thread ? max_thread : (nth < min_thread ? min_thread : nth));  */

  int protecti=0;

  R_len_t nidrowsx = Rf_length(idrowsx_s);
  R_len_t ncols = Rf_length(idcolsx_s);
  R_len_t nrowsx = Rf_isMatrix(x_s) ? Rf_nrows(x_s) : Rf_length(x_s);
  R_len_t nrowsy = Rf_isMatrix(y_s) ? Rf_nrows(y_s) : Rf_length(y_s);
  int* idcolsx =INTEGER(idcolsx_s);
  int* idcolsy =INTEGER(idcolsy_s);
  int* idrowsx =INTEGER(idrowsx_s);
  int* idrowsy =INTEGER(idrowsy_s);
  
  int nomatch=Rf_asInteger(nomatch_s);

  SEXP h_s=PROTECT(create_hash_table(y_s, idrowsy_s, idcolsy_s));protecti++;
  int* h=INTEGER(h_s);
  SEXP result_s = PROTECT(Rf_allocVector(INTSXP,Rf_length(idrowsx_s)));protecti++;
  int* result=INTEGER(result_s);


  size_t id = 0;
  int M = Rf_length(h_s);
  int K = log2(M);

  switch (TYPEOF(x_s)) {
  case LGLSXP:
  case INTSXP: {
    int* x = INTEGER(x_s);
	int* y = INTEGER(y_s);
	//OMP_PARALLEL_FOR(nth)
	OMP_SIMD
    for (R_xlen_t i = 0; i < nidrowsx; ++i) {
      R_xlen_t key = 0;
      for (R_xlen_t j = 0; j < ncols; ++j) {
        R_xlen_t k= idrowsx[i] + idcolsx[j] * nrowsx;
        key ^= HASH(((x[k] == NA_INTEGER)
                     ? 0
                     : x[k]),
                    K) *
          97 * (j + 1);
      }
      id = HASH(key, K);
      while (h[id]) { // Mientras este puesto esté ocupado
        for (R_xlen_t j = 0; j < ncols; ++j) {
          if (y[idrowsy[h[id] - 1] + idcolsy[j] * nrowsy] != x[idrowsx[i] + idcolsx[j] * nrowsx]) {
            goto labelmi1;
          }
        }
        goto labelmi2; // Si son iguales vete al final
      labelmi1:;       // Si no son iguales, sigue mirando
        id++;
        id %= M;
      } // Hemos llegado a un puesto que no está ocupado
      //h[id] = (int)i + 1; // In R format. For id=0 is the NA_integer
      result[i] =nomatch;
      continue;
      // pans[i]++; // Anotamos un 1 en esa row
      /*   count++; // Anotamos cuantos casos distintos llevamos */
    labelmi2:; // Son iguales
      result[i] = h[id];
      /*            // Archivamos la position idx de la primera fila de este grupo */
      /*   groupfirstposition[i] = h[id] - 1; // In C format */
      /*   sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de */
      // cada grupo
      // Rprintf("%d register. %d sizeofthisgrupo %d\n",i,h[id]-1,sizeofthegroup[h[id]-1]);
    }
    break;
  }
    case REALSXP: {
      const double *restrict x = REAL(x_s);
	  const double *restrict y = REAL(y_s);
      union uno tpv;
	  //OMP_PARALLEL_FOR(nth)
		OMP_SIMD
      for (R_xlen_t i = 0; i < nidrowsx; ++i) {
        R_xlen_t key = 0;
        for (R_xlen_t j = 0; j < ncols; ++j) {
          tpv.d = x[idrowsx[i] + idcolsx[j] * nrowsx];
          key ^= HASH(tpv.u[0] + tpv.u[1], K) * 97 * (j + 1);
        }
        tpv.d = key;
        id = HASH(tpv.u[0] + tpv.u[1], K);
        while (h[id]) {
          for (R_xlen_t j = 0; j < ncols; ++j) {
            if (!REQUAL(y[idrowsy[h[id] - 1] + idcolsy[j] * nrowsy],
                        x[idrowsx[i] + idcolsx[j] * nrowsx])) {
              goto labelmr1;
            }
          }
          goto labelmr2;
        labelmr1:;
          id++;
          id %= M;
        }
		result[i] =nomatch;
		continue;
	  labelmr2:;
		result[i] = h[id];
     }
	  
      break;
    }
    case STRSXP: {
      const SEXP *restrict x = STRING_PTR(x_s);
	  const SEXP *restrict y = STRING_PTR(y_s);
	  //OMP_PARALLEL_FOR(nth)
		OMP_SIMD
      for (R_xlen_t i = 0; i < nidrowsx; ++i) {
        R_xlen_t key = 0;
        for (R_xlen_t j = 0; j < ncols; ++j) {
          key ^= HASH(((intptr_t)x[idrowsx[i] + idcolsx[j] * nrowsx] & 0xffffffff), K) *
           97 * (j + 1);
        }
        id = HASH(key, K);
        while (h[id]) {
          for (R_xlen_t j = 0; j < ncols; ++j) {
            if (y[idrowsy[h[id] - 1] + idcolsy[j] * nrowsy] !=
                x[idrowsx[i] + idcolsx[j] * nrowsx]) {
              goto labelms1; // # nocov
            }
          }
          goto labelms2;
        labelms1:; // # nocov
          id++;
          id %= M; // # nocov
        }
		result[i] =nomatch;
		continue;
     labelms2:;
		result[i] = h[id];
      }
      break;
    }
  default: {
    Rf_error("Type '%s' not implemented.", Rf_type2char(TYPEOF(x_s)));
  }
  }


  UNPROTECT(protecti);
  return result_s;
}
