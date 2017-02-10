#ifndef SERVER_H
#define SERVER_H

#include "entity.h"

#ifndef SERVER_PORT
#define SERVER_PORT 3000 
#endif

struct server_state {
  struct entity entities[MAX_NUM_ENTITIES];
  uint64_t ticks;
  uv_loop_t *uv_loop;
};

extern struct server_state state;

#endif
