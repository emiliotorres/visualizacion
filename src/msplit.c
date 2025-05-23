#include "matchmatrix.h"


SEXP get_list_of_rows_with_same_values_in_this_matrix_all_rows(SEXP x_s, SEXP idcols_s) {

  int protecti = 0;
  int ncols = Rf_length(idcols_s);
  R_len_t nrowsx = Rf_isMatrix(x_s) ? Rf_nrows(x_s) : Rf_length(x_s);
  int *idcols = INTEGER(idcols_s);

  // Rprintf("Now we check the size\n");

  const size_t n2 = 2U * (size_t)nrowsx;
  size_t M = 256;
  int K = 8;
  while (M < n2) {
    M *= 2;
    K++;
  }
  R_xlen_t count = 0;
  int *restrict h = (int *)R_Calloc(M, int);
  // int *restrict pans = (int *)Calloc(nrowsx, int);
  int *restrict groupfirstposition = (int *)R_Calloc(nrowsx, int);
  int *restrict sizeofthegroup = (int *)R_Calloc(nrowsx, int);
  size_t id = 0;

  switch (TYPEOF(x_s)) {
  case LGLSXP:
  case INTSXP: { // See kit::kit_dup.c
    int *x = INTEGER(x_s);
    // Rprintf("Empieza el hash\n");
    for (R_xlen_t i = 0; i < nrowsx; ++i) {
      R_xlen_t key = 0;
      for (R_xlen_t j = 0; j < ncols; ++j) {
		R_xlen_t k= i + idcols[j] * nrowsx;
        key ^= HASH(((x[k] == NA_INTEGER)
                         ? 0
                         : x[k]),
                    K) *
               97 * (j + 1);
      }
      id = HASH(key, K);
      while (h[id]) { // Mientras este puesto esté ocupado
        for (R_xlen_t j = 0; j < ncols; ++j) {
          if (x[h[id] - 1 + idcols[j] * nrowsx] != x[i + idcols[j] * nrowsx]) {
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
      count++; // Anotamos cuantos casos distintos llevamos
    labelmi2:; // Son iguales
               // Archivamos la position idx de la primera fila de este grupo
      groupfirstposition[i] = h[id] - 1; // In C format
      sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de
                                   // cada grupo
    }
    break;
  }
  case REALSXP: {
    const double *restrict x = REAL(x_s);
    union uno tpv;
    for (R_xlen_t i = 0; i < nrowsx; ++i) {
      R_xlen_t key = 0;
      for (R_xlen_t j = 0; j < ncols; ++j) {
        tpv.d = x[i + idcols[j] * nrowsx];
        key ^= HASH(tpv.u[0] + tpv.u[1], K) * 97 * (j + 1);
      }
      tpv.d = key;
      id = HASH(tpv.u[0] + tpv.u[1], K);
      while (h[id]) {
        for (R_xlen_t j = 0; j < ncols; ++j) {
          if (!REQUAL(x[h[id] - 1 + idcols[j] * nrowsx],
                      x[i + idcols[j] * nrowsx])) {
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
      groupfirstposition[i] = h[id] - 1;
      sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de
    }
    break;
  }
  /* case STRSXP: { */
  /*   const SEXP *restrict x = STRING_PTR(x_s); */
  /*   for (R_xlen_t i = 0; i < nrowsx; ++i) { */
  /*     R_xlen_t key = 0; */
  /*     for (R_xlen_t j = 0; j < ncols; ++j) { */
  /*       key ^= HASH(((intptr_t)x[i + idcols[j] * nrowsx] & 0xffffffff), K) * */
  /*              97 * (j + 1); */
  /*     } */
  /*     id = HASH(key, K); */
  /*     while (h[id]) { */
  /*       for (R_xlen_t j = 0; j < ncols; ++j) { */
  /*         if (x[h[id] - 1 + idcols[j] * nrowsx] != */
  /*             x[i + idcols[j] * nrowsx]) { */
  /*           goto labelms1; // # nocov */
  /*         } */
  /*       } */
  /*       goto labelms2; */
  /*     labelms1:; // # nocov */
  /*       id++; */
  /*       id %= M; // # nocov */
  /*     } */
  /*     h[id] = (int)i + 1; */
  /*     count++; */
  /*   labelms2:; */
  /*     groupfirstposition[i] = h[id] - 1; // in C format 0,1,2,3 */
  /*     sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de */
  /*   } */
  /*   break; */
  /* } */


  case STRSXP: {
    // Get pointers to the character data using API-compliant methods
	//    const char **restrict x = (const char **)STRING_ELT(x_s, 0);
    for (R_xlen_t i = 0; i < nrowsx; ++i) {
	  R_xlen_t key = 0;
	  for (R_xlen_t j = 0; j < ncols; ++j) {
		// Get the string element from x_s using the calculated index
		SEXP str_elem = STRING_ELT(x_s, i + idcols[j] * nrowsx);
		// Hash the string element
		key ^= HASH(((intptr_t)CHAR(str_elem)[0] & 0xffffffff), K) * 97 * (j + 1);
	  }
	  id = HASH(key, K);
	  while (h[id]) {
		for (R_xlen_t j = 0; j < ncols; ++j) {
		  // Get the string elements for comparison
		  SEXP str_elem1 = STRING_ELT(x_s, h[id] - 1 + idcols[j] * nrowsx);
		  SEXP str_elem2 = STRING_ELT(x_s, i + idcols[j] * nrowsx);
		  // Compare the strings
		  if (strcmp(CHAR(str_elem1), CHAR(str_elem2)) != 0) {
			goto labelms1; // # nocov
		  }
		}
		goto labelms2;
	  labelms1:; // # nocov
		id++;
		id %= M; // # nocov
	  }
	  h[id] = (int)i + 1;
	  count++;
    labelms2:;
	  groupfirstposition[i] = h[id] - 1; // in C format 0,1,2,3
	  sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de
    }
    break;
  }
  default: {
    Rf_error("Type '%s' not implemented.", Rf_type2char(TYPEOF(x_s)));
  }
  }

  // Rprintf("Empieza la lista de vectores: tama->o de la lista %d\n", count);

  // Una lista de vectores, cada elemento son las positions de cada grupo
  SEXP result_s = PROTECT(Rf_allocVector(VECSXP, count));protecti++;
  int *restrict element_of_result =
      (int *)R_Calloc(nrowsx, int); // Element of the list
  int ngrp = 0;
  OMP_SIMD
  for (int i = 0; i < nrowsx; ++i) {
    // Estamos en las primeras filas, donde corresponden a las primeras filas de
    // cada grupo.
    // Rprintf("Row %d\n", i);
    if (i == groupfirstposition[i]) {
      element_of_result[i] = ngrp;

      SEXP positionsofthisgroup_s =
          PROTECT(Rf_allocVector(INTSXP, sizeofthegroup[i]));
      protecti++;
      SET_VECTOR_ELT(result_s, ngrp, positionsofthisgroup_s);
      UNPROTECT(1);
      protecti--;

      /* Rprintf("element_of_result i %d pos in vector %d\n", i, */
      /*         *(element_of_result + i)); */
      ngrp++;
    }

    /* Rprintf("row %d firstposition %d element %d\n", i, groupfirstposition[i],
     */
    /*         element_of_result[groupfirstposition[i]]); */
    int *positionsofthisgroup =
        INTEGER(VECTOR_ELT(result_s, element_of_result[groupfirstposition[i]]));
    int l =
        LENGTH(VECTOR_ELT(result_s, element_of_result[groupfirstposition[i]]));
    positionsofthisgroup[l - sizeofthegroup[groupfirstposition[i]]] =
        i + 1; // In R format
    sizeofthegroup[groupfirstposition[i]]--;
  }

  R_Free(h);
  //  Free(pans);
  R_Free(groupfirstposition);
  R_Free(sizeofthegroup);
  R_Free(element_of_result);

  UNPROTECT(protecti);
  return result_s;
}





// ============================================================
// ============================================================


SEXP get_list_of_rows_with_same_values_in_this_matrix(SEXP x_s, SEXP idrows_s, SEXP idcols_s) {

  int protecti = 0;
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
  R_xlen_t count = 0;
  int *restrict h = (int *)R_Calloc(M, int);
  // int *restrict pans = (int *)Calloc(nrowsx, int);
  int *restrict groupfirstposition = (int *)R_Calloc(nidrows, int);
  int *restrict sizeofthegroup = (int *)R_Calloc(nidrows, int);
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
      count++; // Anotamos cuantos casos distintos llevamos
    labelmi2:; // Son iguales
               // Archivamos la position idx de la primera fila de este grupo
      groupfirstposition[i] = h[id] - 1; // In C format
      sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de
                                   // cada grupo
	  //Rprintf("%d register. %d sizeofthisgrupo %d\n",i,h[id]-1,sizeofthegroup[h[id]-1]);
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
      groupfirstposition[i] = h[id] - 1;
      sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de
    }
    break;
  }
  /* case STRSXP: { */
  /*   const SEXP *restrict x = STRING_PTR(x_s); */
  /*   for (R_xlen_t i = 0; i < nidrows; ++i) { */
  /*     R_xlen_t key = 0; */
  /*     for (R_xlen_t j = 0; j < ncols; ++j) { */
  /*       key ^= HASH(((intptr_t)x[idrows[i] + idcols[j] * nrowsx] & 0xffffffff), K) * */
  /* 		  97 * (j + 1); */
  /*     } */
  /*     id = HASH(key, K); */
  /*     while (h[id]) { */
  /*       for (R_xlen_t j = 0; j < ncols; ++j) { */
  /*         if (x[idrows[h[id] - 1] + idcols[j] * nrowsx] != */
  /*             x[idrows[i] + idcols[j] * nrowsx]) { */
  /*           goto labelms1; // # nocov */
  /*         } */
  /*       } */
  /*       goto labelms2; */
  /*     labelms1:; // # nocov */
  /*       id++; */
  /*       id %= M; // # nocov */
  /*     } */
  /*     h[id] = (int)i+ 1; */
  /*     count++; */
  /*   labelms2:; */
  /*     groupfirstposition[i] = h[id] - 1; // in C format 0,1,2,3 */
  /*     sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de */
  /*   } */
  /*   break; */
  /* } */



  case STRSXP: {
    // Get pointers to the character data using API-compliant methods
	// const char **restrict x = (const char **)STRING_ELT(x_s, 0);
    for (R_xlen_t i = 0; i < nidrows; ++i) {
	  R_xlen_t key = 0;
	  for (R_xlen_t j = 0; j < ncols; ++j) {
		// Get the string element from x_s using the calculated index
		SEXP str_elem = STRING_ELT(x_s, idrows[i] + idcols[j] * nrowsx);
		// Hash the string element
		key ^= HASH(((intptr_t)CHAR(str_elem)[0] & 0xffffffff), K) * 97 * (j + 1);
	  }
	  id = HASH(key, K);
	  while (h[id]) {
		for (R_xlen_t j = 0; j < ncols; ++j) {
		  // Get the string elements for comparison
		  SEXP str_elem1 = STRING_ELT(x_s, idrows[h[id] - 1] + idcols[j] * nrowsx);
		  SEXP str_elem2 = STRING_ELT(x_s, idrows[i] + idcols[j] * nrowsx);
		  // Compare the strings
		  if (strcmp(CHAR(str_elem1), CHAR(str_elem2)) != 0) {
			goto labelms1; // # nocov
		  }
		}
		goto labelms2;
	  labelms1:; // # nocov
		id++;
		id %= M; // # nocov
	  }
	  h[id] = (int)i + 1;
	  count++;
    labelms2:;
	  groupfirstposition[i] = h[id] - 1; // in C format 0,1,2,3
	  sizeofthegroup[h[id] - 1]++; // el tamano del grupo de esa primera fila de
    }
    break;
  }
  default: {
    Rf_error("Type '%s' not implemented.", Rf_type2char(TYPEOF(x_s)));
  }
  }

  // Rprintf("Empieza la lista de vectores: tama->o de la lista %d\n", count);

  // Una lista de vectores, cada elemento son las positions de cada grupo
  SEXP result_s = PROTECT(Rf_allocVector(VECSXP, count));protecti++;
  int *restrict element_of_result =
	(int *)R_Calloc(nrowsx, int); // Element of the list
  int ngrp = 0;
  OMP_SIMD
  for (int i = 0; i < nidrows; ++i) {
    // Estamos en las primeras filas, donde corresponden a las primeras filas de
    // cada grupo.
    // Rprintf("Row %d\n", i);
	int grfstpos=groupfirstposition[i];
    if (i == grfstpos) {
      element_of_result[i] = ngrp;

      SEXP positionsofthisgroup_s =
		PROTECT(Rf_allocVector(INTSXP, sizeofthegroup[i]));
      protecti++;
      SET_VECTOR_ELT(result_s, ngrp, positionsofthisgroup_s);
      UNPROTECT(1);
      protecti--;

      /* Rprintf("element_of_result i %d pos in vector %d\n", i, */
      /*         *(element_of_result + i)); */
      ngrp++;
    }

    /* Rprintf("row %d firstposition %d element %d\n", i, groupfirstposition[i],
     */
    /*         element_of_result[groupfirstposition[i]]); */
    int *positionsofthisgroup =
	  INTEGER(VECTOR_ELT(result_s, element_of_result[grfstpos]));
    int l =
	  LENGTH(VECTOR_ELT(result_s, element_of_result[grfstpos]));
    positionsofthisgroup[l - sizeofthegroup[grfstpos]] =
	  idrows[i] + 1; // In R format
    sizeofthegroup[grfstpos]--;
  }

  R_Free(h);
  //  Free(pans);
  R_Free(groupfirstposition);
  R_Free(sizeofthegroup);
  R_Free(element_of_result);

  UNPROTECT(protecti);
  return result_s;
}
