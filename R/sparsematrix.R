## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-03-03 18:40 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##

                                        # R functions for the SparseMatrix C wrappers
##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title Create a sparse matrix using hash tables
##' @param nrows nrows
##' @param ncols ncols
##' @return .
##' @author emilio
##' @export
##' @examples
##' sm <- create_sm(10,10)
create_sm <- function(nrows, ncols) {
    if(nrows<1L) stop("nrows must be at leas 1, not ",nrows)
    if(ncols<1L) stop("ncols must be at leas 1, not ",ncols)

    sm <- .Call("create_sparsematrix_SEXP", as.integer(nrows), as.integer(ncols))
    
    sm
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param rows rows
##' @param cols cols
##' @param values values Only Integer/logical
##' @param sm sparse matrix
##' @return (Invisible) Number of cells with information (length)
##' @author emilio
##' @export
##' @examples
##' sm <- create_sm(10,10)
##' insert_sm(c(1,2),c(3,4),c(-1,-10),sm)
insert_sm <- function( rows, cols, values, sm) {
    invisible(.Call("insert_elements_sparsematrix_SEXP", as.integer(rows), as.integer(cols), as.integer(values), sm))
}


##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param rows rows
##' @param cols cols
##' @param sm sparse matrix
##' @param zerovalue If the cells are empty, return this value.
##' @return If not empty, return the value of teh cells
##' @author emilio
##' @export
##' @examples
##' sm <- create_sm(10,10)
##' insert_sm(1,1,7,sm)
##' get_sm(1,1,sm)
##' get_sm(2,2,sm)
get_sm <- function(rows, cols, sm, zerovalue = 0L) {
    .Call("get_elements_sparsematrix_SEXP", as.integer(rows), as.integer(cols), sm, as.integer(zerovalue))
}




##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title Return all the values
##' @param sm sm
##' @return A matrix (i,j,values)
##' @author emilio
##' @export
##' @examples
##' sm <- create_sm(10,10)
##' insert_sm(1:10,1:10,10:1,sm)
##' dump_sm(sm)
dump_sm  <- function(sm){

    .Call("dump_long_format_sparsematrix",sm)
}



##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param sm sparse matrix
##' @param n n rows to print. 0 to print all.
##' @param verbose verbose
##' @return .
##' @author emilio
##' @export
print_sm <-  function(sm,n=6L,verbose=TRUE){
    x <- dump_sm(sm)
    if(n<0) n=0
    if(verbose || n>0) {
        dims <- sm[[1]]
        text <- paste0("Class '",class(sm),"' with dimension ",dims[1],"x", dims[2]," and ", length(sm[[2]]), " elements.")
        if(n>0) {
            n <- min(n,nrow(x))
            text <- paste0(text," Showing the first ",n," elements.\n")

        } else{
            text <- paste0(text,"\n")
            n  <- nrow(x)
        }
        cat(text)
    }
    if(n>0) {
        x[1L:n,,drop=FALSE]
    } else {
        x[0,,drop=FALSE]
    }
}


##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x sm
##' @return dimension of sm
##' @author emilio
##' @export 
##' @examples
##' sm <-  create_sm(10,7)
##' dim(sm)
dim.etm_SparseMatrix <- function(x){
    .Call("dim_sparsematrix_SEXP",x)
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x sm
##' @param ... dots
##' @return .
##' @author emilio
##' @export 
print.etm_SparseMatrix <- function(x,...){

    myprint <- function(x,n=6L,verbose=TRUE){
        sm <- x
    x <- dump_sm(sm)
    if(n<0) n=0
    if(verbose || n>0) {
        dims <- dim(x)
        text <- paste0("Class '",class(sm),"' with dimension ",dims[1],"x", dims[2]," and ", length(sm), " elements.")
        if(n>0) {
            n <- min(n,nrow(x))
            text <- paste0(text," Showing the first ",n," elements.\n")
            
        } else{
            text <- paste0(text,"\n")
            n  <- nrow(x)
        }
        cat(text)
    }
    if(n>0) {
        x[1L:n,,drop=FALSE]
    } else {
        x[0,,drop=FALSE]
    }
    }
    myprint(x,...)
    
   
}


##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x x
##' @return number of cells with information.
##' @author emilio
##' @export 
length.etm_SparseMatrix <- function(x){
    .Call("size_sparsematrix_SEXP",x)
}
