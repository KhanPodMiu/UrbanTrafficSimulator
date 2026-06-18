#include "core/renderWindow.hpp"
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <iostream>

RenderWindow::RenderWindow(const char* title, int w, int h) : window(nullptr), renderer(nullptr) {
    window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    if(window == nullptr){
        std::cerr << "Window failed to init" << SDL_GetError();
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

SDL_Renderer* RenderWindow::getRenderer(){
    return renderer;
}

void RenderWindow::renderLine(int x1, int y1, int x2, int y2){
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void RenderWindow::renderRect(int x, int y, int w, int h){
    SDL_Rect rect{x,y,w,h};
    SDL_RenderFillRect(renderer,&rect);
}

void RenderWindow::clear(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void RenderWindow::render(SDL_Texture* texture, Vector2& AimPoint, int OBJECT_RADIUS)
{
    SDL_Rect point;

    point.x = AimPoint.x;
    point.y = AimPoint.y;

    if (OBJECT_RADIUS == 0){
        SDL_QueryTexture(texture, nullptr, nullptr, &point.w, &point.h);
    }
    else{
        point.w = OBJECT_RADIUS * 2;
        point.h = OBJECT_RADIUS * 2;
    }

    SDL_RenderCopy(renderer, texture, nullptr, &point);
}

void RenderWindow::display(){
    SDL_RenderPresent(renderer);
}

void RenderWindow::cleanUp(){
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
}

void RenderWindow::cleanUpTexture(SDL_Texture* texture){
    SDL_DestroyTexture(texture);
}

void RenderWindow::renderRoad(SDL_Texture* texture, const Vector2& start, int length, int width, double angle)
{
    SDL_Rect dstRect;

    dstRect.x = start.x;
    dstRect.y = start.y - width / 2;

    dstRect.w = length;
    dstRect.h = width;

    SDL_Point pivot;
    pivot.x = 0;
    pivot.y = width / 2;

    SDL_RenderCopyEx(renderer, texture, nullptr, &dstRect, angle, &pivot, SDL_FLIP_NONE);
}