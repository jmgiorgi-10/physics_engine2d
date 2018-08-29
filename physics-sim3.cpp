#include <chrono>
#include <cstdlib>
#include <Eigen/Dense>
#include <iostream>
#include <math.h>
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
// OPERATOR OVERLOADS.
  Vector2d operator+(const Vector2d& v2) {
  	Vector2d vout;
  	vout.x = this->x + v2.x;
  	vout.y = this->y + v2.y;
  	return(vout);
  }

  Vector2d operator-(const Vector2d& v2) {
	  Vector2d vout;
	  vout.x = this->x - v2.x;
	  vout.y = this->y - v2.y;
	  return(vout);
  }
  // Dot product.
  float operator*(const Vector2d& v2) {
    float out = this->x * v2.x + this->y * v2.y;
	  return(out);
  }
  Vector2d operator*(const float scalar) {
    Vector2d vout;
    vout.x = this->x * scalar;
    vout.y = this->y * scalar;
    return(vout);
  }
};

Vector2d normalize(Vector2d vec) {
	float length = sqrt(vec.x * vec.x + vec.y * vec.y);
	vec.x = vec.x / length;
	vec.y = vec.y / length;
	return(vec);
}

Vector2d leftNormal(Vector2d vec) {
	Vector2d vout;
	vout.x = -vec.y;
	vout.y = vec.x;
  return(vec);
}

float mag(Vector2d vec) {
	float out = sqrt(vec.x * vec.x + vec.y * vec.y);
	return(out);
}

Vector2d RandomPosition() {
  Vector2d vout;
  vout.x = (rand() % (SCREEN_WIDTH - 50) + 50)/10; // Divided by 10 for conversion to WORLD coordinates.
  vout.y = (rand() % (SCREEN_HEIGHT - 50) + 50)/10;
  return(vout);
}

struct Particle {
	  Vector2d acceleration;
    Vector2d gravity;
	  Vector2d position;
	  Vector2d velocity;
	  Vector2d point1, point2, point3, point4;
	  vector<Vector2d *> Points;
	  Vector2d point1_r, point2_r, point3_r, point4_r;
    Vector2d collision_normal, collision_point;
	  float rot;   // rotation.
	  float vel_rot;  // angular velocity.
	  float dt;
	  int mass;
	  int width;
	  int height;
	  float I;
	  SDL_Rect r;  // graphics.
    bool ground1;

    float restitution;

    float min_overlap;

	  SDL_Renderer * renderer;

	  Particle(int width1, int height1, int mass1, Vector2d position1, Vector2d velocity1, float rot1, bool ground, float I1) {  // Initial conditions.
	  	// Physics values
	    velocity = velocity1;
	    position = position1;  // Position coordinate corresponds to center of particle.
	    acceleration.x = 0;
	    acceleration.y = 0;
      gravity.x = 0;
      gravity.y = 0;
      ground1 = ground;
      if (!ground) {
        gravity.y = -9.8;
        //vel_rot = 0.8;
      }
	    mass = mass1;
	    width = width1;
	    height = height1;
	    rot = rot1;

	    Points.push_back(&point1);
	    Points.push_back(&point2);
	    Points.push_back(&point3);
	    Points.push_back(&point4);

      //vel_rot = 0.8;
      restitution = .5;

	    dt = .033333333;

      //I = (1/12) * mass * (pow(width, 2) + pow(height, 2));

      I = I1;

	  }

    void PositionCorrection(Particle * p2, float overlap) {
      float percent = 10;  // Percent overlap to adjust position by.
      Vector2d pos_correct = collision_normal * overlap * percent;
      if (!ground1) {
        position = position - pos_correct;
      }
      
      if (!p2->ground1) {
        p2->position = p2->position + pos_correct;
      }
    }

	  void UpdatePoints() {
	  	// Set points, then change with orientation.
	  	point1.x = position.x - width/2;
	  	point1.y = position.y + height/2;
	  	point2.x = position.x + width/2;
	  	point2.y = position.y + height/2;
      point3.x = position.x + width/2;
      point3.y = position.y - height/2;
      point4.x = position.x - width/2;
      point4.y = position.y - height/2;

      // Adjust orientation.
      point1 = RotatePoint(point1);
      point2 = RotatePoint(point2);
      point3 = RotatePoint(point3);
      point4 = RotatePoint(point4);
	  }

	  void Step(float time_step) {
      // INTEGRATION //

      velocity = velocity + gravity*time_step;
      position = position + velocity*time_step;

	  	rot += vel_rot * time_step;

	  	UpdatePoints();
	  }

