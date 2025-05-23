## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-05-22 12:23 emilio on emilio-despacho>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##

source("header.R")

tic()
toc()

a <- tic_named("Hola")
1+1
toc_print_timer(a)
toc_timer(a)
