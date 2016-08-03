#pragma once

#include <random>
#include <deque>

#include "Particle.h"
#include "Vector2.h"

class Emitter
{
public:
    static const int MAX_PARTICLES = 65535;

    Vector2 position;

private:
    Texture* texture;
	std::deque<Particle*> particles;
	int maxParticles;
    int rate;
    float lifetime;
    float radius;
    float angle;
    float spread;
    float minSpeed;
    float maxSpeed;
    float gravity;
	bool fade;
    bool enabled;

public:
    Emitter(Texture* texture, const Vector2& position = Vector2::Zero, int rate = 1, float lifetime = 10, bool fade = false, float radius = 10, float angle = 90, float spread = 30, float minSpeed = 0, float maxSpeed = 0, float gravity = 9.8, int maxParticles = 1000);

    ~Emitter();

    void update(float deltaTime);

    void setEnabled(bool value);
    bool getEnabled(); 

    int getParticleCount();

	void setMaxParticles(int value);
	void setRate(float value);
	void setLifeTime(float value);
	void setRadius(float value);
	void setAngle(float value);
	void setSpread(float value);
	void setMinSpeed(float value);
	void setMaxSpeed(float value);
	void setGravity(float value);
	void setFade(bool value);

	int getMaxParticles();
	float getRate();
	float getLifeTime();
	float getRadius();
	float getAngle();
	float getSpread();
	float getMinSpeed();
	float getMaxSpeed();
	float getGravity();
	bool getFade();
};