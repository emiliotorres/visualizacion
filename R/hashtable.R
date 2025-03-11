## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-03-03 14:47 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##
##' .. content for  (no empty lines) ..
##'
##' In case of a matrix, it indexes each row.
##' @title Create a hash set of a vector or a matrix.
##' @param x A vector or a matrix of integers, numeric or characters
##' @param first If you want to search for the first ocurrence (TRUE) or the last occurrence (FALSE)
##' @param cols It x is a matrix, you can select the columns to process
##' @return A hash table.
##' @author emilio
##' @export 
##' @examples
##' ht <- create_ht(1:10)
create_ht <- function(x,first=TRUE,cols=NULL){

    if(is.vector(x) && is.atomic(x)){ ## No lists
        if(is.integer(x)) {
            return(.Call("create_hashtable_SEXP",x,as.logical(first)));
        } else {
            rows <- 0L
            if(length(x)>0){rows <- 1L:length(x)}
            return(.Call("create_matrix_general_hashtable_SEXP",x,rows,1L,as.logical(first)))
        }
    } else if(is.matrix(x)){
        cols <- processcols(x,cols)
        if(is.integer(x) && length(cols) == ncol(x) && all(cols == 1L:ncol(x)) && first){
            return(.Call("create_matrix_hashtable_SEXP",x))
        } else{
             rows <- 0L
            if(length(x)>0){rows <- 1L:nrow(x)}
            return(.Call("create_matrix_general_hashtable_SEXP",x,rows,cols,as.logical(first)))
        }
    } else {
        stop("Not supported.")
    }

}

##' It works as match.
##'
##' .. content for  ..
##' @title Get the first position of value in x.
##' @param value Is value in x? Return the position of value in x.
##' @param x A vector or a matrix of integers, numeric or characters
##' @param ht the hash table created with x and cols
##' @param notfound An integer value
##' @param cols It x is a matrix, you can select the columns to process
##' @return A vector of integers with the position of value in x.
##' @author emilio
##' @export 
##' @examples
##' x <- 1:10
##' ht <- create_ht(x)
##' find_ht(c(1L,30L),x,ht)
find_ht <- function(value,x,ht,notfound=NA_integer_,cols=NULL){
    if(is.vector(x) && is.atomic(x) && is.vector(value) && is.atomic(value)){ ## No lists
        if(is.integer(x) && is.integer(value)) {
            return(.Call("find_position_hashtable_SEXP",value,x,ht,as.integer(notfound)));
        } else {
            rowsx <- 1L:length(x)
            dim(x) <- c(length(x),1L)
            nrowv  <- length(value)
            dim(value) <- c(nrowv,1L)            
            return(.Call("find_position_matrix_general_hashtable_SEXP",value,x,rowsx,1L,ht,as.integer(notfound)))
        }
    } else if(is.matrix(x)){
        cols <- processcols(x,cols)
        if(is.integer(x) && length(cols) == ncol(x) && all(cols == 1L:ncol(x))){
                       
            return(.Call("find_position_matrix_hashtable_SEXP",value,x,ht,as.integer(notfound)));
        } else{
            rows <- 1L:nrow(x)
           
            return(.Call("find_position_matrix_general_hashtable_SEXP",value,x,rows,cols,ht,as.integer(notfound)))
        }
    } else {
        stop("Not supported in this R function (find_ht).")
    }
}

get_positions_ht <- function(ht){
    .Call("get_positions_hashtable_SEXP",ht)
}



##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title Returns the indexes stored in the hash table.
##' @param x A hash table
##' @return a vector of integers.
##' @author emilio
##' @export 
##' @examples
##' x <- c(1:10,10:1)
##' ht <- create_ht(x)
##' dump_ht(ht)
dump_ht <- function(x){
    .Call("get_positions_hashtable_SEXP",x)
}

size_ht <- function(x){
    .Call("get_size_hashtable_SEXP",x)
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x x
##' @return .
##' @author emilio
##' @export 
##' @examples
##' ht <- create_ht(1:10)
##' length(ht)
length.etm_HashTable <- function(x){
    size_ht(x)
}


##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x x
##' @param ... dots
##' @return .
##' @author emilio
##' @export 
##' @examples
##' ht <- create_ht(1:10)
##' print(ht)
print.etm_HashTable <- function(x,...){
    if(1L){
        cat(paste0("Class '",class(x),"' with ",size_ht(x)," elements.\n"))
        }
     dump_ht(x)   
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title Similar to in 
##' @param x x
##' @param table table
##' @param cols cols
##' @return .
##' @author emilio
##' @export 
##' @examples
##' isin(c(1,20),c(1,10))
isin <- function(x,table,cols=NULL){
    ht <- create_ht(table,first=TRUE,cols=cols)
    !is.na(find_ht(x,table,ht,NA_integer_,cols))
}

