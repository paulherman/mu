#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <inttypes.h>
#include "server_connection.h"
#include "log.h"
#include "server.h"

static void buf_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
  buf->base = malloc(size);
  buf->len = size;
}

static void buf_free(const uv_buf_t *buf) {
  free(buf->base);
}

bool server_listen(int port) {
  state.running = false;

  uv_timer_init(state.uv_loop, &state.timer);
  uv_timer_start(&state.timer, server_on_tick, SERVER_TIMER_TIME, SERVER_TIMER_TIME);

  struct sockaddr_in addr;
  uv_ip4_addr("0.0.0.0", port, &addr);

  if (uv_tcp_init(state.uv_loop, &state.tcp_server) != 0) {
    return false;
  }

  if (uv_tcp_bind(&state.tcp_server, (const struct sockaddr *)&addr, 0) != 0) {
    return false;
  }

  if (uv_listen((uv_stream_t *)&state.tcp_server, 128, server_on_connect) != 0) {
    return false;
  }

  state.running = true;

  return true;
}

void server_on_connect(uv_stream_t *server, int status) {
  if (status == -1) {
    log_error("Error on client connection");
  } else {
    uv_tcp_t *connection = calloc(1, sizeof(uv_tcp_t));
    uv_tcp_init(state.uv_loop, connection);
    bool accepted = false;

    if (uv_accept(server, (uv_stream_t *)connection) == 0) {
      for (size_t i = 0; i < MAX_NUM_ENTITIES && !accepted; i++) {
        struct entity *entity = &state.entities[i]; 
        if (entity->type == ENTITY_EMPTY) {
          log_info("Client connected at index %zu", i, state.ticks);
          entity->type = ENTITY_PLAYER;
          entity->player.connection = connection;
          entity->player.last_tick = state.ticks;
          uv_read_start((uv_stream_t *)connection, buf_alloc, server_on_read);
          accepted = true;
        }
      }
    }

    if (!accepted) {
      log_error("Error on client connection");
      server_close_connection(connection);
    }
  }
}

void server_close_connection(uv_tcp_t *connection) {
  uv_close((uv_handle_t *)connection, NULL);
  free(connection);
}

void server_on_tick(uv_timer_t *timer) {
  state.ticks++;

  for (size_t i = 0; i < MAX_NUM_ENTITIES; i++) {
    struct entity *entity = &state.entities[i];
    switch (entity->type) {
      case ENTITY_PLAYER:
        if (state.ticks - entity->player.last_tick > MAX_NUM_IDLE_TICKS) {
          server_disconnect_player(i);
          log_error("Closing inactive client %zu", i);
          fflush(stdout);
        }
        break;
    }
  }
}

void server_disconnect_player(size_t index) {
  assert(index < MAX_NUM_ENTITIES && state.entities[index].type == ENTITY_PLAYER);
  server_close_connection(state.entities[index].player.connection);
  state.entities[index].type = ENTITY_EMPTY;
}

void server_on_read(uv_stream_t *stream, ssize_t length, const uv_buf_t *buf) {
  buf_free(buf);
}
