

#include "matchmatrix.h"




// ============================================================
// ============================================================

// s_x entero de POSICIONES un vector de longitud n, con n<=dims[0]*dims[1]. Es decir, no es toda la matriz.
// Saca un vector de longitud de s_x con los segmentos de puntos conexos de x que representa una matriz.

SEXP get_segments_of_points_matrix(SEXP s_x, SEXP s_dims) {


  if (TYPEOF(s_x) != INTSXP)
    Rf_error("X vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(s_x)));

  if (TYPEOF(s_dims) != INTSXP)
    Rf_error("DIMS vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(s_dims)));

  if (LENGTH(s_dims) != 2)
    Rf_error("DIMS vector not valid. It is '%s' with length %d.",
             Rf_type2char(TYPEOF(s_dims)), LENGTH(s_dims));


  int dim0 = INTEGER(s_dims)[0];
  int dim1 = INTEGER(s_dims)[1];

  if (dim0 * dim1 < LENGTH(s_x) || dim0 <1 || dim1 < 1 || dim0== NA_INTEGER || dim1==NA_INTEGER) {
    Rf_error("X has length %d > WIDTH is %d * HEIGHT is %d.", LENGTH(s_x), dim0,
             dim1);
  }

  //printf("Empezamos 85\n");

  int *px = INTEGER(s_x);
  int lengthx = LENGTH(s_x);
  int lengthm = dim0 * dim1;

  //printf("linea 91 %d\n", lengthm);

  int protecti = 0;
  SEXP s_segments = PROTECT(Rf_allocVector(INTSXP, lengthm)); protecti++;
  int *msegments = INTEGER(s_segments);
  int lengthstackpos = 8*lengthm;
  SEXP s_stackpos = PROTECT(Rf_allocVector(INTSXP, lengthstackpos)); protecti++;
  int *stackpos = INTEGER(s_stackpos);
  SEXP s_iwashere = PROTECT(Rf_allocVector(INTSXP, lengthm)); protecti++;
  int *iwashere = INTEGER(s_iwashere);
  SEXP s_ispoint = PROTECT(Rf_allocVector(INTSXP, lengthm)); protecti++;
  int *ispoint = INTEGER(s_ispoint);
  SEXP s_result = PROTECT(Rf_allocVector(INTSXP, lengthx)); protecti++;
  int *result = INTEGER(s_result);

  for(int i = 0; i != lengthm; i++){
    stackpos[i] = 0;
    ispoint[i] = 0;
    iwashere[i]=0;
    msegments[i] = 0;
  }
  for (int i = 0; i!= lengthx; i++) {
    result[i] =0;
    int pos=px[i]-1; // From R to C format
    if( pos <0 || pos == NA_INTEGER || pos >= lengthm){
      int temp = px[i];
      UNPROTECT(protecti);
      Rf_error("X has negative values or zeros or missing data or an erronous position (value) X[%d]= %d.", i+1, temp );
    }
    ispoint[pos] = 1;
  }

  int chunk[9];
  int k_values[] = {0,3,6,1,7,2,5,8};
  int length_k = sizeof(k_values) / sizeof(k_values[0]);

  int dim0minus1 = dim0 - 1;
  int dim1minus1 = dim1 - 1;

  int segment = 0;
  for (int ix = 0; ix < lengthx; ix++) {
    int pos=px[ix]-1; // Restamos 1
    if (msegments[pos]==0 && ispoint[pos]) {
      segment++;
      //printf("Nuevo segment %d\n", segment);

      //creamos un stack para evitar la recursion
      int top=-1;
      top++;
      stackpos[top] = pos;
      while((top > -1) && (top < lengthstackpos)){
        //printf("top %d\n",top);
        pos = stackpos[top];
        top--;

        if(iwashere[pos]) {
          continue;
        }
        iwashere[pos]=1;

        if(msegments[pos]>0 || !ispoint[pos]){
          continue;
        }
        msegments[pos] = segment;

        int j = pos /dim0;
        int i = pos - j*dim0;

        chunk[3] = (0 < i) ? pos - 1 : pos;
        chunk[4] = pos;
        chunk[5] = (i < dim0minus1) ? pos + 1 : pos;

        chunk[0] = ((0 < i) && (0 < j)) ? chunk[3] - dim0 : pos;
        chunk[1] = ((0 < j)) ? chunk[4] - dim0 : pos;
        chunk[2] = ((0 < j) && (i < dim0minus1)) ? chunk[5] - dim0 : pos;

        chunk[6] = ((j < dim1minus1) && (0 < i)) ? chunk[3] + dim0 : pos;
        chunk[7] = ((j < dim1minus1)) ? chunk[4] + dim0 : pos;
        chunk[8] = ((i < dim0minus1) &&(j < dim1minus1)) ? chunk[5] + dim0 : pos;

        for (int k1 = 0; k1 < length_k; k1++) {
          int k =k_values[k1];
          // if(chunk[k]<0 || chunk[k]>=lengthm){
          //    printf("error en el chunk pos %d k %d chunk %d j %d dim1minus %d\n",pos,k,chunk[k],j,dim1minus1);
          // }
          //printf("pos %d i %d j %d k %d chunk %d iwashere %d ispoint %d segmento %d\n", pos,i,j,k,chunk[k],iwashere[chunk[k]], ispoint[chunk[k]],msegments[chunk[k]]);
          if(!iwashere[chunk[k]] && ispoint[chunk[k]]){
            if(top >= lengthstackpos -1){
              lengthstackpos *= 2;
              s_stackpos = PROTECT(Rf_lengthgets(s_stackpos, lengthstackpos));protecti++;
              stackpos = INTEGER(s_stackpos);
            }
            top++;
            stackpos[top] = chunk[k];

          }
        }
      }
    }
    pos = px[ix]-1;
    result[ix] = msegments[pos];
  }

  UNPROTECT(protecti);
  return s_result;
}




// for( oletra in 1:nrow(dtordenletrasbw)) {
//   ##    print(oletra)
//  dtnoletrasbw[ irowmin1 <= dtordenletrasbw[oletra,irowmax2] & jcolmin1<=  dtordenletrasbw[oletra,jcolmax2],
//                ordensegmento1:= dtordenletrasbw[oletra,ordenletras2] ]
//     }


 SEXP C_buclefor_ordenar(SEXP s_irow1, SEXP s_jcol1,
                                   SEXP s_irow2, SEXP s_jcol2, SEXP s_segmento2) {

  if (TYPEOF(s_irow1) != INTSXP)
    Rf_error("s_irow1 vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(s_irow1)));

  if (TYPEOF(s_jcol1) != INTSXP)
    Rf_error("s_jcol1 vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(s_jcol1)));

  if (TYPEOF(s_irow2) != INTSXP)
    Rf_error("s_irow2 vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(s_irow2)));

  if (TYPEOF(s_jcol2) != INTSXP)
    Rf_error("s_jcol2 vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(s_jcol2)));

  if (TYPEOF(s_segmento2) != INTSXP)
    Rf_error("s_segmento2 vector not valid. It is '%s' (integer?)",
             Rf_type2char(TYPEOF(s_segmento2)));


  if (LENGTH(s_irow1) != LENGTH(s_jcol1)) {
    Rf_error("X has length %d  and %d.", LENGTH(s_irow1), LENGTH(s_irow2));
  }

  if (LENGTH(s_irow2) != LENGTH(s_jcol2) || (LENGTH(s_irow2) != LENGTH(s_segmento2 ))) {
    Rf_error("X has length %d,  %d,  %d.", LENGTH(s_irow2), LENGTH(s_irow2),LENGTH(s_segmento2));
  }

  //printf("Empezamos 85\n");

  int *irow1 = INTEGER(s_irow1);
  int *jcol1 = INTEGER(s_jcol1);
  int length1 = LENGTH(s_irow1);
  int *irow2 = INTEGER(s_irow2);
  int *jcol2 = INTEGER(s_jcol2);
  int *segmento2 = INTEGER(s_segmento2);
  int length2 = LENGTH(s_irow2);


  int protecti = 0;
  SEXP s_resultado = PROTECT(Rf_allocVector(INTSXP, length1)); protecti++;
  int *resultado = INTEGER(s_resultado);
  // Set all elements to NA
  for (R_xlen_t i = 0; i < length1; i++) {
    resultado[i] = NA_INTEGER;
  }

  // for( oletra in 1:nrow(dtordenletrasbw)) {
  //   ##    print(oletra)
  //  dtnoletrasbw[ irowmin1 <= dtordenletrasbw[oletra,irowmax2] & jcolmin1<=  dtordenletrasbw[oletra,jcolmax2],
  //                ordensegmento1:= dtordenletrasbw[oletra,ordenletras2] ]
  //     }


  for (R_xlen_t i2 = 0; i2 < length2; i2++) {
    Rprintf("irow2 es %d. jrow2 %d segmento %d\n",irow2[i2],jcol2[i2],segmento2[i2]);
    for (R_xlen_t i1 = 0; i1 < length1; i1++) {
      if( (irow1[i1] <= irow2[i2]) && (jcol1[i1] <= jcol2[i2]) ) {
        resultado[i1] = segmento2[i2];
      }
    }
  }
  for (unsigned i1 = length1 ; i1-- > 0; ) {
    if(resultado[i1]== NA_INTEGER){
      for (unsigned i2 = length2 ; i2-- > 0 ; ){
        if( irow1[i1] >= irow2[i2]){
          resultado[i1] = segmento2[i2];

        }
      }
    }
  }

  UNPROTECT(protecti);
  return s_resultado;
}



/*
  Local Variables:
  ispell-local-dictionary: "british"
  End:
*/
