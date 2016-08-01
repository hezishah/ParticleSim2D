#pragma once

#include "Texture.h"
#include "Vector2.h"
#include "Emitter.h"

class Body
{
private: 
    Texture* texture;
    Emitter* emitter;

public:
    Vector2 position;

    Body(Texture* texture, Emitter* emitter, const Vector2& position);
    ~Body();

    void update(float deltaTime);

    Emitter* getEmitter();
};

