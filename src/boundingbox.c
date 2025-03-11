#include "boundingbox.h"



// ============================================================
void printcluster(const BoundingBox* clusters,int indexA){
  Rprintf("Cluster %d (size %d) indices: ",clusters[indexA].index,clusters[indexA].size);
  for (int i=0; i< clusters[indexA].size; i++){
    Rprintf("%d ",clusters[indexA].indices[i]);
  }
  Rprintf("\n");
  Rprintf("Bbox: %d %d %d %d\n",clusters[indexA].xleft,clusters[indexA].ybottom,clusters[indexA].xright,clusters[indexA].ytop );
}

void Rprintfbbox(const BoundingBox* b){
  if(!b) {
	Rprintf("La caja es NULL\n");
	return;
  }
  Rprintf("Bbox %d (size %d) indices: ",b->index,b->size);
  for (int i=0; i< b->size; i++){
    Rprintf("%d ",b->indices[i]);
  }
  Rprintf("\n");
  Rprintf("Bbox %d (size %d) positions: ",b->index,b->size);
  for (int i=0; i< b->size; i++){
    Rprintf("%d ",b->positions[i]);
  }
  Rprintf("\n");

  Rprintf("Bbox: width %d height %d (%d %d %d %d)\n",b->xright-b->xleft,b->ybottom- b->ytop,b->xleft,b->ybottom,b->xright,b->ytop );
}







void freebbox(BoundingBox* box){
  if (box) {
    box->size = 0;       // Optional: Reset size to 0
    // Free the indices array if it exists
    if (box->indices) {
      R_Free(box->indices);  // Use free or R_Free as appropriate
    }
    // Free the indices array if it exists
    if (box->positions != NULL) {
      R_Free(box->positions);  // Use free or R_Free as appropriate
    }
    R_Free(box);
  }
}

void freebboxes(BoundingBox **pages, int npages) {
  if (pages){

  for (int i = 0; i < npages; i++) {
	freebbox(pages[i]);
  }
    
  R_Free(pages);
  }
}

void  FreeBoundingBox(BoundingBox* pages,int npages){
  if(pages){
	for (int i = 0; i < npages; i++) {
	  pages[i].size = 0;
	  R_Free(pages[i].indices);
	  R_Free(pages[i].positions);
	}
	R_Free(pages);
  
  }
}

void  FreeBoundingBoxSingle(BoundingBox* pages){
  if(pages){
		  pages->size = 0;
	  R_Free(pages->indices);
	  R_Free(pages->positions);
	}
}


void joinBoundingBoxAandBindex(BoundingBox* boxA, const BoundingBox* boxB, const BoundingBox* boxes, int numBoxes) {

  // Rprintf("Inicio de joinBoundingBoxAandBindex\n");
  if(boxA->index == boxB->index){
    Rf_error("You want to join the same cluster: %d and %d.\n",
             boxA->index,boxB->index);
  }

  // Merge clusterB into clusterA

  // Rprintf("joinBoundingBoxAandBindex %d (size %d) %d (size %d)\n",boxA->index, boxA->size,boxB->index, boxB->size);

  //printcluster(boxes, findpos(boxB->index,boxes,numBoxes));
  int oldsize = boxA->size;
  // Append indices of Cluster A and then clusterB
  if(boxA->indices ==NULL){
    Rprintf("ES nulo!!");
  }

  // Rprintfbbox(boxB);
  
  boxA->indices = R_Realloc(boxA->indices, oldsize + (boxB->size), int);
  boxA->positions = R_Realloc(boxA->positions, oldsize + (boxB->size), int);
  for (int i = 0; i < boxB->size; i++) {

    //Rprintf("B %d %d ",i, boxB->indices[i]);
	//Rprintf("Joining B (%d):  %d ", boxB->index,boxB->positions[i]);
    boxA->indices[oldsize + i] = boxB->indices[i];
	boxA->positions[oldsize + i] = boxB->positions[i];
    //Rprintf("with A (%d) %d \n",boxA->index,boxA->positions[oldsize + i] );
  }
  boxA->size = oldsize + boxB->size;


  int XleftA = INT_MAX, YtopA = INT_MAX;
  int XrightA = INT_MIN, YbottomA = INT_MIN;
  for (int i = 0; i < boxA->size; i++) {
    int k =findpos(boxA->indices[i],boxes,numBoxes);
    if (boxes[k].xleft < XleftA) XleftA = boxes[k].xleft;
    if (boxes[k].ybottom > YbottomA) YbottomA = boxes[k].ybottom;
    if (boxes[k].xright > XrightA) XrightA = boxes[k].xright;
    if (boxes[k].ytop < YtopA) YtopA = boxes[k].ytop;
  }
  boxA->xleft = XleftA;
  boxA->ybottom = YbottomA;
  boxA->xright = XrightA;
  boxA->ytop = YtopA;


  //Rprintfbbox(boxA);
  //Rprintf("Fin de joinBoundingBoxAandBindex\n");

}


