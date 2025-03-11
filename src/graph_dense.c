#include <R.h>
#include <Rdefines.h>
#include <stdlib.h> // For qsort
#include <stdio.h>

#include "boundingbox.h"



// Function to convert a Direction enum value to a string (for easier debugging/logging)
const char* directionToString(Direction dir) {
  switch (dir) {
  case DOT:               return "DOT";
  case DOTDOT:            return "DOTDOT";
  case TILDE: return "TILDE";
  case VIRGULILLA: return "VIRGULILLA";
  case INSIDE:            return "INSIDE";
  case  EQUALSIGN: return "EQUALSIGN";
  case  SEMICOLONSIGN: return   "SEMICOLONSIGN";
  case CLOSESTRIGHT: return "CLOSESTRIGHT";
  case SAMELINERIGHT: return "SAMELINERIGHT";
  case TWOLINESFROMTOPTOBOTTOM: return "TWOLINESFROMTOPTOBOTTOM";
  case TWOLINESFROMBOTTOMTOTOP: return "TWOLINESFROMBOTTOMTOTOP";
  case SCRIPTNORMALSUBFIRST: return "SCRIPTNORMALSUBFIRST";
  case SCRIPTNORMALOVERFIRST: return "SCRIPTNORMALOVERFIRST";
  case TWOLINESFROMBOTTOMTOTOPFRIEND: return "TWOLINESFROMBOTTOMTOTOPFRIEND";
  case CLUSTERSUBNORMALRIGHT: return "CLUSTERSUBNORMALRIGHT";
	
  case   FRACNUMFRAC: return  "FRACNUMFRAC";
  case	FRACFRACDENOM: return 	"FRACFRACDENOM";
  case FRACFRONT1: return "FRACFRONT1";
  case FRACFRONT2: return "FRACFRONT2";
  case   FRACOVER: return "FRACOVER";
  case   FRACUNDER: return "FRACUNDER";
  case	CLUSTERFRACRIGHT: return "CLUSTERFRACRIGHT";
  case CLUSTERFRACLEFT: return "CLUSTERFRACLEFT";


  case WORDRIGHT: return "WORDRIGHT";


  case SCRIPTSUMUNDER:   return "SCRIPTSUMUNDER";
  case SCRIPTSUMOVER:    return "SCRIPTSUMOVER";
  case SCRIPTMATHORDER:   return "SCRIPTMATHORDER";
  case SCRIPTNORMALUNDER:  return "SCRIPTNORMALUNDER";
  case SCRIPTNORMALOVER:   return "SCRIPTNORMALOVER";
  case SCRIPTNORMALORDER:  return "SCRIPTNORMALORDER";
  case   SCRIPTRIGHT: return "SCRIPTRIGHT";
  case SCRIPTLEFT: return "SCRIPTLEFT";
  case  MATHLETTERRIGHT: return "MATHLETTERRIGHT";
  case  MATHLETTERLEFT: return "MATHLETTERLEFT";
  case NORMALLETTERRIGHT: return "NORMALLETTERRIGHT";
  case NORMALLETTERLEFT: return "NORMALLETTERLEFT";
  case OVERSCRIPT:        return "OVERSCRIPT";
  case UNDERSCRIPT:         return "UNDERSCRIPT";
  case FROMUNDERTOOVER:   return "FROMUNDERTOOVER";
  case INMEDIATELYRIGHT:  return "INMEDIATELYRIGHT";
  case RIGHT:             return "RIGHT";
  case INTERMEDIATERIGHT: return "INTERMEDIATERIGHT";
  case INTERMEDIATELEFT: return "INTERMEDIATELEFT";
  case SCRIPTSUMOVERRIGHT: return "SCRIPTSUMOVERRIGHT";
  case SCRIPTSUMOVERLEFT: return "SCRIPTSUMOVERLEFT";
  case SCRIPTSUMUNDERRIGHT: return "SCRIPTSUMUNDERRIGHT";
  case SCRIPTSUMUNDERLEFT: return "SCRIPTSUMUNDERLEFT";
  case   SCRIPTSUMFROMBOTTOMTOTOP: return "SCRIPTSUMFROMBOTTOMTOTOP";

  case OVERLAPYRIGHT: return "OVERLAPYRIGHT";
  case OVERLAPPINGX: return "OVERLAPPINGX";
  case OVERLAPYRIGHTPAGE: return "OVERLAPYRIGHTPAGE";
  case UNKNOWNDIRECTION:  return "UNKNOWNDIRECTION";
  case OVERLAPYRIGHTYTOPPAGE: return "OVERLAPYRIGHTYTOPPAGE";
  case FIRSTINYTOPPAGE: return "FIRSTINYTOPPAGE";
  case FIRSTYTOPPAGE: return "FIRSTYTOPPAGE";
  case  OVERLAPYLINE: return      "OVERLAPYLINE";
  case ABOVELINE: return "ABOVELINE";
  case INTERMEDIATEOVERLAPPINGY: return "INTERMEDIATEOVERLAPPINGY";


  case ORPHANDOUBLECHAR: return "ORPHANDOUBLECHAR";
  case	ORPHANFRIENDBASE: return "ORPHANFRIENDBASE";
  case ORPHANBASENEXT: return "ORPHANBASENEXT";
  case ORPHANDBASEFRIEND: return "ORPHANDBASEFRIEND";
  case ORPHANPREVIOUSBASE: return"ORPHANPREVIOUSBASE";
	
	
  default:                return "UNKNOWN";
  }
}


