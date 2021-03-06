#if !defined(STAR_HH)
#define STAR_HH

#ifdef __CUDACC__
#define CUDA_CALLABLE_MEMBER __host__ __device__
#else
#define CUDA_CALLABLE_MEMBER
#endif 

#include <cmath>

#include "vec2d.hh"

class star {
public:
  // Create a new star with a given position and velocity
  CUDA_CALLABLE_MEMBER star(double mass, vec2d pos, vec2d vel, rgb32 color) : 
    _mass(mass),
    _pos(pos),
    _vel(vel),
    _force(0, 0),
    _color(color),
    _initialized(0) {}

  star() {}
  
  // Update this star's position with a given force and a change in time
  CUDA_CALLABLE_MEMBER void update(double dt) {
    vec2d accel = _force / _mass;
    
    // Verlet integration
    if(_initialized == 0) { // First step: no previous position
      vec2d next_pos = _pos + _vel * dt + accel / 2 * dt * dt;
      _prev_pos = _pos;
      _pos = next_pos;
      _initialized = 1;
    } else {  // Later steps: 
      vec2d next_pos = _pos * 2 - _prev_pos + accel * dt * dt;
      _prev_pos = _pos;
      _pos = next_pos;
    }
    
    // Track velocity, even though this isn't strictly required
    _vel += accel * dt;
    
    // Zero out the force
    _force = vec2d(0, 0);
  }
  
  // Add a force on this star
  CUDA_CALLABLE_MEMBER void addForce(vec2d force) {
    _force += force;
  }

  void changePos(vec2d pos) {
    _pos = pos;
  }
  void changePrev(vec2d pos) {
    _prev_pos = pos;
  }
  void changeVel(vec2d vel){
    _vel = vel;
  }
  void changeInit(int init) {
    _initialized = init;
  }
  void changeMass(double mass){
    _mass = mass;
  }
  
  // Get the position of this star
  CUDA_CALLABLE_MEMBER vec2d pos() { return _pos; }

  // Get the previous position of this star
  CUDA_CALLABLE_MEMBER vec2d prev_pos() { return _prev_pos; }
  
  // Get the velocity of this star
  CUDA_CALLABLE_MEMBER vec2d vel() { return _vel; }
  
  // Get the mass of this star
  CUDA_CALLABLE_MEMBER double mass() { return _mass; }
  
  // Get the radius of this star
  CUDA_CALLABLE_MEMBER double radius() { return pow(_mass / 3.14, 0.33) / 4; }
  
  // Get the color of this star
  CUDA_CALLABLE_MEMBER rgb32 color() { return _color; }

  // Get the force of this star
  CUDA_CALLABLE_MEMBER vec2d force() { return _force; }

  //Get the initialized boolean of this star
  CUDA_CALLABLE_MEMBER int initialized() { return _initialized; }
  
  // Merge two stars
  CUDA_CALLABLE_MEMBER star merge(star other) {
    double mass = _mass + other._mass;
    vec2d pos = (_pos * _mass + other._pos * other._mass) / (_mass + other._mass);
    vec2d vel = (_vel * _mass + other._vel * other._mass) / (_mass + other._mass);
    
    rgb32 color = rgb32(
                        ((double)_color.red*_mass + (double)other._color.red*other._mass) / (_mass + other._mass),
                        ((double)_color.green*_mass + (double)other._color.green*other._mass) / (_mass + other._mass),
                        ((double)_color.blue*_mass + (double)other._color.blue*other._mass) / (_mass + other._mass));
    
    return star(mass, pos, vel, color);
  }
  
private:
  double _mass;       // The mass of this star
  vec2d _pos;         // The position of this star
  vec2d _prev_pos;    // The previous position of this star
  vec2d _vel;         // The velocity of this star
  vec2d _force;       // The accumulated force on this star
  rgb32 _color;       // The color of this star
  int _initialized;  // Has this particle been updated at least once?
};

#endif