// Helper function to remove an element at a given index
void removeBoundingBox(BoundingBox** list, int* numBoxes, int removeIndex) {
  if (list == NULL || *list == NULL || removeIndex < 0 || removeIndex >= *numBoxes) {
    Rprintf("Invalid remove index: %d\n", removeIndex);
    return;
  }

  // Free any internal allocations of the element to be removed.
  FreeBoundingBoxSingle( &((*list)[removeIndex]));

  // Shift subsequent elements left to fill the gap.
  for (int i = removeIndex; i < *numBoxes - 1; i++) {
    (*list)[i] = (*list)[i + 1];
  }

  // Decrement the total number of boxes.
  (*numBoxes)--;

  // Optionally resize the array to the new size.
  if (*numBoxes > 0) { // NO!!! da problemas with valgrind!!!!!!
    //*list = shrinkBoundingBoxArray(*list, *numBoxes);
    //BoundingBox* tmp = R_Realloc(*list, *numBoxes, BoundingBox);
    //*list = tmp; // R_Realloc(*list, *numBoxes, BoundingBox);
  } else {
    // If no elements remain, free the list pointer.
    //  R_Free(*list);
    //*list = NULL;
  }
}


void joinBoundingBoxAandB(BoundingBox* clusters, int* numclusters, int indexA, int indexB,  const BoundingBox* boxes, int* adj, int numBoxes) {

  int indexa = clusters[indexA].index;
  int indexb = clusters[indexB].index;
  Rprintf("Updating cluster of dimension %d: joining group %d and group %d\n",numclusters[0],indexa,indexb);
  if(indexA<0 || indexB < 0 || indexA> numclusters[0] || indexB > numclusters[0]){
    Rf_error("Updating cluster of dimension %d: %d and %d\n",numclusters[0],indexa,indexb);
  }


  if(indexA == indexB){
    Rf_error("You want to join the same cluster: %d and %d.\n",
             clusters[indexA].index,clusters[indexB].index);
  }

  // Merge clusterB into clusterA

  int oldsize = clusters[indexA].size;
  // Append indices of Cluster A and then clusterB
  clusters[indexA].indices = R_Realloc(clusters[indexA].indices, oldsize + clusters[indexB].size , int);
  if (!clusters[indexA].indices) {
    Rf_error("Failed to reallocate memory for merging clusters.");
  }
  for (int i = 0; i <   clusters[indexB].size; i++) {
    clusters[indexA].indices[oldsize + i] = clusters[indexB].indices[i];
  }
  clusters[indexA].size = oldsize + clusters[indexB].size;


  int XleftA = INT_MAX, YtopA = INT_MAX;
  int XrightA = INT_MIN, YbottomA = INT_MIN;
  for (int i = 0; i < clusters[indexA].size; i++) {
    BoundingBox box = boxes[clusters[indexA].indices[i]];
    if (box.xleft < XleftA) XleftA = box.xleft;
    if (box.ybottom > YbottomA) YbottomA = box.ybottom;
    if (box.xright > XrightA) XrightA = box.xright;
    if (box.ytop < YtopA) YtopA = box.ytop;
  }
  clusters[indexA].xleft = XleftA;
  clusters[indexA].ybottom = YbottomA;
  clusters[indexA].xright = XrightA;
  clusters[indexA].ytop = YtopA;

  clusters[indexA].index = clusters[indexA].index;

  removeBoundingBox(&clusters, numclusters, indexB);


  Rprintf("Joining clusters %d and %d. ",indexa,indexb);
  for(int k =0; k< numclusters[0];k++){
    if(clusters[k].index != indexa) continue;
    Rprintf("Elements (%d) :\n",clusters[k].size);
    for(int i=0; i< clusters[k].size; i++){
      Rprintf("%d ",clusters[k].indices[i]);
    }
    Rprintf("\n");
  }


}



