/*
  TITLE:
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-03-08 13:34 emilio on emilio-XPS-15-9570>
*/

#include "boundingbox.h"
#include <stdint.h>
#include <stdlib.h> // for qsort

#define SIZEDOT 9
#define OVERLAPXINT_J (16+1)
#define HEIGHTSUMMATH 49 //(104+1)
#define WIDHTSUMMATH 53 //(84+1)
#define HEIGHTPRODMATH 75 //(104+1)
#define WIDHTPRODMATH 53 //(84+1)
#define DISTYSUMMATHOVER (25)
#define DISTYSUMMATHUNDER (25)
#define DISTXSUBSCRIPT 7 
#define DISTXSUMSUBSCRIPT 9

#define DISTXSAMELETTERNORMALMODE 6
#define DISTXOTHERLETTERNORMALMODE 8




void doorderoverlapY(const BoundingBox *page, const BoundingBox* boxes, int* adj, int numBoxes){

  for(int iibase = 0; iibase< page->size; iibase++){
    int ibase = findpos(page->indices[iibase],boxes,numBoxes);
    for(int jj = 0; jj< page->size; jj++ ){
      int j = findpos(page->indices[jj],boxes,numBoxes);
      if(j!=ibase && noarc(ibase,j,boxes,adj,numBoxes)){
        if(overlapY(&boxes[j], &boxes[ibase]) && boxes[ibase].xleft <= boxes[j].xleft )
          		updateAdjacencyMatrix(ibase,j,OVERLAPYRIGHT,boxes,adj,numBoxes);
      }
    }
  }
}

int samelevelletters(const BoundingBox* boxA, const BoundingBox* boxB){

  if(!overlapCenterInY(boxA, boxB, 0)) return 0;
  if(between(boxA->ytop, boxB->ytop -3, boxB->ytop +3)) return 1;
  if(between(boxA->ybottom, boxB->ybottom-3, boxB->ybottom +3)) return 1;
  // case t j
  if(between(boxA->ybottom, boxB->ybottom, boxB->ytop) &&
     between(boxA->ytop, boxB->ybottom, boxB->ytop) &&
     boxA->ytop - boxB->ytop < 10 * (double) boxHeight(boxB)/78  && // height of j = 80
     boxB->ybottom - boxA->ybottom < 17 * (double) boxHeight(boxB)/78
     ) return 1;
  // case j t
  if(between(boxB->ybottom, boxA->ybottom, boxA->ytop) &&
     between(boxB->ytop, boxA->ybottom, boxA->ytop) &&
     boxB->ytop - boxA->ytop < 10 * (double) boxHeight(boxA)/78  && // height of j = 80
     boxA->ybottom - boxB->ybottom < 17 * (double) boxHeight(boxA)/78)
    return 1;

  return 0;

}

int findclosetcenteredY(int ibase,const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes){
  int dist = INT_MAX;
  int closest = ibase;

  for(int jj = 0; jj< page->size; jj++ ){
    int j = findpos(page->indices[jj],boxes,numBoxes);
    if(j!=ibase && notincludedAinB(boxes,adj,numBoxes,j,ibase)){
      // TODO. Falta comprobar que ambos pertenecen a la misma inclusión
      // int insideY1 = between(boxes[ibase].ybottom,boxes[j].ybottom,boxes[j].ytop) &&
      //    between(boxes[ibase].ytop,boxes[j].ybottom,boxes[j].ytop);
      // int insideY2 = between(boxes[j].ybottom,boxes[ibase].ybottom,boxes[ibase].ytop) &&
      //    between(boxes[j].ytop,boxes[ibase].ybottom,boxes[ibase].ytop);

      if((samelevelletters(&boxes[ibase], &boxes[j]))
         && boxes[ibase].xright <= boxes[j].xleft ){
        if( boxes[j].xleft - boxes[ibase].xright <= dist){
          dist = boxes[j].xleft - boxes[ibase].xright;
          closest = j;
        }
      }
    }
  }
  //  Rprintf("El centered más cercano a %d es %d\n",boxes[ibase].index,boxes[closest].index);
  return closest;
}







