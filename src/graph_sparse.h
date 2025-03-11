#ifndef GRAPH_SPARSE_H
#define GRAPH_SPARSE_H

#include "sparsematrix.h"



SparseMatrix* create_sparse_symmetric_adjacency_matrix_with_positions_of_matrix(int* pos, int n, int nrows);
void find_farthest_nodes(SparseMatrix *adj, int *node1, int *node2, int* distance);
void visit_all_points_of_matrix(int* pos, int n, int nrows, int* result);

// User R
SEXP get_order_pos(SEXP pos_s, SEXP nrows_s); 

#endif