// ============================================================

int isAIncludedInB(const BoundingBox* A, const BoundingBox* B) {
  return (A->xleft >= B->xleft &&
          A->xright <= B->xright &&
          A->ytop >= B->ytop &&
          A->ybottom <= B->ybottom);
}


// Function to check if Bounding Box A is included in B
int isequalcoordenates(const BoundingBox* A, const BoundingBox* B) {
  return (A->xleft == B->xleft &&
          A->xright == B->xright &&
          A->ytop == B->ytop &&
          A->ybottom == B->ybottom);
}


// Function to check if two bounding boxes overlap on the x-axis
int overlapX(const BoundingBox* A, const BoundingBox* B) {
  return (A->xright >= B->xleft && A->xleft <= B->xright);
}

// Function to check if two bounding boxes overlap on the y-axis
int overlapY(const BoundingBox* A, const BoundingBox* B) {
  return (A->ybottom >= B->ytop && A->ytop <= B->ybottom);
}

// Function to check if bounding box A is above bounding box B
int aboveAoverB(const BoundingBox* A, const BoundingBox* B) {
  return (A->ybottom <= B->ytop);
}

// Distance in Y
int distanceY(const BoundingBox* A, const BoundingBox* B) {
  if (A->ytop > B->ybottom) {
    return A->ytop - B->ybottom;
  } else if (B->ytop > A->ybottom) {
    return B->ytop - A->ybottom;
  } else {
    return 0; // Boxes overlap, so distance is considered 0 in this case
  }

}


// Function to calculate the distance between two boxes in the X dimension
int distanceX(const BoundingBox* A, const BoundingBox* B) {
  if (A->xleft > B->xright) {
    return A->xleft - B->xright;
  } else if (B->xleft > A->xright) {
    return B->xleft - A->xright;
  } else {
    return 0; // Boxes overlap in x
  }
}


// Function to find the closest box in the Y direction that does *not* overlap in Y and overlap in X
int ClosestY(int ibase, const BoundingBox *boxes, int numBoxes, int direction) {
  int closestIndex = ibase;
  int minDist = INT_MAX;

  for (int i = 0; i < numBoxes; i++) {
    if (i == ibase) continue; // Skip the base box itself

    if (overlapX(&boxes[ibase], &boxes[i]) && !overlapY(&boxes[ibase], &boxes[i])) { // Check for overlap in X
      int distY = distanceY(&boxes[ibase], &boxes[i]);

      // Consider direction:
      // direction = 1 (up), direction = -1 (down)
      if (direction == 1 && boxes[i].ybottom < boxes[ibase].ytop) { // Box i is above ibase
        if (distY < minDist) {
          minDist = distY;
          closestIndex = i;
        }
      } else if (direction == -1 && boxes[i].ytop > boxes[ibase].ybottom) { // Box i is below ibase
        if (distY < minDist) {
          minDist = distY;
          closestIndex = i;
        }
      }
    }
  }

  return closestIndex;
}


