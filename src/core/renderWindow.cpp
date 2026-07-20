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
    SDL_Rect rect{x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void RenderWindow::clear(){
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void RenderWindow::render(SDL_Texture* texture, Vector2& AimPoint, float zoom, float OBJECT_RADIUS)
{
    SDL_Rect point;
    point.x = (int)AimPoint.x;
    point.y = (int)AimPoint.y;

    if (OBJECT_RADIUS == 0) {
        SDL_QueryTexture(texture, nullptr, nullptr, &point.w, &point.h);
        point.w = (int)(point.w * zoom);
        point.h = (int)(point.h * zoom);
    } else {
        point.w = (int)(OBJECT_RADIUS * 2);
        point.h = (int)(OBJECT_RADIUS * 2);
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


void RenderWindow::renderRoad(SDL_Texture* texture, const Vector2& start, float length, float width, double angle)
{
    int texW, texH;
    SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);

    float scale     = width / (float)texH;
    float tileWidth = texW * scale;
    if (tileWidth <= 0) return;

    // FIX: Overlap mỗi tile 1px để lấp gap do floating point
    const float OVERLAP = 1.0f;

    int fullTiles   = (int)(length / tileWidth);
    float remainder = length - fullTiles * tileWidth;

    for (int i = 0; i <= fullTiles; i++)
    {
        bool isLastTile   = (i == fullTiles);
        float thisTileScreenW = isLastTile ? remainder : (tileWidth + OVERLAP);
        if (thisTileScreenW <= 0.5f) break;

        // FIX: Dùng float thuần, chỉ round khi gán vào SDL_Rect
        float offsetX = i * tileWidth;  // KHÔNG dùng roundf ở đây

        SDL_Rect srcRect;
        srcRect.x = 0;
        srcRect.y = 0;
        srcRect.w = isLastTile
            ? (int)roundf((float)texW * (remainder / tileWidth))
            : texW;
        srcRect.h = texH;

        SDL_Rect dstRect;
        dstRect.x = (int)roundf(start.x + offsetX);
        dstRect.y = (int)roundf(start.y - width / 2.0f);
        dstRect.w = (int)ceilf(thisTileScreenW);   // FIX: ceilf thay roundf để không bị thiếu pixel
        dstRect.h = (int)ceilf(width);

        SDL_Point pivot;
        pivot.x = -(int)roundf(offsetX);
        pivot.y = (int)roundf(width / 2.0f);

        SDL_RenderCopyEx(renderer, texture, &srcRect, &dstRect, angle, &pivot, SDL_FLIP_VERTICAL);
    }
}

void RenderWindow::renderTrafficLight(SDL_Texture* texture, int centerX, int centerY, int size){
    int texW, texH;
    SDL_QueryTexture(texture, nullptr, nullptr, &texW, &texH);

    float scale = (size / (float)texW < size / (float)texH)
                  ? size / (float)texW
                  : size / (float)texH;
    int w = (int)roundf(texW * scale);
    int h = (int)roundf(texH * scale);

    SDL_Rect rect{ centerX - w / 2, centerY - h / 2, w, h };
    SDL_RenderCopy(renderer, texture, nullptr, &rect);
}