#include "Body.h"
#include "Extensions.h"

#include "SDL/SDL_opengl.h"
#include "gl/GLU.h"

Body::
Body(Emitter* emitter, const Vector2& position)
  : emitter(emitter),
	position(position),
	bodySize(20),
	color({0, 0, 0, 255})
{
	emitter->position = position;
}

Body::
~Body()
{

}

void
Body::
update(float deltaTime)
{
    emitter->position = position;
    emitter->update(deltaTime);
}

void
Body::
render()
{
	glPointSize(bodySize);
	glBegin(GL_POINTS);
		glColor4f(color.r / 255.0, color.g / 255.0, color.b / 255.0, color.a / 255.0);
		glVertex2f(position.x, position.y);
	glEnd();

	emitter->render();
}

Emitter* 
Body::
getEmitter()
{
    return emitter;
}