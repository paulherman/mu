#include <stdio.h>
#include "server.h"
#include "entity.h"
#include "log.h"
#include "account.h"
#include "server_connection.h"

struct server_state state;

int main(int argc, char **argv) {
  state.uv_loop = uv_default_loop();
  server_listen(SERVER_PORT);
  uv_run(state.uv_loop, UV_RUN_DEFAULT);
  uv_loop_close(state.uv_loop);

  return 0;
}
