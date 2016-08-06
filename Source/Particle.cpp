#include "Particle.h"
#include "Extensions.h"

#include "SDL/SDL_opengl.h"
#include "gl/GLU.h"


Particle::
Particle()
: position(0, 0),
  velocity(0, 0),
  lifetime(10),
  gravity(9.8),
  fade(false),
  color({0, 128, 255, 255}),
  age(0)
{

}

Particle::
Particle(const Particle& other)
: position(other.position),
  velocity(other.velocity),
  lifetime(other.lifetime),
  gravity(other.gravity),
  fade(other.fade),
  color(other.color),
  age(0)
{
}

Particle::
Particle(const Vector2& position, const Vector2& velocity, float lifetime, float gravity, bool fade, const SDL_Color& color)
: position(position),
  velocity(velocity),
  lifetime(lifetime),
  gravity(gravity),
  fade(fade),
  color(color),
  age(0)
{

}

Particle::
~Particle()
{

}

void 
Particle::
update(float deltaTime)
{
    if (!isDead())
    {
		age += deltaTime;
        velocity += Vector2::Down * gravity * deltaTime; 
        position += velocity * deltaTime;
    }
}

void 
Particle::
render()
{
	glColor4f(color.r / 255.f, color.g / 255.f, color.b / 255.f, (fade ? 1 - std::clamp(age / (lifetime - age), 0.0, 1.0) : 1) * color.a / 255.f);
	glVertex2f(position.x, position.y);
}

bool 
Particle::
isDead()
{
	return (age >= lifetime) || !isInside(-16777216, -16777216, 16777216, 16777216);
}


bool 
Particle::
isInside(int left, int top, int width, int height)
{
	return position.x >= left && position.x <= width && position.y >= top && position.y <= height;
}
