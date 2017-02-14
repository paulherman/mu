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

bool find_entity_by_connection(uv_tcp_t *connection, size_t *index) {
  for (size_t i = 0; i < MAX_NUM_ENTITIES; i++) {
    struct entity_server *entity = &state.entities[i];
    if (entity->type == ENTITY_PLAYER && entity->player.connection == connection) {
      *index = i;
      return true;
    }
  }
  return false;
}

bool server_listen(int port) {
  state.running = false;

  uv_timer_init(state.uv_loop, &state.timer);
  uv_timer_start(&state.timer, server_on_tick, SERVER_TIMER_TIME, SERVER_TIMER_TIME);

  struct sockaddr_in addr;
  uv_ip4_addr("0.0.0.0", port, &addr);

  for (int err = uv_tcp_init(state.uv_loop, &state.tcp_server); err < 0;) {
    log_error("Unable to initialize TCP server - %s", uv_strerror(err));
    return false;
  }

  for (int err = uv_tcp_bind(&state.tcp_server, (const struct sockaddr *)&addr, 0); err < 0;) {
    log_error("Unable to bind server to port %d - %s", port, uv_strerror(err));
    return false;
  }

  for (int err = uv_listen((uv_stream_t *)&state.tcp_server, 128, server_on_connect); err < 0;) {
    log_error("Unable to listen on port %d - %s", port, uv_strerror(err));
    return false;
  }

  state.running = true;
  log_info("Server started port %d", port);

  return true;
}

void server_on_connect(uv_stream_t *server, int status) {
  if (status < 0) {
    log_error("Error on client connection - %s", uv_strerror(status));
  } else {
    uv_tcp_t *connection = calloc(1, sizeof(uv_tcp_t));
    for (int err = uv_tcp_init(state.uv_loop, connection); err < 0;) {
      log_error("Unable to initialize TCP connection for client - %s", uv_strerror(err));
      server_close_connection(connection);
      return;
    }

    for (int err = uv_accept(server, (uv_stream_t *)connection); err < 0;) {
      log_error("Unable to accept client connection - %s", uv_strerror(err));
      server_close_connection(connection);
      return;
    }

    for (size_t i = 0; i < MAX_NUM_ENTITIES; i++) {
      struct entity_server *entity = &state.entities[i]; 
      if (entity->type == ENTITY_EMPTY) {
        log_info("Client connected at index %zu", i);
        log_debug("Client at index %zu uses connection %p", i, connection);
        entity->type = ENTITY_PLAYER;
        entity->player.connection = connection;
        entity->player.last_tick = state.ticks;
        uv_read_start((uv_stream_t *)connection, buf_alloc, server_on_read);
        return;
      }
    }

    log_error("Unable to allocate entity for client");
    server_close_connection(connection);
  }
}

void server_close_connection(uv_tcp_t *connection) {
  uv_close((uv_handle_t *)connection, server_on_client_close);
}

void server_on_client_close(uv_handle_t *handle) {
  size_t index = 0;
  if (find_entity_by_connection((uv_tcp_t *)handle, &index)) {
    state.entities[index].type = ENTITY_EMPTY;
    log_info("Disconnected client %zu", index);
    log_debug("Client %zu freed connection %p", index, handle);
  } else {
    log_error("Attempting to close unknown client %p", handle);
  }
  free(handle);
}

void server_on_tick(uv_timer_t *timer) {
  state.ticks++;

  for (size_t i = 0; i < MAX_NUM_ENTITIES; i++) {
    struct entity_server *entity = &state.entities[i];
    switch (entity->type) {
      case ENTITY_PLAYER:
        if (state.ticks - entity->player.last_tick > MAX_NUM_IDLE_TICKS) {
          log_error("Closing inactive client %zu", i);
          server_disconnect_player(i);
        }
        break;
      case ENTITY_EMPTY:
        break;
      case ENTITY_NPC:
        break;
      case ENTITY_MONSTER:
        break;
    }
  }
}

void server_disconnect_player(size_t index) {
  assert(index < MAX_NUM_ENTITIES && state.entities[index].type == ENTITY_PLAYER);
  server_close_connection(state.entities[index].player.connection);
}


void server_on_read(uv_stream_t *stream, ssize_t length, const uv_buf_t *buf) {
  if (length < 0) {
    server_close_connection((uv_tcp_t *)stream);
  } else {
    size_t index = 0;
    if (find_entity_by_connection((uv_tcp_t *)stream, &index)) {
      log_debug("Received data from client %zu on connection %p", index, stream);

      struct entity_server *entity = &state.entities[index];
      entity->player.last_tick = state.ticks;
    }
    buf_free(buf);
  }
}
