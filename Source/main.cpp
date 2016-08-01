#include <cstdio>
#include <string>
#include <ctime>

#include "guisan.hpp"
#include <guisan/sdl.hpp>

#include <SDL.h>
#include <SDL_image.h>

#include "System/Texture.h"
#include "System/Emitter.h"
#include "System/Body.h"
#include "System/Extensions.h" 

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

// SDL
SDL_Window* sdlWindow = NULL;
SDL_Surface* sdlCanvas = NULL;
SDL_Renderer* sdlRenderer = NULL;

// SDL/GUI
gcn::SDLInput* input;             // Input driver
gcn::SDLGraphics* graphics;       // Graphics driver
gcn::SDLImageLoader* imageLoader; // For loading images

// GUI
gcn::Gui* gui;            // A Gui object - binds it all together
gcn::ImageFont* font;     // A font

// GUI Widgets
gcn::Container* top;                 // A top container
gcn::Label* label;                   // A label
gcn::Icon* icon;                     // An icon (image)
gcn::Button* button;                 // A button
gcn::TextField* textField;           // One-line text field
gcn::TextBox* textBox;               // Multi-line text box
gcn::ScrollArea* textBoxScrollArea;  // Scroll area for the text box
gcn::ListBox* listBox;               // A list box
gcn::DropDown* dropDown;             // Drop down
gcn::CheckBox* checkBox1;            // Two checkboxes
gcn::CheckBox* checkBox2;
gcn::RadioButton* radioButton1;      // Three radio buttons
gcn::RadioButton* radioButton2;
gcn::RadioButton* radioButton3;
gcn::Slider* slider;                 // A slider
gcn::Image *image;                   // An image for the icon
gcn::Window *window;
gcn::Image *guisanLogoImage;
gcn::Icon* guisanLogoIcon;
gcn::ScrollArea* nestedScrollArea;
gcn::Container* nestedContainer;
gcn::Slider* nestedSlider;

/*
* List boxes and dropdowns needs an instance of a listmodel
* to know what elements they have.
*/
class DemoListModel : public gcn::ListModel
{
public:
	int getNumberOfElements()
	{
		return 5;
	}

	std::string getElementAt(int i)
	{
		switch (i) {
		case 0:
			return std::string("zero");
		case 1:
			return std::string("one");
		case 2:
			return std::string("two");
		case 3:
			return std::string("three");
		case 4:
			return std::string("four");
		default: // Just to keep warnings away
			return std::string("");
		}
	}
};

DemoListModel demoListModel;

const char* particleTextureFilename[] =
{
    "blue.bmp",
    "red.bmp",
    "green.bmp"
};

int particleTextureIndex = 0;

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
	/*
	* Create all the widgets
	*/
	label = new gcn::Label("Label");

	image = gcn::Image::load("guisan.png");
	icon = new gcn::Icon(image);

	button = new gcn::Button("Button");

	textField = new gcn::TextField("Text field");

	textBox = new gcn::TextBox("Lorem ipsum dolor sit amet consectetur\n"
		"adipiscing elit Integer vitae ultrices\n"
		"eros Curabitur malesuada dolor imperdieat\n"
		"ante facilisis ut convallis sem rutrum\n"
		"Praesent consequat urna convallis leo\n"
		"aliquam pellentesque Integer laoreet enim\n"
		"vehicula libero blandit at pellentesque\n"
		"ipsum vehicula Mauris id turpis hendrerit\n"
		"tempor velit nec hendrerit nulla");
	textBoxScrollArea = new gcn::ScrollArea(textBox);
	textBoxScrollArea->setWidth(270);
	textBoxScrollArea->setHeight(100);
	textBoxScrollArea->setBorderSize(1);

	listBox = new gcn::ListBox(&demoListModel);
	listBox->setBorderSize(1);

	dropDown = new gcn::DropDown(&demoListModel);

	checkBox1 = new gcn::CheckBox("Checkbox 1");
	checkBox2 = new gcn::CheckBox("Checkbox 2");

	radioButton1 = new gcn::RadioButton("Radio Button 1", "radiogroup", true);
	radioButton2 = new gcn::RadioButton("Radio Button 2", "radiogroup");
	radioButton3 = new gcn::RadioButton("Radio Button 3", "radiogroup");

	slider = new gcn::Slider(0, 10);
	slider->setSize(100, 10);

	window = new gcn::Window("I am a window  Drag me");
	window->setBaseColor(gcn::Color(212, 255, 150, 190));

	guisanLogoImage = gcn::Image::load("guisan-logo.png");
	guisanLogoIcon = new gcn::Icon(guisanLogoImage);
	window->add(guisanLogoIcon);
	window->resizeToContent();

	nestedSlider = new gcn::Slider(0, 10);
	nestedSlider->setSize(100, 10);

	nestedContainer = new gcn::Container();
	nestedContainer->setSize(400, 200);
	nestedContainer->add(nestedSlider, 50, 70);

	nestedScrollArea = new gcn::ScrollArea(nestedContainer);
	nestedScrollArea->setSize(180, 90);
	nestedScrollArea->setBorderSize(1);

	/*
	* Add them to the top container
	*/
	top->add(label, 290, 10);
	top->add(icon, 10, 10);
	top->add(button, 325, 10);
	top->add(textField, 375, 10);
	top->add(textBoxScrollArea, 290, 50);
	top->add(listBox, 290, 200);
	top->add(dropDown, 580, 10);
	top->add(checkBox1, 580, 50);
	top->add(checkBox2, 580, 70);
	top->add(radioButton1, 580, 120);
	top->add(radioButton2, 580, 140);
	top->add(radioButton3, 580, 160);
	top->add(slider, 580, 200);
	top->add(window, 100, 350);
	top->add(nestedScrollArea, 440, 350);

}

