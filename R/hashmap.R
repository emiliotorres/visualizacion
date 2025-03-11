## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-03-04 12:08 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##
##' Create a HashMap of keys and values
##'
##' .. content for  ..
##' @title .
##' @param size Tentative size. Not very important
##' @return  A pointer of HashMap
##' @author emilio
##' @export 
##' @examples
##' k <- list(0L, 1., "2", c(0L,0L),c(1,1), c("2","2"))
##' v <- list(10L, 11., "20", c(10L,10L),c(11,11), c("20","10"))
##' map <- create_map(2L)
##' for(key in k){
##'     for(value in v){
##'         insert_map(key,value,map)
##'     }
##' }
##' for(key in k){
##'     cat("\nKey:\n")
##'     print(key)
##'     cat("Value:\n")
##'     print(get_map(key,map))
##' }
##'  
##' size  <-  length(map)
##' for(i in 1L:size) {
##'     cat("\nPair Key Value:\n")
##'     print(get_key_by_position_map(i,map))
##'     print(get_value_by_position_map(i,map))
##'  }
##'  
##'  insert_map(0L,"CCCC",map)
##'  get_map(0L,map)
##' 
##' size <- 10 # try 10^6
##' t <- 1:size
##' pos <- as.character(t + (t-1)*size)
##' map <- create_map(size)
##' a <- tic_named("Brute force")
##' for(i in 1:length(pos)) insert_map(pos[i],pos[i], map)
##' toc_print_timer(a)
##' map2 <- create_map(size)
##'  k <- as.list(pos)
##' v <- as.list(pos)
##' b <- tic_named("Using lists. It is faster if you have the lists")
##' insert_map(k,v, map2)
##' toc_print_timer(b)
create_map <- function(size){
    .Call("create_hashmap_SEXP",as.integer(2L*size))
}

##' .. content for  (no empty lines) ..
##'
##' With list of vector is faster. Otherwise you can insert element by element
##' @title .
##' @param key (A list of) Vector of logical, integer, numeric or characters
##' @param value (A list of) Vector of logical, integer, numeric or characters
##' @param map A HashTable
##' @return An invisible 0( fails)/1(success)
##' @author emilio
##' @export 
##' @examples
##' ## see create_map
insert_map <- function(key,value,map){
     if(is.list(key) && is.list(value)) {
        invisible(.Call("insert_list_hashmap_SEXP",key,value,map))
    } else {
        invisible(.Call("insert_hashmap_SEXP",key,value,map))
    }
}
##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param key Vector of logical, integer, numeric or characters
##' @param map A HashTable
##' @return value stored with that key. NULL if not exists that key
##' @author emilio
##' @export 
##' @examples
##' ## see create_map
get_map <- function(key,map){
    .Call("get_hashmap_SEXP",key,map)
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x HashMap
##' @return .
##' @author emilio
##' @export 
##' @examples
##' ## see create_map
length.etm_HashMap <- function(x){
    .Call("length_hashmap_SEXP",x)
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param pos 1,.., length(map)
##' @param map map
##' @return the key in that position
##' @author emilio
##' @export 
##' @examples
##' ## see create_map
get_key_by_position_map <- function(pos,map){
    .Call("get_key_by_position_hashmap_SEXP",pos,map)
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param pos 1,.., length(map)
##' @param map HashMap
##' @return The value of that position
##' @author emilio
##' @export 
##' @examples
##' ## see create_map
get_value_by_position_map <- function(pos,map){
    .Call("get_value_by_position_hashmap_SEXP",pos,map)
}


##' .. content for  (no empty lines) ..
##'
##' It returns the internal represenatation of the keys and values of the hashmap.
##' Useful if you only want to deal with integers.
##' @title Convert a vector of integers, numeric, characters into a vector of integers
##' @param x A vector of logical, integers, numeric of characters
##' @return A vector with type (of x), size of integers, and n=size integers with the representation of x
##' @author emilio
##' @export 
##' @examples
##' a <- 1:2
##' k <- convert_vector_to_key(a)
##' b <- convert_key_to_vector(k)
##' identical(a,b)
##'
##' a <- 1.*c(1:2)
##' k <- convert_vector_to_key(a)
##' b <- convert_key_to_vector(k)
##' identical(a,b)
##'
##' a <- as.character(c(1:2))
##' k <- convert_vector_to_key(a)
##' b <- convert_key_to_vector(k)
##' identical(a,b)
convert_vector_to_key <- function(x){
    .Call("convert_vector_to_key_SEXP",x)
}
##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param x A vector of integers. It reprents a key. See convert_vector_to_key.
##' @return .
##' @author emilio
##' @export 
##' @examples
##' ## See convert_vector_to_key
convert_key_to_vector <- function(x){
    .Call("convert_key_to_vector_SEXP",x)
}
