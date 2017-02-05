#include <assert.h>
#include <stdlib.h>
#include "array.h"

struct array array_new(size_t size, size_t element_size) {
  struct array arr = {
    .size = size,
    .data = calloc(size, element_size),
    .element_size = element_size
  };
  return arr;
}

void *array_get(struct array *arr, size_t index) {
  assert(index < arr->size);
  ptrdiff_t offset = index * arr->element_size;
  void *element =  (void *)(((intptr_t)arr->data) + offset);
  return element;
}

void array_delete(struct array *arr) {
  free(arr->data);
  arr->size = 0;
}

void array_iterate(struct array *arr, void (*f)(void *)) {
  for (size_t i = 0; i < arr->size; i++) {
    f(array_get(arr, i));
  }
}
