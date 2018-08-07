#include <chrono>
#include <Eigen/Dense>
#include <iostream>
#include <thread>
#include <ratio>
#include <time.h>
#include <vector>
#include <SDL2/SDL.h>
#include <stdio.h>

using namespace std::chrono;
using namespace std;
//using namespace Eigen;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int METER_TO_PIXEL = 10;
const float PIXEL_TO_METER = 0.1;

unsigned int currentTime;
unsigned int lastTime;

struct Vector2d {
  float x;
  float y;
};

struct Particle {
	  Vector2d acceleration;
	  Vector2d acc_rot;
	  Vector2d position;
	  Vector2d velocity;
	  Vector2d point1, point2, point3, point4;
	  Vector2d point1_r, point2_r, point3_r, point4_r;
	  float rot;   // rotation.
	  float vel_rot;  // angular velocity.
	  float dt;
	  int mass;
	  int width;
	  int height;
	  SDL_Rect r;  // graphics.

	  SDL_Renderer * renderer;

	  Particle(int width1, int height1, int mass1, Vector2d position1, Vector2d velocity1, SDL_Renderer * renderer) {  // Initial conditions.
	  	// Physics values
	    velocity = velocity1;
	    position = position1;  // Position coordinate corresponds to center of particle.
	    acceleration.x = 0;
	    acceleration.y = -9.81;
	    mass = mass1;
	    width = width1;
	    height = height1;
	    dt = .033333333;
      // Render values
	    r.w = width;
	    r.h = height;

	  }

	  void Points() {
	  	// Set points, then change with orientation.
	  	point1.x = position.x - width/2;
	  	point1.y = position.y + height/2;
	  	point2.x = position.x + width/2;
	  	point2.y = position.y + height/2;
      point3.x = position.x + width/2;
      point3.y = position.y - height/2;
      point4.x = position.x - width/2;
      point4.y = position.y - height/2;
	  }

	  void Step() {
	  	velocity.x += acceleration.x * dt;
	  	velocity.y += acceleration.y * dt;
	  	position.x += velocity.x * dt;
	  	position.y += velocity.y * dt;

	  	Points();

      // Adjust orientation.
      // point1 = RotatePoint(point1);
      // point2 = RotatePoint(point2);
      // point3 = RotatePoint(point3);
      // point4 = RotatePoint(point4);
	  }

	  Vector2d RotatePoint(Vector2d point) {
      Vector2d rotated_point;
      rotated_point.x = point.x * cos(rot) - point.y * sin(rot);
      rotated_point.y = point.x * sin(rot) + point.y * cos(rot);
      return(rotated_point);
	  }

	  void Print() {
	  // 	cout << position.x << ' ';
			// cout << position.y << ' ';
   //  	cout << velocity.x << ' ';
   //  	cout << velocity.y << '\n';

   //  	cout << point1_r.x << ' ';
			// cout << point1_r.y << ' ';
   //  	cout << point2_r.x << ' ';
   //  	cout << point2_r.y << ' ';
   //  	cout << point3_r.x << ' ';
   //  	cout << point3_r.y << '\n';

      //cout << position.x << ' ';
      //cout << position.y << '\n';
    	 


	  }

    Vector2d MetersToPixels(Vector2d Vec) {
    	Vec.x = Vec.x * METER_TO_PIXEL;
    	Vec.y = Vec.y * METER_TO_PIXEL;
    	return(Vec);
    }

    Vector2d PixelsToMeters(Vector2d Vec) {
    	Vec.x = Vec.x * PIXEL_TO_METER;
    	Vec.y = Vec.y * PIXEL_TO_METER;
    	return(Vec);
    }

	  void Render() {
      // Convert to pixels for position rendering.
        // Changue position units to pixels.
      position = MetersToPixels(position);
      cout << position.x << '\n';
      Points();

      point1_r = point1;
      point2_r = point2;
      point3_r = point3;
      point4_r = point4;

       // Change position units back to meters.
      position = PixelsToMeters(position);
      Points();  // Reupdate points. 
	  }
};