// Function to find the closest box in the X direction that does *not* overlap in Y
int ClosestX(int ibase, const BoundingBox *boxes, int numBoxes, int direction) {
  int closestIndex = ibase;
  int minDist = INT_MAX;

  for (int i = 0; i < numBoxes; i++) {
    if (i == ibase) continue; // Skip the base box itself

    if (!overlapX(&boxes[ibase], &boxes[i]) && overlapY(&boxes[ibase], &boxes[i])) { // Check for NO overlap in X

      int distX = distanceX(&boxes[ibase], &boxes[i]);

      // Consider direction:
      // direction = 1 (right), direction = -1 (left)
      if (direction == 1 && boxes[i].xleft > boxes[ibase].xright) { // Box i is to the right of ibase
        if (distX < minDist) {
          minDist = distX;
          closestIndex = i;
        }
      } else if (direction == -1 && boxes[i].xright < boxes[ibase].xleft) { // Box i is to the left of ibase
        if (distX < minDist) {
          minDist = distX;
          closestIndex = i;
        }
      }
    }
  }

  return closestIndex;
}


// Function to find the closest box in the Y direction that does *not* overlap in Y and overlap in X
int ClosestYpage(int ibase, const BoundingBox* page,  const BoundingBox *boxes, int numBoxes, int direction) {
  int closestIndex = ibase;
  int minDist = INT_MAX;
  for(int ii = 0; ii < page->size; ii++ ){
    int i = findpos(page->indices[ii],boxes,numBoxes);

    if (i == ibase) continue; // Skip the base box itself

    if (overlapX(&boxes[ibase], &boxes[i]) && !overlapY(&boxes[ibase], &boxes[i])) { // Check for overlap in X
      int distY = distanceY(&boxes[ibase], &boxes[i]);

      // Consider direction:
      // direction = 1 (up), direction = -1 (down)
      if (direction == 1 && boxes[i].ybottom < boxes[ibase].ytop) { // Box i is above ibase
        if (distY < minDist) {
          minDist = distY;
          closestIndex = i;
        }
      } else if (direction <=0 && boxes[i].ytop > boxes[ibase].ybottom) { // Box i is below ibase
        if (distY < minDist) {
          minDist = distY;
          closestIndex = i;
        }
      }
    }
  }

  return closestIndex;
}




int ClosestXpage(int ibase,  const BoundingBox* page, const BoundingBox *boxes, int numBoxes, int direction) {
  int closestIndex = ibase;
  int minDist = INT_MAX;

  for(int ii = 0; ii < page->size; ii++ ){
    int i = findpos(page->indices[ii],boxes,numBoxes);

    if (i == ibase) continue; // Skip the base box itself

    if (!overlapX(&boxes[ibase], &boxes[i]) && overlapY(&boxes[ibase], &boxes[i])) { // Check for NO overlap in X

      int distX = distanceX(&boxes[ibase], &boxes[i]);

      // Consider direction:
      // direction = 1 (right), direction = -1 (left)
      if (direction == 1 && boxes[i].xleft > boxes[ibase].xright) { // Box i is to the right of ibase
        if (distX < minDist) {
          minDist = distX;
          closestIndex = i;
        }
      } else if (direction == -1 && boxes[i].xright < boxes[ibase].xleft) { // Box i is to the left of ibase
        if (distX < minDist) {
          minDist = distX;
          closestIndex = i;
        }
      }
    }
  }

  return closestIndex;
}



