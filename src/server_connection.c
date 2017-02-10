#include <stdlib.h>
#include "server_connection.h"

static void buf_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
  buf->base = malloc(size);
  buf->len = size;
}

static void buf_free(const uv_buf_t *buf) {
  free(buf->base);
}

static uv_tcp_t tcp_server;

void server_listen(uv_loop_t *uv_loop, int port) {
  struct sockaddr_in addr;
  uv_ip4_addr("0.0.0.0", port, &addr);

  uv_tcp_init(uv_loop, &tcp_server);
  uv_tcp_bind(&tcp_server, (const struct sockaddr *)&addr, 0);

  uv_listen((uv_stream_t *)&tcp_server, 128, server_on_connect);
}

void server_on_connect(uv_stream_t *server, int status) {
  if (status == -1) {
    printf("error\n");
  }

  uv_tcp_t *client = calloc(1, sizeof(uv_tcp_t));
  if (uv_accept(server, (uv_stream_t *)client) == 0) {
    uv_read_start((uv_stream_t *)client, buf_alloc, server_on_read);
  } else {
    uv_close((uv_handle_t *)client, NULL);
  }
}

void server_on_read(uv_stream_t *stream, ssize_t length, const uv_buf_t *buf) {
  buf_free(buf);
}
