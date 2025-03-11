/*
  TITLE:
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-03-08 17:43 emilio on emilio-XPS-15-9570>
*/

#include "boundingbox.h"


// Function to free a Page and its mini-pages recursively






// Join Lines that have boxes connected
BoundingBox* createLinesConnectedinAdjacency(const BoundingBox* page, int *numberlines, const BoundingBox* boxes,  int* adj, int numBoxes){



  Rprintf("Number of lines with no overlapping in Y.\n");
  int numberlinestemp;
  numberlinestemp =0;
  BoundingBox *lines= getLinesInThisPage(page, &numberlinestemp,  boxes,  numBoxes);
  Rprintf("Number of lines with no overlapping in Y: %d.\n",numberlinestemp);

  int previous = numberlinestemp;

  Rprintf("Joining lines with scripts.\n");
  int currentline = 0;
  while( numberlinestemp >1 && currentline < (numberlinestemp) -1) {
    int joinlines = 0;
    const BoundingBox* line1 = &lines[currentline];
    const BoundingBox* line2 = &lines[currentline+1];
    for(int ii = 0; ii< line1->size; ii++ ){
      int i = findpos(line1->indices[ii], boxes, numBoxes);
      for(int jj = 0; jj< line2->size; jj++ ){
        int j = findpos( line2->indices[jj], boxes, numBoxes);
        if(!noarc(i,j,boxes,adj,numBoxes)) {
          joinlines =1;
          continue;
        }
      }
    }
    if(joinlines){
      joinBoundingBoxAandB(lines, &numberlinestemp, currentline, currentline+1,
                           boxes, adj, numBoxes);

    } else{
      currentline++;
    }
  }
  if(previous != numberlinestemp){
    Rprintf("AQUI hay cambio: de %d a %d lines.\n",previous,numberlinestemp);
  }


  numberlines[0] = numberlinestemp;
  return lines;

}













void doorderpages(const BoundingBox*pages, int numPages,Direction pageoverlapy, Direction pageabove, const BoundingBox* boxes, int* adj, int numBoxes){

  Rprintf("Ordering minipages (or lines).\n");

  for(int ii = 0; ii< numPages; ii++){
    for(int jj = 0; jj< numPages; jj++ ){
      Direction direction = 0;
      if(ii==jj) continue;
      if(overlapY(&pages[jj], &pages[ii]) &&
         ( pages[ii].xleft < pages[jj].xleft ||
           (pages[ii].xleft == pages[jj].xleft && pages[ii].ytop < pages[jj].ytop)
           )){
        direction =  pageoverlapy; //OVERLAPYRIGHTPAGE;


      }  else if( pages[ii].ytop < pages[jj].ytop) {

        direction =  pageabove; //FIRSTYTOPPAGE;

      }

      if (direction == 0) continue;

      Rprintf("Line/page %d is %s of line/page %d.\n",ii, directionToString(direction),jj);
      for(int k1 = 0; k1 < pages[ii].size; k1++){
        int ik1 =findpos(pages[ii].indices[k1],boxes,numBoxes);
        for(int k2 = 0; k2 < pages[jj].size; k2++){
          int ik2 =findpos(pages[jj].indices[k2],boxes,numBoxes);
          /* if(!norelationlettersAB(boxes,adj,numBoxes,ik1,k2)){ */
          /*   Rf_error("I am in ordering a page with a bounding box %d and another page with a bounding box %d. I expect that there is no relationship between them, but I found %s\n.", boxes[ik1].index, boxes[ik2].index, directionToString(letterApreceedB(boxes,adj,numBoxes,ik1,ik2)) ); */
          /* } */

          updateAdjacencyMatrix(ik1,ik2,direction,boxes,adj,numBoxes);
        }
      }


    }
  }
}

// ============================================================

// ============================================================

