#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Circular doubly linked list node.
 */
struct list_node {
  struct list_node *prev, *next;
};

/**
 * Initializes linked list from node.
 */
void list_head_init(struct list_node *head);

/**
 * Adds node to linked list after head.
 */
void list_append(struct list_node *head, struct list_node *node);

/**
 * Adds node to linked list before head.
 */
void list_prepend(struct list_node *head, struct list_node *node);

/**
 * Remove node from linked list.
 */
void list_remove(struct list_node *node);

/**
 * Tests whether a linked list is empty.
 */
bool list_empty(struct list_node *head);

/**
 * Computes length of list.
 */
size_t list_length(struct list_node *head);

/**
 * Iterate through the list.
 */
void list_iterate(struct list_node *head, void (*f)(void *), ptrdiff_t base_offset);

#endif