// Function to check if the center of box A overlaps box B in X direction AND
// if the center of box B overlaps box A in X direction
int overlapCenterInX(const BoundingBox *boxA, const BoundingBox *boxB, int treshold) {
  // Calculate the center X coordinate of box A
  int centerAX = (boxA->xleft + boxA->xright) / 2;
  // Calculate the center X coordinate of box B
  int centerBX = (boxB->xleft + boxB->xright) / 2;
  // Check if center of A is within the X range of B
  int centerA_in_B = (centerAX >= boxB->xleft - treshold) && (centerAX <= boxB->xright+treshold);
  // Check if center of B is within the X range of A
  int centerB_in_A = (centerBX >= boxA->xleft-treshold) && (centerBX <= boxA->xright+treshold);
  return centerA_in_B && centerB_in_A;  // Both conditions must be true for overlap in X from both perspectives
}


int overlapCenterInY(const BoundingBox  *boxA, const BoundingBox *boxB, int treshold) {
  // Calculate the center Y coordinate of box A
  int centerAY = (boxA->ytop + boxA->ybottom) / 2;
  // Calculate the center Y coordinate of box B
  int centerBY = (boxB->ytop + boxB->ybottom) / 2;
  // Check if center of A is within the Y range of B
  int centerA_in_B = (centerAY >= boxB->ytop- treshold) && (centerAY <= boxB->ybottom+treshold);
  // Check if center of B is within the Y range of A
  int centerB_in_A = (centerBY >= boxA->ytop- treshold) && (centerBY <= boxA->ybottom+treshold);
  return centerA_in_B && centerB_in_A; // Both conditions must be true for overlap in Y
}



// Function to calculate the height of a bounding box
int boxHeight(const BoundingBox* box) {
  return box->ybottom - box->ytop;
}
// Function to calculate the width of a bounding box
int boxWidth(const BoundingBox* box) {
  return box->xright - box->xleft;
}

int intersectAB(const BoundingBox* boxA, const BoundingBox* boxB){
  return distanceX(boxA,boxB) == 0 && distanceY(boxA,boxB) == 0;
}

int isatomicbbox(const BoundingBox* boxBase,  const BoundingBox* boxes, int numBoxes){
  for(int i=0; i< numBoxes; i++){
    if(boxes[i].index == boxBase->index) continue;
    if(isequalcoordenates(&boxes[i],boxBase)) return 0;
    if(isAIncludedInB(&boxes[i], boxBase)) return 0;
  }
  return 1;
}


int getcluster(const BoundingBox* clusters, const int* numclusters, int index){
  int count=0;
  int idx= -1;
  for(int i=0; i<numclusters[0];i++){
    for(int k=0; k< clusters[i].size; k++){
      if(clusters[i].indices[k] == index && idx != i) {
        count++;
        idx = i;
      }
    }
  }
  if(count==0 || count>1) {
    Rf_error("This index %d appears %d times in clusters.",index, count);
  }
  //Rprintf("El cluster del box %d es %d (su index es %d)\n",index,idx,clusters[idx].index);
  return idx;
}



void showdistances(const BoundingBox* boxes, int numBoxes){

  for(int i=0 ; i< numBoxes; i++){
    for(int j=i+1; j < numBoxes;j++){
      // double centerXA = (double) (boxes[i].xleft + boxes[i].xright)/2;
      // double centerXB = (double)(boxes[j].xleft + boxes[j].xright)/2;
      // double centerYA = (double)(boxes[i].ybottom + boxes[i].ytop)/2;
      // double centerYB = (double)(boxes[j].ybottom + boxes[j].ytop)/2;
      Rprintf("Boxes %d %d: distX %d distY %d Y.\n",
              boxes[i].index,boxes[j].index,
              distanceX(&boxes[i],&boxes[j]),
              distanceY(&boxes[i],&boxes[j])              );

    }
  }
}






int findpos(int index, const BoundingBox* boxes, int numBoxes){
  if(index>=0 && index< numBoxes && boxes[index].index==index) return boxes[index].pos;
  for(int k=0; k< numBoxes; k++){
    if(index==boxes[k].index) return boxes[index].pos;
  }
  Rf_error("Index %d not exists!");
  return 0;
}