void doorderlines(const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes){
  int numberlines = 0;
  Rprintf("Do order lines. \n");

  BoundingBox* lines = createLinesConnectedinAdjacency(page, &numberlines,  boxes,  adj, numBoxes);
  //orderoverlappingXinlines(lines,  numberlines, boxes,  adj, numBoxes);
  //orderintermediateYinlines(lines,  numberlines, boxes,  adj, numBoxes);


  doorderpages(lines,  numberlines, OVERLAPYLINE, ABOVELINE, boxes,  adj, numBoxes);


  FreeBoundingBox(lines,numberlines);


}


void orderwithinpage(const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes){

  ordersubscriptsmathmode( page,(const BoundingBox*) boxes, adj, numBoxes);

  orderfracmathmode( page,(const BoundingBox*) boxes, adj, numBoxes);

  ordersubscriptsnormalmode(page, (const BoundingBox*) boxes, adj,  numBoxes);

  // Centered in Y and close
  doorderwordright(page, (const BoundingBox*) boxes, adj, numBoxes);

  // Overlapping in Y
  doorderoverlapY(page, (const BoundingBox*) boxes, adj, numBoxes);


  doorderlines( page,(const BoundingBox*) boxes, adj, numBoxes);



}



int verifyisdoublecharacter(int index, int *ibase, const BoundingBox* boxes, int* adj, int numBoxes){

  *ibase = -1;
  int i =findpos(index, boxes,numBoxes);

  Direction dirs[] = {DOT, TILDE, VIRGULILLA ,DOTDOT,EQUALSIGN,SEMICOLONSIGN};
  size_t len_dirs = sizeof(dirs) / sizeof(dirs[0]);
  for(int j=0; j<numBoxes; j++){
    if(i == j) continue;
    Direction direction = arcFromTo(i,j,boxes,adj,numBoxes);
    for(int d=0; d<len_dirs; d++) {
      if(direction == dirs[d]){
        *ibase = boxes[j].index;
        return 1;
      }
    }
  }
  return 0;

}



