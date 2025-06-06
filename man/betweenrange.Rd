% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/betweenrange.R
\name{betweenrange}
\alias{betweenrange}
\title{Check if a value is between a lower and upper limits.}
\usage{
betweenrange(x, lower, upper)
}
\arguments{
\item{x}{A vector of integers or numeric.}

\item{lower}{A vector of integers or numeric representing the lower limits.}

\item{upper}{A vector of integers or numeric representing the upper limits.
Its length must be the same as the length of the lower vector.}
}
\value{
A matrix with the elements of x that are between
  the corresponding lower and upper limits.
}
\description{
Check if a value is between a lower and upper limits.
}
\details{
This function checks whether each element in a vector is between the corresponding
lower and upper limits. It is a vectorised function, meaning it operates element-wise
on vectors. Internally, it performs garbage collection.
}
\examples{
x  <- 1.0*c(1:15)
lower <- c(0, 4, 8)
upper <- c(2, 6, 12)
betweenrange(x, lower, upper)
}
\author{
Emilio
}