void unionDimensionsA(BoundingBox* boxA, const BoundingBox* boxB) {
  boxA->xleft   = MIN(boxA->xleft, boxB->xleft);
  boxA->ybottom = MAX(boxA->ybottom, boxB->ybottom);
  boxA->xright  = MAX(boxA->xright, boxB->xright);
  boxA->ytop    = MIN(boxA->ytop, boxB->ytop);
}

BoundingBox* unionsingleboxes(const int *indices, int nindices, const BoundingBox* b, int n){
  if(nindices < 1) return NULL;

  int* ind = R_Calloc(nindices,int);
  int count =0;
  // Extract bounding boxes that match indices
  for (int i = 0; i < nindices; i++) {
    int index = indices[i];
    // Find the corresponding BoundingBox in boxes
    for (int j = 0; j < n; j++) {
      if (b[j].index == index) {
        ind[count++] = index;
        break;
      }
    }
  }
  int size = removeduplicatesint(ind, count);
  if(size < 1){
    R_Free(ind);
    return NULL;
  }
  BoundingBox* p =R_Calloc(1,BoundingBox);
  p->xleft   = INT_MAX;
  p->ybottom = INT_MIN;
  p->xright  = INT_MIN;
  p->ytop    = INT_MAX;
  p->size = size;
  p->indices = R_Calloc(p->size,int);
  p->positions = R_Calloc(p->size,int);
  for(int i = 0; i < p->size; i++){
    p->indices[i] = ind[i];
    int index = p->indices[i];
    for (int j = 0; j < n; j++) {
      if (b[j].index == index) {
        p->positions[i] = b[j].pos;
        unionDimensionsA(p, &b[j]);
        break;
      }
    }
  }

  R_Free(ind);
  return p;

}


// Function to cluster overlapping bounding boxes in Y
BoundingBox* getLinesInThisPage(const BoundingBox* page, int* numberLines, const BoundingBox* boxes, int numBoxes){
  if (!page || !numberLines || !boxes) {
    Rf_error("Invalid input parameters (NULL pointer detected)");
  }

  // We have to sort the bounding boxes of the page.
  int* ytops = R_Calloc(page->size,int);
  for(int i=0; i< page->size;i++){
    int k = findpos(page->indices[i],boxes,numBoxes);
    ytops[i] = boxes[k].ytop;
  }
  int* oo= get_sort_indices(ytops, page->size);


  BoundingBox* lines = R_Calloc(page->size,BoundingBox);


  int groupIndex = 0;
  for(int k=0;k < page->size; k++) {
    int currentGroupSize = 0;
    lines[groupIndex].xleft = INT_MAX;
    lines[groupIndex].ytop = INT_MAX;
    lines[groupIndex].xright = INT_MIN;
    lines[groupIndex].ybottom = INT_MIN;
    lines[groupIndex].size = 0;
    //lines[groupIndex].indices =  R_Calloc(0, int);



    Rprintf("Line %d: ", groupIndex);
    int inside = 0;
    // Check overlapping bounding boxes in sorted order
    while (k < page->size &&
           (currentGroupSize == 0 ||
            overlapY(&boxes[page->indices[oo[k]]],
                     &boxes[page->indices[oo[k - 1]]])||
            overlapY(&lines[groupIndex],&boxes[page->indices[oo[k]]]))) {
      // Rprintf("Incrementing line currentGroupSize: %d\n",currentGroupSize);

      if(lines[groupIndex].size==0) {
        lines[groupIndex].indices =   R_Calloc(1, int);
      } else{
        lines[groupIndex].indices = R_Realloc(lines[groupIndex].indices, lines[groupIndex].size+1, int);
      }
      lines[groupIndex].indices[currentGroupSize++] = page->indices[oo[k]];
      lines[groupIndex].size = currentGroupSize;
      unionDimensionsA(&lines[groupIndex], &boxes[page->indices[oo[k]]]);

      Rprintf("%d ", page->indices[oo[k]]);
      inside = 1;
      k++;
    }
    if(k < page->size && inside) k--;

    Rprintf("\n");




    lines[groupIndex].size=currentGroupSize;
    lines[groupIndex].index=groupIndex;
    groupIndex++;
  }


  R_Free(ytops);
  R_Free(oo);

  *numberLines = groupIndex;
  if(numberLines[0])
    {
      lines = R_Realloc(lines, numberLines[0], BoundingBox );
    }

  return lines;
}




