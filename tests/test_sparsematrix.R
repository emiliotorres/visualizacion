## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-03-04 00:09 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##
##library(hashtable)
source("header.R")
## source("paquete.R")
## ##source("hashmap.R")
## source("sparsematrix.R")

test <- function(code,input,expectedooutput,...){
    if(identical(input,expectedooutput)){
        cat("Test", code,"...ok.\n")
    } else {
        stop("Test", code, "...fails!\n")
    }
}

t <- 1:10
sm <- create_sm(10,10)
sm
test(101,class(sm),"etm_SparseMatrix")
test(102,insert_sm(t,t,t,sm),10L)
test(103,get_sm(5,5,sm),5L)
test(104,insert_sm(1,3,3,sm),11L)
test(105,insert_sm(1,3,7,sm),11L)
test(106,insert_sm(c(1,1),c(1,1),c(3,5),sm),11L)



test(107,length(sm),11L)
test(108,get_sm(1,1,sm),5L)

test(109,sum(dump_sm(sm)),
         sum(structure(c(2L, 4L, 1L, 7L, 6L, 3L, 10L, 8L, 9L, 5L, 1L, 2L, 
4L, 3L, 7L, 6L, 3L, 10L, 8L, 9L, 5L, 1L, 2L, 4L, 7L, 7L, 6L, 
3L, 10L, 8L, 9L, 5L, 5L), dim = c(11L, 3L), dimnames = list(NULL, 
    c("i", "j", "x")))))


     


test(111,tryCatch(
            expr={
                insert_sm(1,1,1,a)
            }, error=function(e) {-1L}),-1L)
test(112,tryCatch(
            expr={
                get_sm(1,1,a)
            }, error=function(e) {-1L}),-1L)

test(120,dim(sm),c(10L,10L))

dim(sm)
