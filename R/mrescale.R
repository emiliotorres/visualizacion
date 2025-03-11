## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-01-16 20:52 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##
##' Rescale a matrix using moving averages in X and Y.
##'
##' Useful fo images.
##' @title Rescale a matrix
##' @param m a numeric matrix
##' @param nrow new dimension
##' @param ncol new dimension
##' @return a matrix with dimension nrow x ncol (numeric)
##' @author emilio
##' @export
##' @examples
##' m <- matrix(1:6,2,3)
##' mrescale(m,4,6)
mrescale <- function(m,nrow,ncol){
    if(!is.matrix(m)){
        stop("M is not a matrix: ",paste(class(m),collapse=","),".")
    }
    if(is.na(nrow)) {
        stop("NROW is NA: ",paste(class(nrow),collapse=","),".")
    }
    if(is.na(ncol)) {
        stop("NCOL is NA: ",paste(class(nrow),collapse=","),".")
    }

    if(!is.numeric(nrow) || is.na(nrow) || nrow != as.integer(nrow) ) {
        stop("NROW is not an integer: ",paste(class(nrow),collapse=","),".")
    }

    if(!is.numeric(ncol) || is.na(ncol) || ncol != as.integer(ncol) ) {
        stop("NCOL is not an integer: ",paste(class(ncol),collapse=","),".")
    }

    nrowold <- nrow(m)
    ncolold <- ncol(m)
    ncol <-  as.integer(ncol)
    nrow <- as.integer(nrow)

    .Call("get_rescale_matrix",m,nrow,ncol)
}



## Local Variables:
## ispell-local-dictionary: "british"
## End:
