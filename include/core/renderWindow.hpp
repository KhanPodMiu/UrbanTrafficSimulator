#pragma once

#include "SDL.h"

class RenderWindow{
public:
    RenderWindow(const char* title, int w, int h);
    SDL_Texture* loadTexture(const char* filePath);
    void cleanUp();
    void clear();
    void render(SDL_Texture* texture);
    void display();

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};