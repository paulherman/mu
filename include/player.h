#ifndef PLAYER_H
#define PLAYER_H

#include "const.h"

enum character_class {
  CHARACTER_WIZARD,
  CHARACTER_KNIGHT,
  CHARACTER_ELF,
  CHARACTER_GLADIATOR,
  CHARACTER_LORD
};

struct player_common {
  char name[MAX_ENTITY_NAME_LENGTH];
  struct account *account;
  struct entity *viewport_players[VIEWPORT_WIDTH * VIEWPORT_HEIGHT];
  struct item *viewport_items[VIEWPORT_WIDTH * VIEWPORT_HEIGHT];
  struct item *inventory[MAX_NUM_INVENTORY_ITEMS];
};

struct player_server {
  struct player_common;
  uv_tcp_t *connection;
  uint64_t last_tick;
};

struct player_client {
  struct player_common;
};

#endif
