#include <stdlib.h>
#include "hashtable.h"

static const double load_factor_increase = 0.66;
static const size_t initial_num_buckets = 8;

static struct hashtable_key *hashtable_entry_key(struct hashtable *d, void *entry) {
  return (struct hashtable_key *)((intptr_t)entry + d->key_offset);
}

static void *hashtable_entry_base(struct hashtable *d, struct hashtable_key *entry_key) {
  return (void *)((intptr_t)entry_key - d->key_offset);
}

static size_t hash_scramble(size_t h) {
  h = ((h >> 16) ^ h) * 0x45d9f3b;
  h = ((h >> 16) ^ h) * 0x45d9f3b;
  h = (h >> 16) ^ h;
  return h;
}

static void hashtable_resize(struct hashtable *d) {
  const size_t new_num_buckets = d->num_buckets * 2;
  struct list_node *buckets = d->buckets;

  struct list_node *new_buckets = calloc(new_num_buckets, sizeof(struct list_node));
  for (size_t i = 0; i < new_num_buckets; i++) {
    list_head_init(&new_buckets[i]);
  }

  for (size_t i = 0; i < d->num_buckets; i++) {
    struct list_node *curr = buckets[i].next;
    while (curr != &buckets[i]) {
      struct list_node *next = curr->next;
      struct hashtable_key *curr_key = (struct hashtable_key *)curr;
      list_append(&new_buckets[hash_scramble(curr_key->hash) % new_num_buckets], &curr_key->node);
      curr = next;
    };
  }

  d->buckets = new_buckets;
  d->num_buckets = new_num_buckets;
  free(buckets);
}

struct hashtable hashtable_new(ptrdiff_t key_offset, bool (*eq)(void *, void *)) {
  struct hashtable d = {
    .num_buckets = initial_num_buckets,
    .num_entries = 0,
    .buckets = calloc(initial_num_buckets, sizeof(struct list_node)),
    .key_offset = key_offset,
    .eq = eq,
  };
  for (size_t i = 0; i < initial_num_buckets; i++) {
    list_head_init(&d.buckets[i]);
  }
  return d;
}

void hashtable_delete(struct hashtable *d) {
  free(d->buckets);
  d->num_buckets = 0;
  d->num_entries = 0;
  d->buckets = NULL;
}

void *hashtable_add(struct hashtable *d, void *entry) {
  struct hashtable_key *entry_key = hashtable_entry_key(d, entry);
  size_t entry_hash = hash_scramble(entry_key->hash);
  size_t bucket_id = entry_hash % d->num_buckets;
  struct list_node *bucket_head = &d->buckets[bucket_id];

  for (struct list_node *curr = bucket_head->next; curr != bucket_head; curr = curr->next) {
    struct hashtable_key *curr_key = (struct hashtable_key *)curr;
    void *curr_entry = hashtable_entry_base(d, curr_key);

    if (hash_scramble(curr_key->hash) == entry_hash && d->eq(entry, curr_entry)) {
      list_remove(&curr_key->node);
      list_append(bucket_head, &entry_key->node);
      return curr_entry;
    }
  }
  d->num_entries++;
  list_append(bucket_head, &entry_key->node);

  double current_load_factor = (1.0 * d->num_entries) / (1.0 * d->num_buckets);
  if (current_load_factor > load_factor_increase)
    hashtable_resize(d);

  return NULL;
}

void *hashtable_get(struct hashtable *d, void *entry) {
  struct hashtable_key *entry_key = hashtable_entry_key(d, entry);
  size_t entry_hash = hash_scramble(entry_key->hash);
  size_t bucket_id = entry_hash % d->num_buckets;
  struct list_node *bucket_head = &d->buckets[bucket_id];

  for (struct list_node *curr = bucket_head->next; curr != bucket_head; curr = curr->next) {
    struct hashtable_key *curr_key = (struct hashtable_key *)curr;
    void *curr_entry = hashtable_entry_base(d, curr_key);

    if (hash_scramble(curr_key->hash) == entry_hash && d->eq(entry, curr_entry)) {
      return curr_entry;
    }
  }
  return NULL;
}

void *hashtable_remove(struct hashtable *d, void *entry) {
  struct hashtable_key *entry_key = hashtable_entry_key(d, entry);
  size_t entry_hash = hash_scramble(entry_key->hash);
  size_t bucket_id = entry_hash % d->num_buckets;
  struct list_node *bucket_head = &d->buckets[bucket_id];

  for (struct list_node *curr = bucket_head->next; curr != bucket_head; curr = curr->next) {
    struct hashtable_key *curr_key = (struct hashtable_key *)curr;
    void *curr_entry = hashtable_entry_base(d, curr_key);

    if (hash_scramble(curr_key->hash) == entry_hash && d->eq(entry, curr_entry)) {
      list_remove(&curr_key->node);
      d->num_entries--;
      return curr_entry;
    }
  }
  return NULL;
}

size_t string_hash(const char *str) {
  size_t hash = 5381;
  for (size_t i = 0; str[i] != '\0'; i++) {
    hash = ((hash << 5) + hash) ^ str[i];
  }
  return hash;
}
