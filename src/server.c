#include <stdio.h>
#include "server.h"
#include "entity.h"
#include "log.h"
#include "account.h"
#include "server_connection.h"

struct server_state state;

int main(int argc, char **argv) {
  state.uv_loop = uv_default_loop();
  if (!server_listen(SERVER_PORT)) {
    log_error("Unable to listen on port %d", SERVER_PORT);
  } else {
    log_info("Listening on port %d", SERVER_PORT);
    uv_run(state.uv_loop, UV_RUN_DEFAULT);
  }
  uv_loop_close(state.uv_loop);

  return 0;
}
