## #+TITLE: 
## #+AUTHOR: Emilio Torres Manzanera
## #+DATE: Time-stamp: <2025-02-28 21:20 emilio on emilio-XPS-15-9570>
## #+TAGS: 
## #+PROPERTY: header-args :results output :exports both :session 

##

library(hashtable)

tic()
toc()

a <- tic_named("Hola")
1+1
toc_print_timer(a)
toc_timer(a)