void fosteringorphannodes( BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes){


  Rprintf("fosteringorphannodes page %d (numboxes %d: ",page->index,page->size);
  qsort(page->indices, page->size, sizeof(int),compare_int );
  for(int i=0; i<page->size; i++){
    Rprintf("%d ",page->indices[i]);
  }
  Rprintf(")\n");

  int* topoorder= R_Calloc(numBoxes,int);
  topologicalSort((int*) adj, numBoxes, topoorder);
  int pathLength;
  int* path= longestPathInDAGMatrix(adj, topoorder,  numBoxes, &pathLength);

  int norphannodespage=0;
  int* orphannodes = R_Calloc(page->size,int);
  for(int i=0; i<page->size;i++){
    if(exists_in_array(path, pathLength, page->indices[i]) )continue;
    orphannodes[norphannodespage++] = page->indices[i];
  }

  Rprintf("El camino más largo es: ");
  for(int i=0; i<pathLength;i++){
    Rprintf("%d ",boxes[path[i]].index);
  }
  Rprintf("\n");

  if(norphannodespage){
    Rprintf("No aparecen: ");
    for(int i=0; i<norphannodespage;i++){
      Rprintf("%d ",boxes[orphannodes[i]].index);
    }
    Rprintf("\n");

  }


  if(norphannodespage==0 || pathLength==0){
    R_Free(topoorder);
    R_Free(orphannodes);
    R_Free(path);
    return;
  }



  for(int i=0; i<norphannodespage;i++){
    int ibase = -1;
    int index =boxes[orphannodes[i]].index;
    if(verifyisdoublecharacter(index, &ibase,  boxes, adj, numBoxes)){
      int changed = 0;
      if(ibase != -1){
        int ipos = findpos(index,boxes,numBoxes);
        int basepos = findpos(ibase,boxes,numBoxes);
        for(int k = 0; k< numBoxes; k++){
          if(k==ipos) continue;
          if(noarc(ipos,k,boxes,adj,numBoxes) && !noarc(basepos,k,boxes,adj,numBoxes) ){
            if(arcFromTo(basepos,k,boxes,adj,numBoxes)) {
              changed += updateAdjacencyMatrix(ipos,k,ORPHANDOUBLECHAR,boxes,adj,numBoxes);
            } else {
              changed += updateAdjacencyMatrix(k,ipos,ORPHANDOUBLECHAR,boxes,adj,numBoxes);
            }
          }

        }
        if(changed)    {
          fosteringorphannodes(page,boxes,adj,numBoxes);
          R_Free(topoorder);
          R_Free(orphannodes);
          R_Free(path);
          return;
        }
      }
    }
  }






  int dist = INT_MAX;
  int ibase = -1;
  int ireference = -1;
  int inorelation = -1;
  int pospathreference = -1;
  int pospathnorelation = -1;
  // ireference: the closest (distance) box to ibase that has relationship with ibase.
  // inorelation: the closest (in path) to ireference with no relationship with ibase.
  int flag = 0;
  for(int ii =0; ii< norphannodespage; ii++){
    int i =  orphannodes[ii];

    for(int jj = 0; jj < pathLength; jj++){
      int reference =path[jj];
      if(reference == i) continue;

      if(arcFromTo(reference,i,boxes,adj,numBoxes)==INSIDE) continue;
      if(!arcFromTo(reference,i,boxes,adj,numBoxes)) continue;

      if((distanceX(&boxes[reference],&boxes[i]) + distanceY(&boxes[reference],&boxes[i]) < dist)) // Si la distancia es pequena
        {

          int zz = jj + 1;
          while(zz < pathLength &&
                (!noarc(i,path[zz],boxes,adj,numBoxes)))
            {
              zz++;

            }
          /* int yy = jj-1; */
          /* while(yy>=0 && */
          /*       (!noarc(i,findpos(path[yy],boxes,numBoxes),boxes,adj,numBoxes))) */
          /*   { */
          /*     yy--; */

          /*   } */
          int yy = -1;

          if( (zz < pathLength && zz >=0 && path[zz] < numBoxes && path[zz]>=0 && yy>=0 && (zz-jj < jj -yy)) ||
              (yy<0)) { // If zz is closer to jj in the path
            dist=distanceX(&boxes[i],&boxes[reference]) + distanceY(&boxes[i],&boxes[reference]) ;
            ibase = boxes[i].index;
            ireference = boxes[reference].index;
            inorelation = boxes[path[zz]].index;
            pospathreference = jj;
            pospathnorelation = zz;
            flag =1;
          } else if(yy>=0){
            dist=distanceX(&boxes[i],&boxes[reference]) + distanceY(&boxes[i],&boxes[reference]) ;
            ibase = boxes[i].index;
            ireference = boxes[reference].index;
            inorelation = boxes[path[yy]].index;
            pospathreference = jj;
            pospathnorelation = yy;
            flag =1;
          }

        }

    }

  }

  // Metemos a calzador una relación en la cadena
  int changed =0;
  if(flag){

    Rprintf("Un nodo conflictivo es %d: su previo con relación es %d y el siguiente sin relación es %d \n",
            ibase,ireference, inorelation);
    int kbase = findpos(ibase,boxes,numBoxes);

    int kreference = findpos(ireference,boxes,numBoxes);
    int knorelation = findpos(inorelation,boxes,numBoxes);
    if(noarc(knorelation,kbase,boxes,adj,numBoxes)){
      // kbase < kprevious



      int dbr = distanceX(&boxes[kbase],&boxes[kreference]) + distanceY(&boxes[kbase],&boxes[kreference]);
      int dnr = distanceX(&boxes[knorelation],&boxes[kreference]) + distanceY(&boxes[knorelation],&boxes[kreference]);
      int norelationbeforereference =  pospathnorelation < pospathreference && arcFromTo(kreference,kbase,boxes,adj,numBoxes);

      // distance(kbase,reference) < distance(kbase,knorelation)
      int rulefrac1 = overlapX(&boxes[kbase], &boxes[knorelation]) && aboveAoverB(&boxes[kbase], &boxes[knorelation]) ; // \frac{kbase}{knorelation}
      int rulefrac2 = overlapX(&boxes[kbase], &boxes[knorelation]) && aboveAoverB(&boxes[knorelation], &boxes[kbase]); // \frac{knorelation}{kbase}
      // kreference < kbase
      // kreference < knorelation
      if(  rulefrac1 ){
        Rprintf("Calzador 1\n");
        changed = updateAdjacencyMatrix(kbase,knorelation,ORPHANBASENEXT,boxes,adj,numBoxes);
        if(changed==0) changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANPREVIOUSBASE,boxes,adj,numBoxes);
      } else if(  rulefrac2  ){
        Rprintf("Calzador 2\n");
        changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANPREVIOUSBASE,boxes,adj,numBoxes);
        if(changed==0) changed = updateAdjacencyMatrix(kbase,knorelation,ORPHANBASENEXT,boxes,adj,numBoxes);
      } else if(norelationbeforereference) { // knorelation < kreference
        if(dnr < dbr ){ // base < knorelation < kreference
          Rprintf("Calzador 3\n");
          changed = updateAdjacencyMatrix(kbase,knorelation,ORPHANBASENEXT,boxes,adj,numBoxes);
          if(changed==0) changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANPREVIOUSBASE,boxes,adj,numBoxes);
        } else { // knorelation < base < kreference
          Rprintf("Calzador 4\n");
          changed = updateAdjacencyMatrix(kbase,knorelation,ORPHANBASENEXT,boxes,adj,numBoxes);
          if(changed==0) changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANPREVIOUSBASE,boxes,adj,numBoxes);
        }

      } else if (distanceY(&boxes[kbase], &boxes[knorelation]) < 0.7*MAX(boxHeight(&boxes[kbase]),boxHeight(&boxes[knorelation]))) {
        if(dnr >dbr ){ //  kreference < base  < knorelation
          Rprintf("Calzador 5\n");
          changed = updateAdjacencyMatrix(kbase,knorelation,ORPHANBASENEXT,boxes,adj,numBoxes);
          if(changed==0) changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANPREVIOUSBASE,boxes,adj,numBoxes);
        } else {
          Rprintf("Calzador 6\n");
          changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANPREVIOUSBASE,boxes,adj,numBoxes);
          if(changed==0) changed = updateAdjacencyMatrix(kbase,knorelation,ORPHANBASENEXT,boxes,adj,numBoxes);
        }
      } else if (aboveAoverB(&boxes[kbase], &boxes[knorelation])) {

        Rprintf("Calzador 7 \n");
        changed = updateAdjacencyMatrix(kbase,knorelation,ORPHANBASENEXT,boxes,adj,numBoxes);
        if(changed==0) changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANPREVIOUSBASE,boxes,adj,numBoxes);

      } else if (aboveAoverB(&boxes[kbase], &boxes[knorelation]))  {
        Rprintf("Calzador 8 \n");
        changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANBASENEXT,boxes,adj,numBoxes);
        if(changed==0) changed = updateAdjacencyMatrix(kbase,knorelation,ORPHANPREVIOUSBASE,boxes,adj,numBoxes);

      }  else if (boxes[kbase].ybottom <= boxes[knorelation].ytop ){ // No flag!
        // Puede que aún haya huérfanos.
        // No sé que hacer
        // Pues no hago nada...
		Rprintf("Calzador 9 \n");
        changed = updateAdjacencyMatrix(kbase,knorelation,ORPHANBASENEXT,boxes,adj,numBoxes);
        if(changed==0) changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANPREVIOUSBASE,boxes,adj,numBoxes);

	  }	else {
		  Rprintf("Calzador 10 \n");
		  changed = updateAdjacencyMatrix(knorelation,kbase,ORPHANBASENEXT,boxes,adj,numBoxes);
          
		
      }
    }

    R_Free(path);
    R_Free(orphannodes);

    R_Free(topoorder);


    if(changed) {
      fosteringorphannodes(page,boxes,adj,numBoxes);
    }


  }

}


