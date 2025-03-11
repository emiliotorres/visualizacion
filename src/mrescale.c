
#include <R.h>
#include <Rdefines.h>
#include "matchmatrix.h"




#define MIN(a, b) ((a) < (b) ? (a) : (b))

// ============================================================
// ============================================================


// ============================================================
// Convert a matrix 3 x 4 into a matrix 5 x 6.
// using moving average.

SEXP get_rescale_matrix(SEXP x_s, SEXP new_row_s, SEXP new_col_s) {

  if (!Rf_isMatrix(x_s)) {
    SEXP class_s = Rf_getAttrib(x_s, R_ClassSymbol);
    if (class_s == R_NilValue) {
      Rf_error("X is not a matrix and has no class attribute.");
    } else {
      Rf_error("X is not a matrix. Its class is: %s", CHAR(STRING_ELT(class_s, 0)));
    }
  }


  R_xlen_t oldrow = Rf_nrows(x_s);
  R_xlen_t oldcol = Rf_ncols(x_s);
  if( oldrow < 1 || oldcol < 1 ) {
    Rf_error("Rows of X is %ld and cols is %ld", oldrow,oldcol);
  }

  if (TYPEOF(new_row_s) != INTSXP)
    Rf_error("ROW_NEW integer not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(new_row_s)));

  if (LENGTH(new_row_s) != 1)
    Rf_error("ROW_NEW integer not valid. It is '%s' with length %d.",
             Rf_type2char(TYPEOF(new_row_s)), LENGTH(new_row_s));

  if (TYPEOF(new_col_s) != INTSXP)
    Rf_error("COL_NEW integer not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(new_col_s)));

  if (LENGTH(new_col_s) != 1)
    Rf_error("COl_NEW integer not valid. It is '%s' with length %d.",
             Rf_type2char(TYPEOF(new_col_s)), LENGTH(new_col_s));

  R_xlen_t newrow = INTEGER(new_row_s)[0];
  R_xlen_t newcol = INTEGER(new_col_s)[0];

  if (newrow < 1 || newcol < 1 || newrow== NA_INTEGER || newcol==NA_INTEGER) {
    Rf_error("NEW_ROW is %ld  and NEW_COL is %ld.", newrow, newcol);
  }

  int protecti = 0;

  /* SEXP dim_s = PROTECT(Rf_allocVector(INTSXP, 2));protecti++; */
  /* INTEGER(dim_s)[0] = newrow; */
  /* INTEGER(dim_s)[1] = newcol; */
  // 3. Set the "dim" attribute of the vector to make it a matrix
  //Rf_setAttrib(result_s, R_DimSymbol, dim_s);
  //memset(result, 0, newrow*newcol * sizeof(double)); // Fill it with 0

  R_xlen_t ipos=0;
  R_xlen_t jpos=0;
  int sumarow;
  int sumacol;

  //Rprintf("Original data\n");
  //Rf_PrintValue(x_s);

  // Resize the columns. We have to pass from columns of length 3 to columns of length 5.
  // From a matrix 3 x 4, we create a matrix 5 x 4
  SEXP resizebycolumn_s = PROTECT(Rf_allocMatrix(REALSXP,newrow,oldcol));protecti++;
  double* resizebycolumn = REAL(resizebycolumn_s);

  // Rprintf("Begin resize columns \n");

  if(newrow == oldrow) { // If no rescale. just copy the matrix into resizebycolumn
    // Convert into a double matrix
    switch(TYPEOF(x_s)) {

    case LGLSXP:
    case INTSXP:
      {
        int* x=INTEGER(x_s);
		for(int i = 0; i< newrow*oldcol; i++){
          resizebycolumn[i] = (double) x[i];
        }

        break;
      }
    case REALSXP:
      {
		memcpy(resizebycolumn, REAL(x_s), newrow*oldcol* sizeof(double));
		break;
      }
    default:
      {
        UNPROTECT(protecti);
        Rf_error("Type '%s' is not supported.",Rf_type2char(TYPEOF(x_s)));
      }
    }

  } else {

    memset(resizebycolumn, 0, newrow*oldcol * sizeof(double)); // Fill it with 0

  
    switch(TYPEOF(x_s)) {

    case LGLSXP:
    case INTSXP:
      {
        int* x=INTEGER(x_s);

		for(int jcolumnx = 0; jcolumnx < oldcol; jcolumnx++ ) {

		  sumarow = newrow;
		  sumacol = oldrow;

		  ipos=0;
		  jpos=0;

          while(ipos < oldrow && jpos < newrow) {
            if( sumacol >0  && sumarow >0) {

              int value = MIN(sumacol, sumarow);
              resizebycolumn[jpos + jcolumnx*newrow] += ((double)value/oldrow) * (double)x[ipos + jcolumnx*oldrow];
              sumacol -= value;
              sumarow -= value;

            } else if( sumacol == 0 && sumarow >0) {

              jpos++;
              sumacol = oldrow;

            } else if( sumacol >0 && sumarow ==0 ) {
              ipos++;
              sumarow = newrow;

            } else {
              ipos++;
              jpos++;
              sumacol=oldrow;
              sumarow=newrow;
            }
          }

        }



        break;
      }
    case REALSXP:
      {
        double* x=REAL(x_s);

        for(int jcolumnx = 0; jcolumnx < oldcol; jcolumnx++ ) {

		  sumarow = newrow;
		  sumacol = oldrow;

		  ipos=0;
		  jpos=0;

          while(ipos < oldrow && jpos < newrow) {
            if( sumacol >0  && sumarow >0) {

              int value = MIN(sumacol, sumarow);
              resizebycolumn[jpos + jcolumnx*newrow] += ((double)value/oldrow) * x[ipos + jcolumnx*oldrow];
              sumacol -= value;
              sumarow -= value;

            } else if( sumacol == 0 && sumarow >0) {

              jpos++;
              sumacol = oldrow;

            } else if( sumacol >0 && sumarow ==0 ) {
              ipos++;
              sumarow = newrow;

            } else {
              ipos++;
              jpos++;
              sumacol=oldrow;
              sumarow=newrow;
            }
          }

        }



        break;
      }
    default:
      {
        UNPROTECT(protecti);
        Rf_error("Type '%s' is not supported.",Rf_type2char(TYPEOF(x_s)));
      }

    }
  }


  /* Rprintf("After rescaling columns\n"); */
  /* Rf_PrintValue(resizebycolumn_s); */
  /* Rprintf("Class of resizebycolumn_s %s\n",Rf_type2char(TYPEOF(resizebycolumn_s)) ); */
  /* Rprintf("Begin resize rows \n"); */
  
 
  // resizebycolumn is a matrix 5 x 4
  // Now, we want to convert it into a matrix 5 x 6
  // Result. Create a matrix 5 x 6. Fill it with 0s.

  if(newcol == oldcol) { // If no rescale. just copy the matrix resizebycolumn into result
    // Convert into a double matrix
    UNPROTECT(protecti);
	return resizebycolumn_s;

  }

  SEXP result_s;
  double* result;

  result_s= PROTECT(Rf_allocMatrix(REALSXP,newrow,newcol));protecti++;
  result = REAL(result_s);
  memset(result, 0, newrow*newcol * sizeof(double)); // Fill it with 0

  // Input matrix is resizebycolumn_s
  // Dimension of resizebycolumn_s is newrow x oldcol
  oldrow = newrow;
   
  for(int irowx = 0; irowx < oldrow; irowx++ ) {

	sumarow = newcol;
	sumacol = oldcol;

	ipos=0;
	jpos=0;

    while(ipos < oldcol && jpos < newcol) {
      if( sumacol >0  && sumarow >0) {

        int value = MIN(sumacol, sumarow);
        result[irowx + jpos*newrow] += ((double)value/oldcol) * resizebycolumn[irowx + ipos*oldrow];
        sumacol -= value;
        sumarow -= value;

      } else if( sumacol == 0 && sumarow >0) {

        jpos++;
        sumacol = oldcol;

      } else if( sumacol >0 && sumarow ==0 ) {
        ipos++;
        sumarow = newcol;

      } else {
        ipos++;
        jpos++;
        sumacol=oldcol;
        sumarow=newcol;
      }
    }

  }

  //Rprintf("Vaciamos: de %d a %d\n", LENGTH(result_s),count);
  // SETLENGTH(result_s,count);
  //Rprintf("range: passed setlength\n");
  UNPROTECT(protecti);
  return result_s;

}




/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
