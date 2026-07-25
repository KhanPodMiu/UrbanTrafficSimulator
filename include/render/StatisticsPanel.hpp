#ifndef STATISTICS_PANEL_HPP
#define STATISTICS_PANEL_HPP

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <string>

class RenderWindow;

enum class PanelCommand
{
    None,
    Start,
    Pause,
    Restart
};

class StatisticsPanel
{
private:
    SDL_Texture* backgroundTexture_;
    SDL_Texture* startTexture_;
    SDL_Texture* pauseTexture_;
    SDL_Texture* restartTexture_;
    TTF_Font* timeFont_;

    SDL_Rect startButtonDestination_;
    SDL_Rect pauseButtonDestination_;
    SDL_Rect restartButtonDestination_;
    SDL_Rect startButtonRect_;
    SDL_Rect pauseButtonRect_;
    SDL_Rect restartButtonRect_;

    void renderText(RenderWindow& window,const std::string& text,int x,int y);
    std::string formatTime(double simulationTime) const;

public:
    StatisticsPanel();
    ~StatisticsPanel();

    bool loadAssets(RenderWindow& window);
    PanelCommand handleEvent(const SDL_Event& event) const;
    void render(RenderWindow& window, double simulationTime);
    void cleanUp(RenderWindow& window);
};

#endif
