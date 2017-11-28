#include <stdbool.h>

#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "uv.h"

#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "renderable.h"
#include "transformation.h"
#include "light.h"
#include "error.h"
#include "bmd.h"
#include "log.h"
#include "client.h"
#include "client_connection.h"
#include "const.h"
#include "map.h"

struct client_state state;

int main(int argc, char **argv) {
  SDL_Window *window;
  SDL_GLContext gl_context;
  SDL_Init(SDL_INIT_VIDEO);
  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  window = SDL_CreateWindow("MU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  gl_context = SDL_GL_CreateContext(window);
  glewInit();
  glEnable(GL_CULL_FACE);

  state.uv_loop = uv_default_loop();
  client_connect(SERVER_ADDRESS, SERVER_PORT);

  struct shader shader;
  for (int err = shader_load(&shader, "./res/shader.vert", "./res/shader.frag"); err != SUCCESS; err = SUCCESS)
    log_error("Error loading static shader - %d", err - INT_MIN);

  struct camera camera = camera_new(rad(70.0), 0.1, 100.0, 0, 0.0, 0, 0, 0.0, 0.0);

  size_t map_id = 25;
  if (argc == 2) {
    map_id = atoi(argv[1]);
  }

  struct map_client m;
  if (!map_client_load(&m, map_id)) {
    return 0;
  }

  while (state.running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          client_disconnect();
          break;
        default:
          break;
      }
    }
    

    const uint8_t *keyboard_state = SDL_GetKeyboardState(NULL);
    if (keyboard_state[SDL_SCANCODE_W]) {
      camera_move(&camera, 0, 0.0, 1);
    }
    if (keyboard_state[SDL_SCANCODE_S]) {
      camera_move(&camera, 0, 0.0, -1);
    }
    if (keyboard_state[SDL_SCANCODE_A]) {
      camera_move(&camera, -1, 0.0, 0.0);
    }
    if (keyboard_state[SDL_SCANCODE_D]) {
      camera_move(&camera, 1, 0.0, 0.0);
    }
    if (keyboard_state[SDL_SCANCODE_Q]) { 
      camera_move(&camera, 0, -1, 0.0);
    }
    if (keyboard_state[SDL_SCANCODE_E]) {
      camera_move(&camera, 0, 1, 0.0);
    }

    if (keyboard_state[SDL_SCANCODE_UP]) {
      camera_rotate(&camera, rad(5.0), 0.0, 0.0);
    }
    if (keyboard_state[SDL_SCANCODE_DOWN]) {
      camera_rotate(&camera, -rad(5.0), 0.0, 0.0);
    }
    if (keyboard_state[SDL_SCANCODE_LEFT]) {
      camera_rotate(&camera, 0, rad(-5.0), 0.0);
    }
    if (keyboard_state[SDL_SCANCODE_RIGHT]) {
      camera_rotate(&camera, 0, rad(5.0), 0.0);
    }

    uv_run(state.uv_loop, UV_RUN_NOWAIT);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    map_client_render(&m, &shader, &camera);

    SDL_GL_SwapWindow(window);
  }

  shader_delete(&shader);
  map_client_delete(&m);

  uv_loop_close(state.uv_loop);

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
  return 0;
}
