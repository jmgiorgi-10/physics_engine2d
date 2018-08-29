
#include <vector>

#ifndef POLYGON_H
#define POLYGON_H

struct Vector {
	float x;
	float y;
  // Multiplication by scalar quantity.
  Vector operator*(const float);
  // Addition overload.
  Vector operator+=(const Vector);
};

struct Material {
	float density;
	float restitution;
};

struct Polygon {
  std::vector< Vector * > Points;
  float volume;
  Material m;

  Polygon(std::vector< Vector * >, Material, float);
  float ComputeMass();
};

struct RigidBody {
	float mass;
	float volume;
	float dt;
	// Linear
	Vector position;
	Vector gravity;
    Vector velocity;
	float acceleration;
	// Angular
	float orientation;
	float angularVelocity;
	float torque;
	Polygon * p;
  // Constructor
	RigidBody(Vector, Polygon*);
  // UPDATE at each iteration.	
	void Step();
};
// DECLARATION
extern std::vector<RigidBody*> RigidBodies;  // Global Vector of RigidBody Pointers.

#endif