// *Graph Representation*: The adjacency matrix =adj= represents the
// graph, where =
// adj[j + i*numBoxes] = 1  means ai < aj.
// adj[i + j*numBoxes] = 1  means ai > aj.
// adj = 0 means no defined relationship.
/* = { */
/*    {0, 0, 1, 0, 0}, */
/*    {0, 0, 0, 1, 0}, */
/*    {0, 0, 0, 0, 1}, */
/*    {0, 0, 0, 0, 0}, */
/*    {0, 0, 0, 0, 0} */
/* }; */



// Function to perform topological sort using Kahn's Algorithm
int topologicalSortKahn(int *adjMatrix, int n, int *result) {
  // int inDegree[10] = {0};  // Array to store in-degrees of all vertices
  int* inDegree = R_Calloc(n,int);

  int count = 0;           // To keep track of number of nodes in the result
  // int queue[10];           // Queue to hold nodes with zero in-degree
  int* queue = R_Calloc(n,int);
  int front = 0, rear = 0; // Front and rear indices for the queue

  // Calculate in-degrees of all vertices
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
      if (adjMatrix[i+j*n]) { //      if (adjMatrix[j][i] == 1)
        inDegree[i]++;
      }
    }
  }

  // Enqueue vertices with in-degree 0
  for (int i = 0; i < n; i++) {
    if (inDegree[i] == 0) {
      queue[rear++] = i;
    }
  }

  // Process queue and build topological order
  while (front < rear) {
    int u = queue[front++];
    result[count++] = u;

    for (int v = 0; v < n; v++) {
      //if (adjMatrix[u][v] == 1) {
      if (adjMatrix[v+u*n]) {
        if (--inDegree[v] == 0) {
          queue[rear++] = v;
        }
      }
    }
  }

  R_Free(inDegree);
  R_Free(queue);

  // Check if all vertices were included in the result
  if (count != n) {
    Rprintf("The graph contains a cycle.\n");
    return 0;
  }
  return 1;
}



int dfs(int node, int *adj, int *visited, int *stack, int *top, int* rec_stack, int N) {

   if (rec_stack[node]) {
	 if(DEBUGGING) Rprintf("dfs: cycle detected Base %d\n",  node);
	 return 1;  // Cycle detected. Error
  }
  if (visited[node]) {
    return 0;
  }

  visited[node] = 1;
  rec_stack[node] =1;

  for (int i = 0; i < N; i++) {
	//if(visited[i]) continue;
    //if (*(adj + node * N + i) && !visited[i]) {
    if(*(adj + node * N + i)){
	  //Rprintf("DFS node %d visiting %d with %s\n",node,i, directionToString(*(adj+node*N+i)));
      if(dfs(i, adj, visited, stack, top, rec_stack, N))
        return 1;
    }
  }

  stack[(*top)--] = node;  // Push the node onto the stack

  rec_stack[node] = 0;
  return 0;

}

int topologicalSort(int *adj, int N,int* stack) {
  //int *visited = (int *)malloc(N * sizeof(int));
  int* visited = R_Calloc(N,int);
  int* rec_stack = R_Calloc(N,int);
  //  int *stack = (int *)malloc(N * sizeof(int));
  int top = N - 1;  // Initialize stack top

  // Initialize visited array to 0
  for (int i = 0; i < N; i++) {
    visited[i] = 0;
    rec_stack[i]=0;
  }

  // Perform DFS for each unvisited node
  for (int i = 0; i < N; i++) {
    if(!visited[i]) {
      if(dfs(i, adj, visited, stack, &top, rec_stack, N)){
		R_Free(visited);
		R_Free(rec_stack);
        return 0;  // Cycle detected, not a DAG
      }

    }
  }

  
  /* Rprintf("Topological Order: \n"); */
  /* for (int i = 0; i < N; i++) { */
  /*   Rprintf("%d ", stack[i]); */
  /* } */
  /* Rprintf("\n"); */

  R_Free(visited);
  R_Free(rec_stack);
  return 1;
}