void deleteWidgets()
{
	delete top;
	delete label;
	delete icon;
	delete button;
	delete textField;
	delete textBox;
	delete textBoxScrollArea;
	delete listBox;
	delete dropDown;
	delete checkBox1;
	delete checkBox2;
	delete radioButton1;
	delete radioButton2;
	delete radioButton3;
	delete slider;
	delete window;
	delete guisanLogoIcon;
	delete guisanLogoImage;
	delete nestedScrollArea;
	delete nestedContainer;
	delete nestedSlider;
}

int main(int argc, char* args[])
{
    atexit(pause);
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
    gui = new gcn::Gui();
    // Set gui to use the SDLGraphics object.
    gui->setGraphics(graphics);
    // Set gui to use the SDLInput object
    gui->setInput(input);
    // Set the top container
    gui->setTop(top);
    // Load the image font.
    font = new gcn::ImageFont("fixedfont.bmp", " abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789");
    // The global font is static and must be set.
    gcn::Widget::setGlobalFont(font);

	createWidgets();

    // Simulation

    Texture particleTexture(sdlRenderer);
    particleTexture.Load(particleTextureFilename[particleTextureIndex]);
    particleTexture.setAlpha(192);

    Texture bodyTexture(sdlRenderer);
    bodyTexture.Load("dot.bmp");

    // Emitter
    Vector2 startPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
    int spawnRate = 8;
    float lifetime = 1.5;
    float radius = 11;
    float angleOfEmission = 90;
    float aperture = 30;
    float minSpeed = 280;
    float maxSpeed = 300;
    float gravity = 350;
    Emitter emitter(&particleTexture, startPosition, spawnRate, lifetime, radius, angleOfEmission, aperture, minSpeed, maxSpeed, gravity);

    // Body
    float mass = 1;
    Body body(&bodyTexture, &emitter, startPosition);

    // Loop control
    bool terminated = false;
    uint32_t last = SDL_GetTicks();
    while (!terminated)
    {
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
                case SDLK_SPACE:
                    particleTextureIndex = (particleTextureIndex + 1) % (sizeof(particleTextureFilename) / sizeof(particleTextureFilename[0]));
                    particleTexture.Load(particleTextureFilename[particleTextureIndex]);
                    break;
                case SDLK_ESCAPE: terminated = true; break;
                case SDLK_RETURN: body.getEmitter()->setEnabled(!body.getEmitter()->getEnabled()); break;
                    // case SDLK_UP: body.addForce(Vector2::Up * moveForce); break;
                    // case SDLK_DOWN: body.addForce(Vector2::Down * moveForce); break;
                    // case SDLK_LEFT: body.addForce(Vector2::Left * moveForce); break;
                    // case SDLK_RIGHT: body.addForce(Vector2::Right * moveForce); break;
                }
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

		// Apply canvas
		SDL_Texture* texture = SDL_CreateTextureFromSurface(sdlRenderer, sdlCanvas);
		SDL_RenderCopy(sdlRenderer, texture, 0, 0);
		SDL_DestroyTexture(texture);

		// Update body
		uint32_t now = SDL_GetTicks();
		float deltaTime = float(now - last) / 1000;
		last = now;
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