// i^j_k
int ordersubscriptsnormalmodeibase( int i, const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes){
  Rprintf("ordersubscriptsnormalmode base %d\n",boxes[i].index);

  if(boxHeight(&boxes[i]) > 70) return 0;
  int closest01 = findpos(ClosestYpage(i, page, boxes, numBoxes, 1),boxes,numBoxes);    // direction = 1 (up), direction = -1 (down)
  //    int closest02 = findpos(ClosestYpage(i, page, boxes, numBoxes, -1),boxes,numBoxes);

  for(int jj = 0; jj< page->size; jj++){
    int j = findpos(page->indices[jj],boxes,numBoxes);
    if(i==j) continue;
    if(arcFromTo(j,i,boxes,adj,numBoxes)) continue;
    if(overlapCenterInY(&boxes[i],&boxes[j],0)) continue;
    if(boxes[i].xleft > boxes[j].xleft || (boxes[i].xleft == boxes[j].xleft && boxes[i].xright>= boxes[j].xright) ) continue;
	if(distanceY(&boxes[i],&boxes[j]) > MAX(boxHeight(&boxes[i]),boxHeight(&boxes[j]))) continue;
    int burro = boxes[i].index==9 && boxes[j].index==12;
    if(burro)      Rprintf("ordersubscriptsnormalmodeibase burro1 base %d over %d\n",boxes[i].index,boxes[j].index);



    if(boxes[i].ybottom < boxes[j].ybottom ) continue;
    if(boxes[i].ytop < boxes[j].ytop ) continue;
    if(distanceX(&boxes[i],&boxes[j])>10) continue;
    if(distanceX(&boxes[i],&boxes[j])==0 && arcFromTo(j,i,boxes,adj,numBoxes)) continue; // \int^j_k
	if(burro)      Rprintf("ordersubscriptsnormalmodeibase burro2 base %d over %d\n",boxes[i].index,boxes[j].index);
    if(distanceX(&boxes[i],&boxes[j])==0 && distanceY(&boxes[i],&boxes[j])>0) continue;
    int closest03 = findpos(ClosestYpage(j, page, boxes, numBoxes, 1),boxes,numBoxes);      // direction = 1 (up), direction = -1 (down)
    int closest04 = findpos(ClosestYpage(j, page, boxes, numBoxes, -1),boxes,numBoxes);

    for(int kk = 0; kk< page->size; kk++){
      int k = findpos(page->indices[kk],boxes,numBoxes);
      if(i==k || j==k) continue;
	  if(arcFromTo(k,i,boxes,adj,numBoxes)) continue;
      if(boxes[i].xleft > boxes[k].xleft || (boxes[i].xleft == boxes[k].xleft && boxes[i].xright>= boxes[k].xright) ) continue;

	  if(distanceY(&boxes[i],&boxes[k]) >= 0.97*MAX(boxHeight(&boxes[i]),boxHeight(&boxes[k]))) continue;

	   burro = boxes[i].index==31 && boxes[k].index==34;
	  
	  if(burro)      Rprintf("ordersubscriptsnormalmodeibase burro118 base %d over %d sub %d\n",boxes[i].index,boxes[j].index,boxes[k].index);

	  // 31 y 34
      //int print=(i==28 && j==30 && k ==31);
      /* if(i==28 && j==30 && k ==31){ */
      /*   Rprintf("ordersubscriptsnormalmodeibase base %d over %d sub %d\n",boxes[i].index,boxes[j].index,boxes[k].index); */
      /* } */


      if(distanceX(&boxes[i],&boxes[k])>10) continue;
      if(distanceY(&boxes[j], &boxes[k])==0) continue;
      if(distanceX(&boxes[j], &boxes[k])>10) continue; // Se han de solapar
      if(boxes[j].ybottom > boxes[k].ytop) continue; // j must be up
      if(distanceX(&boxes[i],&boxes[k])==0 && distanceY(&boxes[i],&boxes[k])>0) continue;
	  
	  
      // if(print)           Rprintf("ordersubscriptsnormalmodeibase base %d over %d sub %d:chek3 \n",boxes[i].index,boxes[j].index,boxes[k].index);
	  if(burro)      Rprintf("ordersubscriptsnormalmodeibase burro142 base %d over %d sub k= %d. closest04 %d\n",
							 boxes[i].index,boxes[j].index,boxes[k].index,closest04);
      if(!nothingbetweenYoverlapX(closest04, k,page, boxes, numBoxes)) continue;

	  if(burro)      Rprintf("ordersubscriptsnormalmodeibase burro145 base %d over %d sub %d\n",boxes[i].index,boxes[j].index,boxes[k].index);
      // if(print)           Rprintf("ordersubscriptsnormalmodeibase base %d over %d sub %d:chek4 \n",boxes[i].index,boxes[j].index,boxes[k].index);

      if(closest01 != i && closest01 != closest03) continue;
      //if(print)           Rprintf("ordersubscriptsnormalmodeibase base %d over %d sub %d:chek5 \n",boxes[i].index,boxes[j].index,boxes[k].index);
	  if(burro)      Rprintf("ordersubscriptsnormalmodeibase burro151 base %d over %d sub %d\n",boxes[i].index,boxes[j].index,boxes[k].index);
	  
      //int closest05 = findpos(ClosestYpage(k, page, boxes, numBoxes, -1),boxes,numBoxes);
      //if(print)           Rprintf("ordersubscriptsnormalmodeibase base %d over %d sub %d:chek6: closestof \n",boxes[i].index,boxes[j].index,boxes[k].index);
      //    if(closest02 != i && closest05 != closest02) continue;

      //Ok, it seems that j and k are over and under scripts
      

	  int checkover =  updateAdjacencyMatrix(i,j,SCRIPTNORMALOVERFIRST,boxes,adj,numBoxes);
	  int checksub =  updateAdjacencyMatrix(i,k,SCRIPTNORMALSUBFIRST,boxes,adj,numBoxes);
	  
      BoundingBox* cluster = copyBoundingBox(&boxes[i]);

      BoundingBox* lineover = getLinefromthisbox(boxes[j].index,  page, 10,1,boxes, numBoxes);

      BoundingBox* linesub = getLinefromthisbox(boxes[k].index,  page, 10,1,boxes, numBoxes);


      int sizecommon = 0;
      int* commonboxes= R_Calloc(sizecommon,int);

	  
      for(int zz =0; linesub && zz < linesub->size && checksub;zz++){
        int z = findpos(linesub->indices[zz],boxes,numBoxes);
        for(int hh = 0; lineover && hh < lineover->size && checkover ; hh++) {
          int h = findpos(lineover->indices[hh],boxes,numBoxes);
          if(h==z) {
            commonboxes =R_Realloc(commonboxes,sizecommon+1,int);
            commonboxes[sizecommon++] = linesub->indices[zz];
            continue;
          }
          if(overlapY(&boxes[z],&boxes[h])) {
            if(overlapCenterInY(&boxes[z], &boxes[i],0)){
              commonboxes =R_Realloc(commonboxes,sizecommon+1,int);
              commonboxes[sizecommon++] = linesub->indices[zz];
            }
            if(overlapCenterInY(&boxes[h], &boxes[i],0)){
              commonboxes =R_Realloc(commonboxes,sizecommon+1,int);
              commonboxes[sizecommon++] = linesub->indices[hh];
            }

          }
        }

      }



      Rprintf("Common boxes: ");
      for(int zz =0; zz < sizecommon;zz++){
        Rprintf("%d ",commonboxes[zz]);
      }
      Rprintf("\n");

      for(int zz = 0; linesub && zz < linesub->size ; zz++) {
        int z = findpos(linesub->indices[zz],boxes,numBoxes);
        if(exists_in_array(commonboxes, sizecommon, boxes[z].index)){
          break;
        }
        for(int hh = 0; lineover && hh < lineover->size  ; hh++) {
          int h = findpos(lineover->indices[hh],boxes,numBoxes);
          if(exists_in_array(commonboxes, sizecommon, boxes[h].index)){
            break;
          }

          if(checkover && checksub){
			updateAdjacencyMatrix(z,h,TWOLINESFROMBOTTOMTOTOPFRIEND,boxes,adj,numBoxes);
		  }
		  if(checkover) joinBoundingBoxAandBindex(cluster,&boxes[h],boxes,numBoxes);
		  if(checksub)  joinBoundingBoxAandBindex(cluster,&boxes[z],boxes,numBoxes);
          
        }
      }

      R_Free(commonboxes);

	  if(cluster){
      int distance=INT_MAX;
      int mascercano =boxes[i].index;
      for(int zz=0 ; zz < page->size; zz++){
        int zindex = page->indices[zz];
        if( exists_in_array(cluster->indices, cluster->size, zindex)) continue;
        int z =findpos(zindex,boxes,numBoxes);
        if(!overlapY(&boxes[i],&boxes[z])) continue;
        if(boxes[i].xleft > boxes[z].xleft) continue;
        if(distance < distanceX(&boxes[i],&boxes[z])) continue;
        distance = distanceX(&boxes[i],&boxes[z]);
        mascercano = zindex;
      }
      if(mascercano != boxes[i].index){
        int t = findpos(mascercano,boxes,numBoxes);
        for(int zz=0 ; zz < cluster->size; zz++){
          int zindex = cluster->indices[zz];
          int z =findpos(zindex,boxes,numBoxes);
        
		  updateAdjacencyMatrix(z,t,CLUSTERSUBNORMALRIGHT,boxes,adj,numBoxes);

        }
      }

	  } 

      freebbox(cluster);
      freebbox(lineover);
      freebbox(linesub);



      return 1;


    }

  }
  return 0;
}



