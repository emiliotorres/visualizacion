#ifndef SPARSEMATRIX_H
#define SPARSEMATRIX_H


#include "hashmap.h"


typedef struct {
  int nrows;         // Number of rows in the matrix
  int ncols;         // Number of columns in the matrix
  HashMap* map;
} SparseMatrix;


SparseMatrix* create_sparsematrix(int nrows, int ncols);
void insert_sparsematrix( int row, int col, int value, SparseMatrix* matrix);
int get_sparsematrix( int row, int col,  SparseMatrix* matrix,int zerovalue);
void free_sparsematrix( SparseMatrix* matrix );
void print_sparsematrix(SparseMatrix* matrix);
int size_sparsematrix(SparseMatrix* m);

#endif
