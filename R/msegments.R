## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-31 11:48 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x points of a matrix
##' @param dims dimension of the matrix
##' @return .
##' @author emilio
##' @export 
##' @examples
##' x <- as.integer(c(2,3,5,6,7,9))
##' msegments(x,dims=c(10L,1L))
msegments <-  function(x,dims=dim(x)){
    dims <- as.integer(dims)
    .Call("get_segments_of_points_matrix",x,dims)
}


