#include <stdio.h>
#include "server.h"
#include "entity.h"
#include "log.h"
#include "account.h"
#include "server_connection.h"

int main(int argc, char **argv) {
  uv_loop_t *uv_loop = uv_default_loop();
  server_listen(uv_loop, SERVER_PORT);
  uv_run(uv_loop, UV_RUN_DEFAULT);
  uv_loop_close(uv_loop);

  return 0;
}
