#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "uv.h"

#define SERVER_TIMER_TIME 300
#define MAX_NUM_IDLE_TICKS 10

void server_on_connect(uv_stream_t *server, int status);
void server_on_read(uv_stream_t *stream, ssize_t length, const uv_buf_t *buf);
void server_on_tick(uv_timer_t *timer);
void server_listen(int port);

void server_close_connection(uv_tcp_t *connection);
void server_disconnect_player(size_t index);

#endif
