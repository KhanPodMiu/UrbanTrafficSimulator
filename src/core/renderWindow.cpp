#include "core/renderWindow.hpp"
#include "SDL.h"
#include "SDL_image.h"
#include <iostream>

RenderWindow::RenderWindow(const char* title, int w, int h) : window(nullptr), renderer(nullptr) {
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    
    if(window == nullptr){
        std::cerr << "Window failed to init" << SDL_GetError();
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    if(renderer == nullptr){
        std::cerr << "Renderer failed to init" << SDL_GetError();
    }
}

void RenderWindow::cleanUp(){
    SDL_DestroyWindow(window);
}