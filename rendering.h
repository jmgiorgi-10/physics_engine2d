
#include <vector>

#ifndef RENDERING_H
#define RENDERING_H

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

SDL_Renderer* InitializeRender();
void Render(SDL_Renderer * , std::vector< RigidBody * >);

#endif