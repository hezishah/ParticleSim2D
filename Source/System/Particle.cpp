#include "Particle.h"

Particle::
Particle(Texture* texture)
: texture(texture),
  position(0, 0),
  velocity(0, 0),
  lifetime(10)
{

}

Particle::
Particle(const Particle& other)
: texture(other.texture),
  position(other.position),
  velocity(other.velocity),
  lifetime(other.lifetime),
  alpha(1)
{
}

Particle::
Particle(Texture* texture, const Vector2& position, const Vector2& velocity, float lifetime, float gravity, bool fade)
: texture(texture),
  position(position),
  velocity(velocity),
  lifetime(lifetime),
  gravity(gravity),
  fade(fade),
  alpha(1)
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
		if (alpha > 0)
		{
			alpha -= (deltaTime / lifetime);
			if (alpha < 0) alpha = 0;
		}
		lifetime -= deltaTime;

        velocity += Vector2::Down * gravity * deltaTime;  // particles don't have mass
        position += velocity * deltaTime;

        float rx = position.x - (texture->getWidth() / 2.0);
        float ry = position.y - (texture->getHeight() / 2.0);
		texture->setAlpha((fade) ? (uint8_t)(255 * alpha) : 255);
        texture->render(rx, ry);
    }
}

bool 
Particle::
isDead()
{
    return lifetime <= 0;
}

