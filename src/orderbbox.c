#include "boundingbox.h"
#include "char_data.h"





SEXP orderBoundingBoxes(SEXP xleft_s, SEXP ybottom_s, SEXP xright_s, SEXP ytop_s) {
  if (TYPEOF(xleft_s) != INTSXP)
    Rf_error("XLEFT vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(xleft_s)));

  if (TYPEOF(ybottom_s) != INTSXP)
    Rf_error("YBOTTOM vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(ybottom_s)));

  if (TYPEOF(xright_s) != INTSXP)
    Rf_error("XRIGHT vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(xright_s)));

  if (TYPEOF(ytop_s) != INTSXP)
    Rf_error("YTOP vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(ytop_s)));

  if (LENGTH(xleft_s) != LENGTH(ybottom_s) || LENGTH(xleft_s) != LENGTH(xright_s) || LENGTH(xleft_s) != LENGTH(ytop_s)) {
    Rf_error("Different lengths in the bounding box limits:  %d,  %d, %d, %d.", LENGTH(xleft_s), LENGTH(ybottom_s), LENGTH(xright_s), LENGTH(ytop_s));
  }


  int numBoxes = LENGTH(xleft_s);


  // Initialize boxes
  BoundingBox* boxes = R_Calloc(numBoxes,BoundingBox);
  for (int i = 0; i < numBoxes; i++) {
    boxes[i].xleft = INTEGER(xleft_s)[i];
    boxes[i].ybottom = INTEGER(ybottom_s)[i];
    boxes[i].xright = INTEGER(xright_s)[i];
    boxes[i].ytop = INTEGER(ytop_s)[i];
    boxes[i].index = i;
    boxes[i].pos = i;

    if( boxes[i].xleft == NA_INTEGER || boxes[i].ybottom == NA_INTEGER || boxes[i].xright == NA_INTEGER || boxes[i].ytop == NA_INTEGER ||
        ( boxes[i].xleft > boxes[i].xright) || (boxes[i].ybottom < boxes[i].ytop)){
      Rf_error("Verify this bounding box %d: xleft= %d, ybottom= %d, xright= %d, ytop= %d.\n",
               boxes[i].xleft, boxes[i].ybottom, boxes[i].xright, boxes[i].ytop);
    }
    boxes[i].size = 1;
    boxes[i].indices =  R_Calloc(boxes[i].size, int);
    boxes[i].indices[0] = i;
    boxes[i].positions =  R_Calloc(boxes[i].size, int);
    boxes[i].positions[0] = i;

  }

  //for(int i=0; i <numBoxes;i++) printcluster(boxes,i);

  int protecti=0;
  SEXP result_s = PROTECT(allocVector(INTSXP, numBoxes));protecti++;
  int* result = INTEGER(result_s);

  for (int i = 0; i < numBoxes; i++) {
    result[i]=0;
  }


  getorderBoundingBoxes_C(boxes, numBoxes, result);


  for (int i = 0; i < numBoxes; i++) {
    result[i] += 1; //into R
  }


  Rprintf("Order BoundingBoxing Result:\n");
  for (int i = 0; i < numBoxes; i++) {
    Rprintf("%d ", result[i]);
  }
  Rprintf("\n");

  for (int i = 0; i < numBoxes; i++) {
    Rprintf("%d ", result[i]);
  }
  Rprintf("\n");

  FreeBoundingBox(boxes, numBoxes);

  UNPROTECT(protecti);
  return result_s;

}






// ============================================================



// ============================================================

int sortLexicographyBoundingBoxes(const BoundingBox* page, BoundingBox* boxes, int *adj,  int numBoxes){

  Rprintf("sortLexicographyBoundingBoxes page %d with %d size\n",page->index,page->size);
  if(page->size < 2) return 1;
  int n=0;
  BoundingBox** pages = splitminipages(page,&n,boxes,adj,numBoxes);
  Rprintf("sortLexicographyBoundingBoxes page %d has %d boxes and %d minipages\n",page->index,page->size,n);
  if(n>=1) {
    for(int i = 0; i< n; i++){
      sortLexicographyBoundingBoxes(pages[i], boxes, adj, numBoxes);
    }
    // Y aquí debería ir ordenar las minipages
  }

  Rprintf("sortLexicographyBoundingBoxes page %d has %d boxes and %d minipages (despues del recursivo)\n",page->index,page->size,n);
  joinFragmentedCharacters(page, boxes,adj, numBoxes);

  Rprintf("sortLexicographyBoundingBoxes despues de tildes\n",n,page->size);

  orderwithinpage(page, (const BoundingBox*) boxes, adj, numBoxes);




  freebboxes(pages,n);

  return 1;
}

// ============================================================

// ============================================================

void getorderBoundingBoxes_C( BoundingBox* boxes, const int numBoxes, int* result){



  int *adj= R_Calloc(numBoxes*numBoxes,int);
  int* indices= R_Calloc(numBoxes,int);
  for(int i=0; i< numBoxes; i++){
    indices[i] =boxes[i].index;
    //indices[i] = MAX(2,boxes[i].index);
  }

  BoundingBox* page = unionsingleboxes(indices, numBoxes, boxes,  numBoxes);
  page->index = -1;
  R_Free(indices);

  Rprintfbbox(page);

  sortLexicographyBoundingBoxes(page, boxes, adj, numBoxes);

  Rprintf("Last chance!: De perdidos, al río.\n");

  lookingorphannodes( boxes,  adj,  numBoxes);


  int* posboxesordenados = R_Calloc(numBoxes,int);
  int isdag = topologicalSort(adj, numBoxes, posboxesordenados);

  if(DEBUGGING){

	if(!isdag){
	  R_Free(posboxesordenados);
	  freebbox(page);
	  R_Free(adj);
	  Rf_error("Error con DFS: Not a dag");
	}

	int source=posboxesordenados[0];
	int target=posboxesordenados[numBoxes-1];
	int pathLength;
	int *path = longestPathInDAG(adj, posboxesordenados,numBoxes,source, target, &pathLength);

	if(pathLength==numBoxes){

	  
	  Rprintf("Mision cumplida! (o eso espero). Path:\n");
	  for (int i = 0; i < numBoxes; i++) {
		Rprintf("%d ",boxes[path[i]].index);
	  }
	  Rprintf("\n");
	    }
	R_Free(path);
  }

  
    for (int i = 0; i < numBoxes; i++) {
      result[posboxesordenados[i]] = i;
    }



   
   

    R_Free(posboxesordenados);

    freebbox(page);
    R_Free(adj);

}



/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
