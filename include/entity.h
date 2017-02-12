#ifndef ENTITY_H
#define ENTITY_H

#include "uv.h"
#include "const.h"

enum character_class {
  CHARACTER_WIZARD,
  CHARACTER_KNIGHT,
  CHARACTER_ELF,
  CHARACTER_GLADIATOR,
  CHARACTER_LORD
};

struct player {
  char name[MAX_ENTITY_NAME_LENGTH];
  struct account *account;
  struct entity *viewport_players[VIEWPORT_WIDTH * VIEWPORT_HEIGHT];
  struct item *viewport_items[VIEWPORT_WIDTH * VIEWPORT_HEIGHT];
  struct item *inventory[MAX_NUM_INVENTORY_ITEMS];
  uv_tcp_t *connection;
  uint64_t last_tick;
};

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
