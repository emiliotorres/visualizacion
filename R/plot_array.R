## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-31 12:51 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##
##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title Plot an array as image.
##' @param image A matrix or array.
##' @param rowlimits To zoom the image.
##' @param collimits To zoom the image.
##' @param ... .
##' @return .
##' @author emilio
##' @importFrom graphics par rasterImage
##' @export 
##' @examples
##' plot_array(matrix(0:1, ncol = 50, nrow = 27))
plot_array <- function(image,rowlimits=c(1L,dim(image)[1]),collimits=c(1L,dim(image)[2]),...){
    par(mai=c(0,0,0,0),bty="n",bty = "n", xaxs = "i", yaxs = "i", xaxt = "n", yaxt = "n", col = "white") # margins
    plot(collimits, rowlimits, type = "n", xlab = "", ylab = "",...)
    rasterImage(image, 1, 1, dim(image)[2], dim(image)[1],...)
}


