#include <cstdio>
#include <string>
#include <cstring>
#include <ctime>

#include "guisan.hpp"
#include <guisan/sdl.hpp>

#include <SDL.h>
#include <SDL_image.h>

#include "System/Texture.h"
#include "System/Emitter.h"
#include "System/Body.h"
#include "System/Extensions.h" 
#include "System/StringUtil.h" 

class EditBlock
{
private: 
	bool integerOnly;
	double value;
	gcn::Label* label;
	gcn::TextField* textField;
	gcn::Slider* slider;

public:
	const double EPSILON = 0.0000001;

	EditBlock(const char* caption, double scaleStart = 0, double scaleEnd = 1.0, bool integerOnly = false) :
		integerOnly(integerOnly),
		label(new gcn::Label(caption)),
		textField(new gcn::TextField()),
		slider(new gcn::Slider(scaleStart, scaleEnd))
	{
		value = slider->getValue();
	}

	~EditBlock()
	{
		delete label;
		delete textField;
		delete slider;
	}

	void addToContainer(gcn::Container* container, int x = 8, int y = 8)
	{
		textField->setSize(80, 16);
		slider->setSize(80, 10);
		container->add(label, x, y);
		container->add(textField, x + 92, y + 18);
		container->add(slider, x, y + 21);
	}

	gcn::Label* getLabel() { return label; }

	gcn::TextField* getTextField() { return textField; }

	gcn::Slider* getSlider() { return slider; }

	double getValue() { return slider->getValue(); }

	void setValue(double value) 
	{
		slider->setValue(value);
		textField->setText(std::format("%g", value));
	}

	void update()
	{
		try
		{
			double sliderValue = slider->getValue();
			if (integerOnly)
				sliderValue = (int)sliderValue;

			if (fabs(value - sliderValue) > EPSILON)
			{
				value = sliderValue;
				textField->setText(std::format("%g", value));
			}
			else
			{
				const std::string& s = textField->getText();
				double textValue = s.empty() ? slider->getScaleStart()
											 : std::stod(textField->getText());

				if (integerOnly)
				{
					if (fabs(textValue - (int)textValue) > EPSILON)
					{
						textValue = (int)textValue;
						textField->setText(std::format("%d", value));
						textField->setCaretPosition(UINT_MAX);
					}
				}
					
				if (fabs(value - textValue) > EPSILON)
				{
					if (textValue >= slider->getScaleStart() && textValue <= slider->getScaleEnd())
					{
						slider->setValue(textValue);
						value = textValue;
					}
					else
					{
						textField->setText(std::format("%g", value));
						textField->setCaretPosition(UINT_MAX);
					}
				}
			}
		}
		catch (std::invalid_argument& e)
		{
			textField->setText(std::format("%g", value));
		}
	}
};

struct TextureEnumList : public gcn::ListModel
{
	int getNumberOfElements()
	{
		return 3;
	}

	std::string getElementAt(int i)
	{
		switch (i) {
		case 0:
			return std::string("Blue");
		case 1:
			return std::string("Red");
		case 2:
			return std::string("Green");
		default:
			return std::string("");
		}
	}
};

// Constants 

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

const char* PARTICLE_TEXTURE_FILENAME[] =
{
	"blue.bmp",
	"red.bmp",
	"green.bmp"
};

const char* FONT_FILENAME = "fixedfont.bmp";

// SDL
SDL_Window* sdlWindow = NULL;
SDL_Surface* sdlCanvas = NULL;
SDL_Renderer* sdlRenderer = NULL;

// SDL/GUI
gcn::SDLInput* input;				// Input driver
gcn::SDLGraphics* graphics;			// Graphics driver
gcn::SDLImageLoader* imageLoader;	// For loading images

// GUI
gcn::Gui* gui;						// A Gui object - binds it all together
gcn::ImageFont* font;				// A font

// GUI Widgets
gcn::Container* top;                // A top container
gcn::Container* settings;			// right container
gcn::Label* lblFPS;
gcn::TextBox* textDescription;
gcn::CheckBox* chkEnabled;
gcn::DropDown* ddTexture;
gcn::Label* lblCurrentParticleCount;

EditBlock* editMaxParticles;
EditBlock* editLifeTime;
gcn::CheckBox* chkFade;
EditBlock* editRate;
EditBlock* editRadius;
EditBlock* editAngle;
EditBlock* editSpread;
EditBlock* editMinSpeed;
EditBlock* editMaxSpeed;
EditBlock* editGravity;

TextureEnumList textureList;

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