void DrawTest(SDL_Renderer * renderer) {
	SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
	//SDL_RenderDrawLine(renderer, 50, 50, 100, 100);
	//SDL_RenderDrawLine(renderer, 20, 20, 40, 40);

	//Particle * particle_ptr = &(Particles.at(0));

  //SDL_RenderDrawLine(renderer, particle_ptr->point1_r.x, SCREEN_HEIGHT - point1_r.y, point2_r.x, SCREEN_HEIGHT - point2_r.y);
      // SDL_RenderDrawLine(renderer, point2_r.x, SCREEN_HEIGHT  - point2_r.y, point3_r.x, SCREEN_HEIGHT - point3_r.y);
      // SDL_RenderDrawLine(renderer, point3_r.x, SCREEN_HEIGHT - point3_r.y, point4_r.x, SCREEN_HEIGHT - point4_r.y);
      // SDL_RenderDrawLine(renderer, point4_r.x, SCREEN_HEIGHT  - point4_r.y, point1_r.x, SCREEN_HEIGHT  - point1_r.y);

	SDL_RenderPresent(renderer);
}

bool GetCollision(Particle * ptr1, Particle * ptr2) {
	float diff = abs(ptr1->position.x + (ptr1->width)/2 - (ptr1->position.x + (ptr1->width)/2 ));
  if (diff <= 0) {
  	return(true);  // There was a collision
  }
  diff = abs(ptr1->position.y + (ptr1->height)/2 - (ptr1->position.y + (ptr1->height)/2 ));
  if (diff <= 0) {
  	return(true);  // There was a collision
  }
  return(false);  // If neither condition true, no collision
}

int main(int argc, char **argv) {
   SDL_Window * window = NULL;
    window = SDL_CreateWindow
    (
        "Jeu de la vie", SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        640,
        480,
        SDL_WINDOW_SHOWN
    );

    // Setup renderer
    SDL_Renderer* renderer = NULL;
    renderer =  SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Set render color to red ( background will be rendered in this color )
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255 );

    // Clear winow
    SDL_RenderClear(renderer);

	  // Vector of Particle instances.
  vector<Particle*> Particles;

	Particle * particle_ptr;

	SDL_Rect * rects[2];

  Vector2d velocity1;
  velocity1.x = 0;
  velocity1.y = 0;
  Vector2d position1;
  position1.x = 10;
  position1.y = 10;

  Vector2d velocity2;
  velocity2.x = 0;
  velocity2.y = 0;
  Vector2d position2;
  position2.x = 30;
  position2.y = 30;

	Particle p1(20, 20, 1, position1, velocity1, renderer);
	Particle p2(20, 20, 1, position2, velocity2, renderer);
	Particles.push_back(&p1);
	Particles.push_back(&p2);

	lastTime = SDL_GetTicks();

	// Main physics loop
	while(1) {
    currentTime = SDL_GetTicks();
    // Keep rendering at all times.
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	  SDL_RenderClear(renderer);

	  for (int i = 0; i < Particles.size(); i++) {
      Particle * p = Particles.at(i);
      p->Render();
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      SDL_RenderDrawLine(renderer, p->point1_r.x, SCREEN_HEIGHT - p->point1_r.y, p->point2_r.x, SCREEN_HEIGHT - p->point2_r.y);
      SDL_RenderDrawLine(renderer, p->point2_r.x, SCREEN_HEIGHT - p->point2_r.y, p->point3_r.x, SCREEN_HEIGHT - p->point3_r.y);
      SDL_RenderDrawLine(renderer, p->point3_r.x, SCREEN_HEIGHT - p->point3_r.y, p->point4_r.x, SCREEN_HEIGHT - p->point4_r.y);
      SDL_RenderDrawLine(renderer, p->point4_r.x, SCREEN_HEIGHT - p->point4_r.y, p->point1_r.x, SCREEN_HEIGHT - p->point1_r.y);

	  }
	  SDL_RenderPresent(renderer);  // Draw on window.





    //Update Step at specified frequency.
		if (currentTime - lastTime > 33) { // 33 ms period -> 30 Hz loop.
                 
	    for (int i = 0; i < Particles.size(); i++) {
	    	particle_ptr = Particles.at(i);
	    	particle_ptr->Step();
	    	particle_ptr->Print();

	    }  // loop through steps of Particle instances.

        lastTime = SDL_GetTicks();
	  }
	  
  }
	return(0);
}
