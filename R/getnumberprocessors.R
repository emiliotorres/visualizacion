## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-27 17:48 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##' Returns the number of processors available to the device.
##'
##' This function returns the number of processors available to the device.
##'
##' @title Returns the number of processors available to the device.
##' @return The number of processors available.
##' @author Emilio
##' @export 
##' @examples
##' getnumberprocessors()
getnumberprocessors <- function(){
    .Call("get_number_of_processors_available")
}



checknumberofnthread <- function(nThread){
    nth <- as.integer(nThread)
    maxnth <- getnumberprocessors()
    if(nth < 1L) return(maxnth)
    if(nth > maxnth) return(maxnth)
    nth
}
