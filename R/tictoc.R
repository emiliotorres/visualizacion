## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-02-28 21:23 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##
##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @return .
##' @author emilio
##' @export 
tic <- function(){
    invisible(.Call("tic_SEXP"))
}
##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @return .
##' @author emilio
##' @export 
toc <- function(){
    .Call("toc_SEXP")
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x A character
##' @return a integer number
##' @author emilio
##' @export 
##' @examples
##' a <- tic_named("Hello")
##' toc_print_timer(a)
tic_named <- function(x){
    .Call("tic_named_SEXP",as.character(x))
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x The integer returned by  tic_timer
##' @return .
##' @author emilio
##' @export 
##' @examples
##' a <- tic_named("HH")
##' toc_timer(a)
toc_timer <- function(x){
    .Call("toc_timer_SEXP",as.integer(x))
}
##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x The integer returned by  tic_timer
##' @return .
##' @author emilio
##' @export 
##' @examples
##' a <- tic_named("bb")
##' toc_print_timer(a)
toc_print_timer <- function(x){
    invisible(.Call("toc_print_timer_SEXP",as.integer(x)))
}
