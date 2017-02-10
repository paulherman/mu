#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include "client_connection.h"
#include "client.h"
#include "log.h"

static void buf_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
  buf->base = malloc(size);
  buf->len = size;
}

static void buf_free(const uv_buf_t *buf) {
  free(buf->base);
}


void client_connect(const char *ip, int port) {
  state.running = true;
  state.ticks = 0;
  state.last_tick = 0;

  uv_timer_init(state.uv_loop, &state.timer);
  uv_timer_start(&state.timer, client_on_tick, CLIENT_TIMER_TIME, CLIENT_TIMER_TIME);

  struct sockaddr_in server_addr;
  uv_ip4_addr(ip, port, &server_addr);

  uv_tcp_init(state.uv_loop, &state.tcp_client);

  uv_tcp_connect(&state.tcp_connection, &state.tcp_client, (const struct sockaddr *)&server_addr, client_on_connect);
}

void client_disconnect() {
  uv_close((uv_handle_t *)&state.tcp_client, client_on_close);
  state.running = false;
}

void client_on_connect(uv_connect_t *connection, int status) {
  if (status < 0) {
    log_error("Unable to connect to server");
    client_disconnect();
  } else {
    uv_stream_t *stream = connection->handle;
    log_info("Connected to server");

    state.last_tick = state.ticks;
    uv_read_start(stream, buf_alloc, client_on_read);
  }
}

void client_on_read(uv_stream_t *stream, ssize_t length, const uv_buf_t *buf) {
  state.last_tick = state.ticks;
  buf_free(buf);
}

void client_on_close(uv_handle_t *handle) {
}

void client_on_write(uv_write_t* req, int status) {
}

void client_on_tick(uv_timer_t *timer) {
  state.ticks++;
  if (state.ticks - state.last_tick > CLIENT_MAX_IDLE_TICKS) {
    client_disconnect();
    log_error("Disconnected from server due to idling");
  }
}

void client_send_handshake(uv_stream_t *stream) {
}
