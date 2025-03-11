/*
  TITLE:
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-03-08 11:48 emilio on emilio-XPS-15-9570>
*/

#include "graph_sparse.h"
#include "hashtable.h"
#include "tictoc.h"


// Directions: 8 possible movements in a 2D grid
#define MAX_NEIGHBORS 8
const int directions[MAX_NEIGHBORS][2] = {
  {-1, -1}, {0, -1}, {1, -1},
  {-1,  0},         {1,  0},
  {-1,  1}, {0,  1}, {1,  1}
};


#define jCOLC(pos,nrows) ((int)((pos)/(nrows)))
#define iROWC(pos,nrows) ((pos)  - (jCOLC((pos),(nrows)))* (nrows))
#define pPOSC(row,col,nrows)  ( (row) + (col)* (nrows))
///void get_irow_jcol_C(int* pos, int* rows, int* cols, int n, int nrows);

// Function to convert positions to row and column indices

void get_irow_jcol_C(int* pos, int* rows, int* cols, int n, int nrows) {
  for (int i = 0; i < n; i++) {
    int col = (pos[i]) / nrows ; // pos is C: 0,1,2,
    int row = pos[i]  - (col)* nrows ;
    cols[i] = col;
    rows[i] = row;
  }
}


// Perform BFS to find the shortest paths from a starting node.
// Return the longest shortest path and the distances(fromstart, toallnodes)
// Find the farthest nodes in the connected graph
int bfs_rows_cols( int start, int* rows, int* cols, int n, int nrows, int *distances, SparseMatrix* points) {
  int* visited = R_Calloc(n,int);
  int* queue=  R_Calloc(n,int);
  int front = 0, rear = 0;

  int maxcols = -1;
  // Initialize distances
  for (int i = 0; i < n; i++) {
    distances[i] = INT_MAX;
    if(maxcols < cols[i]) maxcols = cols[i];
  }

  // Start BFS
  queue[rear++] = start;
  visited[start] = 1;
  distances[start] = 0;

  int farthest_node = start;

  while (front < rear) {
    int current = queue[front++];

    OMP_SIMD
      for(int d = 0; d < MAX_NEIGHBORS; d++) {
        int newRow = rows[current] + directions[d][0];
        int newCol = cols[current] + directions[d][1];
        int neighbor = get_sparsematrix(newRow,newCol,points,-1);
        if( neighbor > -1 &&  !visited[neighbor]){
          visited[neighbor] = 1;
          distances[neighbor] = distances[current] + 1;
          queue[rear++] = neighbor;

          // Track the farthest node
          if (distances[neighbor] > distances[farthest_node]) {
            farthest_node = neighbor;
          }
        }
      }
  }
  R_Free(visited);
  R_Free(queue);
  return farthest_node;
}

// Find the farthest nodes in the connected graph
void find_farthest_nodes_rows_cols(int* rows, int* cols, int n, int nrows, int *node1, int *node2, int* distance, SparseMatrix* points) {
  int* distances = R_Calloc(n,int);
  int farthest_node;

  // Start BFS from an arbitrary node (0)
  farthest_node = bfs_rows_cols(0,rows,cols,n, nrows, distances, points);

  // Perform BFS again from the farthest node to find the farthest pair
  *node1 = farthest_node;
  *node2 = bfs_rows_cols(farthest_node,rows,cols,n, nrows, distances, points);

  *distance = distances[*node2];
  R_Free(distances);
}



// Visit all nodes starting From.
// visited 0-1 if visit a node,
// result is the nodes visited in that order.
// return the steps in the correct direction (from up to down and from right to left)
int walk_and_count_steps_in_the_correct_direction_rows_cols(int from,  int *visited, int *result, int *rows, int *cols, int n,int nrows, SparseMatrix* points){

  if(from >= 0 && from < n && visited[from]) return 0;

  int steps =0;

  // Arrays for visited flags and queue
  int *queue = R_Calloc(3*n,int); // To avoid problems with disconnected nodes.


  int front = 0, rear = 0; // queue indices
  int idx = 0;             // index into result array
  OMP_SIMD
    for (int i = 0; i < n; i++) {
      idx += (visited[i] >0); // To avoid problems with disconnected nodes.

    }


  // Start from the 'from' node
  visited[from] = 1;
  queue[rear++] = from;
  result[idx++] = from;

  // Standard BFS loop
  while (front < rear) {
    int current = queue[front++];
    // Check all possible neighbors
    OMP_SIMD
      for(int d = 0; d < MAX_NEIGHBORS; d++) {
        int newRow = rows[current] + directions[d][0];
        int newCol = cols[current] + directions[d][1];
        int neighbor = get_sparsematrix(newRow,newCol,points,-1);
        if( neighbor > -1 &&  !visited[neighbor]){
          visited[neighbor] = 1;
          queue[rear++] = neighbor;
          result[idx++] = neighbor;
          if(cols[current] < cols[neighbor] ) steps+= 3; // Going from right to left
          if(rows[current] < rows[neighbor] ) steps++; // Going from up to down
          //    Rprintf("%d ",i);
        }
      }
  }

  // Rprintf("\n");
  // Clean up
  R_Free(queue);
  return steps;
}






