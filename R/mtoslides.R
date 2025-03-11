## #+TITLE:
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2024-01-13 12:30 emilio on emilio-XPS-15-9570>
## #+TAGS:
## #+PROPERTY: header-args :results output :exports both :session

##
##library(png)
##library(parallel)



##' Slides must be in the subdirectory 'slides', and must be of the form 'slide-03.png'.
##'
##' We need the external program 'ffmpeg'.
##' @title Convert a slide of beamer cormorant style to a video, writting each symbol in a frame. 
##' @param slide number of the slide to convert to video.
##' @param nseconds Duration of the video
##' @param readPNG Use this function png::readPNG
##' @param writePNG Use this function png::writePNG
##' @param npixelslettermin minimum number of pixels for letters
##' @param directoriodiapo directory of diapo
##' @return Save a video in the hard disk.
##' @author emilio
##' @export 
##' @examples
##' 1
generatevideoslide <- function(slide, nseconds,readPNG,writePNG,npixelslettermin=15L,directoriodiapo ="slides"){
    ##    require(png)
    
    precision = 1000
    colorsbackground = matrix(c(98L, 98L, 98L),ncol=3L)
    colorsletters = matrix(c(898L, 898L, 898L),ncol=3L)
    colnames(colorsbackground)  <- colnames(colorsletters) <- c("color_red", "color_green", "color_blue")

    nslide <- sprintf("%02d",slide)
    file <- file.path(directoriodiapo,paste0("slide-",nslide,".png"))
    image <- readPNG(file)
    dims <- dim(image)[1:2]

    framerate <- 30L



    m <- processcolorsimage(image,colorsletters,colorsbackground,precision,npixelslettermin)

    m <- setslides(m,nseconds,framerate)
    
    writeslide(image, nslide, colorsbackground, m, directoriodiapo,writePNG,precision)

    if(TRUE){
        directoriodiaponslide <- paste0(directoriodiapo,nslide)
        ficherovideo <- paste0("video-",nslide,"-a.mp4")
        if(file.exists(ficherovideo))file.remove(ficherovideo)
        if (nzchar(Sys.which("ffmpeg")) == 0) {
            stop("ffmpeg is not installed. Try in your sistem 'sudo apt install ffmpeg'.")
            }
        system(paste0("ffmpeg -f image2 -r ",framerate," -i ",directoriodiaponslide,"/data%04d.png -vcodec libx264 -crf 18  -pix_fmt yuv420p ",ficherovideo))
        unlink(directoriodiaponslide,recursive=TRUE)

        ## Creamos un video fake de 5 second.
        ficherovideo <- paste0("video-",nslide,"-b.mp4")
        if(file.exists(ficherovideo))file.remove(ficherovideo)
        system(paste0("ffmpeg -loop 1 -i '",file,"' -c:v libx264 -t 5 -pix_fmt yuv420p ", ficherovideo))
        

    }
    cat("Fin slide ",nslide,"\n")

}



processcolorsimage <- function(image,colorsletters,colorsbackground,precision,npixelslettermin){
    dims <- dim(image)[1:2]
    ##
    nms <- c("pos","irow","jcol","color_red","color_green","color_blue","isnotbackground","islettercolor","islettercolor","seglettercolor","isletter","segbw","seglettercolor","segletter","linebw","lineletters")
    m <- matrix(0L,nrow=prod(dims),ncol=length(nms))
    colnames(m) <-nms
    ## Positions as vector, as irow, and as jcol
    m[,"pos"] <- seq_len(prod(dims))
    m[,"irow"] <- postoirow(m[,"pos"], dims[1])
    m[,"jcol"] <- postojcol(m[,"pos"], dims[1])
    ## Color RGB as integer.
    ## Check that length(dim(image)) = 3 && dim(image)[3] = 3
    m[,c("color_red","color_green","color_blue")]  <- as.integer(precision*image)

    ## background
    f <- (m[,"color_red"] == colorsbackground[,"color_red"]) & (m[,"color_green"] == colorsbackground[,"color_green"]) &  (m[,"color_blue"] == colorsbackground[,"color_blue"]) # background
    m[!f,"isnotbackground"] <- 1L

    ## Segments of tiny information
    f <- m[,"isnotbackground"] ==1L
    m[f,"segbw"]  <- msegments(m[f,"pos"],dims)

    ## letter
    f <- (m[,"color_red"] == colorsletters[,"color_red"]) & (m[,"color_green"] == colorsletters[,"color_green"]) & (m[,"color_blue"] == colorsletters[,"color_blue"])
    m[f,"islettercolor"] <- 1L
    m[f,"seglettercolor"]  <- msegments(m[f,"pos"],dims)
    ## But if there are images, this colour may appear in the graphics.
    ## We remove the segments of this colour with few points: we want segments with
    ## at least 5 pixels.
    npixelsforsegment <- mdoby(m, function(x)c(n=nrow(x)), by=c("seglettercolor"),rows=f)
    seglettersreallyletters <- npixelsforsegment[npixelsforsegment[,"n"]>npixelslettermin, "seglettercolor"]
    m[ m[,"seglettercolor"]  %in% seglettersreallyletters,"isletter"] <- 1L
    ## And now we get the segments of these letters
    f <- m[,"isletter"] == 1L
    pos <- m[f,"pos"]
    m[f,"segletter"]  <- msegments(pos,dims)

## TODO. check out if there are two segletter in the same segbw. Then send a warning suggesting increasing npixelslettermin.
    
    ## Lines of bw.
    temp <- m[,"isnotbackground"]
    dim(temp) <- dims
    x <- as.integer(rowSums(temp))
    f <- x>0L
    pos <- seq_along(x)[f]
    linebw <- msegments(pos,c(dims[1],1L))
    ## Están ordenados según jcol, por lo tanto esto es más rápido
    ## Es decir (irow=1,2, ,1080,1,2,..,1080)
    m[f,"linebw"] <- linebw


    temp <- m[,"isletter"]
    dim(temp) <- dims
    x <- as.integer(rowSums(temp))
    linebw <- msegments(seq_along(x)[x>0L],c(dims[1],1L))
    ## Están ordenados según jcol, por lo tanto esto es más rápido
    ## Es decir (irow=1,2, ,1080,1,2,..,1080)
    m[x>0L,"lineletters"] <- linebw

    m
}


