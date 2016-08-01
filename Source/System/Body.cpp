#include "Body.h"
#include "Extensions.h"

Body::
Body(Texture* texture, Emitter* emitter, const Vector2& position)
: texture(texture),
  emitter(emitter),
  position(position)
{

}

Body::
~Body()
{

}

void
Body::
update(float deltaTime)
{
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