// Helper function to check if two intervals overlap
int intervalsOverlap(int start1, int end1, int start2, int end2) {
  return !(end1 < start2 || end2 < start1);
}


int nothingbetweenYoverlapX(int i, int j, const BoundingBox* page, const BoundingBox* boxes, int numBoxes){

  if(i==j) return 1;
  // Determine the top and bottom of the interval between boxes i and j
  int top = (boxes[i].ytop < boxes[j].ytop) ? boxes[i].ytop : boxes[j].ytop;
  int bottom = (boxes[i].ybottom > boxes[j].ybottom) ? boxes[i].ybottom : boxes[j].ybottom;

  for(int kk = 0; kk < page->size; kk++ ){
    int k = findpos(page->indices[kk],boxes,numBoxes);

    if (k == i || k==j) continue; // Skip the base box itself
    if(  isequalcoordenates(&boxes[i], &boxes[k]) || isequalcoordenates(&boxes[j], &boxes[k])) continue;
    if( !overlapX(&boxes[i], &boxes[k]) || !overlapX(&boxes[j], &boxes[k])) continue;
    // Check if box k overlaps the interval between boxes i and j
    if (intervalsOverlap(top, bottom, boxes[k].ytop, boxes[k].ybottom)) {
      return 0; // Found an overlapping box
    }

  }
  return 1;


}






// Get the boxes in the r
BoundingBox* getLinefromthisbox(int indexbase, const BoundingBox* page, int maxdistx, int direction, const BoundingBox* boxes, int numBoxes){
  if (!page  || !boxes) {
    Rf_error("Invalid input parameters (NULL pointer detected)");
  }

  //Rprintf("Inicio de getLinefromthisbox \n");
  
  int ibase = findpos(indexbase,boxes,numBoxes);
  BoundingBox* line =  copyBoundingBox(&boxes[ibase]);

  //Rprintfbbox(line);

  // We have to sort the bounding boxes of the page.
  int* xlefts = R_Calloc(page->size,int);
  int* indices = R_Calloc(page->size,int);
  int size = 0;

  for(int i=0; i< page->size;i++){
    int k = findpos(page->indices[i],boxes,numBoxes);
    if(direction>0){
      if( boxes[k].xleft >= boxes[ibase].xleft){
        xlefts[size] = boxes[k].xleft;
        indices[size] = boxes[k].index;
        size++;
      }} else {
      if( boxes[k].xleft <= boxes[ibase].xleft){
        xlefts[size] = (-boxes[k].xleft);
        indices[size] = boxes[k].index;
        size++;
      }

    }
  }


  int* oo= get_sort_indices(xlefts, size); // Defined in maths.c

  Rprintf("Sub line from this base: %d, ",boxes[ibase].index);
  for(int kk=0; kk < size; kk++){
    if(indices[oo[kk]] == line->index) continue;
    int k = findpos(indices[oo[kk]],boxes,numBoxes);
    if(!overlapY(&boxes[k],line)) continue;
    if(distanceX(&boxes[k], line) > maxdistx) continue;
	if(isAIncludedInB(line,&boxes[k]) && !isequalcoordenates(line, &boxes[k])) continue;

    //  Rprintf("\n");
    //printcluster(boxes,k);

	//Rprintfbbox(line);
	//	Rprintfbbox(&boxes[k]);	

	joinBoundingBoxAandBindex(line, &boxes[k],  boxes,  numBoxes);

	//Rprintfbbox(line);

    Rprintf("%d ",boxes[k].index);
  }
  Rprintf("\n");


  R_Free(oo);
  R_Free(indices);
  R_Free(xlefts);

  //Rprintfbbox(line);
  //Rprintf("Fin de getLinefromthisbox \n");
  return line;

}



