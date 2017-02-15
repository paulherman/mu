#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>
#include "client_connection.h"
#include "client.h"
#include "log.h"
#include "protocol.h"

static void buf_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
  buf->base = malloc(size);
  buf->len = size;
}

static void buf_free(const uv_buf_t *buf) {
  free(buf->base);
}


bool client_connect(const char *ip, int port) {
  state.running = false;
  state.ticks = 0;
  state.last_read_tick = 0;
  state.last_write_tick = 0;
  state.ip = ip;
  state.port = port;

  for (int err = uv_timer_init(state.uv_loop, &state.timer); err < 0;) {
    log_error("Unable to initialize timer - %s", uv_strerror(err));
    return false;
  }

  for (int err = uv_timer_start(&state.timer, client_on_tick, CLIENT_TIMER_TIME, CLIENT_TIMER_TIME); err < 0;) {
    log_error("Unable to start timer - %s", uv_strerror(err));
    return false;
  }

  struct sockaddr_in server_addr;
  uv_ip4_addr(ip, port, &server_addr);

  for (int err = uv_tcp_init(state.uv_loop, &state.tcp_client); err < 0;) {
    log_error("Unable to initialize TCP connection - %s", uv_strerror(err));
    return false;
  }

  for (int err = uv_tcp_connect(&state.tcp_connection, &state.tcp_client, (const struct sockaddr *)&server_addr, client_on_connect); err < 0;) {
    log_error("Unable to connect to server at %s:%d - %s", ip, port, uv_strerror(err));
    return false;
  }

  state.running = true;
  return true;
}

void client_disconnect() {
  uv_close((uv_handle_t *)&state.tcp_client, client_on_close);
  state.running = false;
}

void client_on_connect(uv_connect_t *connection, int status) {
  if (status < 0) {
    log_error("Unable to connect to server at %s:%d - %s", state.ip, state.port, uv_strerror(status));
    client_disconnect();
  } else {
    uv_stream_t *stream = connection->handle;
    log_info("Connected to server at %s:%d", state.ip, state.port);

    state.network_errors = 0;
    state.last_read_tick = state.ticks;
    state.last_write_tick = state.ticks;
    uv_read_start(stream, buf_alloc, client_on_read);
  }
}

void client_on_read(uv_stream_t *stream, ssize_t length, const uv_buf_t *buf) {
  state.last_read_tick = state.ticks;
  buf_free(buf);
}

void client_on_close(uv_handle_t *handle) {
  log_info("Closed connection to server");
}

void client_on_write(uv_write_t *req, int status) {
  uv_buf_t *buf = req->data;

  if (status < 0) {
    state.network_errors++;
    log_error("Error sending data to server - %s", uv_strerror(status));
  } else {
    state.last_write_tick = state.ticks;

    int32_t type = packet_get_type(buf->base, buf->len);
    if (0 <= type && type < NUM_PACKAGE_TYPES)
      log_debug("Sent packet %s to server", packet_names[type]);
    else
      log_error("Sent unknown packet %d to server", type);
  }

  free(buf->base);
  free(buf);
  free(req);
}

void client_on_tick(uv_timer_t *timer) {
  state.ticks++;

  if (state.ticks - state.last_read_tick > CLIENT_MAX_IDLE_TICKS) {
    //client_disconnect();
    //log_error("Disconnected from server due to idling");
  }

  if (state.ticks - state.last_write_tick > CLIENT_MAX_IDLE_TICKS / 2) {
    client_send_ping();
  }
}

static bool client_send_packet(int32_t packet_type, void *packet_data) {
  uv_buf_t *buf = malloc(sizeof(uv_buf_t));

  buf->len = packet_buf_length(packet_type);
  buf->base = malloc(buf->len);

  memcpy(buf->base, &packet_type, sizeof(int32_t));
  memcpy(buf->base + sizeof(int32_t), packet_data, buf->len - sizeof(int32_t));

  uv_write_t *request = malloc(sizeof(uv_write_t));
  request->data = buf;

  for (int err = uv_write(request, (uv_stream_t *)&state.tcp_client, buf, 1, client_on_write); err < 0;) {
    log_error("Unable to send data to server - %s", uv_strerror(err));
    return false;
  }
  return true;
}

bool client_send_ping() {
  struct packet_cs_ping packet = {
    .ticks = state.ticks
  };
  return client_send_packet(PACKET_CS_PING, &packet);
}