    void StepBack(float time_step) {

      rot -= vel_rot * time_step;

      position = position - velocity*time_step;
      velocity = velocity - gravity*time_step;
    }
 
	  bool CollisionCheck2(Particle * p2) {     

      float contact_point; 
	  	// Get axes
      Vector2d axis1 = point2 - point1;
      axis1 = normalize(axis1);
      axis1 = leftNormal(axis1);  // Actually giving normal to each axis. (for use in SAT).
      Vector2d axis2 = point3 - point2;
      axis2 = normalize(axis2);
      axis2 = leftNormal(axis2);
      vector<Vector2d> Axes = {axis1, axis2};

      Vector2d Min1, Max1, Min2, Max2;

      //Vector2d C = position-p2->position;

      min_overlap = 10000;

      for (auto axis : Axes) {
      	float min1 = 10000;
      	float max1 = -10000;
        for (auto point : p2->Points) {
      	  Vector2d A = *point;  // get point stored as ptr.
          float proj = A*axis;
          if (proj > max1) {
          	max1 = proj;
            Max1 = A;
          }
          if (proj < min1) {
          	min1 = proj;
            Min1 = A;
          }
        }
        float min2 = 10000;
      	float max2 = -10000;     
        // If collide, axis with smallest overlap gives normal & contact point.
        if (max1 > min2) {
          float overlap = max1 - min2;
          if (overlap < min_overlap) {
            min_overlap = overlap;
            collision_normal = axis;
            collision_point = Min2;
          }
        }
        if (max2 > min1) {
          float overlap = max2 - min1;
          if (overlap < min_overlap) {
            min_overlap = overlap;
            collision_normal = axis;
            collision_point = Min1;
          }
        }
    
        if (min1 > max2 || min2 > max1) {
        	return(false);  // No collision b/c seperating axis found.
        }
      }
      cout << "COLLISION" << '\n';
      return(true);
	  }

    void CollisionResponse(Particle * p2) {
      //Vector2d relative_vel = velocity - p2->velocity;
      Vector2d radius_AP = collision_point - position;
      radius_AP = leftNormal(radius_AP);
      radius_AP = normalize(radius_AP);
      Vector2d radius_BP = collision_point - p2->position;
      radius_BP = leftNormal(radius_BP);
      radius_BP = normalize(radius_BP);

      Vector2d vel_AP = velocity + radius_AP*vel_rot;
      Vector2d vel_BP = p2->velocity + radius_BP*(p2->vel_rot);
      Vector2d relative_vel = vel_AP - vel_BP;

      float impulse = -((1 + restitution) * (relative_vel*collision_normal)) / ((1/mass + 1/p2->mass) + pow(radius_AP*collision_normal, 2)/I 
        + pow(radius_BP*collision_normal, 2)/(p2->I));

      //  Update velocities from impulse.
      velocity = velocity + collision_normal*(impulse/mass);
      p2->velocity = p2->velocity - collision_normal*(impulse / (p2->mass));

      vel_rot = vel_rot + (radius_AP * (collision_normal*impulse)) / I;
      p2->vel_rot = p2->vel_rot - (radius_BP * (collision_normal*impulse)) / (p2->I);
    }

