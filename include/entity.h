#ifndef ENTITY_H
#define ENTITY_H

#include "uv.h"

enum entity_type {
  ENTITY_EMPTY,
  ENTITY_PLAYER,
  ENTITY_MONSTER,
  ENTITY_NPC,
  NUM_ENTITY_TYPES
};

#define VIEWPORT_WIDTH 15
#define VIEWPORT_HEIGHT 15
#define MAX_NUM_INVENTORY_ITEMS 255
#define MAX_NUM_ENTITIES 10000

struct entity_player {
  char name[10];
  struct account *account;
  struct entity *viewport_players[VIEWPORT_WIDTH * VIEWPORT_HEIGHT];
  struct item *viewport_items[VIEWPORT_WIDTH * VIEWPORT_HEIGHT];
  struct item *inventory[MAX_NUM_INVENTORY_ITEMS];
  uv_tcp_t *connection;
  uint64_t last_tick;
};

struct entity_monster {
  struct item *inventory[MAX_NUM_INVENTORY_ITEMS];
};

struct entity_npc {
};

struct entity {
  int type;
  union {
    struct entity_player player;
    struct entity_monster monster;
    struct entity_npc npc;
  };
};

#endif
