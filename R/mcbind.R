## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-03-07 20:02 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session
##' Combine two matrices.
##'
##' This function combine (cbind) two matrices x and y. 
##' It extracts the specified rows and columns from each matrix and then combines them.
##' When there are few rows, cbind is faster, but when there are more than 5,000 rows,
##' this method used the threads of the computer.
##'
##' @title Combine two matrices.
##' @param x A matrix.
##' @param rowsx Rows to extract from matrix x. Can be either an integer vector or a logical vector.
##' @param colsx Columns to extract from matrix x.
##' @param y A matrix.
##' @param rowsy Rows to extract from matrix y. Must be of the same length as rowsx.
##' @param colsy Columns to extract from matrix y. Can be either an integer vector or a character vector.
##' @param suffixes Suffixes to append to column names when they are duplicated.
##' @return A matrix with the subset matrix of x and the subset matrix of y.
##' @author emilio
##' @export
##' @examples
##' x <- as.matrix(iris[,1:2])
##' y <- as.matrix(iris[,3:4])
##' rowsx <- 1:50
##' colsx <- 2
##' rowsy <- 101:150
##' colsy <- 1
##' res1 <- cbind(x[rowsx,colsx,drop=FALSE],y[rowsy,colsy,drop=FALSE])
##' res2 <- mcbind(x,rowsx,colsx,y,rowsy,colsy)
##' all.equal(res1,res2)
mcbind <- function(x,rowsx,colsx,y,rowsy,colsy,suffixes=c(".x",".y")){
    
    colsx <- processcols(x=x,cols=colsx)
    idrowsx <- processrowstoid(x=x,rows=rowsx)
    colsy <- processcols(x=y,cols=colsy)
    idrowsy <- processrowstoid(x=y,rows=rowsy)
        
    m <- .Call("cbind_matrix",x,idrowsx,colsx-1L,y,idrowsy,colsy-1L)

   ## print("mcbind m")
##print(str(m))
    
    ## Duplicated colnames. From merge.data.frame
    nm <- nmx <- colnames(x)[colsx]
    nmy <- colnames(y)[colsy]
    has.common.nms <- any(cnm <- nmx %in% nmy)
    if (has.common.nms) {
        nmx[cnm] <- paste0(nmx[cnm], suffixes[1L])
        cnm <- nmy %in% nm
        nmy[cnm] <- paste0(nmy[cnm], suffixes[2L])
        colnames(m) <- c(nmx,nmy)
    }

     ##print("mcbind 2 m")
    ##print(str(m))
   
    m

}


##' Extract a submatrix using threads.
##'
##' This function uses threads to efficiently extract a submatrix from the input matrix x.
##'
##' @title Extract a submatrix.
##' @param x The input matrix.
##' @param rows Rows to extract. Can be either an integer vector or a logical vector.
##' @param cols Columns to extract. Can be either an integer vector or a character vector.
##' @return The extracted submatrix.
##' @author emilio
##' @export
##' @examples
##' x <- as.matrix(iris)
##' res1 <- x[1:10,3:4,drop=FALSE]
##' res2 <- msubset(x,1:10,3:4)
##' all.equal(res1,res2)
msubset <- function(x,rows=NULL,cols=NULL){
    if(is.null(rows)) rows <- seq_len(nrow(x))
    if(is.null(cols)) cols <- seq_len(ncol(x))
    mcbind(x=x,rowsx=rows,colsx=cols,y=x,rowsy=rows,colsy=integer(0))
}