	  Vector2d RotatePoint(Vector2d point) {
      Vector2d rotated_point;
        // translation so center (axis of rotation) is origin, then rotation.
      rotated_point.x = (point.x - position.x) * cos(rot) - (point.y - position.y) * sin(rot);
      rotated_point.y = (point.x - position.x) * sin(rot) + (point.y - position.y) * cos(rot);
        // translate center back to OG coordinates.
      rotated_point.x += position.x;
      rotated_point.y += position.y; 
      return(rotated_point);
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
    // Converting Between World & Sreen Coordinates.
	  void Render() {
      width = width * METER_TO_PIXEL;
      height = height * METER_TO_PIXEL;
      position = MetersToPixels(position);
      UpdatePoints();

      point1_r = point1;
      point2_r = point2;
      point3_r = point3;
      point4_r = point4;

      //cout << point1_r.x << ' ' << point1_r.y << '\n';

       // Change position units back to meters.
      width = width * PIXEL_TO_METER;
      height = height * PIXEL_TO_METER;
      position = PixelsToMeters(position);
      UpdatePoints();  // Reupdate points. 

      //cout << point1.x << ' ' << point1.y << '\n'; 
	  }
};

// Loop through all possible collision checks (WITHOUT duplicates).
// If twe vertices intersect, set contact point at middle of edge.
void Collisions(vector<Particle*> Particles) {
  int index = 0;
  for (auto ptr : Particles) {
    for (int i = index + 1; i < Particles.size(); i++) {
      Particle * ptr2 = Particles.at(i);

      float step = .0333333333;
      float new_step = step;
      float rem_step = 0;
      // If in contact with ground, set gravity to zero -> AVOID penetration.
      // if (ptr->ground1 || ptr2->ground1) {
      //   ptr->gravity.y = 0;
      //   ptr2->gravity.y = 0;
      // }

      if (ptr->CollisionCheck2(ptr2)) {
        //while (ptr->CollisionCheck2(ptr2)) {
        
       
        // Step backwards one integration step.
       // ptr->StepBack(step);
        //ptr2->StepBack(step);
        // Divide new integration step by 2, and go forward that amount
        //new_step = new_step / 2;
        //ptr->Step(new_step);
        //ptr2->Step(new_step);
        //}
        // Catch up to full integration step.
        ptr->CollisionResponse(ptr2);
        //ptr->Step(step - new_step);
        //ptr2->Step(step - new_step);

        

      //}



      
    }
    index++;
  }
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

	Particle * p;

  Vector2d velocity1;
  velocity1.x = 0;
  velocity1.y = 0;
  Vector2d position1;
  position1.x = 20;
  position1.y = 20;

  Vector2d velocity2;
  velocity2.x = 0;
  velocity2.y = 0;
  Vector2d position2;
  position2.x = 20;
  position2.y = 40;

  Vector2d position3;
  position3.x = (SCREEN_WIDTH/2)/10;
  position3.y = 1.5;

	Particle p1(2, 2, 1, position1, velocity1, 0, false, 5);
	Particle p2(2, 2, 1, position2, velocity2, 1, false, 5);
  Particle p3(3, 2, 1, RandomPosition(), velocity2, 1, false, 6);
  Particle p4(3, 2, 1, RandomPosition(), velocity2, 0, false, 6);
  Particle p5(3, 2, 1, RandomPosition(), velocity2, 0, false, 6);
  Particle p6(3, 2, 1, RandomPosition(), velocity2, 1, false, 6);
  Particle p7(3, 2, 1, RandomPosition(), velocity2, 1, false, 6);
  Particle p8(3, 2, 1, RandomPosition(), velocity2, 1, false, 6);
  Particle p9(3, 2, 1, RandomPosition(), velocity2, 1, false, 6);
  Particle p10(64, 3, 100000, position3, velocity1, 0, true, 100000);
	
  Particles.push_back(&p1);
	Particles.push_back(&p2);
  Particles.push_back(&p3);
  Particles.push_back(&p4);
  Particles.push_back(&p5);
  Particles.push_back(&p6);
  Particles.push_back(&p7);
  Particles.push_back(&p8);
  Particles.push_back(&p9);
  Particles.push_back(&p10);

	lastTime = SDL_GetTicks();



	// Main physics loop
	while(1) {
    // Keep rendering at all times.
   
    //lastTime = currentTime;
    currentTime = SDL_GetTicks();

    // Should be allowed to pass in the first iteration.

    //Update Step at specified frequency.
    //cout << currentTime - lastTime << '\n';
		if (currentTime - lastTime > 33.33) { // 33.3 ms period -> 30 Hz loop. 


       SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
       SDL_RenderClear(renderer);

      for (int i = 0; i < Particles.size(); i++) {
        p = Particles.at(i);
        p->Render();
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
        SDL_RenderDrawLine(renderer, p->point1_r.x, SCREEN_HEIGHT - p->point1_r.y, p->point2_r.x, SCREEN_HEIGHT - p->point2_r.y);
        SDL_RenderDrawLine(renderer, p->point2_r.x, SCREEN_HEIGHT - p->point2_r.y, p->point3_r.x, SCREEN_HEIGHT - p->point3_r.y);
        SDL_RenderDrawLine(renderer, p->point3_r.x, SCREEN_HEIGHT - p->point3_r.y, p->point4_r.x, SCREEN_HEIGHT - p->point4_r.y);
        SDL_RenderDrawLine(renderer, p->point4_r.x, SCREEN_HEIGHT - p->point4_r.y, p->point1_r.x, SCREEN_HEIGHT - p->point1_r.y);

      }
      SDL_RenderPresent(renderer);  // Draw on window.


	    for (int i = 0; i < Particles.size(); i++) {
	    	p = Particles.at(i);
	    	p->Step(.03333333);
        // ALL Particles take step forward.
	    }  
        Collisions(Particles);  // Check ALL possible collisions, adjusting positions & velocities.
        lastTime = SDL_GetTicks();
	  }
	  
  }
	return(0);
}
