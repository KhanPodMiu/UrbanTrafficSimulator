#ifndef STATISTICS_PANEL_HPP
#define STATISTICS_PANEL_HPP

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <string>

class RenderWindow;

class StatisticsPanel
{
private:
    SDL_Texture* backgroundTexture_;
    TTF_Font* timeFont_;
    void renderText(RenderWindow& window,const std::string& text,int x,int y);
    std::string formatTime(double simulationTime) const;

public:
    StatisticsPanel();
    ~StatisticsPanel();

    bool loadAssets(RenderWindow& window);
    void render(RenderWindow& window, double simulationTime);
    void cleanUp(RenderWindow& window);
};

#endif