setslides <- function(m,nseconds,framerate){
    ## For letters, order the segment by lineletters and jcol
    f <- (m[,"isletter"] == 1L)
    cols <- c("segbw","segletter")
    res <- mdoby(m,
                 function(x){
                     c(irowmin=min(x[,"irow"]),jcolmin=min(x[,"jcol"]),irowmax=max(x[,"irow"]),jcolmax=max(x[,"jcol"]),lineletters=min(x[,"lineletters"]))
                 },
                 by=cols, rows= f)
    oo <- order(res[,"lineletters"],res[,"jcolmin"])
    msegmentsletters <- res[oo,]

    ## We order the slides by the number of letters.
    ## This is the  most important order: Fix the order of the slides.
    nsegments  <- nrow(msegmentsletters) ## How many letters are?
    if(nsegments>0){
        slide <- as.integer((framerate*nseconds-1)*c(1:(nsegments))/(nsegments+1))+2L
        msegmentsletters <- cbind(msegmentsletters,slide=slide) # The slide where this letter appears.
    } else {
        ## If there is no letter, print all in the second slide
        return(cbind(m,slide=as.integer(min(c(2L,framerate*nseconds)))))
    }

     
    mletters <- msegmentsletters[,c("segbw","slide"),drop=FALSE]

  ## we order the segments by linebw and jcolmin
    msegbw <- mdoby(m,function(x){
        c(jcolminsegbw=min(x[,"jcol"]),linebw =min(x[,"linebw"]))
    },by="segbw")
    ## If a segment has a letter, assign its slide.
    msegbw <- mmerge(msegbw,mletters)
    ## Background is slide 1
    f <- msegbw[,"segbw"]==0L ## background is slide 1
    msegbw[f,"slide"]  <- 1L
    ## we order the segments by linebw and jcolmin
    ## to find out the segments with no letters.
    oo <- order(msegbw[,"linebw"],msegbw[,"jcolminsegbw"],msegbw[,"slide"])
    msegbw <- msegbw[oo,,drop=FALSE]
    ## Check that the last segment has a slide
    if(is.na(msegbw[ nrow(msegbw),"slide"])) msegbw[ nrow(msegbw),"slide"] <-  as.integer((framerate*nseconds))
    ## Very dirty trick. As the segments are ordered, if there is no slide in one of them, get the next slide.
    while(anyNA(msegbw[,"slide"])){ ## Implement in C?
        f <- is.na(msegbw[,"slide"])
        pos <- seq_len(nrow(msegbw))[f]
        msegbw[pos,"slide"] <- msegbw[pos+1L,"slide"]
    }
  
    m <- mmerge(m,msegbw[,c("segbw","slide")])
    m
}




writeslide <- function(image, nslide, colorsbackground, m, directoriodiapo,writePNG,precision,...){

    directoriodiapos <- paste0(directoriodiapo,nslide)
    unlink(directoriodiapos,recursive=TRUE)
    dir.create(directoriodiapos)

    colorsbackground
    dims <- c(dim(image)[1],dim(image)[2])
    npoints <- prod(dims)
    mmask <- c(rep(colorsbackground[,"color_red"]/precision,npoints),
               rep(colorsbackground[,"color_green"]/precision,npoints),
               rep(colorsbackground[,"color_blue"]/precision,npoints)
               )
    dim(mmask) <- dim(image)

    f <- m[,"isnotbackground"] ==1L
    m <- m[f,c("pos","slide"),drop=FALSE]

    ## For each slide
    maxslide <- max(m[,"slide"])
  
    res <- lapply(1L:maxslide , function(i) {
                                        #i <- 1L
        cat(paste("Slide", nslide, " faltan diapo ", maxslide-i,"\n"))
        filename <- sprintf(paste0(directoriodiapos,"/data%04d.png"), i)
        ##print(filename)

        posf <- m[ m[,"slide"] <= i, "pos"]
        posred <- posf
        posgreen <- npoints + posf
        posblue <- 2*npoints + posf
        pos <- c(posred,posgreen,posblue)
        mmask[pos]  <- image[pos]

        writePNG(mmask,target=filename)

    })



}


postojcol <- function(pos,nrow){
    as.integer((pos -1L)/nrow) + 1L
}

postoirow <- function(pos,nrow){
    as.integer(pos - (postojcol(pos,nrow)-1L)*nrow)
}
