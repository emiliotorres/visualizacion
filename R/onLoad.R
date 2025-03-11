## Emilio Torres Manzanera
## University of Oviedo
## Time-stamp: <2025-03-08 15:32 emilio on emilio-XPS-15-9570>
## ============================================================

## http://r-pkgs.had.co.nz/src.html

##' i) To export the library.
##'    Something similar to @rawNamespace useDynLib(matchmatrix); useDynLib(emiliocpp)
##'
##' ii) import methods is included to skip the warning:
##' * checking dependencies in R code ... NOTE
##' Namespace in Imports field not imported from: ‘methods’
##' All declared Imports should be used.
##' 
##' @title Always include this file
##' @param libpath 1 
##' @return 1
##' @author emilio
##' @import methods
##' @useDynLib visualizacion
.onUnload <- function(libpath) {
  library.dynam.unload("visualizacion", libpath)
}
