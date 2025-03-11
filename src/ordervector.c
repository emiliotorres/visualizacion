#include <stdlib.h> // For qsort
//#include <math.h> // fabs
//#include <stdio.h>
#include <R.h>
#include <Rdefines.h>

#include <string.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) < (b)) ? (b) : (a))

int between(double x, double a, double b){
  if(x >= MIN(a,b) && x <= MAX(a,b)) return 1;
  return 0;
}


// Structure to hold the value and its original index
typedef struct {
  int value;
  int index;
} IndexedValue;

// Comparison function for qsort.  Compares based on the 'value' field.
int compareIndexedValues(const void *a, const void *b) {
  IndexedValue *indexed_a = (IndexedValue *)a;
  IndexedValue *indexed_b = (IndexedValue *)b;
  return (indexed_a->value - indexed_b->value);
}

// Function to get the indices that would sort the input vector
int* get_sort_indices(int *vector, int size) {
  if (size <= 0) {
	return vector; // Handle empty or invalid input
  }

  // 1. Create an array of structs to store values and their original indices
  IndexedValue *indexed_vector = (IndexedValue*) malloc(size * sizeof(IndexedValue));
  if (indexed_vector == NULL) {
	return NULL; // Memory allocation failed
  }

  for (int i = 0; i < size; i++) {
	indexed_vector[i].value = vector[i];
	indexed_vector[i].index = i; // Store the original index (0-based)
  }

  // 2. Sort the array of structs based on the 'value' field
  qsort(indexed_vector, size, sizeof(IndexedValue), compareIndexedValues);

  // 3. Create an array to store the sort indices
  int *sort_indices = (int*) malloc(size * sizeof(int));
  if (sort_indices == NULL) {
	free(indexed_vector); // Clean up allocated memory
	return NULL; // Memory allocation failed
  }

  // 4. Extract the original indices from the sorted structs
  //    These indices now represent the order to sort the original vector
  for (int i = 0; i < size; i++) {
	sort_indices[i] = indexed_vector[i].index;
	  }

  // 5. Free the temporary struct array
  free(indexed_vector);

  return sort_indices;
}



// Function to check if an element exists in an array
int exists_in_array(int *arr, int size, int value) {
  for (int i = 0; i < size; i++) {
	if (arr[i] == value) {
	  return 1; // Found
	}
  }
  return 0; // Not found
}



// Function to remove common elements from both arrays
int* common_elements(int *a, int sizea, int *b, int sizeb, int *sizecommon) {

  int *temp = R_Calloc( sizea,int);
  
  int count = 0;
  for (int i = 0; i < sizea; i++) {
	if (exists_in_array(b, sizeb, a[i])) {
	  temp[count++] = a[i]; // Add element if in 'b'
	}
  }

  // Allocate exact memory needed
  int *result = R_Calloc(count,int);
  if (!result) {
	R_Free(temp);
	*sizecommon = 0;
	return NULL; // Memory allocation failed
  }

  // Copy valid elements
  for (int i = 0; i < count; i++) {
	result[i] = temp[i];
  }

  R_Free(temp); // Free temporary memory
  *sizecommon = count;
  return result;
}



// Comparison function for qsort (ascending order)
// Example comparison function for integers
int compare_int(const void* a, const void* b) {
  int int_a = *(const int*)a;
  int int_b = *(const int*)b;
  if (int_a < int_b) return -1;
  if (int_a > int_b) return 1;
  return 0;
}

// Example comparison function for doubles
int compare_double(const void* a, const void* b) {
  double double_a = *(const double*)a;
  double double_b = *(const double*)b;
  if (double_a < double_b) return -1;
  if (double_a > double_b) return 1;
  return 0;
}

// Example comparison function for strings (using strcmp)
int compare_string(const void* a, const void* b) {
  return strcmp(*(const char**)a, *(const char**)b);
}


// Function to remove duplicates from the indices array
int removeduplicatesint(int* arr, int size) {
  if (size <= 0) return 0; // No elements to process

  // Step 1: Sort the array
  qsort(arr, size, sizeof(int), compare_int);

  // Step 2: Remove duplicates in-place
  int unique_count = 1; // At least one unique element exists

  for (int i = 1; i < size; i++) {
	if (arr[i] != arr[i - 1]) { // If the current element is different from the previous one
	  arr[unique_count] = arr[i]; // Move it to the new position
	  unique_count++;
	}
  }

  return unique_count; // New size of the array
}




// Function to check if an element exists in an array
int elementExists(const void* element, const void* array, size_t array_len, size_t element_size, int (*compare)(const void*, const void*)) {
  for (size_t i = 0; i < array_len; ++i) {
	const void* current_element = (const char*)array + i * element_size;
	if (compare(element, current_element) == 0) {
	  return 1;
	}
  }
  return 0;
}

// Generic function to replicate R's %in%
int* contains(const void* x, size_t len_x, const void* y, size_t len_y, size_t element_size, int (*compare)(const void*, const void*)) {
  if (x == NULL || y == NULL || len_x == 0) {
	return NULL; // Or handle the error as needed
  }

  int* result = R_Calloc(len_x,int);

  for (size_t i = 0; i < len_x; ++i) {
	const void* current_x = (const char*)x + i * element_size;
	result[i] = elementExists(current_x, y, len_y, element_size, compare);
  }

  return result;
}





int isSymmetricMatrix(int* m, int n) {
  // Only need to check upper triangular against lower triangular
  for (int i = 0; i < n - 1; i++) {
	for (int j = i + 1; j < n; j++) {
	  if (m[i * n + j] != m[j * n + i]) {
		return 0;  // Not symmetric
	  }
	}
  }
  return 1;  // Symmetric
}
