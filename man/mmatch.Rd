% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/mmatch.R
\name{mmatch}
\alias{mmatch}
\title{Match for matrices.}
\usage{
mmatch(
  x,
  y,
  by = NULL,
  by.x = by,
  by.y = by,
  nomatch = NA_integer_,
  rowsx = NULL,
  rowsy = NULL
)
}
\arguments{
\item{x}{A matrix with rows to be matched.}

\item{y}{A matrix whose rows are to be matched against.}

\item{by}{Specifications of the columns used for matching.}

\item{by.x}{Same as \code{by}.}

\item{by.y}{Same as \code{by}.}

\item{nomatch}{The value to be returned in the case when no match is found. It is coerced to \code{integer}.}

\item{rowsx}{Rows of matrix x to be matched.
Can be either an integer vector or a logical vector.}

\item{rowsy}{Rows of matrix y to be matched against.
Can be either an integer vector or a logical vector.}
}
\value{
A vector with the first position on matrix y that is equal to the corresponding row of matrix x.
}
\description{
Match for matrices.
}
\details{
This function attaches a hash table to matrix y, similar to the functionality in \pkg{fastmatch},
for efficient matching of rows between two matrices x and y.
}
\examples{
x  <- matrix(sample(1:100,1e6,TRUE),ncol=2)
s  <- matrix(1:100,ncol=2)
colnames(x) <- colnames(s) <- c("v1","v2")
## the first call to mmatch is comparable to match
system.time(mmatch(s,x))
## but the subsequent calls take no time!
system.time(mmatch(s,x))
}
\author{
emilio
}
