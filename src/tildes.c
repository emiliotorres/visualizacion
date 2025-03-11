/*
  TITLE:
  AUTHOR: Emilio Torres Manzanera
  DATE: Time-stamp: <2025-02-20 12:36 emilio on emilio-XPS-15-9570>
*/

#include "boundingbox.h"

// ============================================================
// Titles, tildes and puntuaction marks. Also inclusion
// ============================================================
// To use with
//
// \documentclass[hmargin=0.01cm,vmargin=0cm,head=0.05cm,headsep=0pt,foot=0.05cm,margin=0.01cm,scaleratio=169]{beamer}
// \usepackage[utf8]{inputenc}
// \usepackage{fouriernc}
// \usepackage[T1]{fontenc}
// \usecolortheme{cormorant}
// \beamertemplatenavigationsymbolsempty
// \usefonttheme{professionalfonts}

#define MAXHEIGHTCHARACTERBASETILDE 100 // Normal fonts are about 64 pixels in upper cases and 80 in case of j, lower letters about 41 pixels. 84x104 is Sigma
#define RATIOHEIGHTBASETILDE 0.334
#define MAXDISTYBASETILDE 14
#define MAXDISTYBASEEQUALSIGN 20
#define MAXDISTYBASESEMICOLONSIGN 20
#define SIZEDOT (4+1)
// i 5 de ancho , 4 de alto, distanciaY: 6
// í 12 de ancho, 5 de alto, distanciaY: 3
// ñ 9 de ancho, 5 de alto, distanciaY: 3
#define RATIODOTIDISTANCEY 1.5 // 6/4






// Function to check if there is nothing between boxA and boxB along the Y-axis
int nothingbetweenY(int iA, int jB, const BoundingBox* boxes, int numBoxes) {
  const BoundingBox* boxA = &boxes[iA];
  const BoundingBox* boxB = &boxes[jB];
  // Determine the vertical range between boxA and boxB
  int top = MAX(boxA->ytop,boxB->ytop);    // Top boundary
  int bottom = MIN(boxB->ybottom , boxA->ybottom); // Bottom boundary

  // Iterate through all boxes
  for (int i = 0; i < numBoxes; i++) {
    // Skip boxA and boxB
    if (i == iA || i == jB) continue;
    if( isAIncludedInB(boxA,&boxes[i]) && isAIncludedInB(boxB,&boxes[i])) continue;

    // Check if the current box overlaps with both boxA and boxB in the X-axis
    if (overlapX(&boxes[i], boxA) && overlapX(&boxes[i], boxB)) {
      // Check if the current box is vertically between boxA and boxB
      if ( between(boxes[i].ybottom, bottom, top) ||
           between(boxes[i].ytop, bottom, top) ||
           (intersectAB(&boxes[i], boxA) && intersectAB(&boxes[i], boxB))
           ) {
        return 0; // There is something between boxA and boxB
      }
    }
  }

  return 1; // Nothing is between boxA and boxB
}

// Large
// normalsize
// small
// footnotesize
int isdotofcharacter(int idot, int icharacter, int above, const BoundingBox* boxes, int numBoxes){

  const BoundingBox* boxdot = &boxes[idot];
  const BoundingBox* boxB = &boxes[icharacter];
  int distX = distanceX(boxdot, boxB);
  if(distX > 0) return 0; // Overlap in X
  if(aboveAoverB(boxdot,boxB) != above ) return 0;
  int distY = distanceY(boxdot, boxB);
  if(distY==0) return 0;
  int heightdot = boxHeight( boxdot );
  int widthdot = boxWidth(boxdot);
  //int heightB = boxHeight( boxB);
  //int widthB = boxWidth( boxB);
  if(!between(heightdot,3,5)) return 0;
  if(heightdot+3 < widthdot) return 0;
  if(widthdot+3 < distY) return 0;
  // Si no es tiene la distancia más corta, nada.
  int closesty  = ClosestY(icharacter,boxes, numBoxes, (above > 0)? (1): (-1)); // direction = 1 (up), direction = -1 (down)
  if (distY != distanceY(boxB, &boxes[closesty])) return 0;
  Rprintf("Box %d has a dot %d.\n",boxes[icharacter].index, boxes[idot].index);
  return 1;
}


