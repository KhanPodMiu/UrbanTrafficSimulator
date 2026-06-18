#pragma once

#include "SDL2/SDL.h"

class RenderWindow{
public:
    RenderWindow(const char* title, int w, int h);
    SDL_Texture* loadTexture(const char* filePath);
    void cleanUp();
    void clear();
    void render(SDL_Texture* texture);
    void display();
    void cleanUpTexture(SDL_Texture* texture);

    void renderLine(int x1, int x2, int y1, int y2);
    void renderRect(int x, int y, int w, int h);
    SDL_Renderer* getRenderer();

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};