void ordersubscriptsnormalmode( const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes){
  Rprintf("ordersubscriptsnormalmode\n");

  int numberlines = 0;
  BoundingBox* lines = createLinesConnectedinAdjacency(page, &numberlines,  boxes,  adj, numBoxes);
  Rprintf("ordersubscriptsnormalmode. There are %d lines\n",numberlines);
  if(numberlines ==1) {
    int iline =0;
    for(int ii =0; ii< lines[iline].size; ii++){
      int i = findpos(lines[iline].indices[ii],boxes,numBoxes);
      ordersubscriptsnormalmodeibase(  i, &lines[iline], boxes,  adj,  numBoxes);

    }
  } else{
    for(int iline = 0; iline< numberlines; iline++){
      ordersubscriptsnormalmode(&lines[iline], boxes, adj, numBoxes);
    }
  }
  FreeBoundingBox(lines,numberlines);

}




int isprod(int i, const BoundingBox* boxes, int* adj, int numBoxes){
  if(between(boxWidth(&boxes[i]), 0.95*WIDHTPRODMATH,1.05*WIDHTPRODMATH) &&
     between(boxHeight(&boxes[i]), 0.95*HEIGHTPRODMATH,1.05*HEIGHTPRODMATH)) return 1;
  return 0;

}

int issum(int i, const BoundingBox* boxes, int* adj, int numBoxes){
  if(between(boxWidth(&boxes[i]), 0.95*WIDHTSUMMATH,1.05*WIDHTSUMMATH) &&
     between(boxHeight(&boxes[i]), 0.95*HEIGHTSUMMATH,1.05*HEIGHTSUMMATH)) return 1;
  return 0;

}





