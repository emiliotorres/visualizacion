## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-30 02:35 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session


##' Match for matrices.
##'
##' This function attaches a hash table to matrix y, similar to the functionality in \pkg{fastmatch},
##' for efficient matching of rows between two matrices x and y.
##'
##' @title Match for matrices.
##' @param x A matrix with rows to be matched.
##' @param y A matrix whose rows are to be matched against.
##' @param by Specifications of the columns used for matching.
##' @param by.x Same as \code{by}.
##' @param by.y Same as \code{by}.
##' @param nomatch The value to be returned in the case when no match is found. It is coerced to \code{integer}.
##' @param rowsx Rows of matrix x to be matched.
##'   Can be either an integer vector or a logical vector.
##' @param rowsy Rows of matrix y to be matched against.
##'   Can be either an integer vector or a logical vector.
##' @return A vector with the first position on matrix y that is equal to the corresponding row of matrix x.
##' @author emilio
##' @export
##' @examples
##' x  <- matrix(sample(1:100,1e6,TRUE),ncol=2)
##' s  <- matrix(1:100,ncol=2)
##' colnames(x) <- colnames(s) <- c("v1","v2")
##' ## the first call to mmatch is comparable to match
##' system.time(mmatch(s,x))
##' ## but the subsequent calls take no time!
##' system.time(mmatch(s,x))
mmatch <- function(x,y,by=NULL, by.x=by, by.y=by, nomatch=NA_integer_,rowsx=NULL,rowsy=NULL) {

    colsx <- seq_len(ncol(x))
    colsy <- seq_len(ncol(y))

    if(is.null(by) && is.null(by.x) && is.null(by.y)) {
        if(length(colnames(x)) && length(colnames(y))) {
            colsy <- colsx  <- intersect(colnames(x),colnames(y))
        } else {
            colsx <- seq_len(ncol(x))
            colsy <- seq_len(ncol(y))
        }
    } else {

        colsx <- if(is.null(by.x)) {
                     if(is.null(by)) {
                         seq_len(ncol(x))
                     } else {
                         by
                     }
                 } else {
                     by.x
                 }

        colsy <- if(is.null(by.y)) {
                     if(is.null(by)) {
                         seq_len(ncol(y))
                     } else {
                         by
                     }
                 } else {
                     by.y
                 }


    }


    colsx <- processcols(x=x,cols=colsx)
    idrowsx <- processrowstoid(x=x,rows=rowsx)
    idcolsx <- colsx-1L

    colsy <- processcols(x=y,cols=colsy)
    idrowsy <- processrowstoid(x=y,rows=rowsy)
    idcolsy <- colsy-1L

    ## nth <- checknumberofnthread(nThread)

    .Call("match_matrix",x,idrowsx,idcolsx,y,idrowsy,idcolsy,as.integer(nomatch))

}
