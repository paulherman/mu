#ifndef CLIENT_H
#define CLIENT_H

struct client_state {
  bool running;
  uint64_t ticks;
  uint64_t last_read_tick;
  uint64_t last_write_tick;
  int port;
  size_t network_errors;
  const char *ip;
  uv_loop_t *uv_loop;
  uv_tcp_t tcp_client;
  uv_connect_t tcp_connection;
  uv_timer_t timer;
};

extern struct client_state state;

#endif
