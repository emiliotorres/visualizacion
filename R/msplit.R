## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-31 11:49 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 


##' Get a list of rows for each group defined by columns.
##'
##' This function divides a matrix into groups defined by the specified columns.
##'
##' @title Divide a matrix into groups defined by the columns 'cols'.
##' @param x A matrix.
##' @param by A character or integer vector specifying the columns for grouping. NULL means all the columns.
##' @param rows An integer or logical vector indicating the rows to include. NULL means all the rows.
##' @return A list of integers. Each element of this list is a vector with the positions of the rows of each group.
##' @author emilio
##' @export 
##' @examples
##' x <- as.matrix(iris)
##' a <- msplit(x,by="Species")
##' str(a)
msplit  <- function(x,by=NULL,rows=NULL) {
    cols <- processcols(x=x,cols=by)
    idrows <- processrowstoid(x=x,rows=rows)
    .Call("get_list_of_rows_with_same_values_in_this_matrix",x,idrows,cols-1L)
}

##' Get the first element of a list of integers.
##'
##' 
##' @title Get the first element of a list of vector of integers.
##' @param listofintegers A list of vector of integers
##' @return A vector of integers.
##' @author emilio
##' @export 
##' @examples
##' x <- list(c(1L,2L),c(3L,4L,5L,100L,200L),c(6L,7L))
##' get_first_element_of_list_of_integers(x)
get_first_element_of_list_of_integers <- function(listofintegers){
    ##nth <- checknumberofnthread(nThread)
    .Call("get_first_element_of_list_of_integers",listofintegers)
}


## Local Variables:
## ispell-local-dictionary: "british" 
## End:
