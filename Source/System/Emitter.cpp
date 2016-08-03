#include "Emitter.h"

#include <algorithm>
#include <cmath>

#define PI 3.14159265

#include "Extensions.h"

Emitter::
Emitter(Texture* texture, const Vector2& position, int rate, float lifetime, bool fade, float radius, float angle, float spread, float minSpeed, float maxSpeed, float gravity, int maxParticles)
: texture(texture),
//  particles(),
  maxParticles(maxParticles < 1 ? 1 : maxParticles > MAX_PARTICLES ? MAX_PARTICLES : maxParticles),
  position(position),
  rate(rate),
  lifetime(lifetime),
  radius(radius),
  angle(angle < 0 ? 0 : angle > 360 ? 360 : angle),
  spread(spread < 0 ? 0 : spread > 360 ? 360 : spread),
  minSpeed(minSpeed),
  maxSpeed(maxSpeed),
  gravity(gravity),
  fade(fade),
  enabled(false)
{
}

Emitter::
~Emitter()
{
	for (std::deque<Particle*>::iterator it = particles.begin(); it != particles.end(); ++it)
	{
		delete *it;
	}

	particles.clear();
}

void 
Emitter::
update(float deltaTime)
{
	// remove dead particles
	for (std::deque<Particle*>::iterator it = particles.begin(); it != particles.end();)
	{
		Particle* p = *it;
		if (p->isDead())
		{
			it = particles.erase(it);
			delete p;
		}
		else
		{
			p->update(deltaTime);
			++it;
		}
	}

	// create new ones
	if (enabled)
	{
		int spawns = std::min((int)std::ceil(rate * deltaTime), maxParticles - (int)particles.size());
		while (spawns > 0)
		{
			float halfspread = spread / 2;
			float radians = (angle + std::random(-halfspread, +halfspread)) * PI / 180.0;
			float speed = std::random(minSpeed, maxSpeed);

			Vector2 point(cos(radians), -sin(radians));
			Particle* p = new Particle(texture, position + (point * radius), point * speed, lifetime, gravity, fade);
			particles.push_back(p);

			spawns--;
		}
	}
}

void 
Emitter::
setEnabled(bool value)
{
    enabled = value;
}

bool 
Emitter::
getEnabled()
{
    return enabled;
}

int
Emitter::
getParticleCount()
{
	return particles.size();
}

void 
Emitter::
setMaxParticles(int value)
{
	maxParticles = value;
}

void 
Emitter::
setRate(float value)
{
	rate = value;
}

void 
Emitter::
setLifeTime(float value)
{
	lifetime = value;
}

void 
Emitter::
setRadius(float value)
{
	radius = value;
}

void 
Emitter::
setAngle(float value)
{
	angle = value;
}

void 
Emitter::
setSpread(float value)
{
	spread = value;
}

void 
Emitter::
setMinSpeed(float value)
{
	minSpeed = value;
}

void 
Emitter::
setMaxSpeed(float value)
{
	maxSpeed = value;
}

void 
Emitter::
setGravity(float value)
{
	gravity = value;
}

void
Emitter::
setFade(bool value)
{
	fade = value;
}

int 
Emitter::
getMaxParticles()
{
	return maxParticles;
}

float 
Emitter::
getRate()
{
	return rate;
}

float
Emitter::
getLifeTime()
{
	return lifetime;
}

float 
Emitter::
getRadius()
{
	return radius;
}

float 
Emitter::
getAngle()
{
	return angle;
}

float 
Emitter::
getSpread()
{
	return spread;
}

float 
Emitter::
getMinSpeed()
{
	return minSpeed;
}

float 
Emitter::
getMaxSpeed()
{
	return maxSpeed;
}

float 
Emitter::
getGravity()
{
	return gravity;
}

bool 
Emitter::
getFade()
{
	return fade;
}
