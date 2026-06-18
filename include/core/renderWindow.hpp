#pragma once

#include "SDL2/SDL.h"
#include "utils/vector2i.hpp"

class RenderWindow{
public:
    RenderWindow(const char* title, int w, int h);
    SDL_Texture* loadTexture(const char* filePath);
    void cleanUp();
    void clear();
    void render(SDL_Texture* texture, Vector2& AimPoint, int OBJECT_RADIUS = 0);
    void display();
    void cleanUpTexture(SDL_Texture* texture);

    void renderRoad(SDL_Texture* texture, const Vector2& start, int length, int width, double angle);
    void renderLine(int x1, int x2, int y1, int y2);
    void renderRect(int x, int y, int w, int h);
    SDL_Renderer* getRenderer();

private:
    SDL_Window *window;
    SDL_Renderer *renderer;
};