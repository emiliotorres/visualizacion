## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-03-04 12:10 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##
##source("paquete.R")

source("header.R")
##library(hashtable)


test <- function(code,input,expectedooutput,...){
    if(identical(input,expectedooutput)){
        cat("Test", code,"...ok.\n")
    } else {
        stop("Test", code, "...fails!\n")
    }
}

k <- list(0L, 1., "2", c(0L,0L),c(1,1), c("2","2"))
v <- list(10L, 11., "20", c(10L,10L),c(11,11), c("20","10"))
map <- create_map(2L)
for(key in k){
    for(value in v){
        insert_map(key,value,map)
    }
}
ntest <- 300L
for(key in k){
    ntest <- ntest+1L
    test(ntest,get_map(key,map),c("20","10"))
}

size  <-  length(map)
ntest <- ntest+1L
test(ntest,length(map),6L)

for(i in 1L:size) {
    ntest <- ntest+1L
    test(ntest, get_value_by_position_map(i,map),c("20","10"))

}

insert_map(0L,"CCCC",map)
ntest <- ntest+1L
test(ntest, get_map(0L,map),c("CCCC"))

ntest <- ntest+1L
test(ntest,get_map(34,map),NULL)


ntest <- ntest+1L
size <- 10L
t <- 1:size
p <- as.character(t)
k <- as.list(p)
v <- as.list(p)
map <- create_map(0)
insert_map(k,v,map)

test(ntest,get_map(k[[3]],map),v[[3]])

ntest <- 400
a <- 1:2
k <- convert_vector_to_key(a)
b <- convert_key_to_vector(k)
test(ntest,b,a)

ntest <- ntest+1L
a <- 1*c(1:2)
k <- convert_vector_to_key(a)
b <- convert_key_to_vector(k)
test(ntest,b,a)

ntest <- ntest+1L
a <- as.character(c(1,2))
k <- convert_vector_to_key(a)
b <- convert_key_to_vector(k)
test(ntest,b,a)
