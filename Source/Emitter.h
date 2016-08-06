#pragma once

#include <random>
#include <deque>

#include "SDL/SDL.h"

#include "Particle.h"
#include "Vector2.h"

class Emitter
{
public:
    static const int MAX_PARTICLES = 65535;

    Vector2 position;

private:
	int width;
	int height;
	std::deque<Particle*> particles;
	int maxParticles;
    int rate;
	float particleSize;
    float lifetime;
    float radius;
    float angle;
    float spread;
    float minSpeed;
    float maxSpeed;
    float gravity;
	bool fade;
    bool enabled;
	SDL_Color color;

public:
    Emitter(const Vector2& position = Vector2::Zero, int width = 200, int height = 200, int rate = 1, float particleSize = 2, float lifetime = 10, bool fade = false, float radius = 10, float angle = 90, float spread = 30, float minSpeed = 0, float maxSpeed = 0, float gravity = 9.8, int maxParticles = 2048, const SDL_Color& color = { 255, 255, 255, 255 });

    ~Emitter();

    void update(float deltaTime);
	void render();

    void setEnabled(bool value);
    bool getEnabled(); 

    int getParticleCount();

	void setMaxParticles(int value);
	void setRate(float value);
	void setParticleSize(float value);
	void setLifeTime(float value);
	void setRadius(float value);
	void setAngle(float value);
	void setSpread(float value);
	void setMinSpeed(float value);
	void setMaxSpeed(float value);
	void setGravity(float value);
	void setFade(bool value);
	void setColor(const SDL_Color& value);

	int getMaxParticles();
	float getRate();
	float getParticleSize();
	float getLifeTime();
	float getRadius();
	float getAngle();
	float getSpread();
	float getMinSpeed();
	float getMaxSpeed();
	float getGravity();
	bool getFade();
	SDL_Color getColor();
};