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
  window = SDL_CreateWindow("MU", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  gl_context = SDL_GL_CreateContext(window);
  glewInit();

  state.uv_loop = uv_default_loop();
  client_connect(SERVER_ADDRESS, SERVER_PORT);

  struct shader shader;
  shader_load(&shader, "./res/shader.vert", "./res/shader.frag");

  struct transformation transformation = transformation_new(0, 0.0, -10.0, 0.0, 0, 0.0, 0.1);
  struct camera camera = camera_new(deg(70.0), 0.1, 1000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);
  struct vec3f look_at = vec3f_new(0.0, 1.0, 10.0);
  camera_look_at(&camera, look_at);

  struct buffer lights = buffer_new(1, sizeof(struct light));
  *(struct light *)buffer_get(&lights, 0) = light_new(0, 0, 0, 1.0, 1.0, 1.0);

  struct bmd_entity bmd;
  bmd_load(&bmd, "./res/Sword01.bmd", "./res/");

  struct map_client m;
  map_client_load(&m, 2);

  bool key_up = false, key_down = false;
  while (state.running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYDOWN: {
          SDL_KeyboardEvent keyboard_ev = event.key;
          switch (keyboard_ev.keysym.scancode) {
            case SDL_SCANCODE_W: 
              key_up = true;
              break;
            case SDL_SCANCODE_S: 
              key_down = true;
              break;
            default:
              break;
          }
          break;
        }
        case SDL_KEYUP: {
          SDL_KeyboardEvent keyboard_ev = event.key;
          switch (keyboard_ev.keysym.scancode) {
            case SDL_SCANCODE_W: 
              key_up = false;
              break;
            case SDL_SCANCODE_S: 
              key_down = false;
              break;
            default:
              break;
          }
          break;
        }
        case SDL_MOUSEMOTION: {
          SDL_MouseMotionEvent mouse_motion_ev = event.motion;
          camera_rotate(&camera, 0.0, (1.0 * mouse_motion_ev.xrel) / (1.0 * 512), 0.0);
          break;
        }
        case SDL_QUIT:
          client_disconnect();
          break;
        default:
          break;
      }
    }

    uv_run(state.uv_loop, UV_RUN_NOWAIT);

    transformation_rotate(&transformation, 0.0, 0.01, 0.0);

    struct vec3f *camera_rotation = &camera.transformation.rotation;
    if (key_up) {
      struct vec4f trans = vec3f_to_vec4f(vec3f_new(0.0, 0.0, -0.1), 1.0);
      struct vec3f move = vec4f_to_vec3f(mat4f_dot(mat4f_rotation(*camera_rotation), trans));
      camera_move(&camera, move.x, move.y, move.z);
    }
    if (key_down) {
      struct vec4f trans = vec3f_to_vec4f(vec3f_new(0.0, 0.0, 0.1), 1.0);
      struct vec3f move = vec4f_to_vec3f(mat4f_dot(mat4f_rotation(*camera_rotation), trans));
      camera_move(&camera, move.x, move.y, move.z);
    }

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    bmd_render(&bmd, &shader, &camera, &transformation, &lights);

    SDL_GL_SwapWindow(window);
  }

  bmd_delete(&bmd);
  shader_delete(&shader);

  uv_loop_close(state.uv_loop);

  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
  return 0;
}
