/*
  TITLE: 
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-02-18 07:27 emilio on emilio-XPS-15-9570>
*/

#include <R.h>
#include <Rdefines.h>

#include <R.h>
#include <Rdefines.h>

/*
 * Extract a submatrix from matrix m based on specified row and column indices
 * Parameters:
 *   m: pointer to the input matrix (stored in row-major order)
 *   nrows: number of rows in the original matrix
 *   ncols: number of columns in the original matrix
 *   subrows: array of row indices to extract (0-based)
 *   nsubrows: number of rows to extract
 *   subcols: array of column indices to extract (0-based)
 *   nsubcols: number of columns to extract
 * Returns:
 *   Pointer to the new submatrix (nsubrows × nsubcols)
 */
int* submatrix_int(const int* m, int nrows, int ncols,
				   const int* subrows, int nsubrows, 
				   const int* subcols, int nsubcols) {
  // Allocate memory for the submatrix
  int* result =  R_Calloc(nsubrows * nsubcols, int);
  
  // Extract elements from the original matrix
  int i, j;
  for (i = 0; i < nsubrows; i++) {
	for (j = 0; j < nsubcols; j++) {
	  // Check if indices are within bounds
	  if (subrows[i] >= nrows || subcols[j] >= ncols || 
		  subrows[i] < 0 || subcols[j] < 0) {
		R_Free(result);
		Rf_error("Submatrix indices out of bounds.");
		return NULL;
	  }
            
	  // Calculate the position in the original matrix (row-major order)
	  int orig_pos = subrows[i] * ncols + subcols[j];
	  // Calculate the position in the result matrix
	  int new_pos = i * nsubcols + j;
	  // Copy the element
	  result[new_pos] = m[orig_pos];
	}
  }

  return result;
}

/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
