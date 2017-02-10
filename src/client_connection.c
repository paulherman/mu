#include <stdlib.h>
#include <stdbool.h>
#include "client_connection.h"

bool client_running = true;

static void buf_alloc(uv_handle_t *handle, size_t size, uv_buf_t *buf) {
  buf->base = malloc(size);
  buf->len = size;
}

static void buf_free(const uv_buf_t *buf) {
  free(buf->base);
}

static uv_tcp_t tcp_client;
static uv_connect_t tcp_connection;

void client_connect(uv_loop_t *uv_loop, const char *ip, int port) {
  struct sockaddr_in server_addr;
  uv_ip4_addr(ip, port, &server_addr);

  uv_tcp_init(uv_loop, &tcp_client);

  uv_tcp_connect(&tcp_connection, &tcp_client, (const struct sockaddr *)&server_addr, client_on_connect);
}

void client_disconnect() {
  uv_close((uv_handle_t *)&tcp_client, client_on_close);
  fflush(stdout);
  client_running = false;
}

void client_on_connect(uv_connect_t *connection, int status) {
  uv_stream_t *stream = connection->handle;

  if (status < 0) {
    printf("error");
  }
  printf("done\n");

  client_send_handshake(stream);
  uv_read_start(stream, buf_alloc, client_on_read);
}

void client_on_read(uv_stream_t *stream, ssize_t length, const uv_buf_t *buf) {
  buf_free(buf);
}

void client_on_close(uv_handle_t *handle) {
}

void client_on_write(uv_write_t* req, int status) {
}

void client_send_handshake(uv_stream_t *stream) {
}
