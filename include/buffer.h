#ifndef BUFFER_H
#define BUFFER_H

#include <stdint.h>
#include <stddef.h>

struct buffer {
  size_t size;
  void *data;
  size_t capacity;
  const size_t element_size;
};

/**
 * Creates a new buffer.
 */
struct buffer buffer_new(size_t size, size_t element_size);

/**
 * Gets an element from the buffer.
 */
void *buffer_get(struct buffer *vec, size_t index);

/**
 * Appends an element to the buffer.
 */
void *buffer_push(struct buffer *vec);

/**
 * Removes the last element of the buffer.
 */
void buffer_pop(struct buffer *vec);

/**
 * Iterates over the elements of the buffer.
 */
void buffer_iterate(struct buffer *vec, void (*f)(void *));

/**
 * Deletes a buffer.
 */
void buffer_delete(struct buffer *arr);

#endif