// ============================================================



 







/* 
   Count the number of distinct paths from source to target in a DAG.
   We process nodes in topological order and use dp[u] to represent the 
   number of paths from source to node u.
   For our purpose, we cap the counts at 2 (i.e. 2 means ">=2").
   Returns:
   -1 if the graph is not a DAG,
   0 if there is no path,
   1 if exactly one path,
   2 if there is more than one path.
*/
int countPathsInDAG(const int *adj, const int *topoorder,int n, int source, int target) {
  //int *order =   topologicalSort(n, adj);
      
  int *dp = R_Calloc(n, int);
 
    
  dp[source] = 1;
  // Process nodes in topologically sorted order.
  for (int i = 0; i < n; i++) {
	int u = topoorder[i];
	if (dp[u] == 0) continue;
	for (int v = 0; v < n; v++) {
	  //   Direction adj[ib+ ia*numBoxes]; From a to b
	  if (adj[v + u*n] != 0) {
		dp[v] += dp[u];
		// Cap at 2: since we only care if there is >1 path.
		if (dp[v] > 1)
		  dp[v] = 2;
	  }
	}
  }
    
  int paths = dp[target];
  R_Free(dp);
  //R_Free(order);
  return paths;
}


void showmultiplepaths(const int *adj,const int *topoorder, int n ){

  for(int ii = 0; ii<n; ii++){
	int i =topoorder[ii];
	for(int jj=ii+1; jj <n ; jj++){
	  int j =topoorder[jj];
	  int paths = countPathsInDAG(adj, topoorder,n, i, j);
	  if(paths>1) {
		Rprintf("Multiples paths from %d to %d\n",i,j);
		break;
	  }
	}
  }
}


// ---------------------------------------------------------------------
// Function: longestPathInDAG
//
// Computes the longest (unweighted) path from 'source' to 'target' in a DAG.
// 'adj' is an n x n matrix representing the graph.
// If a path exists, the function returns a dynamically allocated array
// containing the sequence of node indices from source to target,
// and the number of nodes in the path is stored in *pathLength.
// If no path exists (or if the graph is not a DAG), the function returns NULL.
int* longestPathInDAG(const int *adj, const int *topoorder, int n, int source, int target, int *pathLength) {
  //int *order = topologicalSort(n, adj);
    
  // dp[i] will hold the length (i.e. number of edges) of the longest path from
  // 'source' to node i. Initialize all entries to a very small value.
  int *dp = R_Calloc(n ,int);
  // pred[i] will record the predecessor of i on the longest path.
  int *pred = R_Calloc(n,int);
 
    
  for (int i = 0; i < n; i++) {
	dp[i] = INT_MIN;  // Use INT_MIN as "negative infinity"
	pred[i] = -1;
  }
  dp[source] = 0;  // The longest path from source to itself has 0 edges.
    
  // Process nodes in topological order.
  for (int i = 0; i < n; i++) {
	int u = topoorder[i];
	if (dp[u] == INT_MIN) continue;  // No path from source to u.
	for (int v = 0; v < n; v++) {
	  //   Direction adj[ib+ ia*numBoxes]; From a to b
	  if (adj[v+u*n] != 0) {
		// If we can relax the edge u->v, then update.
		if (dp[u] + 1 > dp[v]) {
		  dp[v] = dp[u] + 1;
		  pred[v] = u;
		}
	  }
	}
  }
    
     
  if (dp[target] == INT_MIN) {
	// No path exists from source to target.
	*pathLength = 0;
	R_Free(dp);
	R_Free(pred);
	return NULL;
  }
    
  // Reconstruct the longest path from target back to source using the pred array.
  // The number of nodes in the path is dp[target] + 1.
  int length = dp[target] + 1;
  *pathLength = length;
  int *path = R_Calloc(length, int);
    
  int curr = target;
  for (int i = length - 1; i >= 0; i--) {
	path[i] = curr;
	curr = pred[curr];
  }
    
  R_Free(dp);
  R_Free(pred);
    
  return path;
}

// ---------------------------------------------------------------------

// ---------------------------------------------------------------------