// Visit all points (pixels) with positions pos (in C: 0,1,2,3,) of a matrix with nrows
// result is the oo (in C format: 0,1,2), so pos[result] are the sorted points.
void visit_all_points_of_matrix(int* pos, int n, int nrows, int* result){
  //SparseMatrix* adj= create_sparse_symmetric_adjacency_matrix_with_positions_of_matrix( pos, n,  nrows);

  Rprintf("graphsparse l210\n");
  //Rprintf("Adjency matrix (%d x %d) nodes with %d arcs.\n",adj->nrows,adj->ncols,adj->size);
  //print_sparsematrix(adj);







  int* visited = R_Calloc(n,int);
  int* rows = R_Calloc(n,int);
  int* cols = R_Calloc(n,int);
  get_irow_jcol_C( pos, rows, cols,  n, nrows);

  SparseMatrix* points = create_sparsematrix(nrows,1); // Not important the dimensions
  OMP_SIMD
    for (int i = 0; i < n; i++) {
      insert_sparsematrix(rows[i],cols[i],i,points);
    }


  int node_1, node_2, distance;


  //int t1 = tic_named("farthest 1");
  find_farthest_nodes_rows_cols(rows,cols,n,nrows,&node_1, &node_2,  &distance, points);
  // toc_print_timer(t1);

  Rprintf("Farthest nodes are %d (%d) and %d (%d) (distance %d).\n",pos[node_1],node_1,pos[node_2],node_2,distance);

  int steps_1;
  steps_1= walk_and_count_steps_in_the_correct_direction_rows_cols(node_1,  visited, result, rows, cols, n, nrows,points);

  int* visited_2 = R_Calloc(n,int);
  int* result_2 = R_Calloc(n,int);

  int steps_2 =  walk_and_count_steps_in_the_correct_direction_rows_cols(node_2,  visited_2, result_2, rows, cols, n, nrows,points);

  Rprintf("Steps1 %d steps 2 %d\n",steps_1,steps_2);
  
  if(steps_1 < steps_2){
    Rprintf("Interchange From and To. We start in %d (%d)\n",pos[node_2],node_2);
    memcpy(visited,visited_2,n*sizeof(int));
    memcpy(result,result_2,n*sizeof(int));
  }
  R_Free(visited_2);
  R_Free(result_2);



  // disconnected nodes? No debería, pero por si acaso
  for (int i = 0; i < n; i++) {
    // If there's an edge and it's not visited yet
    if (!visited[i]) {
      Rprintf("Maldición. Hay grafos desconectados. Vuelta a empezar en walk and count %d (%d)\n",pos[i],i);
      // Habría que comprobar si este nodo es el mejor o no para empezar...
      // Pero en estos boxes estoy suponiendo que están todos los puntos conectados.
      walk_and_count_steps_in_the_correct_direction_rows_cols(i,  visited, result, rows, cols, n, nrows,points);
    }
  }
  R_Free(visited);
  R_Free(rows);
  R_Free(cols);
  free_sparsematrix(points);


}




// ============================================================
// pos is a vector of positions of a matrix with nrows
// return the oo to visit them: that is pos[oo] is the sorted path.
// pos should be connected.
// If length(pos)>maxlength, choose the first element as beginning of the graph
SEXP get_order_pos(SEXP pos_s, SEXP nrows_s) {

  if (TYPEOF(pos_s) != INTSXP)
    Rf_error("POS vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(pos_s)));

  if (TYPEOF(nrows_s) != INTSXP)
    Rf_error("NROWS vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(nrows_s)));

  if (LENGTH(nrows_s) != 1)
    Rf_error("NROWS is not a singe integer. It is '%s' with length %d.",
             Rf_type2char(TYPEOF(nrows_s)), LENGTH(nrows_s));



  int nrows = INTEGER(nrows_s)[0];
  int *pos = INTEGER(pos_s);
  int n = LENGTH(pos_s);

  if ( nrows<1  || nrows== NA_INTEGER) {
    Rf_error("X has length %d but nrows of matrix is %d (nrows>0).", LENGTH(pos_s), nrows);
  }

  int protecti = 0;
  SEXP result_s = PROTECT(Rf_allocVector(INTSXP, n));protecti++;
  int *result = INTEGER(result_s);

  if(n < 1) {
    UNPROTECT(protecti);
    return result_s;
  }

  int* ordendevisita= R_Calloc(n,int);
  visit_all_points_of_matrix(pos, n, nrows, ordendevisita);

  for(int i=0; i<n;i++){
    int k =ordendevisita[i];
    result[k] = i+1;
    //Rprintf("Ordenados pos %d (original place %d) ordendevisita %d\n", pos[k],k,result[k]);
  }

  R_Free(ordendevisita);

  UNPROTECT(protecti);
  return result_s;
}


/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
