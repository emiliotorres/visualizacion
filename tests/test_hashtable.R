## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-03-03 14:46 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##
##library(hashtable)
source("header.R")

test <- function(code,input,expectedooutput,...){
    if(identical(input,expectedooutput)){
        cat("Test", code,"...ok.\n")
    } else {
        stop("Test", code, "...fails!\n")
    }
}

x <- c(1:10,10:1)
ht <- create_ht(x)
test(22, class(ht),"etm_HashTable")
test(24, find_ht(c(10L,20L),x,ht,-1), c(10L,-1L))
test(25, isin(c(10L,11L),x), c(TRUE,FALSE))
test(26, length(ht), 10L)
test(27, sort(dump_ht(ht)),sort(c(1L, 10L, 6L, 9L, 7L, 4L, 3L, 8L, 2L, 5L)))


x <- c(1:10,10:1)
ht <- create_ht(x,first=FALSE)
test(28, class(ht),"etm_HashTable")
test(29, find_ht(c(10L,20L),x,ht,-1), c(11L,-1L))
test(30, isin(c(10L,11L),x), c(TRUE,FALSE))
test(31, length(ht), 10L)
test(32, sort(dump_ht(ht)),sort(c(20L, 11L, 15L, 12L, 14L, 17L, 18L, 13L, 19L, 16L)))

x  <- matrix( c(1L,2L,
                1L,2L,
                2L,1L),ncol=2L,byrow=TRUE)
v <- matrix( c(2L,1L,
               1L,2L,
               3L,7L),ncol=2L,byrow=TRUE)
ht <- create_ht(x)
test(35, find_ht(v,x,ht,-1), c(3L,1L,-1L))
test(36, length(ht), 2L)
test(37, sort(dump_ht(ht)),sort(c(3L, 1L)))
cols <- c(2L,1L)
ht <- create_ht(x,cols=cols)
test(38, find_ht(v, x,ht,-1,cols),c(1L,3L,-1L))

ht <- create_ht(x,first=FALSE)
test(39,sort(dump_ht(ht)),sort(c(3L,2L)))
test(40, find_ht(v, x,ht,-1),c(3L,2L,-1L))




x <- c(1:10,10:1)*1.0
ht <- create_ht(x)
test(42, class(ht),"etm_HashTable")
test(43, find_ht(c(10L,20L)*1.0,x,ht,-1), c(10L,-1L))
test(44, isin(c(10L,11L)*1.0,x), c(TRUE,FALSE))
test(45, length(ht), 10L)
test(46, sort(dump_ht(ht)),sort(c(6L, 5L, 2L, 9L, 4L, 1L, 8L, 7L, 10L, 3L)))

ht <- create_ht(x,first=FALSE)
test(47, find_ht(c(1:2)*1., x,ht,-1,cols),c(20L,19L))


x  <- matrix( c(1L,2L,
                1L,2L,
                2L,1L),ncol=2L,byrow=TRUE)*1.
v <- matrix( c(2L,1L,
               1L,2L,
               3L,7L),ncol=2L,byrow=TRUE)*1.
ht <- create_ht(x)
dump_ht(ht)
find_ht(v,x,ht,-1)

test(48, find_ht(v,x,ht,-1), c(3L,1L,-1L))
test(49, length(ht), 2L)
test(50, sort(dump_ht(ht)),sort(c(3L, 1L)))
cols <- c(2L,1L)
ht <- create_ht(x,cols=cols)
test(51, find_ht(v, x,ht,-1,cols),c(1L,3L,-1L))


x <- c(1:10,10:1)*1.0
x <- as.character(x)
ht <- create_ht(x)
test(52, class(ht),"etm_HashTable")
test(54, find_ht(as.character(c(10L,20L)*1.0),x,ht,-1), c(10L,-1L))
test(55, isin(as.character(c(10L,11L)*1.0),x), c(TRUE,FALSE))
test(56, length(ht), 10L)
test(57, sort(dump_ht(ht)),sort(c(10L, 6L, 4L, 2L, 5L, 7L, 3L, 9L, 1L, 8L)))

ht <- create_ht(x,first=FALSE)
test(58, sort(dump_ht(ht)),sort(c(11L, 15L, 17L, 19L, 16L, 14L, 18L, 12L, 20L, 13L)))



x  <- matrix( c(1L,2L,
                1L,2L,
                2L,1L),ncol=2L,byrow=TRUE)*1.
v <- matrix( c(2L,1L,
               1L,2L,
               3L,7L),ncol=2L,byrow=TRUE)*1.
dims <- dim(x)
x <- as.character(x)
v <- as.character(v)
dim(x) <- dims
dim(v) <- dims
ht <- create_ht(x)
test(65, find_ht(v,x,ht,-1), c(3L,1L,-1L))
test(66, length(ht), 2L)
test(67, sort(dump_ht(ht)),sort(c(3L, 1L)))
cols <- c(2L,1L)
ht <- create_ht(x,cols=cols)
test(68, find_ht(v, x,ht,-1,cols),c(1L,3L,-1L))
ht <- create_ht(x,first=FALSE)
test(69, find_ht(v, x,ht,-1),c(3L,2L,-1L))

set.seed(123)
lx <- list(1L, matrix(c(2L,3L),ncol=2),1, matrix(c(2,3),ncol=2), "1",matrix(c("2","3"),ncol=2))
ly <- list(12L, matrix(c(22L,23L),ncol=2),12, matrix(c(22,32),ncol=2), "21",matrix(c("22","23"),ncol=2))
test(70,tryCatch(
            expr={
                .Call("create_hashtable_SEXP",NULL)
            }, error=function(e) {-1L}),-1L)
count = 70
for(a in lx){ ## valgrind
    for(b in ly){
        count  <- count +1
        test(count,tryCatch(
                       expr={
                           ht <- create_ht(a)
                           find_ht(b,a,ht,-1)
                       }, error=function(e) {-1L}),-1L)
    }
}
