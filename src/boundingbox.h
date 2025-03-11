#ifndef BOUNDINBBOX_H
#define BOUNDINBBOX_H
 

#include "sparsematrix.h"
#include "tictoc.h"

#define DEBUGGING 1

#ifdef _OPENMP
//#ifdef match // #include <R.h> problems whit omp.h
//#undef match
//#endif
#include <omp.h>
#define omp_enabled true
#define max_thread omp_get_num_procs()
#define min_thread 1
#define OMP_PARALLEL_FOR(nth) _Pragma("omp parallel for num_threads(nth)")
#define OMP_SIMD _Pragma("omp simd")
#else
#define omp_enabled false
#define max_thread 1
#define min_thread 1
#define omp_get_thread_num() 0
#define OMP_PARALLEL_FOR(n)
#define OMP_SIMD
#endif


#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) < (b)) ? (b) : (a))

#define TRESHOLDOVERSCRIPTMATHY 17
#define TRESHOLDSUBCRIPTMATHY 19
#define TRESHOLDOVERSCRIPTMATHX 26
#define TRESHOLDSUBCRIPTMATHX 26
#define TRESHOLDSCRIPTNORMALX 11
#define TRESHOLDSCRIPTNORMALOVERLAPX 4 // M_j
#define SEPSCRIPTSX 12 // M^{abcd}
#define SEPMATHLETTERSX 18 // \sum \sum
#define SEPNORMALLETTERSX 47 // M i M a M a

#define TRESHOLDCENTERX 26
#define TRESHOLDCENTERY 14
#define TRESHOLDDOT 12

typedef struct {
  int xleft;   // Left x-coordinate of the bounding box
  int xright;  // Right x-coordinate of the bounding box
  int ytop;    // Top y-coordinate of the bounding box
  int ybottom; // Bottom y-coordinate of the bounding box
  int index;   // Index of this box. Just to debug program
  int pos;     // position on the adjacency matrix
  int size;    // Number of close friends
  int* indices;// Index of close friends 
  int* positions; // Positions of close friends
} BoundingBox;








void  FreeBoundingBox(BoundingBox* pages,int npages);

typedef enum Direction { // Order matters!
  INSIDE=100,  
  DOT,
  DOTDOT,
  TILDE,
  VIRGULILLA,
  EQUALSIGN,
  SEMICOLONSIGN,

  WORDRIGHT,
  OVERLAPYRIGHT,
  CLOSESTRIGHT,
  SCRIPTNORMALSUBFIRST,
  SCRIPTNORMALOVERFIRST,
 
  TWOLINESFROMTOPTOBOTTOM,
  TWOLINESFROMBOTTOMTOTOP,
  TWOLINESFROMBOTTOMTOTOPFRIEND,
  CLUSTERSUBNORMALRIGHT,
  
  SAMELINERIGHT,
  
  FRACNUMFRAC,
  FRACFRACDENOM,
  FRACFRONT1,
  FRACFRONT2,
  FRACOVER,
  FRACUNDER,
  CLUSTERFRACRIGHT,
  CLUSTERFRACLEFT,
  
  SCRIPTSUMUNDER,
  SCRIPTSUMOVER,
  SCRIPTSUMOVERRIGHT,
  SCRIPTSUMOVERLEFT,
  SCRIPTSUMUNDERRIGHT,
  SCRIPTSUMUNDERLEFT,
  SCRIPTSUMFROMBOTTOMTOTOP,
  
  SCRIPTNORMALUNDER,
  SCRIPTNORMALOVER,
  SCRIPTMATHORDER,
  SCRIPTNORMALORDER,
  SCRIPTRIGHT,
  SCRIPTLEFT,
  MATHLETTERRIGHT,
  MATHLETTERLEFT,
  NORMALLETTERRIGHT,
  NORMALLETTERLEFT,  
  OVERSCRIPT,        
  UNDERSCRIPT,         
  FROMUNDERTOOVER, 
  INMEDIATELYRIGHT, 
  RIGHT,
  INTERMEDIATERIGHT,
  INTERMEDIATELEFT,  
 
  OVERLAPPINGX,
  OVERLAPYRIGHTPAGE,
  FIRSTINYTOPPAGE,
  OVERLAPYRIGHTYTOPPAGE,
  FIRSTYTOPPAGE,
  OVERLAPYLINE,
  ABOVELINE,
  INTERMEDIATEOVERLAPPINGY,

  ORPHANDOUBLECHAR,
  ORPHANFRIENDBASE,ORPHANBASENEXT,ORPHANDBASEFRIEND,ORPHANPREVIOUSBASE,
  UNKNOWNDIRECTION
} Direction;


