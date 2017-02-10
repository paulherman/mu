#ifndef SERVER_PROTOCOL_H
#define SERVER_PROTOCOL_H

#include "uv.h"

void server_on_connect(uv_stream_t *server, int status);
void server_on_read(uv_stream_t *stream, ssize_t length, const uv_buf_t *buf);
void server_listen(uv_loop_t *uv_loop, int port);

#endif
