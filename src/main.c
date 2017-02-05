#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <stdbool.h>
#include "mesh.h"
#include "shader.h"
#include "texture.h"
#include "renderable.h"
#include "transformation.h"
#include "light.h"
#include "error.h"
#include "bmd.h"

int main(int argc, char **argv) {
  SDL_Window *window;
  SDL_GLContext gl_context;
  SDL_Init(SDL_INIT_VIDEO);
  int img_error = IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);
  if (img_error != IMG_INIT_PNG) {
    printf("IMG_Init: %s\n", IMG_GetError());
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  window = SDL_CreateWindow("muie", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
  gl_context = SDL_GL_CreateContext(window);
  glewInit();

  struct mesh stall;
  mesh_load_obj(&stall, "./res/stall.obj");

  struct renderable stall_renderable;
  printf("%d\n", renderable_load_mesh(&stall_renderable, &stall));

  struct shader shader;
  printf("%d\n", shader_load(&shader, "./res/shader.vert", "./res/shader.frag"));

  struct texture texture;
  printf("%d\n", texture_load_ozt(&texture, "./res/ship05.OZT"));

  struct transformation transformation = transformation_new(0, 0.0, -10.0, 0.0, 3.14, 0.0, 0.5);

  struct camera camera = camera_new(1.22, 0.1, 1000.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0);

  struct buffer lights = buffer_new(1, sizeof(struct light));
  *(struct light *)buffer_get(&lights, 0) = light_new(0, 0, 0, 1.0, 1.0, 1.0);

  struct bmd_mesh bmd;
  printf("%d\n", bmd_load(&bmd, "./res/Ship01.bmd"));
  for (size_t i = 0; i < bmd.num_objects; i++) {
    printf("%s\n", bmd.objects[i].texture);
  }

  bool running = true;
  while (running) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_KEYDOWN:
          break;
        case SDL_KEYUP:
          break;
        case SDL_QUIT:
          running = false;
          break;
        default:
          break;
      }
    }
    transformation_rotate(&transformation, 0.0, 0.01, 0.0);

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.0, 0.0, 0.0, 1.0);

    if (renderable_render(&stall_renderable, &shader, &texture, &camera, &transformation, &lights) != SUCCESS)
      fprintf(stderr, "Rendering error with VAO %u and shader program %u\n", stall_renderable.vertex_array_object, shader.program);

    SDL_GL_SwapWindow(window);
  }

  bmd_delete(&bmd);
  shader_delete(&shader);
  renderable_delete(&stall_renderable);
  mesh_delete(&stall);
  SDL_GL_DeleteContext(gl_context);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();
  return 0;
}
