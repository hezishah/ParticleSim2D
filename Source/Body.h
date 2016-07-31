#pragma once

#include "Texture.h"
#include "Vector2.h"
#include "Emitter.h"

class Body
{
private: 
    Texture* texture;
    Emitter* emitter;
    Vector2 resultant;

public:
    Vector2 position;
    Vector2 velocity;
    float mass;

    Body(Texture* texture, Emitter* emitter, const Vector2& position, float mass = 1);
    ~Body();

    void addForce(const Vector2& force);
    void update(float deltaTime);

    Emitter* getEmitter();
};

