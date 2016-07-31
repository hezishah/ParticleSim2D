#pragma once

#include <random>

#include "Particle.h"
#include "Vector2.h"

class Emitter
{
public:
    static const int MAX_PARTICLES = 2048;

    Vector2 position;

private:
    Texture* texture;
    Particle* particles[MAX_PARTICLES];
    int spawnRate;
    float lifetime;
    float radius;
    float angle;
    float aperture;
    float minSpeed;
    float maxSpeed;
    float gravity;
    int count;
    bool enabled;

public:
    Emitter(Texture* texture, const Vector2& position, int spawnRate = 1, float lifetime = 10, float radius = 1, float angle = 90, float aperture = 30, float minSpeed = 0, float maxSpeed = 0, float gravity = 9.8);

    ~Emitter();

    void update(float deltaTime);

    void setEnabled(bool value);
    bool getEnabled(); 

    int getParticleCount();
};