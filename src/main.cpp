#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>
#include "core/renderWindow.hpp"
#include "simulation/WorldClock.hpp"

int main(int argc, char* args[]) {

    SDL_Init(SDL_INIT_EVERYTHING);
    RenderWindow* window = new RenderWindow("Urban Traffic", 1600, 900);
    SDL_Texture* MapBackground = window -> loadTexture("assets/maps/background.png");

    if(MapBackground == nullptr){
        std::cerr << "Hey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    //Declaration area
    bool is_game_running = true;
    SDL_Event event;

    WorldClock clock;
    const double TARGET_FPS = 30.0;
    const double TARGET_FRAME_TIME = 1.0 / TARGET_FPS;

    while(is_game_running){
        Uint64 frameStart = SDL_GetPerformanceCounter();
        clock.update();

        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                is_game_running = false;
            }
        }

        //Working area
        window -> clear();
        window -> render(MapBackground);
        window -> display();

        


        //Timestepping
        Uint64 frameEnd = SDL_GetPerformanceCounter();
        double frameDuration = static_cast<double>(frameEnd - frameStart) / static_cast<double>(SDL_GetPerformanceFrequency());

        if (frameDuration < TARGET_FRAME_TIME) {
            Uint32 delayMs = static_cast<Uint32>((TARGET_FRAME_TIME - frameDuration) * 1000.0);
            SDL_Delay(delayMs);
        }
    }

    window -> cleanUp();
    window -> cleanUpTexture(MapBackground);

    SDL_Quit();
    return 0;
}