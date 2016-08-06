#include <cstdio>
#include <string>
#include <cstring>
#include <ctime>
#include <iostream>

#if defined(_WIN32)
#include <windows.h>
#endif

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_opengl.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#include "nanogui/nanogui.h"

#include "MainScreen.h"
#include "Extensions.h"
#include "Body.h"
#include "Emitter.h"
#include "Vector2.h"

// Constants 
const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 600;

const float FIXED_DELTA_TIME = 0.022;

void pause()
{
    system("pause");
}

void error(const char* fmt, ...)
{
    if (fmt)
    {
        va_list ap;
        va_start(ap, fmt);
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }
    exit(EXIT_FAILURE);
}


class Simulation
{
	MainScreen* settings;
	Emitter* emitter;
	Body* body;
	bool dragging;

	void Settings_EnableChanged(bool value)
	{
		emitter->setEnabled(value);
	}

	void Settings_ColorChanged(const nanogui::Color& value)
	{
		SDL_Color color;
		color.r = value.r() * 255;
		color.g = value.g() * 255;
		color.b = value.b() * 255;
		color.a = 255;
		emitter->setColor(color);
	}

	void Settings_MaxParticlesChanged(int value)
	{
		emitter->setMaxParticles(value);
	}

	void Settings_ParticleSizeChanged(float value)
	{
		emitter->setParticleSize(value);
	}

	void Settings_LifeTimeChanged(float value)
	{
		emitter->setLifeTime(value);
	}

	void Settings_FadeChanged(bool value)
	{
		emitter->setFade(value);
	}

	void Settings_RateChanged(float value)
	{
		emitter->setRate(value);
	}

	void Settings_RadiusChanged(float value)
	{
		emitter->setRadius(value);
	}

	void Settings_AngleChanged(float value)
	{
		emitter->setAngle(value);
	}

	void Settings_SpreadChanged(float value)
	{
		emitter->setSpread(value);
	}

	void Settings_MinSpeedChanged(float value)
	{
		emitter->setMinSpeed(value);
	}

	void Settings_MaxSpeedChanged(float value)
	{
		emitter->setMaxSpeed(value);
	}

	void Settings_GravityChanged(float value)
	{
		emitter->setGravity(value);
	}

public:
	Simulation(const std::string& title, SDL_Window* window, int width, int height)
		: dragging(false)
	{
		Vector2 startPosition(width / 2, height / 2);

		settings = new MainScreen("Particle Sim 2D", window, width, height);
		auto& color = settings->getColor();
		SDL_Color sdlColor;
		sdlColor.r = color.r() * 255;
		sdlColor.g = color.g() * 255;
		sdlColor.b = color.b() * 255;
		sdlColor.a = 255;

		emitter = new Emitter(
			startPosition,
			width,
			height,
			settings->getRate(),
			settings->getParticleSize(),
			settings->getLifeTime(),
			settings->getFade(),
			settings->getRadius(),
			settings->getAngle(),
			settings->getSpread(),
			settings->getMinSpeed(),
			settings->getMaxSpeed(),
			settings->getGravity(),
			settings->getMaxParticles(),
			sdlColor
			
		);
		body = new Body(emitter, startPosition);

		settings->enableChanged = std::bind(&Simulation::Settings_EnableChanged, this, std::placeholders::_1);
		settings->colorChanged = std::bind(&Simulation::Settings_ColorChanged, this, std::placeholders::_1);
		settings->maxParticlesChanged = std::bind(&Simulation::Settings_MaxParticlesChanged, this, std::placeholders::_1);
		settings->particleSizeChanged = std::bind(&Simulation::Settings_ParticleSizeChanged, this, std::placeholders::_1);
		settings->lifeTimeChanged = std::bind(&Simulation::Settings_LifeTimeChanged, this, std::placeholders::_1);
		settings->fadeChanged = std::bind(&Simulation::Settings_FadeChanged, this, std::placeholders::_1);
		settings->rateChanged = std::bind(&Simulation::Settings_RateChanged, this, std::placeholders::_1);
		settings->radiusChanged = std::bind(&Simulation::Settings_RadiusChanged, this, std::placeholders::_1);
		settings->angleChanged = std::bind(&Simulation::Settings_AngleChanged, this, std::placeholders::_1);
		settings->spreadChanged = std::bind(&Simulation::Settings_SpreadChanged, this, std::placeholders::_1);
		settings->minSpeedChanged = std::bind(&Simulation::Settings_MinSpeedChanged, this, std::placeholders::_1);
		settings->maxSpeedChanged = std::bind(&Simulation::Settings_MaxSpeedChanged, this, std::placeholders::_1);
		settings->gravityChanged = std::bind(&Simulation::Settings_GravityChanged, this, std::placeholders::_1);
	}