int istildeofcharacter(int idot, int icharacter, const BoundingBox* boxes, int numBoxes){

  const BoundingBox* boxdot = &boxes[idot];
  const BoundingBox* boxB = &boxes[icharacter];
  int distX = distanceX(boxdot, boxB);
  if(distX > 0) return 0; // Overlap in X
  int distY = distanceY(boxdot, boxB);
  if(distY==0) return 0;
  int heightdot = boxHeight( boxdot );
  int widthdot = boxWidth(boxdot);
  //int heightB = boxHeight( boxB);
  //int widthB = boxWidth( boxB);
  if(!between(heightdot,5,7)) return 0;
  if(heightdot+8 < widthdot) return 0;
  if(heightdot < distY) return 0;
  // Si no es tiene la distancia más corta, nada.
  int closesty  = ClosestY(icharacter,boxes, numBoxes,  1); // direction = 1 (up), direction = -1 (down)
  if (distY != distanceY(boxB, &boxes[closesty])) return 0;
  Rprintf("Box %d has a tilde %d.\n",boxes[icharacter].index, boxes[idot].index);
  return 1;
}


int isvirgulillaofcharactern(int idot, int icharacter, const BoundingBox* boxes, int numBoxes){

  const BoundingBox* boxdot = &boxes[idot];
  const BoundingBox* boxB = &boxes[icharacter];
  int distX = distanceX(boxdot, boxB);
  if(distX > 0) return 0; // Overlap in X
  int distY = distanceY(boxdot, boxB);
  if(distY==0) return 0;
  int heightdot = boxHeight( boxdot );
  int widthdot = boxWidth(boxdot);
  //int heightB = boxHeight( boxB);
  int widthB = boxWidth( boxB);
  if(!between(heightdot,4,6)) return 0;
  if(widthdot +3  < widthB) return 0;
  if(heightdot < distY) return 0;
  // Si no es tiene la distancia más corta, nada.
  int closesty  = ClosestY(icharacter,boxes, numBoxes,  1); // direction = 1 (up), direction = -1 (down)
  if (distY != distanceY(boxB, &boxes[closesty])) return 0;
  Rprintf("Box %d has a virgulilla %d.\n",boxes[icharacter].index, boxes[idot].index);
  return 1;
}