// Function to get the longest path in an adjacency matrix of a DAG
int* longestPathInDAGMatrix(const int *adj, const int *topoorder, int n, int *pathLength) {
  int maxLength = 0;
  int *longestPath = NULL;

  for (int source = 0; source < n; source++) {
	for (int target = 0; target < n; target++) {
	  if (source == target) continue; // No need to find path from node to itself

	  int currPathLength;
	  int *currPath = longestPathInDAG(adj, topoorder, n, source, target, &currPathLength);

	  if (currPath != NULL && currPathLength > maxLength) {
		if (longestPath != NULL) R_Free(longestPath);
		longestPath = currPath;
		maxLength = currPathLength;
	  } else {
		R_Free(currPath);
	  }
	}
  }

  *pathLength = maxLength;
  return longestPath;
}






int insertDirectionInAdjacencyMatrix(int from, int to, Direction direction, const BoundingBox* boxes, int* adj, int numBoxes, char* message, int verbose){

  //Rprintf("upadate adj %d %d %d\n",numBoxes,indexA,indexB);
  char newmessage[1000];
  if(from<0 || to < 0 || from>numBoxes || to > numBoxes){
	snprintf(newmessage,1000,"Upadating an adjacent matrix of dimension %d: pos %d and pos %d.\n",numBoxes,from, to);
	strcat(message,newmessage);
	if(DEBUGGING){
	  Rf_error(newmessage);//"Upadating an adjacent matrix of dimension %d: pos %d and pos %d.\n",numBoxes,from, to);
	} else {
	  return 0;
	}
  }

  int a=boxes[from].index;
  int b=boxes[to].index;
 
  if(from == to || a==b){
	snprintf(newmessage,1000,"You want to create a loop in box %d (pos %d, pos %d) with %s.\n",
			a, from, to, directionToString(direction));
	strcat(message,newmessage);
	if(DEBUGGING){
	  Rf_error(newmessage);//"Upadating an adjacent matrix of dimension %d: pos %d and pos %d.\n",numBoxes,from, to);
	} else {
	  return 0;
	}

  
  }


  if( adj[from + to*numBoxes] ){
	snprintf(newmessage,1000,"You want to Box %d preceed %d with %s, but Box %d preceed box %d (%s).\n",
			a,b,directionToString(direction),b,a, directionToString(adj[a + b*numBoxes])
			);
	strcat(message,newmessage);
	if(DEBUGGING){
	  Rf_error(newmessage);//"Upadating an adjacent matrix of dimension %d: pos %d and pos %d.\n",numBoxes,from, to);
	} else {
	  return 0;
	}
   
  }

  Direction prev=adj[to+ from*numBoxes];
  if(prev && prev < direction){
	snprintf(newmessage,1000,"Box %d preceed box %d with %s, but you want now  %s.\n",
			a,b,directionToString(prev),directionToString(direction));
	strcat(message,newmessage);
	if(DEBUGGING){
	  Rf_error(newmessage);//"Upadating an adjacent matrix of dimension %d: pos %d and pos %d.\n",numBoxes,from, to);
	} else {
	  return 0;
	}
  }
  
  adj[to + from*numBoxes] = direction ; // if >0, A < B
  if(verbose){
	snprintf(newmessage,1000,"Box %d preceed box %d with %s (prev. %s).\n",a,b,directionToString(direction),directionToString(prev));
	strcat(message,newmessage);
  }
  return 1;

}




int updateAdjacencyMatrix(int from, int to, Direction direction, const BoundingBox* boxes, int *adj,  int numBoxes){

  int* tempadj = R_Calloc(numBoxes*numBoxes,int);
  memcpy(tempadj,adj, numBoxes*numBoxes*sizeof(int) );

  char* message=R_Calloc(1000,char);
  int verbose = (direction != ABOVELINE) && DEBUGGING;
  if(verbose) snprintf(message,1000,"Checking... ");
  int check= insertDirectionInAdjacencyMatrix(from, to, direction, boxes,  tempadj, numBoxes,message,0);
  if(!check){

	if(DEBUGGING) Rprintf(message);
	R_Free(message);
	R_Free(tempadj);
	return 0;
	
  }
 
  int* result=R_Calloc(numBoxes,int);
  check = topologicalSort(tempadj, numBoxes,result);
  R_Free(result);
  R_Free(tempadj);

  if(check){
	if(verbose) strcat(message,"ok. ");
	
	if(direction == ABOVELINE)  verbose = 0;
	insertDirectionInAdjacencyMatrix(from, to, direction, boxes,  adj, numBoxes,message, verbose);

  } else {
	char newmessage[1000];
	snprintf(newmessage,1000,"...You are trying to create a cycle in %d with %d (%s). I do not allow it!\n",
			boxes[from].index,boxes[to].index,directionToString(direction));
	strcat(message,newmessage);
	
  }

  if(verbose) Rprintf(message);

  R_Free(message);
  return  check;
  
}
  
