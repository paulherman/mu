#ifndef CLIENT_CONNECTION_H
#define CLIENT_CONNECTION_H

#include <stddef.h>
#include "uv.h"


void client_on_connect(uv_connect_t *connection, int status);
void client_on_read(uv_stream_t *stream, ssize_t length, const uv_buf_t *buf);
void client_on_close(uv_handle_t *handle);
void client_on_write(uv_write_t* req, int status);
void client_on_tick(uv_timer_t *timer);

bool client_connect(const char *ip, int port);
void client_disconnect();

bool client_send_ping();

#endif