void ordersubscriptsmathmode( const BoundingBox* page, const BoundingBox* boxes, int* adj, int numBoxes){
  
  for(int iibase = 0; iibase< page->size; iibase++){
	int ibase = page->positions[iibase];
      const BoundingBox* bbase = &boxes[ibase];
    //  if(boxes[ibase].index == 21){
    //  Rprintf("Chequeo box %d: width %d height %d, %d %d\n",boxes[ibase].index,boxWidth(bbase),boxHeight(bbase), WIDHTSUMMATH,HEIGHTSUMMATH);
    //}
    if(!( issum(ibase, boxes, adj,numBoxes) || isprod(ibase,boxes,adj,numBoxes)         )) continue; // Not a \Sigma or \Prod
    Rprintf("Box %d is like Sigma or Prod\n",bbase->index);

    BoundingBox* bover = NULL; // Initialize all fields to zero
    BoundingBox* bunder = NULL; // Initialize all fields to zero


	BoundingBox* overlapybase = getLinefromthisbox(boxes[ibase].index,  page, DISTXSUBSCRIPT*10, 1,  boxes, numBoxes);

     
    // Over
    int posclosestover =  ClosestYpage(ibase, page, boxes, numBoxes, 1);
    if(posclosestover!=ibase  && distanceY(&boxes[ibase],&boxes[posclosestover]) < DISTYSUMMATHOVER &&
	   !arcFromTo(posclosestover,ibase,boxes,adj,numBoxes)){

	  Rprintf("Sum over sigma is %d and over is %d\n",boxes[ibase].index,boxes[posclosestover].index);
	  
      BoundingBox* A = getLinefromthisbox(boxes[posclosestover].index,  page, DISTXSUMSUBSCRIPT, 0,  boxes, numBoxes);
	  Rprintfbbox(A);
	  
      BoundingBox* B = getLinefromthisbox(boxes[posclosestover].index,  page, DISTXSUMSUBSCRIPT, 1,  boxes, numBoxes);
	  Rprintfbbox(B);
      B->index = -1;

	  Rprintfbbox(A);
	  Rprintfbbox(B);
		
      joinBoundingBoxAandBindex(A,B,boxes,numBoxes);
      bover = copyBoundingBox(A);//copyBoundingBoxAtoB(A,&bover);
      freebbox(A);
      freebbox(B);
	  Rprintfbbox(bover);
      bover->size = removeduplicatesint(bover->indices, bover->size);

	  // Check no \sum_a^b \int_c^d
	  int newsize =0;
	  int* newindices = R_Calloc(bover->size,int);
	 
	  for(int ii =0 ; ii < bover->size; ii++){
		int i = findpos(bover->indices[ii],boxes,numBoxes);
		int common =0;
		for(int jj =0; jj<overlapybase->size; jj++){
		  int j = findpos(overlapybase->indices[jj],boxes,numBoxes);
		  if(i==j) {
			common++;
			break;
		  }
		}
		if(!common){
		  newindices[newsize++] = bover->indices[ii];
		} 
	  }
	  bover->size = newsize;
	  for(int ii =0 ; ii < bover->size; ii++){
		bover->indices[ii] = newindices[ii];
		bover->positions[ii] = findpos(newindices[ii],boxes,numBoxes);
	  }

	  R_Free(newindices);

	  Rprintfbbox(bover);
	  
     
    }
    // under
    int posclosestunder =  ClosestYpage(ibase, page, boxes, numBoxes, 0);
    Rprintf("UNDER base %d, closest %d: dist %d\n",boxes[ibase].index, boxes[posclosestunder].index,distanceY(&boxes[ibase],&boxes[posclosestunder]));
    if(posclosestunder!=ibase  && distanceY(&boxes[ibase],&boxes[posclosestunder]) < DISTYSUMMATHUNDER &&
	   !arcFromTo(posclosestunder,ibase,boxes,adj,numBoxes)  ){

	  
	  Rprintf("Sum under sigma is %d and under is %d\n",boxes[ibase].index,boxes[posclosestunder].index);
	  
      BoundingBox* A = getLinefromthisbox(boxes[posclosestunder].index,  page, DISTXSUMSUBSCRIPT, 0,  boxes, numBoxes);
	 
	  
      BoundingBox* B = getLinefromthisbox(boxes[posclosestunder].index,  page, DISTXSUMSUBSCRIPT, 1,  boxes, numBoxes);
	  
      B->index = -1;
	  Rprintfbbox(A);
	  Rprintfbbox(B);
      joinBoundingBoxAandBindex(A,B,boxes,numBoxes);
      bunder = copyBoundingBox(A); //copyBoundingBoxAtoB(A,&bunder);
      freebbox(A);
      freebbox(B);
	  bunder->size = removeduplicatesint(bunder->indices, bunder->size);

	  Rprintf("size %d",bunder->size);
	  // Check no \sum_a^b \int_c^d
	  int newsize =0;
	  int* newindices = R_Calloc(bunder->size,int);
	  for(int ii =0 ; bunder && ii < bunder->size; ii++){
		int i = findpos(bunder->indices[ii],boxes,numBoxes);
		int iscommon =0;
		for(int jj =0; jj<overlapybase->size; jj++){
		  int j = findpos(overlapybase->indices[jj],boxes,numBoxes);
		  if(i==j) {
			iscommon++;
			break;
		  }
		}
		Rprintf("common %d newsize %d\n",iscommon,newsize);
		if(!iscommon) {
		  Rprintf("dentron common %d newsize %d\n",iscommon,newsize,bunder->indices[ii]);
		  newindices[newsize++] = bunder->indices[ii];
		}
	  }
	  bunder->size = newsize;
	  for(int ii =0 ;bunder && ii < bunder->size; ii++){
		bunder->indices[ii] = newindices[ii];
		bunder->positions[ii] = findpos(newindices[ii],boxes,numBoxes);
	  }

	  R_Free(newindices);

	  Rprintfbbox(bunder);
	  
    
    }

	int countover =0;
	int countunder=0;

	Rprintfbbox(bover);
	
	int* indicesover = NULL; 
	int* indicesunder =NULL;

	if(bover){

	  indicesover = R_Calloc(bover->size,int);
		for(int kk = 0; kk<bover->size; kk++){
	  int k =findpos(bover->indices[kk],boxes,numBoxes);
	  int check= updateAdjacencyMatrix(ibase,k,SCRIPTSUMOVER,boxes,adj,numBoxes);
	  if(check) indicesover[countover++] =  bover->indices[kk];
	}

	}

	if(bunder){
	 indicesunder =R_Calloc(bunder->size,int);
	
	for(int kk = 0; kk<bunder->size; kk++){
	  int k =findpos(bunder->indices[kk],boxes,numBoxes);
	  int check= updateAdjacencyMatrix(ibase,k,SCRIPTSUMUNDER,boxes,adj,numBoxes);
	  if(check) indicesunder[countunder++] =  bunder->indices[kk];
	  
	}

	}
    for(int i1=0; i1< countunder; i1++){
      int pbase = findpos(indicesunder[i1],boxes,numBoxes);
      for(int i2=0; i2< countover; i2++){
        int second = findpos(indicesover[i2],boxes,numBoxes);
        //Rprintf("Linking base %d with %d\n",pbase,isecond);
		updateAdjacencyMatrix(pbase,second,SCRIPTSUMFROMBOTTOMTOTOP,boxes,adj,numBoxes);
	  }
    }

	R_Free(indicesover);
	R_Free(indicesunder);
    freebbox(bover);
    freebbox(bunder);

	freebbox(overlapybase);



  }

}




