#include <R.h>
#include <Rinternals.h>
#include <stdlib.h> // for NULL
#include <R_ext/Rdynload.h>

/* FIXME: 
   Check these declarations against the C/Fortran source code.
*/

/* .Call calls */
extern SEXP betweenrangeoo(void *, void *, void *);
extern SEXP cbind_matrix(void *, void *, void *, void *, void *, void *);
extern SEXP convert_key_to_vector_SEXP(void *);
extern SEXP convert_vector_Rrows_to_Crows(void *, void *);
extern SEXP convert_vector_to_key_SEXP(void *);
extern SEXP create_hashmap_SEXP(void *);
extern SEXP create_hashtable_SEXP(void *, void *);
extern SEXP create_matrix_general_hashtable_SEXP(void *, void *, void *, void *);
extern SEXP create_matrix_hashtable_SEXP(void *);
extern SEXP create_sparsematrix_SEXP(void *, void *);
extern SEXP dim_sparsematrix_SEXP(void *);
extern SEXP dump_long_format_sparsematrix(void *);
extern SEXP find_position_hashtable_SEXP(void *, void *, void *, void *);
extern SEXP find_position_matrix_general_hashtable_SEXP(void *, void *, void *, void *, void *, void *);
extern SEXP find_position_matrix_hashtable_SEXP(void *, void *, void *, void *);
extern SEXP get_elements_sparsematrix_SEXP(void *, void *, void *, void *);
extern SEXP get_first_element_of_list_of_integers(void *);
extern SEXP get_hashmap_SEXP(void *, void *);
extern SEXP get_key_by_position_hashmap_SEXP(void *, void *);
extern SEXP get_list_of_rows_with_same_values_in_this_matrix(void *, void *, void *);
extern SEXP get_number_of_processors_available(void);
extern SEXP get_order_pos(void *, void *);
extern SEXP get_positions_hashtable_SEXP(void *);
extern SEXP get_rescale_matrix(void *, void *, void *);
extern SEXP get_segments_of_points_matrix(void *, void *);
extern SEXP get_size_hashtable_SEXP(void *);
extern SEXP get_value_by_position_hashmap_SEXP(void *, void *);
extern SEXP insert_elements_sparsematrix_SEXP(void *, void *, void *, void *);
extern SEXP insert_hashmap_SEXP(void *, void *, void *);
extern SEXP insert_list_hashmap_SEXP(void *, void *, void *);
extern SEXP length_hashmap_SEXP(void *);
extern SEXP match_matrix(void *, void *, void *, void *, void *, void *, void *);
extern SEXP orderBoundingBoxes(void *, void *, void *, void *);
extern SEXP size_sparsematrix_SEXP(void *);
extern SEXP tic_named_SEXP(void *);
extern SEXP tic_SEXP(void);
extern SEXP toc_print_timer_SEXP(void *);
extern SEXP toc_SEXP(void);
extern SEXP toc_timer_SEXP(void *);

static const R_CallMethodDef CallEntries[] = {
    {"betweenrangeoo",                                   (DL_FUNC) &betweenrangeoo,                                   3},
    {"cbind_matrix",                                     (DL_FUNC) &cbind_matrix,                                     6},
    {"convert_key_to_vector_SEXP",                       (DL_FUNC) &convert_key_to_vector_SEXP,                       1},
    {"convert_vector_Rrows_to_Crows",                    (DL_FUNC) &convert_vector_Rrows_to_Crows,                    2},
    {"convert_vector_to_key_SEXP",                       (DL_FUNC) &convert_vector_to_key_SEXP,                       1},
    {"create_hashmap_SEXP",                              (DL_FUNC) &create_hashmap_SEXP,                              1},
    {"create_hashtable_SEXP",                            (DL_FUNC) &create_hashtable_SEXP,                            2},
    {"create_matrix_general_hashtable_SEXP",             (DL_FUNC) &create_matrix_general_hashtable_SEXP,             4},
    {"create_matrix_hashtable_SEXP",                     (DL_FUNC) &create_matrix_hashtable_SEXP,                     1},
    {"create_sparsematrix_SEXP",                         (DL_FUNC) &create_sparsematrix_SEXP,                         2},
    {"dim_sparsematrix_SEXP",                            (DL_FUNC) &dim_sparsematrix_SEXP,                            1},
    {"dump_long_format_sparsematrix",                    (DL_FUNC) &dump_long_format_sparsematrix,                    1},
    {"find_position_hashtable_SEXP",                     (DL_FUNC) &find_position_hashtable_SEXP,                     4},
    {"find_position_matrix_general_hashtable_SEXP",      (DL_FUNC) &find_position_matrix_general_hashtable_SEXP,      6},
    {"find_position_matrix_hashtable_SEXP",              (DL_FUNC) &find_position_matrix_hashtable_SEXP,              4},
    {"get_elements_sparsematrix_SEXP",                   (DL_FUNC) &get_elements_sparsematrix_SEXP,                   4},
    {"get_first_element_of_list_of_integers",            (DL_FUNC) &get_first_element_of_list_of_integers,            1},
    {"get_hashmap_SEXP",                                 (DL_FUNC) &get_hashmap_SEXP,                                 2},
    {"get_key_by_position_hashmap_SEXP",                 (DL_FUNC) &get_key_by_position_hashmap_SEXP,                 2},
    {"get_list_of_rows_with_same_values_in_this_matrix", (DL_FUNC) &get_list_of_rows_with_same_values_in_this_matrix, 3},
    {"get_number_of_processors_available",               (DL_FUNC) &get_number_of_processors_available,               0},
    {"get_order_pos",                                    (DL_FUNC) &get_order_pos,                                    2},
    {"get_positions_hashtable_SEXP",                     (DL_FUNC) &get_positions_hashtable_SEXP,                     1},
    {"get_rescale_matrix",                               (DL_FUNC) &get_rescale_matrix,                               3},
    {"get_segments_of_points_matrix",                    (DL_FUNC) &get_segments_of_points_matrix,                    2},
    {"get_size_hashtable_SEXP",                          (DL_FUNC) &get_size_hashtable_SEXP,                          1},
    {"get_value_by_position_hashmap_SEXP",               (DL_FUNC) &get_value_by_position_hashmap_SEXP,               2},
    {"insert_elements_sparsematrix_SEXP",                (DL_FUNC) &insert_elements_sparsematrix_SEXP,                4},
    {"insert_hashmap_SEXP",                              (DL_FUNC) &insert_hashmap_SEXP,                              3},
    {"insert_list_hashmap_SEXP",                         (DL_FUNC) &insert_list_hashmap_SEXP,                         3},
    {"length_hashmap_SEXP",                              (DL_FUNC) &length_hashmap_SEXP,                              1},
    {"match_matrix",                                     (DL_FUNC) &match_matrix,                                     7},
    {"orderBoundingBoxes",                               (DL_FUNC) &orderBoundingBoxes,                               4},
    {"size_sparsematrix_SEXP",                           (DL_FUNC) &size_sparsematrix_SEXP,                           1},
    {"tic_named_SEXP",                                   (DL_FUNC) &tic_named_SEXP,                                   1},
    {"tic_SEXP",                                         (DL_FUNC) &tic_SEXP,                                         0},
    {"toc_print_timer_SEXP",                             (DL_FUNC) &toc_print_timer_SEXP,                             1},
    {"toc_SEXP",                                         (DL_FUNC) &toc_SEXP,                                         0},
    {"toc_timer_SEXP",                                   (DL_FUNC) &toc_timer_SEXP,                                   1},
    {NULL, NULL, 0}
};

void R_init_visualizacion(DllInfo *dll)
{
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
