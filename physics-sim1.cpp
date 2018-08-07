#include <chrono>
#include <iostream>
#include <thread>
#include <time.h>
#include <vector>

using namespace std::chrono;

int NUM_PARTICLES;
vector<Particle> particles;
Particle * particle;
particles.reserve(NUM_PARTICLES);
double dt;

struct Vector2d {  
  float x;
  float y;
}

struct Particle
{
	Vector2d position;
	Vector2d velocity;
	float mass;
};

void PrintParticles() {
	for (int i = 0; i < NUM_PARTICLES; I++) {
		particle = &(particles.at(i)); 
		cout << particle->position.x << ' ';
		cout << particle->position.y << ' ';
    cout << particle->velocity.x << ' ';
    cout << particle->velocity.y << '\n';
	}
}

void GetTime() {
	milliseconds ms = duraction_cast<milliseconds>(
		system_clock::now().time_since_epoch().count()
	);
}

void SimStep() {  // One more step in simulation.
  for (int i = 0; i < NUM_PARTICLES; i++) {
  	particle = &(particles.at(i));
  	(*particle).velocity.x += acceleration.x * dt;
  	(*particle).velocity.y += acceleration.y * dt;
    (*particle).position.x += particle->velocity.x * dt;
    (*particle).position.y += particle->velocity.y * dt;
  }
}

void InitParticles() {
	 
}

int main() {
  
  dt = 0.5; // Time step for simulation.
  NUM_PARTICLES = 3;

  while (1) {
    SimStep();
    // Sleep for 0.5 seconds.
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  

  return(0);
}