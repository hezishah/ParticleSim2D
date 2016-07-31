#include "Emitter.h"

#include <algorithm>
#include <cmath>

#define PI 3.14159265

#include "Extensions.h"

Emitter::
Emitter(Texture* texture, const Vector2& position, int spawnRate, float lifetime, float radius, float angle, float aperture, float minSpeed, float maxSpeed, float gravity)
: texture(texture),
  position(position),
  spawnRate(spawnRate),
  lifetime(lifetime),
  radius(radius),
  angle(angle < 0 ? 0 : angle > 360 ? 360 : angle),
  aperture(aperture < 0 ? 0 : aperture > 360 ? 360 : aperture),
  minSpeed(minSpeed),
  maxSpeed(maxSpeed),
  gravity(gravity),
  count(0),
  enabled(false)
{
    for (int i = 0; i < MAX_PARTICLES; ++i)
        particles[i] = NULL;
}

Emitter::
~Emitter()
{
    for (int i = 0; i < count; ++i)
    {
        delete particles[i];
        particles[i] = NULL;
    }
    count = 0;
    enabled = false;
}

void 
Emitter::
update(float deltaTime)
{
    if (enabled || count > 0)
    { 
        int spawns = spawnRate;

        for (int i = 0; i < MAX_PARTICLES; ++i)
        {
            Particle** p = &particles[i];
            if (*p != NULL)
            {
                if ((*p)->isDead())
                {
                    delete *p;
                    *p = NULL;
                    count--;
                }
            }

            if (enabled && (*p == NULL) && (spawns > 0))
            {
                float r = radius; // std::random(0, radius);
                float radians = (angle + std::random(-aperture, +aperture) ) * PI / 180.0;
                float speed = std::random(minSpeed, maxSpeed);

                Vector2 point(r * cos(radians), -r * sin(radians));
                *p = new Particle(texture, position + point, Vector2::Normalize(point) * speed, lifetime, gravity);
                spawns--;
                count++;
            }

            if (*p != NULL)
            {
                (*p)->update(deltaTime);
            }
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
    return count;
}

