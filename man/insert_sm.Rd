% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/sparsematrix.R
\name{insert_sm}
\alias{insert_sm}
\title{.}
\usage{
insert_sm(rows, cols, values, sm)
}
\arguments{
\item{rows}{rows}

\item{cols}{cols}

\item{values}{values Only Integer/logical}

\item{sm}{sparse matrix}
}
\value{
(Invisible) Number of cells with information (length)
}
\description{
.. content for  (no empty lines) ..
}
\details{
.. content for  ..
}
\examples{
sm <- create_sm(10,10)
insert_sm(c(1,2),c(3,4),c(-1,-10),sm)
}
\author{
emilio
}
