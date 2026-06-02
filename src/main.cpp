#include "SDL.h"
#include "SDL_image.h"
#include <iostream>
#include "core/renderWindow.hpp"

int main(int argc, char* args[]) {

    if(SDL_Init(SDL_INIT_VIDEO) != 0){
        std::cerr << "Hey... SDL has failed" << SDL_GetError();
        return 1;
    }

    if(!IMG_Init(IMG_INIT_PNG)){
        std::cerr << "IMG_Init has failed" << SDL_GetError();
        return 1;
    }

    RenderWindow* window = new RenderWindow("Urban Traffic", 1600, 900);

    bool is_game_running = true;

    SDL_Event event;

    while(is_game_running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                is_game_running = false;
            }
        }
    }

    window -> cleanUp();
    delete window;

    SDL_Quit();
    return 0;
}