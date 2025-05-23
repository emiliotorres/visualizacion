% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/mcbind.R
\name{mcbind}
\alias{mcbind}
\title{Combine two matrices.}
\usage{
mcbind(x, rowsx, colsx, y, rowsy, colsy, suffixes = c(".x", ".y"))
}
\arguments{
\item{x}{A matrix.}

\item{rowsx}{Rows to extract from matrix x. Can be either an integer vector or a logical vector.}

\item{colsx}{Columns to extract from matrix x.}

\item{y}{A matrix.}

\item{rowsy}{Rows to extract from matrix y. Must be of the same length as rowsx.}

\item{colsy}{Columns to extract from matrix y. Can be either an integer vector or a character vector.}

\item{suffixes}{Suffixes to append to column names when they are duplicated.}
}
\value{
A matrix with the subset matrix of x and the subset matrix of y.
}
\description{
Combine two matrices.
}
\details{
This function combine (cbind) two matrices x and y. 
It extracts the specified rows and columns from each matrix and then combines them.
When there are few rows, cbind is faster, but when there are more than 5,000 rows,
this method used the threads of the computer.
}
\examples{
x <- as.matrix(iris[,1:2])
y <- as.matrix(iris[,3:4])
rowsx <- 1:50
colsx <- 2
rowsy <- 101:150
colsy <- 1
res1 <- cbind(x[rowsx,colsx,drop=FALSE],y[rowsy,colsy,drop=FALSE])
res2 <- mcbind(x,rowsx,colsx,y,rowsy,colsy)
all.equal(res1,res2)
}
\author{
emilio
}
