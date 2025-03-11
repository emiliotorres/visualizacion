## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-31 11:46 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##
##' A tiny version of data.table[rows,FUN,by] focused in matrices.
##'
##' The results of the FUN are silently coerced to the type of the matrix (integer,double or character).
##' @title Do something FUN to each group of rows splitted by some columns in a matrix
##' @param x A matrix of integer, double or character data.
##' @param FUN A function to apply to a subset of the matrix. Should return vectors of the same type of x (integer, double or character).
##' @param ... Additional arguments to FUN.
##' @param by Split the matrix by the values of these columns.
##' @param rows A logical filter or integer vector selecting the rows of the matrix. By default it uses all the rows.
##' @return A matrix with the values of the columns used in by and the results of the FUN.
##' @author emilio
##' @export 
##' @examples
##' x <- as.matrix(iris[,-5])
##' f <- iris[["Species"]] == "virginica"
##' mdoby(x,function(x){c(n=nrow(x),min=min(x[,"Sepal.Length"]),max=max(x[,"Sepal.Length"]))}, by=c("Petal.Length"),rows=f)
mdoby <- function(x,FUN,...,by,rows=NULL) {
   ##print(paste("mdoby",ncol(x),mode(x)))
    cols <- processcols(x=x,cols=by)
    idrows <- processrowstoid(x=x,rows=rows)
    ##print((paste("mdoby a por el call")))
    listofrowsby <- .Call("get_list_of_rows_with_same_values_in_this_matrix",x,idrows,cols-1L)
    ##print((paste("mdoby a por el lapply", length(listofrowsby))))
    if(length(listofrowsby) == 0L) {
        ## No rows. I need the colnames of the FUN
        r <- FUN(x[integer(0),,drop=FALSE],...) # Fake result
        ##   print(paste0("mdoby despues de FUn"))
        if(is.integer(x)) mode(r) <- "integer"
        else if(is.double(x)) mode(r) <- "double"
        else if(is.character(x)) mode(r) <- "character"
        ## print(length(r))
       ## print(mode(x[1]))
         ## print(class(r[0]))
        nms <- names(r)
        dim(r) <- c(1L,length(r))
        colnames(r) <- nms
        ## print(str(r))
        ##print(paste("mdoby a por el mcbind en caso raro"))
        ##print(class(r))
        ##print(str(r))
        return(mcbind(x=x,rowsx=integer(0),colsx=cols, y=r,rowsy=integer(0),colsy=NULL))
    } 
    r <- lapply(listofrowsby,function(pos,...){
        FUN(x[pos,,drop=FALSE],...)
    },...)
    r <- do.call(rbind,r)
     if(is.integer(x)) mode(r) <- "integer"
        else if(is.double(x)) mode(r) <- "double"
        else if(is.character(x)) mode(r) <- "character"
       
    firstpos <- get_first_element_of_list_of_integers(listofrowsby)
    ##print(paste("mdboy"))
    ##print(str(r))
    mcbind(x=x,rowsx=firstpos,colsx=cols, y=r,rowsy=NULL,colsy=NULL)
}



