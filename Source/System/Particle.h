#pragma once

#include "Texture.h"
#include "Vector2.h"

class Particle
{
private:
    Vector2 position;
    Vector2 velocity;
    Texture* texture;
    float lifetime;
    float gravity;
	bool fade;
	double alpha;

public:
    Particle(Texture* texture);
    Particle(const Particle& other);
	Particle(Texture* texture, const Vector2& position, const Vector2& velocity, float lifetime = 10, float gravity = 9.8, bool fade = false);
    
    ~Particle();

    void update(float deltaTime);

    bool isDead();
};

