## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-31 15:34 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##

##' .. content for  (no empty lines) ..
##'
##' ..
##' @title Univariate table
##' @param x An atomic vector
##' @return A list with the frequencies (lenths) and the values, ordered by frequency.
##' @author emilio
##' @export
##' @examples
##' table2(c(1,2,2,2,3,3))
table2 <- function(x){
    res <- rle(sort(x))
    oo <- order(res$lengths,decreasing = TRUE)
    res$lengths <- res$lengths[oo]
    res$values <- res$values[oo]
    res
}
