#include <stddef.h>
#include <stdlib.h>
#include "list.h"

static void *list_node_base(struct list_node *node, ptrdiff_t base_offset) {
    return (void *)((intptr_t)node - base_offset);
}

static inline void list_insert(struct list_node *prev, struct list_node *next, struct list_node *new_node) {
  new_node->next = next;
  new_node->prev = prev;
  next->prev = new_node;
  prev->next = new_node;
}

void list_head_init(struct list_node *head) {
  head->next = head;
  head->prev = head;
}

void list_append(struct list_node *head, struct list_node *node) {
  list_insert(head, head->next, node);
}

void list_prepend(struct list_node *head, struct list_node *node) {
  list_insert(head->prev, head, node);
}

void list_remove(struct list_node *node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
  node->prev = NULL;
  node->next = NULL;
}

void list_iterate(struct list_node *head, void (*f)(void *), ptrdiff_t base_offset) {
  for (struct list_node *curr = head->next; curr != head; curr = curr->next) {
    f(list_node_base(curr, base_offset));
  }
}

bool list_empty(struct list_node *head) {
  return head->next == head;
}

size_t list_length(struct list_node *head) {
  size_t length = 0;
  for (struct list_node *curr = head->next; curr != head; curr = curr->next) {
    length++;
  }
  return length;
}
