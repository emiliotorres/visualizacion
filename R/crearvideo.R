## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-05-26 21:38 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##
## source("paquete.R")
## source("obtenerboundingbox.R")
## library(hashtable) # tictoc


##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title ordena los pixels de la imagen
##' @param img imagen
##' @return .
##' @author emilio
##' @export 
##' @examples
##' 1
ordenarpixels <- function(img){
    m <- obtenersegmentos(img)
    m <- cbind(m,ordenpixel=0L,frame=1L,oop1=0L)

    ## Ordenamos segmentos y pixels
    mboundingbox  <- obtenerboundingbox(img)
    oo <- .Call("orderBoundingBoxes",mboundingbox[,"xleft"],mboundingbox[,"ybottom"],mboundingbox[,"xright"],mboundingbox[,"ytop"])
    mboundingbox <- cbind(mboundingbox,ordersegmento=oo)
    m <- mmerge(m,mboundingbox[,c("segmento","ordersegmento")])

    dims <- dim(img)[1:2]

    for(i in unique(m[,"segmento"])){

        if(i>0){ ## Background can contain disconnected segments.
        f <- m[,"segmento"] ==i
        pos <- m[f,"pos"]
       
        cat(paste("Ordenando pixels Segmento",i, "length", length(pos), "... "))
        t1 <- tic_named(paste0("Ordenar segmento ",i, " length", length(pos)))
        oo <- .Call("get_order_pos",pos,dims[1])
        

        m[f,"oop1"] <- oo
        toc_print_timer(t1)
        }
    }

    oo <- order(m[,"ordersegmento"],m[,"oop1"])
    m <- m[oo,]
    m[,"ordenpixel"] <- 1:nrow(m)
    m
}

crearvideoparaestaslide <- function(slide, directoriodiaposlide,framerate){
    ficherovideo <- paste0("video-",slide,"-a.mp4")
    if(file.exists(ficherovideo))file.remove(ficherovideo)
    if (nzchar(Sys.which("ffmpeg")) == 0) {
        stop("ffmpeg is not installed. Try in your sistem 'sudo apt install ffmpeg'.")
    }
    instruction <- paste0("ffmpeg -f image2  -r ",framerate," -i ",directoriodiaposlide,"/data%04d.png   -vf \"pad=ceil(iw/2)*2:ceil(ih/2)*2\" -vcodec libx264 -crf 18  -pix_fmt yuv420p ",ficherovideo)
    cat(instruction,"\n")
    system( instruction    )
    unlink(directoriodiaposlide,recursive=TRUE)

   
}


crearframesyvideo <- function(img,m, slide,nseconds, writePNG, directoriodiapos = "slides",colortexto=c(898898898,890890890),colorfondo=98098098,precision=1000L,framerate=25L){
    ## Creamos los frames


    ## Información para cada frame
    ## m ya está ordenado por ordenpixel
    ## Asignamos los frames
    f <- m[,"color"]  %in%  colortexto # color del texto
    npixelstext  <-  sum(f)
    ntotalframes  <-  nseconds*framerate
    sumpixelstext  <- cumsum(f)
    textpixelsperframe = (npixelstext/(ntotalframes-1)) ## La primera debería ser negra
    f <- m[,"color"] == colorfondo
    m[f,"frame"]  <- 1L
    m[!f,"frame"]  <- 2L +  as.integer(sumpixelstext[!f]/(textpixelsperframe))



    ## Guardamos cada frame

    directoriodiaposlide <- paste0(directoriodiapos,slide)
    unlink(directoriodiaposlide,recursive=TRUE)
    dir.create(directoriodiaposlide)

    dims <- c(dim(img)[1],dim(img)[2])
    npoints <- prod(dims)
    r <- trunc(colorfondo/precision^2)
    g <- trunc((colorfondo - r*precision^2)/precision)
    b <- (colorfondo- r*precision^2 - g*precision)
    mmask <- c(rep(r/precision,npoints),
               rep(g/precision,npoints),
               rep(b/precision,npoints)
               )
    dim(mmask) <- dim(img)


    f <- m[,"color"] != colorfondo
    mfinal <- m[f,c("pos","frame"),drop=FALSE]

    ## For each frame
    maxslide <- max(mfinal[,"frame"])

    for(i in 1L:maxslide){

        cat(paste("Slide", slide, " faltan frame ", maxslide-i,"\n"))
        filename <- sprintf(paste0(directoriodiaposlide,"/data%04d.png"), i)
        ##print(filename)

        posf <- mfinal[ mfinal[,"frame"] == i, "pos"]
        posred <- posf
        posgreen <- npoints + posf
        posblue <- 2*npoints + posf
        pos <- c(posred,posgreen,posblue)
        mmask[pos]  <- img[pos]
        length(pos)

        writePNG(mmask,target=filename)

    }

    crearvideoparaestaslide(slide,directoriodiaposlide,framerate)


}

##' .. content for  (no empty lines) ..
##'
##' .. content for  ..
##' @title .
##' @param img image
##' @param slide number of image
##' @param nseconds number of seconds
##' @param writePNG png::writePNG
##' @param ... .
##' @return .
##' @author emilio
##' @export 
##' 
crearvideo <- function(img,slide,nseconds,writePNG,...){

    m <- ordenarpixels(img)

    crearframesyvideo(img,m, slide,nseconds,writePNG,...)

    return(0)

}
