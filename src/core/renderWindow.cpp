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

SDL_Texture* RenderWindow::loadTexture(const char* filePath){
    SDL_Texture* texture = nullptr;
    texture = IMG_LoadTexture(renderer, filePath);

    if(texture == nullptr){
        std::cerr << "Failed to load texture" << SDL_GetError();
        return nullptr;
    }

    return texture;
}

void RenderWindow::clear(){
    SDL_RenderClear(renderer);
}

void RenderWindow::render(SDL_Texture* texture){
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
}

void RenderWindow::display(){
    SDL_RenderPresent(renderer);
}

void RenderWindow::cleanUp(){
    SDL_DestroyWindow(window);
}