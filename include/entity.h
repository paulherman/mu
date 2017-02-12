#ifndef ENTITY_H
#define ENTITY_H

#include "uv.h"
#include "const.h"
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

struct entity_server {
  int type;
  union {
    struct player_server player;
    struct monster monster;
    struct npc npc;
  };
};

struct entity_client {
  int type;
  union {
    struct player_client player;
    struct monster monster;
    struct npc npc;
  };
};

#endif
