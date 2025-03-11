// Emilio Torres Manzanera
// University of Oviedo
// Time-stamp: <2023-12-27 22:54 emilio on emilio-XPS-15-9570>
// ============================================================

#ifndef MATCHMATRIX_H
#define MATCHMATRIX_H



#define R_NO_REMAP
#include <R.h>
#include <Rinternals.h>
#define SEXPPTR_RO(x) ((const SEXP *)DATAPTR_RO(x))  // STRING_ELT and VECTOR_ELT
#include <stdint.h>    // for uint64_t rather than unsigned long long
#include <stdbool.h> // true false

#ifdef _OPENMP
#include <omp.h>
#define omp_enabled true
#define max_thread omp_get_num_procs()
#define min_thread 1
#define OMP_PARALLEL_FOR(nth) _Pragma("omp parallel for num_threads(nth)")
#define OMP_SIMD _Pragma("omp simd")
#else
#define omp_enabled false
#define max_thread 1
#define min_thread 1
#define omp_get_thread_num() 0
#define OMP_PARALLEL_FOR(n)
#define OMP_SIMD
#endif

#ifdef __cplusplus
#define EXTERN_C_BEGIN extern "C" {
#define EXTERN_C_END   }
#define EXTERN_C       extern "C"
#else
#define EXTERN_C_BEGIN /* Nothing */
#define EXTERN_C_END   /* Nothing */
#define EXTERN_C       extern /* Or Nothing */
#endif /* __cplusplus */

// From kit::kit.h
#define HASH(key, K) (3141592653U * (unsigned int)(key) >> (32 - (K))) // kit.h
#define N_ISNAN(x, y) (!ISNAN(x) && !ISNAN(y))
#define B_IsNA(x, y) (R_IsNA(x) && R_IsNA(y))
#define B_IsNaN(x, y) (R_IsNaN(x) && R_IsNaN(y))
#define B_ISNAN(x, y) (ISNAN(x) && ISNAN(y))
#define REQUAL(x, y)											\
  (N_ISNAN(x, y) ? (x == y) : (B_IsNA(x, y) || B_IsNaN(x, y)))
union uno {
  double d;
  unsigned int u[2];
};


// ============================================================
// Comparations
// ============================================================




// ============================================================
// ============================================================
// utils.c

#endif //ETMUTILS_H
