#pragma once

#include "Vector2.h"
#include "SDL/SDL.h"

class Particle
{
private:
    Vector2 position;
    Vector2 velocity;

    float lifetime;
    float gravity;
	bool fade;
	SDL_Color color;
	float age;

public:
    Particle();
    Particle(const Particle& other);
	Particle(const Vector2& position, const Vector2& velocity, float lifetime = 10, float gravity = 9.8, bool fade = false, const SDL_Color& color = {255, 255, 255, 255});
    
    ~Particle();

    void update(float deltaTime);
	void render();

    bool isDead();
	bool isInside(int left, int top, int width, int height);
};

