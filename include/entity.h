#ifndef ENTITY_H
#define ENTITY_H

#include "uv.h"

#define VIEWPORT_WIDTH 15
#define VIEWPORT_HEIGHT 15
#define MAX_NUM_INVENTORY_ITEMS 255
#define MAX_NUM_ENTITIES 10000
#define MAX_ENTITY_NAME_LENGTH 10

#include "player.h"

enum entity_type {
  ENTITY_EMPTY,
  ENTITY_PLAYER,
  ENTITY_MONSTER,
  ENTITY_NPC,
  NUM_ENTITY_TYPES
};

struct monster {
  struct item *inventory[MAX_NUM_INVENTORY_ITEMS];
};

struct npc {
};

struct entity {
  int type;
  union {
    struct player player;
    struct monster monster;
    struct npc npc;
  };
};

#endif
