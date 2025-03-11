## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-30 02:35 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session


##' Merge two matrices.
##'
##' This function merges two matrices, x and y, based on specified columns and rows.
##' All rows of x are selected. If there are duplicated values on y, it only takes the first occurrence.
##' If there is no match, the row is filled with NA.
##'
##' @title Merge two matrices.
##' @param x The first matrix.
##' @param y The second matrix.
##' @param by Columns used for merging.
##' @param by.x Columns from matrix x used for merging.
##' @param by.y Columns from matrix y used for merging.
##' @param rowsx Rows from matrix x to be included in the result.
##'   Can be either an integer vector or a logical vector.
##' @param rowsy Rows from matrix y to be included in the result.
##'   Can be either an integer vector or a logical vector.
##' @param suffixes Suffixes to append to column names when they are duplicated.
##' @return A merged matrix with all the rowsx of x.
##' @author emilio
##' @export
##' @examples
##' x  <- matrix(sample(1:10,1e3,TRUE),ncol=2)
##' y  <- matrix(1:10,ncol=5)
##' colnames(x)  <- c("v1","v2")
##' colnames(y) <- c("v1","v2","v3","v4","v5")
##' res <- mmerge(x,y)
##' str(res)
mmerge <- function(x,y,by=NULL, by.x=by, by.y=by,rowsx=NULL,rowsy=NULL,suffixes=c(".x",".y")){

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
    idcolsx <- colsx -1L

    colsy <- processcols(y,colsy)
    idrowsy <- processrowstoid(y,rowsy)
    idcolsy <- colsy-1L



    res <- .Call("match_matrix",x,idrowsx,idcolsx,y,idrowsy,idcolsy,NA_integer_)
    res <- idrowsy[res]+1L

    newcolsx <- c(colsx, processcols(x=x,cols=-colsx))
    newcolsy <- processcols(x=y,cols=-colsy)


    if(anyNA(res)) { ## Missing values in res.
        ## mcbind does not work with positions NA of rows.
        ## Create the second matrix Y with the new results,
        ## and then join with cbind due to nThread (parallelisation)
        y <- y[res, newcolsy, drop=FALSE] ## Do it in R::base.
        res <- seq_len(nrow(y))
        newcolsy <- seq_len(ncol(y))
    }

    mcbind(x=x, rowsx=idrowsx+1L, colsx=newcolsx, y=y, rowsy=res, colsy=newcolsy, suffixes=suffixes)

}