int checkisfrac(int numerator, int frac, int denominator,const BoundingBox* boxes, int numBoxes ){
  if(numerator == frac || numerator==denominator || frac==denominator) return 0;
  //Rprintf("fracmath check1\n");
  if(!overlapX(&boxes[numerator],&boxes[frac]) || !overlapX(&boxes[denominator],&boxes[frac]) ) return 0;
  //Rprintf("fracmath check2\n");
  if(!between(boxes[numerator].xleft,boxes[frac].xleft,boxes[frac].xright )) return 0;
  //Rprintf("fracmath check3\n");
  if(!between(boxes[denominator].xleft,boxes[frac].xleft,boxes[frac].xright )) return 0;
  //Rprintf("fracmath check4\n");
  if(!between(boxes[numerator].xright,boxes[frac].xleft,boxes[frac].xright )) return 0;
  //Rprintf("fracmath check5\n");
  if(!between(boxes[denominator].xright,boxes[frac].xleft,boxes[frac].xright )) return 0;
  //Rprintf("fracmath check6\n");
  if(boxHeight(&boxes[frac])> 6) return 0;
  // Rprintf("fracmath check7\n");
  if(distanceY(&boxes[numerator],&boxes[frac])> 20) return 0;
  //Rprintf("fracmath check8\n");
  if(distanceY(&boxes[denominator],&boxes[frac])> 24) return 0;
  if(boxes[denominator].ybottom < boxes[frac].ytop) return 0;
  if(boxes[frac].ybottom < boxes[numerator].ytop) return 0;

  return 1;
}



