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

    SDL_Texture* MapBackground = window -> loadTexture("assets/maps/background.png");

    if(MapBackground == nullptr){
        std::cerr << "Hey.. recheck the IMG PATH" << SDL_GetError();
        return 1;
    }

    bool is_game_running = true;

    SDL_Event event;

    while(is_game_running){
        while(SDL_PollEvent(&event)){
            if(event.type == SDL_QUIT){
                is_game_running = false;
            }
        }

        window -> clear();
        window -> render(MapBackground);
        window -> display();
    }

    window -> cleanUp();

    delete MapBackground;

    SDL_Quit();
    return 0;
}