void lookingorphannodes( const BoundingBox* boxes, int* adj, int numBoxes){
  // Looking for orphan nodes

  BoundingBox page;
  page.size = numBoxes;
  page.indices = R_Calloc(numBoxes,int);
  page.index = 0;
  for(int i=0; i< numBoxes; i++){
    page.indices[i] = boxes[i].index;
  }
  int numberlines;
  BoundingBox* lines = createLinesConnectedinAdjacency(&page, &numberlines,  boxes,  adj, numBoxes);
  for(int i=0; i<numberlines; i++){
    fosteringorphannodes( &lines[i],boxes,adj,numBoxes);

  }
  FreeBoundingBox(lines,numberlines);
  R_Free(page.indices);

}


BoundingBox** splitminipages(const BoundingBox* p, int* n, BoundingBox* boxes, int *adj,  int numBoxes){
  for(int i = 0; i< p->size ;i ++){
    int i1 = p->positions[i];
    for(int j = 0; j< p->size ;j ++){
      int i2 = p->positions[j];
      if(i1== i2) continue;
      int first = -1;
      int second = -1;
      if(isequalcoordenates(&boxes[i1], &boxes[i2]) && i1 <= i2) {
        first = i1;
        second = i2;
      } else if(isAIncludedInB(&boxes[i2], &boxes[i1])) {
        first = i1;
        second = i2;
      }
      if(first!= -1 && second!=-1){
        updateAdjacencyMatrix(first,second,INSIDE,boxes,adj,numBoxes);
      }
    }
  }


  int nminipages =0;
  BoundingBox** minipages=NULL;

  int* topoorder = R_Calloc(numBoxes,int);
  topologicalSort(adj, numBoxes,topoorder);

  // hmmm, puede ocurrir que un box esté en dos minipages disjuntas.
  for(int i = 0; i< p->size ;i ++){
    int i1 = p->positions[i];
    int countofboxes =0;
    int isincludedinanotherpage=0;
    for(int j = 0; j< p->size ;j ++){
      int i2 = p->positions[j];
      if(i1==i2) continue;
      if(arcFromTo(i1, i2, boxes, adj, numBoxes)==INSIDE) countofboxes++;
      //if(arcFromTo(i2, i1, boxes, adj, numBoxes)==INSIDE) isincludedinanotherpage++;
    }
    if(isincludedinanotherpage) continue;
    if(countofboxes==0) continue;

    if(nminipages == 0) {
      minipages = R_Calloc(1,BoundingBox*);
    } else {
      minipages = R_Realloc(minipages,nminipages+1,BoundingBox*);
    }

    int* indices = R_Calloc(countofboxes,int);
    int count=0;
    for(int j = 0; j< p->size ;j ++){
      int i2 = p->positions[j];
      if(arcFromTo(i1, i2, boxes, adj, numBoxes)==INSIDE) {
        int currPathLength;
        int* path = longestPathInDAG(adj, topoorder, numBoxes, i1, i2, &currPathLength);
        Rprintf("Box %d : to %d steps: %d\n",boxes[i1].index,boxes[i2].index, currPathLength);
        R_Free(path);
        // No insides intermedios
        if(currPathLength == 2) indices[count++] = boxes[i2].index;
      }
    }

    Rprintf("Box %d :numero de count %d\n",boxes[i1].index,count);
    if(count){
      minipages[nminipages] =  unionsingleboxes(indices, count,  boxes, numBoxes);
      minipages[nminipages]->index = boxes[i1].index;


      nminipages++;
    }
    R_Free(indices);
  }

  R_Free(topoorder);

  *n = nminipages;
  return minipages;

}


/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