int istildeAofBase2(int iA, int jB, int above, const BoundingBox* boxes, int numBoxes){
  const BoundingBox* boxdot = &boxes[iA];
  const BoundingBox* boxB = &boxes[jB];
  int distX = distanceX(boxdot, boxB);
  if(distX > 0) return 0; // Overlap in X
  if(aboveAoverB(boxdot,boxB) != above ) return 0;
  int distY = distanceY(boxdot, boxB);
  if(distY==0) return 0;
  int heightdot = boxHeight( boxdot );
  int widthdot = boxWidth(boxdot);
  int heightB = boxHeight( boxB);
  int widthB = boxWidth( boxB);
  if(heightdot > heightB) return 0;
  double ratiodotistanceyheight = (double) distY/heightdot;
  Rprintf("Probando Box %d has a  %d (above %d) (distY %d, ratio %0.2f) (hBase %d htil %d).\n",boxes[jB].index, boxes[iA].index,above, distY, ratiodotistanceyheight,heightB,heightdot);

  // Si no es tiene la distancia más corta, nada.
  int closesty  = ClosestY(jB,boxes, numBoxes, (above > 0)? (1): (-1)); // direction = 1 (up), direction = -1 (down)
  if (distY != distanceY(boxB, &boxes[closesty])) return 0;

  // i í
  if( heightdot > widthdot) return 0;
  // i 5 de ancho , 4 de alto, distanciaY: 6
  // í 12 de ancho, 5 de alto, distanciaY: 3
  // ñ 9 de ancho, 5 de alto, distancia Y: 3
  // :
  // ;
  //double ratiodotistanceyheight = (double) distY/heightdot;
  // if( ratiodotistanceyheight > RATIODOTIDISTANCEY+0.01 && heightdot>SIZEDOT ) return 0;
  if( ratiodotistanceyheight > RATIODOTIDISTANCEY+0.01  ) return 0;

  if(heightdot*widthdot > heightB*widthB + 1 ) return 0;
  if(heightdot > (double) heightB /3) return 0;
  if(!isatomicbbox(boxdot,boxes,numBoxes)) return 0;
  if(!isatomicbbox(boxB,boxes,numBoxes)) return 0;

  /* if(heightA > SIZEDOT) return 0; */
  /* if(boxHeight(boxA) > SIZEDOT) return 0; */
  /* if( heightA > heightB * RATIOHEIGHTBASETILDE) return 0; */

  /* if(distY > heightB * RATIOHEIGHTBASETILDE) return 0; */
  /* if(!isatomicbbox(boxA,boxes,numBoxes)) return 0; */
  /* if(!isatomicbbox(boxB,boxes,numBoxes)) return 0; */
  /* if(heightB > MAXHEIGHTCHARACTERBASETILDE) return 0; */
  /* if(distY > MAXDISTYBASETILDE) return 0; */

  //double ratioXYbase = (double)heightB/(double)boxWidth(boxB);
  //if(between(ratioXYbase, 0.769,1.3)) return 0; // Is it Sigma?

  Rprintf("Box %d has a tilde %d (above %d) (distY %d, ratio %0.2f) (hBase %d htil %d).\n",boxes[jB].index, boxes[iA].index,above, distY, ratiodotistanceyheight,heightB,heightdot);
  return 1;
}


int isequalsign(int iA, int jB, const BoundingBox* boxes, int numBoxes){
  // Rprintf("Box %d check sign = %d.\n",boxes[jB].index, boxes[iA].index);

  const BoundingBox* boxA = &boxes[iA];
  const BoundingBox* boxB = &boxes[jB];
  int distX = distanceX(boxA, boxB);
  if(distX > 0) return 0;
  if(!aboveAoverB(boxA,boxB)) return 0;
  if(boxA->xleft != boxB->xleft) return 0;
  //Rprintf("Box %d check2 sign = %d.\n",boxes[jB].index, boxes[iA].index);
  int heightA = boxHeight( boxA );
  int heightB = boxHeight( boxB);
  if(!between(heightA,heightB-1,heightB+1)) return 0;
  if(boxWidth(boxA)!=boxWidth(boxB)) return 0;
  int distY = distanceY(boxA, boxB);
  // Rprintf("Box %d check3 sign = %d. distancia %d\n",boxes[jB].index, boxes[iA].index,distY);
  if(distY > 9) return 0;
  if(!isatomicbbox(boxA,boxes,numBoxes)) return 0;
  if(!isatomicbbox(boxB,boxes,numBoxes)) return 0;
  //Rprintf("Box %d check4 sign = %d.\n",boxes[jB].index, boxes[iA].index);


  //double ratioXYbase = (double)heightB/(double)boxWidth(boxB);
  //if(between(ratioXYbase, 0.769,1.3)) return 0; // Is it Sigma?

  Rprintf("Box %d has a equal sign = %d.\n",boxes[jB].index, boxes[iA].index);
  return 1;
}


int issemicolon(int iA, int jB, const BoundingBox* boxes, int numBoxes){
  const BoundingBox* boxA = &boxes[iA];
  const BoundingBox* boxB = &boxes[jB];
  int distX = distanceX(boxA, boxB);
  if(distX > 0) return 0;
  if(!aboveAoverB(boxA,boxB)) return 0;

  int heightA = boxHeight( boxA );
  if(! between(heightA,SIZEDOT-1,SIZEDOT+1)) return 0;

  int distY = distanceY(boxA, boxB);
  if(distY > MAXDISTYBASESEMICOLONSIGN+1) return 0;
  if(!isatomicbbox(boxA,boxes,numBoxes)) return 0;
  if(!isatomicbbox(boxB,boxes,numBoxes)) return 0;

  Rprintf("Box %d has a semicolon sign ; %d.\n",boxes[jB].index, boxes[iA].index);
  return 1;
}