	~Simulation()
	{
		delete body;
		delete emitter;
		delete settings;
	}

	void
	update(float deltaTime)
	{
		if (dragging)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			body->position.x = x;
			body->position.y = y;
		}
		body->update(deltaTime);
		settings->setParticleCount(emitter->getParticleCount());
	}

	void
	handle_event(SDL_Event& e)
	{
		switch (e.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			if (e.button.button == SDL_BUTTON_LEFT)
			{
				Vector2 mousePosition(e.button.x, e.button.y);
				if (Vector2::Distance(mousePosition, body->position) < 10)
					dragging = true;
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (e.button.button == SDL_BUTTON_LEFT)
			{
				dragging = false;
			}
			break;
		}

		settings->onEvent(e);
	}

	void
	render()
	{
		// Set ModelView matrix mode and reset to the default identity matrix and orthogonal projection
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluOrtho2D(0.0f, SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f);

		body->render();

		settings->drawAll();
	}

	void
	setFPS(int value)
	{
		settings->setFPS(value);
	}
};

int main(int argc, char* args[])
{
    // atexit(pause);
    std::randomize();

	// SDL
	SDL_Window* sdlWindow = NULL;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        error("SDL could not initialize! SDL Error: %s\n", SDL_GetError());

	//Set texture filtering to linear
	if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		fprintf(stderr, "WARNING: Linear texture filtering not enabled!");

	// GL Settings (OpenGL 2.0 with Double Buffer and core profile)
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 1);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);

    //Create window
    sdlWindow = SDL_CreateWindow("Particle Sim 2D", 
		SDL_WINDOWPOS_CENTERED, 
		SDL_WINDOWPOS_CENTERED,
		SCREEN_WIDTH, 
		SCREEN_HEIGHT, 
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);

    if (sdlWindow == NULL)
        error("Window could not be created! SDL Error: %s\n", SDL_GetError());

	// Create GL Context
	SDL_GLContext context = SDL_GL_CreateContext(sdlWindow);

	// Set background color
	glClearColor(0.2, 0.2, 0.2, 1);

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
        error("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());

	nanogui::init();

	Simulation* simulation = new Simulation("Particle Sim 2D", sdlWindow, SCREEN_WIDTH, SCREEN_HEIGHT);

	// Disable depth testing (because we're working in 2D!)
	glDisable(GL_DEPTH_TEST);
	// Enable blending (we need this to be able to use an alpha component)
	glEnable(GL_BLEND);
	// For circles rather than square points
	glEnable(GL_POINT_SMOOTH);

    // Loop control
	uint32_t last = SDL_GetTicks();
    bool terminated = false;
	float fpsElapsedTime = 0;
	int fpsFrameCount = 0;
	float elapsedTime = 0;
	try
	{
		while (!terminated)
		{
			// Delta Time and FPS
			uint32_t now = SDL_GetTicks();
			float deltaTime = float(now - last) / 1000;
			last = now;

			elapsedTime += deltaTime;
			for (;elapsedTime >= FIXED_DELTA_TIME; elapsedTime -= FIXED_DELTA_TIME)
			{
				simulation->update(FIXED_DELTA_TIME);
			}

			fpsElapsedTime += deltaTime;
			fpsFrameCount++;
			if (fpsElapsedTime >= 2)
			{
				simulation->setFPS(fpsFrameCount / fpsElapsedTime);
				fpsFrameCount = 0;
				fpsElapsedTime = 0;
			}

			// Handle Input
			SDL_Event e;
			while (SDL_PollEvent(&e) != 0)
			{
				switch (e.type)
				{
				case SDL_QUIT:
					terminated = true;
					break;
				}

				// Update Simulation
				simulation->handle_event(e);
			}		

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			simulation->render();

			//Update screen
			SDL_GL_SwapWindow(sdlWindow);
		}
	}
	catch (const std::runtime_error &e)
	{
		std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
		#if defined(_WIN32)
			MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
		#else
			std::cerr << error_msg << endl;
		#endif
	}

	delete simulation;

	nanogui::shutdown();

    //Destroy window	
    SDL_DestroyWindow(sdlWindow);

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();

    return 0;
}