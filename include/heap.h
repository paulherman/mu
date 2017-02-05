#ifndef HEAP_H
#define HEAP_H

#include <stdbool.h>
#include "buffer.h"

struct heap {
  struct buffer data;
  bool (*less)(void *, void *);
};

struct heap heap_new(size_t element_size, bool (*less)(void *, void *));

void heap_push(struct heap *h, void *element);

void heap_pop(struct heap *h);

const void *heap_min(struct heap *h);

void heap_delete(struct heap *h);

#endif