// frac{i}{k}: j is the central line
void orderfracmathmode( const BoundingBox* page,const BoundingBox* boxes, int *adj, int numBoxes){
  Rprintf("Orderfracmathmode\n");

  for(int jj = 0; jj< page->size; jj++ ){
    int j = findpos(page->indices[jj],boxes,numBoxes);

    for(int ii = 0; ii < page->size; ii++ ){
      int i = findpos(page->indices[ii],boxes,numBoxes);
      if(i== j ||
         !noarc(i,j,boxes,adj,numBoxes) ||
         !overlapX(&boxes[i],&boxes[j])
         ) continue;

      for(int kk = 0; kk< page->size; kk++ ){
        int k = findpos(page->indices[kk],boxes,numBoxes);

        // Rprintf("fracmath %d %d %d: %d\n", i,j,k,checkisfrac(i, j, k, boxes,  numBoxes ));

        if(checkisfrac(i, j, k, boxes,  numBoxes )){


          //updateAdjacencyMatrixFirstASecondB(boxes,adj,numBoxes,i,j,FRACNUMFRAC);
          //updateAdjacencyMatrixFirstASecondB(boxes,adj,numBoxes,j,k,FRACFRACDENOM);


          BoundingBox* cluster = copyBoundingBox(&boxes[j]);

          BoundingBox* lineover = getLinefromthisboxFrac(boxes[i].index, boxes[j].index, page, 1 ,boxes, numBoxes);

          BoundingBox* linesub = getLinefromthisboxFrac(boxes[k].index, boxes[j].index, page, 0 ,boxes, numBoxes);


          for(int zz = 0; zz < linesub->size ; zz++) {
            int z = findpos(linesub->indices[zz],boxes,numBoxes);
            for(int hh = 0; hh < lineover->size  ; hh++) {
              int h = findpos(lineover->indices[hh],boxes,numBoxes);
			  int check;
			  check = updateAdjacencyMatrix(h,j,FRACOVER,boxes,adj,numBoxes);
			  if(check) joinBoundingBoxAandBindex(cluster,&boxes[h],boxes,numBoxes);
			  check = updateAdjacencyMatrix(j,z,FRACUNDER,boxes,adj,numBoxes);
              if(check) joinBoundingBoxAandBindex(cluster,&boxes[z],boxes,numBoxes);
			}
          }

          // right
          int distance=INT_MAX;
          int mascercano =boxes[j].index;
          for(int zz=0 ; zz < page->size; zz++){
            int zindex = page->indices[zz];
            if(exists_in_array(cluster->indices, cluster->size, zindex)) continue;
            int z =findpos(zindex,boxes,numBoxes);
            if(!overlapY(&boxes[j],&boxes[z])) continue;
            if(boxes[j].xleft > boxes[z].xleft) continue;
            if(distance < distanceX(&boxes[j],&boxes[z])) continue;
            if(!notincludedAinB(boxes,adj, numBoxes,j, z)) continue;
            distance = distanceX(&boxes[j],&boxes[z]);
            mascercano = zindex;
          }
          if(mascercano != boxes[j].index){
            int t = findpos(mascercano,boxes,numBoxes);
            for(int zz=0 ; zz < cluster->size; zz++){
              int zindex = cluster->indices[zz];
              int z =findpos(zindex,boxes,numBoxes);
			  updateAdjacencyMatrix(z,t,CLUSTERFRACRIGHT,boxes,adj,numBoxes);
              
            }
          }


          //left
          distance=INT_MAX;
          mascercano =boxes[j].index;
          for(int zz=0 ; zz < page->size; zz++){
            int zindex = page->indices[zz];
            if(exists_in_array(cluster->indices, cluster->size, zindex)) continue;
            int z =findpos(zindex,boxes,numBoxes);
            if(!overlapY(&boxes[j],&boxes[z])) continue;
            if(boxes[j].xleft < boxes[z].xleft) continue;
            if(distance < distanceX(&boxes[j],&boxes[z])) continue;
            if(!notincludedAinB(boxes,adj, numBoxes,j, z)) continue;
            distance = distanceX(&boxes[j],&boxes[z]);
            mascercano = zindex;
          }
          if(mascercano != boxes[j].index){
            int t = findpos(mascercano,boxes,numBoxes);
            for(int zz=0 ; zz < cluster->size; zz++){
              int zindex = cluster->indices[zz];
              int z =findpos(zindex,boxes,numBoxes);
			  updateAdjacencyMatrix(t,z,CLUSTERFRACLEFT,boxes,adj,numBoxes);
            }
          }

          freebbox(linesub);
          freebbox(lineover);
          freebbox(cluster);

        }

      }
    }


  }}



void doorderwordright( const BoundingBox* page,const BoundingBox* boxes, int *adj, int numBoxes){
  Rprintf("Orderwordrigth \n");

  for(int ii = 0; ii < page->size; ii++ ){
    int i = findpos(page->indices[ii],boxes,numBoxes);
    for(int jj = 0; jj< page->size; jj++ ){
      int j = findpos(page->indices[jj],boxes,numBoxes);
      if(i== j ||
         !noarc(i,j,boxes,adj,numBoxes) ||
         !overlapY(&boxes[i],&boxes[j])
         ) continue;

      int closest = findpos(ClosestXpage(i, page, boxes, numBoxes, 1),boxes,numBoxes);
      if(j!= closest) continue;
      if(!samelevelletters(&boxes[i], &boxes[j])) continue;
      if(distanceX(&boxes[i], &boxes[j]) > DISTXSAMELETTERNORMALMODE ) continue;

      	  updateAdjacencyMatrix(i,j,WORDRIGHT,boxes,adj,numBoxes);
    }

  }
}



/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
