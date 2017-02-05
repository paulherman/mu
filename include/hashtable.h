#ifndef HASHTABLE_H 
#define HASHTABLE_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "list.h"

struct hashtable {
  size_t num_buckets;
  size_t num_entries;
  struct list_node *buckets;
  const ptrdiff_t key_offset;
  bool (* const eq)(void *, void *);
};

struct hashtable_key {
  struct list_node node;
  size_t hash;
};

/**
 * Creates a new hashtableionary.
 */
struct hashtable hashtable_new(ptrdiff_t key_offset, bool (*eq)(void *, void *));

/**
 * Deletes the hashtableionary.
 */
void hashtable_delete(struct hashtable *d);

/**
 * Adds element to the hashtableionary.
 */
void *hashtable_add(struct hashtable *d, void *entry);

/**
 * Finds element in the hashtableionary.
 */
void *hashtable_get(struct hashtable *d, void *entry);

/**
 * Removes element from hashtableionary.
 */
void *hashtable_remove(struct hashtable *d, void *entry);

/**
 * Default implementation for hashing strings.
 */
size_t hash_string(const char *str);

#endif
