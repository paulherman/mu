#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "heap.h"

static inline void heap_swap(struct heap *h, size_t parent, size_t child) {
  size_t element_size = h->data.element_size;
  void *child_data = buffer_get(&h->data, child);
  void *parent_data = buffer_get(&h->data, parent);
  void *temp = calloc(1, element_size);
  memcpy(temp, child_data, element_size);
  memcpy(child_data, parent_data, element_size);
  memcpy(parent_data, temp, element_size);
  free(temp);
}

struct heap heap_new(size_t element_size, bool (*less)(void *, void *)) {
  struct heap h = {
    .data = buffer_new(0, element_size),
    .less = less
  };
  return h;
}

void heap_push(struct heap *h, void *element) {
  buffer_push(&h->data);

  size_t index = h->data.size - 1;
  size_t parent_index = (index - 1) / 2;

  while (index != 0 && !h->less(buffer_get(&h->data, parent_index), element)) {
    void *parent = buffer_get(&h->data, parent_index);
    void *child = buffer_get(&h->data, index);
    memcpy(child, parent, h->data.element_size);
  }
  memcpy(buffer_get(&h->data, index), element, h->data.element_size);
}

void heap_pop(struct heap *h) {
  assert(h->data.size > 0);

  heap_swap(h, 0, h->data.size - 1);
  buffer_pop(&h->data);
  
  size_t index = 0;
  bool swapped = true;
  while (index < h->data.size && swapped) {
    void *parent = buffer_get(&h->data, index);
    void *left_child = 2 * index + 1 < h->data.size ? buffer_get(&h->data, 2 * index + 1) : NULL;
    void *right_child = 2 * index + 2 < h->data.size ?  buffer_get(&h->data, 2 * index + 2) : NULL;

    void *max = parent;
    size_t max_index = index;
    if (left_child != NULL && h->less(left_child, max)) {
      max = left_child;
      max_index = 2 * index + 1;
    }
    if (right_child != NULL && h->less(right_child, max)) {
      max = right_child;
      max_index = 2 * index + 2;
    }

    if (index != max_index) {
      swapped = true;
      heap_swap(h, index, max_index);
      index = max_index;
    }
  }
}

const void *heap_min(struct heap *h) {
  assert(h->data.size > 0);
  return buffer_get(&h->data, 0);
}

void heap_delete(struct heap *h) {
  buffer_delete(&h->data);
}