void createWidgets()
{
	settings = new gcn::Container();
	settings->setSize(200, 600);
	settings->setBaseColor(gcn::Color(0xC7, 0xC7, 0xC7));

	lblFPS = new gcn::Label("FPS:");
	textDescription = new gcn::TextBox("The small black circle has a particle emitter attached to it.    \n" 
									   "Left-click and drag to move. You can change settings on the left \n"
		                               "panel but the changes will only apply to new particles.          \n"); 

	textDescription->setEditable(false);
	chkEnabled = new gcn::CheckBox("Enabled", false);
	ddTexture = new gcn::DropDown(&textureList);
	lblCurrentParticleCount = new gcn::Label("Particle Count: 0");

	editMaxParticles = new EditBlock("Max Number of Particles:", 1, Emitter::MAX_PARTICLES, true);
	editLifeTime = new EditBlock("Life Time:", 0.1, 10);

	editRate = new EditBlock("Rate:", 0.1, 1000);
	editRadius = new EditBlock("Radius:", 0, 1000);
	editAngle = new EditBlock("Angle:", 0, 360);
	editSpread = new EditBlock("Spread:", 0, 360);
	editMinSpeed = new EditBlock("Min Speed:", 0, 1000);
	editMaxSpeed = new EditBlock("Max Speed:", 0, 1000);
	editGravity = new EditBlock("Gravity:", 0, 1000);

	editMaxParticles->setValue(1024);
	editLifeTime->setValue(4);
	chkFade = new gcn::CheckBox("Fade", true);
	editRate->setValue(100);
	editRadius->setValue(10);
	editAngle->setValue(90);
	editSpread->setValue(60);
	editMinSpeed->setValue(80);
	editMaxSpeed->setValue(100);
	editGravity->setValue(98);

	settings->add(lblFPS, 8, 8);
	settings->add(chkEnabled, 8, 78);	
	settings->add(ddTexture, 80, 78);
	settings->add(lblCurrentParticleCount, 8, 98);

	editMaxParticles->addToContainer(settings, 8, 120);
	editLifeTime->addToContainer(settings, 8, 165);

	settings->add(chkFade, 8, 205);

	editRate->addToContainer(settings, 8, 225);
	editRadius->addToContainer(settings, 8, 270);
	editAngle->addToContainer(settings, 8, 315);
	editSpread->addToContainer(settings, 8, 360);
	editMinSpeed->addToContainer(settings, 8, 405);
	editMaxSpeed->addToContainer(settings, 8, 450);
	editGravity->addToContainer(settings, 8, 495);

	top->add(textDescription, 8, 8);
	top->add(settings, 600, 0);
}

void updateWidgets()
{
	editMaxParticles->update();
	editLifeTime->update();
	editRate->update();
	editRadius->update();
	editAngle->update();
	editSpread->update();
	editMinSpeed->update();
	editMaxSpeed->update();
	editGravity->update();
}

void deleteWidgets()
{
	delete top;
	delete settings;

	delete lblFPS;
	delete textDescription;
	delete chkEnabled;
	delete ddTexture;
	delete lblCurrentParticleCount;

	delete editMaxParticles;
	delete editLifeTime;
	delete chkFade;
	delete editRate;
	delete editRadius;
	delete editAngle;
	delete editSpread;
	delete editMinSpeed;
	delete editMaxSpeed;
	delete editGravity;
}

