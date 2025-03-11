## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-28 06:14 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##' A fast alternative for 'unique' with matrices.
##'
##' This function provides a fast alternative for 'unique' when dealing with matrices.
##'
##' @title A fast alternative for 'unique' with matrices.
##' @param x The input matrix.
##' @param cols Columns to consider for uniqueness.
##' @param rows A logical or integer vector. If logical, its length must be nrow(x).
##' @return A matrix with no duplicated rows.
##' @author emilio
##' @export 
##' @examples
##' x <- matrix(rep(1:10,4),ncol=2)
##' all.equal(munique(x),unique(x))
munique <- function(x,cols=NULL,rows=NULL){
    cols <- processcols(x=x,cols=cols)
    splitofm <- msplit(x=x,by=cols,rows=rows)
    firstpos <- get_first_element_of_list_of_integers(listofintegers=splitofm)
    x[firstpos,cols,drop=FALSE]
}