// ============================================================

int arcFromTo(int from, int to, const BoundingBox* boxes, const int *adj, int numBoxes){
  if(from< 0 || to < 0 || from>= numBoxes || to >= numBoxes){
    Rf_error("Positions of boxes out of range (0-%d): posA %d, posB %d\n", numBoxes,from, to);
  }
  
  return adj[to+ from*numBoxes];
}

int noarc(int i, int j, const BoundingBox* boxes, const int *adj, int numBoxes){
  if(i< 0 || j < 0 || i>= numBoxes || j >= numBoxes){
    Rf_error("Positions of boxes out of range (0-%d): posA %d, posB %d\n", numBoxes,i,j);
  }
  
  return adj[i + j*numBoxes] == 0 && adj[j + i*numBoxes] == 0;
}

int notincludedAinB(const BoundingBox* boxes,const int *adj, int numBoxes, int indexA, int indexB){
  return arcFromTo(indexB,indexA,boxes, adj,numBoxes)!=INSIDE;
}


//To find the diameter of graph using its adjacency matrix, you can use the Floyd-Warshall algorithm to compute the shortest paths between all pairs of nodes. The diameter of the graph is the longest shortest path between any two nodes. Once you have the shortest paths, you can determine the nodes that form this diameter.
void floydWarshall(int* adj, int n, int* dist) {
  // Initialize distance matrix
   OMP_SIMD
  for (int i = 0; i < n; i++) {
	for (int j = 0; j < n; j++) {
	  if (i == j) {
		dist[i * n + j] = 0;
	  } else if (adj[i * n + j] >= 1) {
		dist[i * n + j] = 1;
	  } else {
		dist[i * n + j] = INT_MAX;
	  }
	}
  }

  // Floyd-Warshall algorithm
  OMP_SIMD
  for (int k = 0; k < n; k++) {
	for (int i = 0; i < n; i++) {
	  for (int j = 0; j < n; j++) {
		if (dist[i * n + k] != INT_MAX && dist[k * n + j] != INT_MAX &&
			dist[i * n + k] + dist[k * n + j] < dist[i * n + j]) {
		  dist[i * n + j] = dist[i * n + k] + dist[k * n + j];
		}
	  }
	}
  }
}


void floydWarshall_adjsymmetric(int* adj, int n, int* dist) {
  // Initialize distance matrix - only upper triangular part
  OMP_SIMD
  for (int i = 0; i < n; i++) {
	dist[i * n + i] = 0;  // diagonal
	for (int j = i + 1; j < n; j++) {
	  int idx = i * n + j;
	  dist[idx] = (adj[idx] >= 1) ? 1 : INT_MAX;
	  dist[j * n + i] = dist[idx];  // mirror value due to symmetry
	}
  }

  // Floyd-Warshall algorithm
  OMP_SIMD
  for (int k = 0; k < n; k++) {
	for (int i = 0; i < n - 1; i++) {
	  if (dist[i * n + k] == INT_MAX) continue;  // Skip if no path through k
            
	  for (int j = i + 1; j < n; j++) {
		if (dist[k * n + j] == INT_MAX) continue;  // Skip if no path through k
                
		int newDist = dist[i * n + k] + dist[k * n + j];
		if (newDist < dist[i * n + j]) {
		  dist[i * n + j] = newDist;
		  dist[j * n + i] = newDist;  // mirror value due to symmetry
		}
	  }
	}
  }
}



void get_most_distant_nodes(int* adj, int n, int issymmetric, int* from, int* to, int* distance) {
  
  int* dist = R_Calloc(n*n, int);

  if(issymmetric)   floydWarshall_adjsymmetric(adj, n, dist);
  else floydWarshall(adj, n, dist);

  *distance = 0;
  *from = -1;
  *to = -1;

  // Find the diameter
  OMP_SIMD
  for (int i = 0; i < n; i++) {
	for (int j = i + 1; j < n; j++) {
	  if (dist[i * n + j] != INT_MAX && dist[i * n + j] > *distance) {
		*distance = dist[i * n + j];
		*from = i;
		*to = j;
	  }
	}
  }

  R_Free(dist);
}