int* get_sort_indices(int *vector, int size);

void getorderBoundingBoxes_C(BoundingBox* boxes, const int numBoxes, int* result);

BoundingBox unionBoundingBox(BoundingBox *boxA, BoundingBox *boxB, const BoundingBox* boxes);
void joinBoundingBoxAandB(BoundingBox* clusters, int* numclusters, int indexA, int indexB,  const BoundingBox* boxes, int* adj, int numBoxes);

int isAIncludedInB(const BoundingBox* A, const BoundingBox* B);
// Function to check if Bounding Box A is included in B
int isequalcoordenates(const BoundingBox* A, const BoundingBox* B);
// Function to check if two bounding boxes overlap on the x-axis
int overlapX(const BoundingBox* A, const BoundingBox* B);
// Function to check if two bounding boxes overlap on the y-axis
int overlapY(const BoundingBox* A, const BoundingBox* B);
// Function to check if bounding box A is above bounding box B
int aboveAoverB(const BoundingBox* A, const BoundingBox* B);
// Function to calculate the horizontal distance between two bounding boxes
int distanceX(const BoundingBox* A, const BoundingBox* B);
int distanceY(const BoundingBox* A, const BoundingBox* B);
// Function to check if the center of box A overlaps box B in X direction AND
// if the center of box B overlaps box A in X direction
int overlapCenterInX(const BoundingBox *boxA, const BoundingBox *boxB, int tr);
int overlapCenterInY(const BoundingBox *boxA, const BoundingBox *boxB, int tr);
// Function to calculate the height of a bounding box
int boxHeight(const BoundingBox* box);
// Function to calculate the width of a bounding box
int boxWidth(const BoundingBox* box);
int intersectAB(const BoundingBox* boxA, const BoundingBox* boxB);
int getcluster(const BoundingBox* clusters, const int* numclusters, int index);
int isatomicbbox(const BoundingBox* boxBase,  const BoundingBox* boxes, int numBoxes);
//BoundingBox getcolumnbetweenX(int xleft, int xright, const BoundingBox* boxes, int numBoxes);
BoundingBox* getLinesInThisPage(const BoundingBox* page, int* numberLines, const BoundingBox* boxes, int numBoxes);
BoundingBox* getLinefromthisbox(int indexbase, const BoundingBox* page, int maxdistx, int direction, const BoundingBox* boxes, int numBoxes);
int findindex(int i, const BoundingBox* boxes, int numBoxes);
void unionDimensionsA(BoundingBox* boxA, const BoundingBox* boxB);
int ClosestY(int ibase, const BoundingBox *boxes, int numBoxes, int direction);
int ClosestX(int ibase, const BoundingBox *boxes, int numBoxes, int direction);
int ClosestXpage(int ibase,  const BoundingBox* page, const BoundingBox *boxes, int numBoxes, int direction);
int ClosestYpage(int ibase, const BoundingBox* page,  const BoundingBox *boxes, int numBoxes, int direction);
int nothingbetweenYoverlapX(int i, int j, const BoundingBox* page, const BoundingBox* boxes, int numBoxes);
void showdistances(const BoundingBox* boxes, int numBoxes);
//void printcluster(const BoundingBox* clusters,int indexA);
BoundingBox* copyBoundingBox(const BoundingBox *A);
void copyBoundingBoxAtoB(const BoundingBox* A, BoundingBox* B);
void joinBoundingBoxAandBindex(BoundingBox* boxA, const BoundingBox* boxB, const BoundingBox* boxes, int numBoxes);
BoundingBox* getLinefromthisboxFrac(int indexbase, int indexfrac, const BoundingBox* page,  int direction, const BoundingBox* boxes, int numBoxes); 
void Rprintfbbox(const BoundingBox* b);
BoundingBox* unionsingleboxes(const int *indices, int nindices, const BoundingBox* b, int n);
void freebbox(BoundingBox* box);
void freebboxes(BoundingBox **pages, int npages);
int findpos(int index, const BoundingBox* boxes, int numBoxes);


