/*
  TITLE:
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-05-22 10:40 emilio on emilio-despacho>
*/


#include "sparsematrix.h"


SparseMatrix* create_sparsematrix(int nrows, int ncols) {
  if (nrows < 0 || ncols < 0){
    Rf_warning("create_sparsematrix: You are creating a matrix with dimension: (%d,%d). I return NULL.\n", nrows,ncols);
    return NULL;
  }
  SparseMatrix* m = (SparseMatrix*) R_Calloc(1, SparseMatrix);
  m->nrows = nrows;
  m->ncols = ncols;
  
  m->map = create_hashmap(0);
    
  return m;
}

void free_sparsematrix(SparseMatrix* m){
  if(m==NULL) return;
  if(m->map != NULL) free_hashmap(m->map);
  R_Free(m);
}


void insert_sparsematrix( int row, int col, int x, SparseMatrix* m) {
  // We dont check, since it is an internal function
  /* if(matrix==NULL || (*matrix) == NULL) return; */
  /* if (row < 0 || row >= (*matrix)->nrows || */
  /*     col < 0 || col >= (*matrix)->ncols) { */
  /*   //  Rf_warning("insert_element_sparsematrix: (row,col) out of bounds: (%d,%d). Matrix is dimension (%d,%d)\n", row, col,(*matrix)->nrows,(*matrix)->ncols); */
  /*   return; */
  /* } */
    
  Key* key = R_Calloc(1,Key);
  key->type = INT;
  key->size = 2;
  key->x = R_Calloc(2,int);
  key->x[0] = row ;
  key->x[1] = col;

  Key* value = R_Calloc(1,Key);
  value->type =INT;
  value->size = 1;
  value->x = R_Calloc(1,int);
  value->x[0] = x;
  
  insert_hashmap(key, value, m->map);
   
  free_key(key);
  free_key(value);
  
}


int get_sparsematrix( int row, int col, SparseMatrix* m, int zerovalue) {
  // Create a unique key for the hash table (e.g., row * cols + col)
  /* if(matrix==NULL) return zerovalue; */
  /* if (row < 0 || row >= matrix->nrows || */
  /*     col < 0 || col >= matrix->ncols) { */
  /*   Rf_warning("get_element_sparsematrix: (row,col) out of bounds: (%d,%d). Matrix is dimension (%d,%d)\n", row, col,matrix->nrows,matrix->ncols); */
  /*   return zerovalue; */
  /* } */

  Key* key = R_Calloc(1,Key);
  key->type = INT;
  key->size = 2;
  key->x = R_Calloc(2,int);
  key->x[0] = row;
  key->x[1] = col;

  Key* value = R_Calloc(1,Key);

  int check = get_hashmap(key,  m->map, value);

  /* Rprintf("l138\n"); */
  /* print_hashmap(map,1); */
  /* print_key(key); */
  /* print_key(value); */

  int result;
  if(check) {
	result = value->x[0];
  } else {
	result = zerovalue;
  }

  free_key(key);
  free_key(value);
	
  
  return  result;
}



// Print the sparse matrix. Just for debugging
void print_sparsematrix(SparseMatrix* matrix) {
  Rprintf("Sparse Matrix (%dx%d) with %ld elements (closed %d):\n", matrix->nrows, matrix->ncols,matrix->map->size,matrix->map->closed);
  for (int i = 0; i < matrix->nrows; i++) {
    for (int j = 0; j < matrix->ncols; j++) {
      int value = get_sparsematrix( i, j, matrix,0);
      Rprintf("%d\t", value);
    }
    Rprintf("\n");
  }
}

int size_sparsematrix(SparseMatrix* m){
  return size_hashmap(m->map);
}

/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
