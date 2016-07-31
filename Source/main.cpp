#include <cstdio>
#include <string>
#include <ctime>

#include <SDL.h>
#include <SDL_image.h>

#include "Texture.h"
#include "Emitter.h"
#include "Body.h"
#include "Extensions.h" 

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;


bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
        {
            printf("Warning: Linear texture filtering not enabled!");
        }

        //Create window
        gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if (gWindow == NULL)
        {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
            if (gRenderer == NULL)
            {
                printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if (!(IMG_Init(imgFlags) & imgFlags))
                {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

void close()
{
    //Destroy window	
    SDL_DestroyRenderer(gRenderer);
    SDL_DestroyWindow(gWindow);
    gWindow = NULL;
    gRenderer = NULL;

    //Quit SDL subsystems
    IMG_Quit();
    SDL_Quit();
}


const char* particleTextureFilename[] = 
{
    "blue.bmp",
    "red.bmp",
    "green.bmp"
};

int particleTextureIndex = 0;

int main(int argc, char* args[])
{
    std::randomize();

    //Start up SDL and create window
    if (!init())
    {
        printf("Failed to initialize!\n");
    }
    else
    {
        Texture particleTexture(gRenderer);
        particleTexture.Load(particleTextureFilename[particleTextureIndex]);
        particleTexture.setAlpha(192);

        Texture bodyTexture(gRenderer);
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
        Body body(&bodyTexture, &emitter, startPosition, mass);       

        // Movement settings
        float moveForce = 400;

        // Loop control
        bool terminated = false;
        clock_t before = clock();
        SDL_Event e;

        // Main Loop
        while (!terminated)
        {
            clock_t now = clock();
            float deltaTime = float(now - before) / CLOCKS_PER_SEC;
            before = now;

            // Handle Input
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
                            case SDLK_UP: body.addForce(Vector2::Up * moveForce); break;
                            case SDLK_DOWN: body.addForce(Vector2::Down * moveForce); break;
                            case SDLK_LEFT: body.addForce(Vector2::Left * moveForce); break;
                            case SDLK_RIGHT: body.addForce(Vector2::Right * moveForce); break;
                        }
                        break;
                }
            }
           
            //Clear screen
            SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
            SDL_RenderClear(gRenderer);

            body.update(deltaTime);

            //Update screen
            SDL_RenderPresent(gRenderer);
        }
    }

    //Free resources and close SDL
    close();

    return 0;
}
