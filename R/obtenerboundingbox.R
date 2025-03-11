## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-03-08 17:13 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##
##library(png)
##library(matchmatrix)
##library(data.table)
##library(hashtable)
##library(stringi)


##source("video.R")

isvalidarray <- function(image) {
    if( !is.array(image) || is.vector(image)) {
        stop("image must be an array. Its class is ",class(image),".")
    } ##&& !is.matrix(m) && !is.vector(m)  # TRU
    dimsa <- dim(image)
    if(!length(dimsa)  %in% c(2,3)){
        texto <- paste0("image must be an array of dimension 2 (matrix) or 3 (array). Its dimension is (",paste(dimsa,collapse=","),").",collapse="")
        if(inherits(image, "cimg") ) {
            texto <- paste0(texto, " The class of image is ", paste(class(image),collapse=","),".",collapse="")
            texto <- paste0(texto, " Try newimage <- image[,,1,]")
        }
        stop(texto)
    }
    if(length(dimsa)==3 && dimsa[3]!=3){
        texto <- paste0("image must be an array of dimension 2 (matrix) or 3 (array). When it is an array, the third dimension must be 3 (red, green, blue). Its dimension is (",paste(dimsa,collapse=","),").",collapse="")
        if(inherits(image, "cimg") ) {
            texto <- paste0(texto, " The class of image is ", paste(class(image),collapse=","),".",collapse="")
            texto <- paste0(texto, " Try newimage <- image[,,1,]")
        }
        stop(texto)
    }
    TRUE
}

converttograyscale <- function(image) {
    isvalidarray(image)
    if(length(dim(image))==2) {
        return(image)
    }
    if(length(dim(image))!=3){
        stop("image must be a array of dimension 3")
    }
    dims <- dim(image)[1:2]
    ##nsm <- c("r","g","b")
    ##m <- matrix(image,nrow=prod(dims),ncol=3)
    newimage <-  (image[,,1] + image[,,2] + image[,,3])/3
    dim(newimage) <- dims
    newimage
}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title Pixels y segmentos de la imagen
##' @param img imagen
##' @return .
##' @author emilio
##' @export 
##' @examples
##' 1
obtenersegmentos <- function(img){
    isvalidarray(img)
    ##dim(image)
    width <- dim(img)[1]
    height <- dim(img)[2]

    precision <- 1000L
    nms <- c("pos","irow","jcol","grey","color","segmento")
    m <- matrix(0L,nrow=width*height,ncol=length(nms))
    colnames(m) <-nms
    ## Positions as vector, as irow, and as jcol
    m[,"pos"] <- seq_len(nrow(m))
    m[,"irow"] <- postoirow(m[,"pos"], width)
    m[,"jcol"] <- postojcol(m[,"pos"], width)
    ## Color RGB as integer.
    ## Check that length(dim(image)) = 3 && dim(image)[3] = 3
    if(is.matrix(img)){
        m[,c("color")]  <-  as.integer(precision*img)
    } else {
        m[,c("color")]  <- as.integer((precision^3)*round(img[,,1],3) + (precision^2)*round(img[,,2],3) + precision*round(img[,,3],3))
    }

    tableofcolors <- table2(m[,"color"])
    colormashabitual <- tableofcolors[["values"]][1]
    ##f <- m[,"color"] !=colormashabitual ## Suponemos que es el fondo de
    ## 1001001000 blanco  1000
    f <- m[,"color"] != colormashabitual ##
    m[f,"segmento"] <- msegments(m[f,"pos"],dims=c(width,height))
    m
}


##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title Obtain the bounding boxes
##' @param img An image
##' @return A matrix with the bounding boxes.
##' @author emilio
##' @export 
##' @examples
##' 1
obtenerboundingbox <- function(img) { # matrix

    m <- obtenersegmentos(img)

    ## mmask <- rep(1L,prod(dim(image)))
    ## dim(mmask) <- dim(image)
    ## mmask[f] <- 0
    ## plot_array(mmask)

    ##print(head(m))
    ## ¿Cuántos segmentos hay?
    ## Si solo hay 1,
    ## Para cada segmento de cada color, creamos
    ## - bounding box: xleft=, ybottom=, xright=, ytop=
    ## - área del boundingbox
    ## - pixels de ese segmento
    mboundingbox <-  mdoby(m,
                           function(x){
                               tableofcolors <- table2(x[,"color"])
                               colormashabitual <- tableofcolors[["values"]][1]
                               c(pixels=nrow(x),color=colormashabitual,
                                 xleft=min(x[,"jcol"]),ybottom=max(x[,"irow"]),xright=max(x[,"jcol"]),ytop=min(x[,"irow"]))},
                           by=c("segmento"),rows=NULL)

    mboundingbox

}

rescaleimage <- function(img,nrow,ncol){
    isvalidarray(img)
    if(is.matrix(img)) {
        return(mrescale(img,nrow,ncol))
    } else {
        newimag <- array(NA,c(nrow,ncol,3))
        newimag[,,1] <- mrescale(img[,,1],nrow,ncol)
        newimag[,,2] <- mrescale(img[,,2],nrow,ncol)
        newimag[,,3] <- mrescale(img[,,3],nrow,ncol)
        return(newimag)
    }
}













crearmascara <- function(img, pos, color){
    npixels <- prod(dim(img)[1:2])
    if(max(pos) > npixels) {
        stop("POS must be less than nrow x ncol of img.")
    }
    img[pos]  <- color[1]
    if( length(dim(img)) == 3){
        img[pos + npixels] <- color[2]
        img[pos + 2*npixels ] <- color[3]
    }
    img
}


