#include "Body.h"
#include "Extensions.h"

Body::
Body(Texture* texture, Emitter* emitter, const Vector2& position, float mass)
: texture(texture),
  emitter(emitter),
  resultant(0, 0),
  position(position),
  velocity(0, 0),
  mass(mass < 0 ? 1: mass)
{

}

Body::
~Body()
{

}

void 
Body::
addForce(const Vector2& force)
{
    resultant += force;
}

void
Body::
update(float deltaTime)
{
    velocity += resultant * deltaTime / mass;    // F = ma = mv/t --> v = Ft / m
    position += velocity * deltaTime;            // v = s/t       --> s = vt

    resultant.Reset();

    // awful hack, yeah I know but we don't have a camera neither a viewport, and I'm sick of this 
    // thing gettng out of the screen...!
    float maxX = 800 - texture->getWidth();
    if (position.x < 0)
    {
        velocity.x = 0;
        position.x = 0;
    }
    else if (position.x > maxX)
    {
        velocity.x = 0;
        position.x = maxX;
    }

    float maxY = 600 - texture->getHeight();
    if (position.y < 0)
    {
        velocity.y = 0;
        position.y = 0;
    }
    else if (position.y > maxY)
    {
        velocity.y = 0;
        position.y = maxY;
    }

    float rx = position.x - (texture->getWidth() / 2.0);
    float ry = position.y - (texture->getHeight() / 2.0);
    texture->render(rx, ry);

    emitter->position = position;
    emitter->update(deltaTime);
}

Emitter* 
Body::
getEmitter()
{
    return emitter;
}