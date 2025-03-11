## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2023-12-31 13:45 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##
##image <- png::readPNG(file)
##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param image .
##' @param backgroundcolor .
##' @return .
##' @author emilio
##' @export 
##' @examples
##' 1
changepngtocormorantstyle  <- function(image,backgroundcolor=98L){

    if(!is.array(image)) {
        message <- "IMAGE should be an array matrix, as an object obtained with png::readPNG"
        message <- paste(message, "The class of this object is ",class(image),collapse=" ")
        stop(message)
    }
    if(!length(dim(image))  %in%  c(2,3)){
        stop("La imagen de ", file," no tiene dimensiXn 2 o 3, sino ",length(dim(image)))
    }
    if(length(dim(image) )== 3 ){
        if(!dim(image)[3]  %in% c(3,4)){
            stop("La tercera dimensiXn de ", file," no es 3 ni 4, sino ",dim(image)[3])

        }
        image <- image[,,1:3]
        bw <- as.integer(1000*(image[,,1] + image[,,2] + image[,,3]) /3)
    } else {
        bw <- image
    }
    ## Guardamos las dimensiones.
    dims <- dim(image)[1:2]

    ## Guardamos las dimensiones.
    dims <- dim(image)[1:2]
    ## Convert to black and white

    ## Buscamos el color de fondo, que es el que mas aparece
    tt <-  rle(sort(bw)) # tabla de frecuencias
    backcolimage <- tt[["values"]][which.max(tt[["lengths"]])][1]
    f <- bw == backcolimage
    image <- 1 - image
    image[f]  <- backgroundcolor/1000
    image
}



##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param infilepdf .
##' @param outfilepng .
##' @param width .
##' @param height .
##' @return .
##' @author emilio
##' @export 
##' @examples
##' ## unlink("slides",recursive=TRUE)
##' ## dir.create("slides")
##' ## infilepdf <- "164_presentacion.pdf"
##' ## convertfilepdftopng(infilepdf,outfilepng="slides/slide", width = 1920, height = 1080)
convertfilepdftopng <- function(infilepdf,outfilepng="slides/slide", width = 1920, height = 1080){
    ## width= 380* 2480/210
    ## height=263* 3508/297
    if (!nzchar(Sys.which("pdftoppm"))) stop("'pdftoppm' not installed: sudo apt install poppler-utils")
    comando <- paste0("pdftoppm -png -scale-to-x ", as.integer(width)," -scale-to-y ", as.integer(height), "  '", infilepdf,"'  ", outfilepng)
    system(comando)
    invisible()
}
