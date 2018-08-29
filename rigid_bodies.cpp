// Joaquin Matias Giorgi jmgiorgi@bu.edu Impulse-Based Physics Engine 2D //
#include <iostream>
#include <math.h>
#include <SDL2/SDL.h>
#include <vector>
#include "polygon.h"
#include "rendering.h"

using namespace std;

//Declaration
vector<RigidBody*> RigidBodies;

// VECTOR STUFF //
  // Multiplication by scalar quantity.
  Vector Vector::operator*(const float scalar) {
	  Vector vout;
	  vout.x = this->x * scalar;
	  vout.y = this->y * scalar;
	  return(vout);
  }
  // Addition overload.
  Vector Vector::operator+=(const Vector vec) {
  	Vector vout;
  	vout.x = this->x + vec.x;
  	vout.y = this->y + vec.y;
  	return(vout);
  }

float dot (Vector vec1, Vector vec2) {
	float out = (vec1.x * vec2.x) + (vec1.y * vec2.y);
	return(out);
}

float cross2d (Vector vec1, Vector vec2) {
	// magnitude of perpendicular vector in 3d case.
	float out = (vec1.x * vec2.y) - (vec1.y * vec2.x);
	return(out);
}

// NEW BODIES //

void NewRigidBody(RigidBody * r_new) {
  RigidBodies.push_back(r_new);
}

// POLYGON Struct Methods //

Polygon::Polygon(vector< Vector * > Points1, Material m1, float volume1) {
    Points = Points1;
    m = m1;
    volume = volume1;
  }

 float Polygon::ComputeMass() {
    float mass = m.density * volume;
    return(mass);
 }
 // RIGID BODY Struct Methods //

	RigidBody::RigidBody(Vector position1, Polygon * p1) {
    position = position1;
    p = p1;
    mass = p1->ComputeMass();
    orientation = 0;
    angularVelocity = 0;
    dt = .033;
    gravity.x = 0;
    gravity.y = -9.8;
    velocity.x = 0;
    velocity.y = 0;
	}

  // UPDATE at each iteration.	
	void RigidBody::Step() {
    velocity += gravity * dt;
    position += velocity * dt;
    orientation += angularVelocity * dt;
	}


// MAIN
int main() {
	unsigned int lastTime = SDL_GetTicks();
	unsigned int currentTime;
	SDL_Renderer* renderer = InitializeRender();

	while(1) {
    currentTime = SDL_GetTicks();
    if (currentTime - lastTime > 33) {
    	Render(renderer, RigidBodies);
    	lastTime = SDL_GetTicks();
    }
	}

}
