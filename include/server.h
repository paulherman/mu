#ifndef SERVER_H
#define SERVER_H

#include <stdbool.h>
#include "entity.h"
#include "const.h"

struct server_state {
  struct entity entities[MAX_NUM_ENTITIES];
  uint64_t ticks;
  bool running;
  uv_loop_t *uv_loop;
  uv_tcp_t tcp_server;
  uv_timer_t timer;
};

extern struct server_state state;

#endif
