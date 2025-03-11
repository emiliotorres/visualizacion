## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-31 17:27 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##' Transform specified columns into an integer vector.
##'
##' This function transforms specified columns of an object into an integer vector.
##' If all the values are negative, then it selects the other columns.
##' If a name does not appear in the names of the columns, it is removed.
##' 
##'
##' @title Transform into an integer vector the specified columns.
##' @param x An object with columns.
##' @param cols A character or an integer vector specifying the names or positions of the columns.
##' @return An integer vector with the position of that column in x.
##' @author emilio
##' @export 
##' @examples
##' processcols(iris,c("Species")) # 5
##' processcols(iris,-c(2:4))
processcols <- function(x,cols=NULL){
    if(is.null(cols)) {
        if(ncol(x)>0) cols <- seq_len(ncol(x))
        else cols <- integer(0) # empty vector
        return(cols)
    }
    if(is.character(cols)){
        temp <- cols
        cols <- match(cols,colnames(x))
        if(anyNA(cols)) {
            stop("Cols is a character vector with values different to columnnames(x):",paste(temp[is.na(cols)],collapse=", "))
        }
        cols <- cols[!is.na(cols)]
    } else if(is.numeric(cols)){
        cols <- as.integer(cols)
        cols <- cols[!is.na(cols)]
    } else stop("Cols is not a valid colnames of x (it must be a character or integer vector).")
    if(any(pos <- (abs(cols) < 1L | abs(cols)>ncol(x)))){
        stop(paste("Cols is not a valid vector: it has values <1L or >ncol(x)=",ncol(x),": ",paste(cols[pos],collapse=",")), ". If you want to pass a vector of length 0, use 'cols=integer(0)'.")
    }
    if( any(cols < 0L) && any(cols>0L)) {
        stop("Cols contains positive and negative numbers. All numbers must be either >0L or <0L. Negative numbers imply using the other columns.")
    }
    if(all(cols < 0L)) {
        cols <- seq_len(ncol(x))[cols]
    }
    if(length(cols)>ncol(x)) {
        stop("Cols has a length of ", length(cols), ", when the number of columns is ",ncol(x),".")
    }
    if(anyDuplicated(cols)) {
        stop("Duplicated numbers in Cols: ",paste(cols[duplicated(cols)],collapse=","))
    }
    cols
}




processrowstoid <- function(x,rows=NULL){
    ## Accept rows=Null, logical or integer vectors.
    .Call("convert_vector_Rrows_to_Crows",x,rows)
    }
