#ifndef CLIENT_H
#define CLIENT_H

#ifndef SERVER_ADDRESS
#define SERVER_ADDRESS "0.0.0.0"
#endif

#ifndef SERVER_PORT
#define SERVER_PORT 3000 
#endif

#define CLIENT_TIMER_TIME 300
#define CLIENT_MAX_IDLE_TICKS 10

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
