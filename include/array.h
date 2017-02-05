#ifndef ARRAY_H
#define ARRAY_H

#include <stdint.h>
#include <stddef.h>

struct array {
  size_t size;
  void *data;
  const size_t element_size;
};

/**
 * Creates a new array.
 */
struct array array_new(size_t size, size_t element_size);

/**
 * Gets an element from the array.
 */
void *array_get(struct array *arr, size_t index);

/**
 * Applies function for each element of the array.
 */
void array_iterate(struct array *arr, void (*f)(void *));

/**
 * Deletes an array.
 */
void array_delete(struct array *arr);

#endif
