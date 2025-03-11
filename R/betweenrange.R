## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-28 19:39 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

## ============================================================
## ============================================================


##' Check if a value is between a lower and upper limits.
##'
##' This function checks whether each element in a vector is between the corresponding
##' lower and upper limits. It is a vectorised function, meaning it operates element-wise
##' on vectors. Internally, it performs garbage collection.
##'
##' @title Check if a value is between a lower and upper limits.
##'
##' @param x A vector of integers or numeric.
##' @param lower A vector of integers or numeric representing the lower limits.
##' @param upper A vector of integers or numeric representing the upper limits.
##'   Its length must be the same as the length of the lower vector.
##'
##' @return A matrix with the elements of x that are between
##'   the corresponding lower and upper limits.
##'
##' @author Emilio
##'
##' @export 
##' @examples
##' x  <- 1.0*c(1:15)
##' lower <- c(0, 4, 8)
##' upper <- c(2, 6, 12)
##' betweenrange(x, lower, upper)
##' @export
betweenrange <- function(x,lower,upper){
    ## Ordering is vital to reduce the used memory.
    x <- sort(x=x)
    oo <- order(lower,upper)
    lower <- lower[oo]
    upper <- upper[oo]
    gc(verbose=FALSE) # garbage collection. We need to reserve a lot memory.
    m <- .Call("betweenrangeoo",x,lower,upper) # It is hungry of memory! It can became a length(x)*length(lower)*6 vector!
    ## Internally it removes the unused memory. But, the system is silly and does not recognise the free memory.
    ## It can be useful to call gc after a large object has been removed, as this may prompt R to return memory to the operating system.
    ## betweenrange kill my R session on my laptop due to lack of memory.
    gc(verbose=FALSE) # garbage collection
    m <- matrix(data=m,ncol=3L,byrow=TRUE)
    colnames(m) <- c("x","lower","upper")
    m
}


