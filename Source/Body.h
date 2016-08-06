#pragma once

#include "Vector2.h"
#include "Emitter.h"

class Body
{
private: 
    Emitter* emitter;
	float bodySize;
	SDL_Color color;

public:
    Vector2 position;

    Body(Emitter* emitter, const Vector2& position);
    ~Body();

    void update(float deltaTime);

	void render();

    Emitter* getEmitter();
};