int between(double x, double a, double b);
int* common_elements(int *a, int sizea, int *b, int sizeb, int *sizecommon);
int exists_in_array(int *arr, int size, int value);
int removeduplicatesint(int* arr, int size);
int compare_int(const void* a, const void* b);
int isSymmetricMatrix(int* m, int n);



int* submatrix_int(const int* m, int nrows, int ncols, const int* subrows, int nsubrows, 
				   const int* subcols, int nsubcols);

const char* directionToString(Direction dir);
int topologicalSort(int *adj, int N,int* stack);
void showmultiplepaths(const int *adj,const int *topoorder, int n );
int* longestPathInDAG(const int *adj, const int *topoorder, int n, int source, int target, int *pathLength);
void get_most_distant_nodes(int* adj, int n, int issymmetric, int* from, int* to, int* distance);
void floydWarshall(int* adj, int n, int* dist);
int* getorphanodes(const int* adj, int numBoxes, int *norphannodes);
int* longestPathInDAGMatrix(const int *adj, const int *topoorder, int n, int *pathLength);
int checkupdateAdjacencyMatrixFirstASecondB(const BoundingBox* boxes, int* adj, int numBoxes, int indexA, int indexB, Direction direction);
int arcFromTo(int from, int to, const BoundingBox* boxes, const int *adj, int numBoxes);
int noarc(int i, int j, const BoundingBox* boxes, const int *adj, int numBoxes);
int updateAdjacencyMatrix(int from, int to, Direction direction, const BoundingBox* boxes, int *adj,  int numBoxes);
int notincludedAinB(const BoundingBox* boxes,const int *adj, int numBoxes, int indexA, int indexB);

BoundingBox** splitminipages(const BoundingBox* p, int* n, BoundingBox* boxes, int *adj,  int numBoxes);
void orderwithinpage(const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes);

int joinFragmentedCharacters(const BoundingBox* page, BoundingBox* boxes, int* adj, int numBoxes);

BoundingBox* doinclusion( const BoundingBox* boxes, int* adj, int numBoxes);
void processFragmentedCharacters(BoundingBox* boxes, int*adj,const int numBoxes);
void doorderoverlapY(const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes);
void ordersubscriptsnormalmode(const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes);
void ordersubscriptsmathmode(const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes);
void doorderlines(const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes);
void orderfracmathmode( const BoundingBox* page,const BoundingBox* boxes, int *adj, int numBoxes);
void doorderwordright( const BoundingBox* page,const BoundingBox* boxes, int *adj, int numBoxes);
void processpage(const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes);
void lookingorphannodes( const BoundingBox* boxes, int* adj, int numBoxes);

BoundingBox* createLinesConnectedinAdjacency(const BoundingBox* page, int *numberlines, const BoundingBox* boxes,  int* adj, int numBoxes);
//void  orderoverlappingXinlines(const BoundingBox* pages, int numberpages, const BoundingBox* boxes,  int* adj, int numBoxes);
//void doorderpages(const BoundingBox*pages, int numPages, const BoundingBox* boxes, int* adj, int numBoxes);



#endif


/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