int main(int argc, char* args[])
{
    // atexit(pause);
    std::randomize();

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        error("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    
    //Set texture filtering to linear
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        fprintf(stderr, "WARNING: Linear texture filtering not enabled!");

    //Create window
    sdlWindow = SDL_CreateWindow("Particle Sim 2D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (sdlWindow == NULL)
        error("Window could not be created! SDL Error: %s\n", SDL_GetError());

    //Create renderer for window
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (sdlRenderer == NULL)
        error("Renderer could not be created! SDL Error: %s\n", SDL_GetError());

    sdlCanvas = SDL_GetWindowSurface(sdlWindow);
    if (sdlCanvas == NULL)
        error("Canvas could not be created! SDL Error: %s\n", SDL_GetError());

    //Initialize renderer color
    SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

    //Initialize PNG loading
    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags))
        error("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());


    // GUI
    imageLoader = new gcn::SDLImageLoader();
    // The ImageLoader in use is static and must be set to be able to load images
    gcn::Image::setImageLoader(imageLoader);
    graphics = new gcn::SDLGraphics();
    // Set the target for the graphics object to be the screen.
    // In other words, we will draw to the screen.
    // Note, any surface will do, it doesn't have to be the screen.
    graphics->setTarget(sdlCanvas);
    input = new gcn::SDLInput();

    // Initialize and create the gui with Guichan stuff.
    top = new gcn::Container();
    // Set the dimension of the top container to match the screen.
    top->setDimension(gcn::Rectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
	// Set base color
	top->setBaseColor(gcn::Color(255, 255, 255));

    gui = new gcn::Gui();
    // Set gui to use the SDLGraphics object.
    gui->setGraphics(graphics);
    // Set gui to use the SDLInput object
    gui->setInput(input);
    // Set the top container
    gui->setTop(top);
    // Load the image font.
    font = new gcn::ImageFont(FONT_FILENAME, " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.:-");
    // The global font is static and must be set.
    gcn::Widget::setGlobalFont(font);
	
	// Create Widgets
	createWidgets();

    // Create Texture to use for particles
    Texture particleTexture(sdlRenderer);
	int particleTextureIndex = ddTexture->getSelected();
    particleTexture.Load(PARTICLE_TEXTURE_FILENAME[particleTextureIndex]);
	// particleTexture.setAlpha(192);

	// Create Texture to use for body
    Texture bodyTexture(sdlRenderer);
    bodyTexture.Load("dot.bmp");

    // Create Emitter
	Emitter emitter(&particleTexture);

    // Create Body
    Body body(&bodyTexture, &emitter, Vector2(SCREEN_HEIGHT / 2, SCREEN_HEIGHT / 2));

    // Loop control
	bool dragging = false;
    bool terminated = false;
    uint32_t last = SDL_GetTicks();
	while (!terminated)
	{
		// Delta Time and FPS
		uint32_t now = SDL_GetTicks();
		float deltaTime = float(now - last) / 1000;
		last = now;

		// Handle Input
		SDL_Event e;
		while (SDL_PollEvent(&e) != 0)
		{
			switch (e.type)
			{
			case SDL_QUIT:
				terminated = true;
				break;

			case SDL_KEYDOWN:
				switch (e.key.keysym.sym)
				{
					case SDLK_ESCAPE: terminated = true; break;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (e.button.button == SDL_BUTTON_LEFT)
				{
					//Get the mouse offsets
					int x = e.button.x;
					int y = e.button.y;

					int minX = body.position.x - 10;
					int maxX = body.position.x + 10;
					int minY = body.position.y - 10;
					int maxY = body.position.y + 10;
					if (x >= minX && x <= maxX && y >= minY && y <= maxY)
						dragging = true;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				if (e.button.button == SDL_BUTTON_LEFT)
					dragging = false;
				break;
			}

			// Now that we are done polling and using SDL events we pass
			// the leftovers to the SDLInput object to later be handled by
			// the Gui. (This example doesn't require us to do this 'cause a
			// label doesn't use input. But will do it anyway to show how to
			// set up an SDL application with Guichan.)            
			input->pushInput(e);
		}

		//Clear screen
		SDL_SetRenderDrawColor(sdlRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderClear(sdlRenderer);

		// Let the gui perform it's logic (like handle input)
		gui->logic();
		// Draw the gui
		gui->draw();

		
		// Update widgets
		lblFPS->setCaption(std::format("FPS: %g", 1 / deltaTime));
		lblFPS->adjustSize();

		lblCurrentParticleCount->setCaption(std::format("Particle Count: %d", emitter.getParticleCount()));
		lblCurrentParticleCount->adjustSize();

		updateWidgets();

		// Update Particles
		if (particleTextureIndex != ddTexture->getSelected())
		{
			particleTextureIndex = ddTexture->getSelected();
			particleTexture.Load(PARTICLE_TEXTURE_FILENAME[particleTextureIndex]);
		}

		// Update Emitter
		emitter.setEnabled(chkEnabled->isSelected());
		emitter.setMaxParticles((int)editMaxParticles->getValue());
		emitter.setRate(editRate->getValue());
		emitter.setLifeTime(editLifeTime->getValue());
		emitter.setRadius(editRadius->getValue());
		emitter.setAngle(editAngle->getValue());
		emitter.setSpread(editSpread->getValue());
		emitter.setMinSpeed(editMinSpeed->getValue());
		emitter.setMaxSpeed(editMaxSpeed->getValue());
		emitter.setGravity(editGravity->getValue());
		emitter.setFade(chkFade->isSelected());

		// Update Body
		if (dragging)
		{
			int x, y;
			SDL_GetMouseState(&x, &y);
			if (x < 600)
				body.position.x = x;
			body.position.y = y;
		}

		// Render Canvas
		SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, sdlCanvas);
		SDL_RenderCopy(sdlRenderer, texture, 0, 0);
		SDL_DestroyTexture(texture);

		
		// Render Body
		body.update(deltaTime);

        //Update render
        SDL_RenderPresent(sdlRenderer);
    }

    //Free resources and close SDL

    // Free GUI
    delete font;
    delete gui;

	deleteWidgets();

    delete input;
    delete graphics;
    delete imageLoader;  

    //Destroy window	
    SDL_DestroyRenderer(sdlRenderer);
    SDL_DestroyWindow(sdlWindow);
    sdlWindow = NULL;
    sdlRenderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();

    return 0;
}