// Function to copy BoundingBox A to BoundingBox B
BoundingBox* copyBoundingBox(const BoundingBox *A) {
  if (!A) return NULL;  // Safety check for NULL pointers
  BoundingBox *B = R_Calloc(1,BoundingBox);

  // Copy simple integer fields
  B->xleft   = A->xleft;
  B->xright  = A->xright;
  B->ytop    = A->ytop;
  B->ybottom = A->ybottom;
  B->index   = A->index;
  B->size    = A->size;

  // Allocate memory for indices array in B if size > 0
  if (A->size > 0) {
    B->indices = R_Calloc(A->size,int);
	memcpy(B->indices, A->indices, A->size * sizeof(int));
	B->positions = R_Calloc(A->size,int);
	memcpy(B->positions, A->positions, A->size * sizeof(int));
  } else {
    B->indices = NULL; // No friends, set to NULL
	B->positions = NULL;
  }
  return B;
}




// direction 1 numerator, direction 0 denominator
BoundingBox* getLinefromthisboxFrac(int indexbase, int indexfrac, const BoundingBox* page,  int direction, const BoundingBox* boxes, int numBoxes){
  if (!page  || !boxes) {
    Rf_error("Invalid input parameters (NULL pointer detected)");
  }

  int ibase = findpos(indexbase,boxes,numBoxes);
  int ifrac = findpos(indexfrac,boxes,numBoxes);
  BoundingBox* line = copyBoundingBox(&boxes[ibase]);

  // We have to sort the bounding boxes of the page.
  int* xlefts = R_Calloc(page->size,int);
  int* indices = R_Calloc(page->size,int);
  int size = 0;

  for(int i=0; i< page->size;i++){
    int k = findpos(page->indices[i],boxes,numBoxes);
    if(distanceX(&boxes[ifrac],&boxes[k])>0) continue;
    if(overlapY(&boxes[ifrac],&boxes[k])) continue;
    //if(!overlapY(&boxes[ibase],&boxes[k])) continue;
    if( !between(boxes[k].xleft,boxes[ifrac].xleft,boxes[ifrac].xright)) continue;
    if( !between(boxes[k].xright,boxes[ifrac].xleft,boxes[ifrac].xright)) continue;
    if(direction>0 && boxes[k].ybottom >= boxes[ifrac].ytop) continue;
    if(direction<=0 && boxes[k].ytop <= boxes[ifrac].ybottom) continue;
    xlefts[size] = boxes[k].xleft;
    indices[size] = boxes[k].index;
    size++;
  }



  int* oo= get_sort_indices(xlefts, size);


  Rprintf("Sub line FRAC from this base: %d, ",boxes[ibase].index);
  for(int kk=0; kk < size; kk++){
    if(indices[oo[kk]] == line->index) continue;
    int k = findpos(indices[oo[kk]],boxes,numBoxes);
    if(!overlapY(&boxes[k],line)) continue;
    //if(distanceX(&boxes[k], &line) > maxdistx) continue;

    //  Rprintf("\n");
    //printcluster(boxes,k);

    joinBoundingBoxAandBindex(line, &boxes[k],  boxes,  numBoxes);

    Rprintf("%d ",boxes[k].index);
  }
  Rprintf("\n");


  R_Free(oo);
  oo = NULL;

  R_Free(indices);
  indices =NULL;

  R_Free(xlefts);
  xlefts =NULL;


  return line;

}
