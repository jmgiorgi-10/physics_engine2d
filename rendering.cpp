
#include <SDL2/SDL.h>
#include "polygon.h"
#include "rendering.h"

SDL_Renderer* InitializeRender() {
	  SDL_Window * window = NULL;
    window = SDL_CreateWindow
    (
        "RIGID BODIES SIM", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN
    );

    // Setup renderer
    SDL_Renderer * renderer = NULL;
    renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    return(renderer);
}

void Render(SDL_Renderer * renderer, std::vector< RigidBody * > RigidBodies) {
	float scale = 10;  // METERS to PIXELS.
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // Clear screen.
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);  // Set polygon drawing color (GREEN)

  for (auto RigidBody : RigidBodies) {
  	Polygon * polyPtr = RigidBody->p;
  	int size = polyPtr->Points.size();
  	for (int i = 0; i < size; i++) {
  		auto pointPtr1 = polyPtr->Points.at(i);
  		auto pointPtr2 = polyPtr->Points.at(i % (size - 1));  // Modulo so last point links back up to first one.
      SDL_RenderDrawLine(renderer, pointPtr1->x * scale, SCREEN_HEIGHT - pointPtr1->y * scale, 
      	pointPtr2->x, SCREEN_WIDTH - pointPtr2->y * scale);
  	}
  }
  SDL_RenderPresent(renderer);
}
