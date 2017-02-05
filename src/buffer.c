#include <assert.h>
#include <stdlib.h>
#include "buffer.h"

static const double load_factor_inc = 1.0;
static const double load_factor_dec = 0.25;
static const size_t initial_capacity = 8;

struct buffer buffer_new(size_t size, size_t element_size) {
  size_t capacity = initial_capacity;
  while (capacity < size) {
    capacity *= 2;
  }

  struct buffer vec = {
    .size = size,
    .data = calloc(capacity, element_size),
    .capacity = capacity,
    .element_size = element_size
  };
  return vec;
}

void *buffer_get(struct buffer *vec, size_t index) {
  assert(vec->size > index);
  ptrdiff_t offset = index * vec->element_size;
  return (void *)((intptr_t)vec->data + offset);
}

void *buffer_push(struct buffer *vec) {
  vec->size++;
  double load_factor = (1.0 * vec->size) / (1.0 * vec->capacity);

  if (load_factor >= load_factor_inc) {
    vec->capacity *= 2;
    vec->data = realloc(vec->data, vec->capacity * vec->element_size);
  }
  return buffer_get(vec, vec->size - 1);
}

void buffer_pop(struct buffer *vec) {
  assert(vec->size > 0);
  vec->size--;
  double load_factor = (1.0 * vec->size) / (1.0 * vec->capacity);

  if (load_factor <= load_factor_dec) {
    vec->capacity /= 2;
    vec->data = realloc(vec->data, vec->capacity * vec->element_size);
  }
}

void buffer_iterate(struct buffer *vec, void (*f)(void *)) {
  for (size_t i = 0; i < vec->size; i++) {
    f(buffer_get(vec, i));
  }
}

void buffer_delete(struct buffer *vec) {
  free(vec->data);
  vec->size = 0;
}