void jointildes(const BoundingBox* p, BoundingBox* boxes, int*adj,const int numBoxes){

  Rprintf("tildes page %d (size %d)\n",p->index,p->size);
  for(int i = 0; i< p->size ;i ++){
    int i1 = p->positions[i];
    int first = -1;
    int second = -1;
    int third = -1;
    int numberofdots =0;
    Direction direction = 0;
    for(int j = 0; j< p->size ;j ++){
      int i2 = p->positions[j];
      if(i1== i2) continue;
      if(isdotofcharacter(i2, i1, 0, boxes,  numBoxes) ||
         isdotofcharacter(i2, i1, 1, boxes,  numBoxes) ){
        first = i1;
        direction = DOT;
        if(numberofdots==0) { second = i2; }
        else if(numberofdots==1) {third = i2;}
        numberofdots++;
      } else  if(istildeofcharacter(i2, i1, boxes,  numBoxes)){
        first = i1;
        direction = TILDE;
        if(numberofdots==0) { second = i2; }
        else if(numberofdots==1) {third = i2;}
        numberofdots++;
      } else if(isvirgulillaofcharactern(i2, i1, boxes,  numBoxes)){
        first = i1;
        direction = VIRGULILLA;
        if(numberofdots==0) { second = i2; }
        else if(numberofdots==1) {third = i2;}
        numberofdots++;
      }
    }
    int check12 = 0;
    int check13 = 0;
    int check23 = 0;
    if(numberofdots == 2) { //ü
      if(boxes[second].xleft > boxes[third].xleft) {
        int temp = third;
        third = second;
        second = temp;
      }
    }
    if(first != -1 && second != -1 && noarc(first,second,boxes,adj,numBoxes)){
      check12 = updateAdjacencyMatrix(first,second,direction,boxes,adj,numBoxes);
    }
    if(first != -1 && third != -1 && check12 && noarc(first,third,boxes,adj,numBoxes)){
      check13 = updateAdjacencyMatrix(first,third,direction,boxes,adj,numBoxes);
    }
    if(check12 && check13 && noarc(second,third,boxes,adj,numBoxes)){
      check23 = updateAdjacencyMatrix(second,third,DOTDOT,boxes,adj,numBoxes);
    }

    if(check13){
      unionDimensionsA(&boxes[third], &boxes[first]);
    }
    if(check23){
      unionDimensionsA(&boxes[third], &boxes[second]);
    }
    if(check12 && !check13){
      unionDimensionsA(&boxes[second], &boxes[first]);
    }


  }

}





void joinequalsign(const BoundingBox* p, BoundingBox* boxes, int*adj,const int numBoxes){

  for(int i = 0; i< p->size ;i ++){
    int i1 = p->positions[i];
    int first = -1;
    int second = -1;
    int numberofdots =0;
    Direction direction = 0;
    for(int j = 0; j< p->size ;j ++){
      int i2 = p->positions[j];
      if(i1== i2) continue;
      if(isequalsign(i1, i2, boxes,  numBoxes)){
        first = i1;
        second = i2;
        direction = EQUALSIGN;
        numberofdots++;
      } else if(issemicolon(i1, i2, boxes,  numBoxes)){
        first = i1;
        second = i2;
        direction = SEMICOLONSIGN;
        numberofdots++;
      }
    }
    if(numberofdots == 1 && first!=-1 && second!= -1 && noarc(first,second,boxes,adj,numBoxes)){
      int check = updateAdjacencyMatrix(first,second,direction,boxes,adj,numBoxes);
      if(check)   unionDimensionsA(&boxes[second], &boxes[first]);
    }

  }
}






int joinFragmentedCharacters(const BoundingBox* page, BoundingBox* boxes, int* adj, int numBoxes){
  jointildes(page,boxes,adj,numBoxes);
  joinequalsign(page,boxes,adj,numBoxes);

  return 1;
}

/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
