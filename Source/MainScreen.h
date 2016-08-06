#pragma once

#include <string>
#include <functional>

#include <nanogui/nanogui.h>

class MainScreen :	public nanogui::Screen
{
	// Widgets
	nanogui::TextBox* textFPS;
	nanogui::TextBox* textParticleCount;

	// Settings
	bool enabled;
	nanogui::Color color;
	int maxParticles;
	float particleSize;
	float lifeTime;
	bool fade;
	float rate;
	float radius;
	float angle;
	float spread;
	float minSpeed;
	float maxSpeed;
	float gravity;

public:
	MainScreen(const std::string& title, SDL_Window* pwindow, int rwidth, int rheight);
	~MainScreen();

	virtual bool keyboardEvent(int key, int scancode, int action, int modifiers);

	virtual void draw(NVGcontext *ctx);

	virtual void drawContents();

	void setFPS(int value);
	void setParticleCount(int value);

	std::function<void(bool)> enableChanged;
	std::function<void(const nanogui::Color&)> colorChanged;
	std::function<void(int)> maxParticlesChanged;
	std::function<void(float)> particleSizeChanged;
	std::function<void(float)> lifeTimeChanged;
	std::function<void(bool)> fadeChanged;
	std::function<void(float)> rateChanged;
	std::function<void(float)> radiusChanged;
	std::function<void(float)> angleChanged;
	std::function<void(float)> spreadChanged;
	std::function<void(float)> minSpeedChanged;
	std::function<void(float)> maxSpeedChanged;
	std::function<void(float)> gravityChanged;

	bool getEnabled();
	nanogui::Color getColor();
	int getMaxParticles();
	float getParticleSize();
	float getLifeTime();
	bool getFade();
	float getRate();
	float getRadius();
	float getAngle();
	float getSpread();
	float getMinSpeed();
	float getMaxSpeed();
	float getGravity();

	void setEnabled(bool value);
	void setColor(const nanogui::Color& value);
	void setMaxParticles(int value);
	void setParticleSize(float value);
	void setLifeTime(float value);
	void setFade(bool value);
	void setRate(float value);
	void setRadius(float value);
	void setAngle(float value);
	void setSpread(float value);
	void setMinSpeed(float value);
	void setMaxSpeed(float value);
	void setGravity(float value);
};


