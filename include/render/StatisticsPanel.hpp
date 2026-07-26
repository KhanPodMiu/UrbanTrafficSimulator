#ifndef STATISTICS_PANEL_HPP
#define STATISTICS_PANEL_HPP

#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"

#include <array>
#include <string>

class RenderWindow;

enum class PanelCommand
{
    None,
    Start,
    Pause,
    Restart,
    SpeedChanged,
    AlgorithmChanged
};

enum class RoutingAlgorithm
{
    Dijkstra,
    BFS,
    AStar
};

class StatisticsPanel
{
private:
    SDL_Texture* backgroundTexture_;
    SDL_Texture* startTexture_;
    SDL_Texture* pauseTexture_;
    SDL_Texture* restartTexture_;
    std::array<SDL_Texture*, 5> speedTextures_;
    std::array<SDL_Texture*, 3> algorithmTextures_;
    TTF_Font* timeFont_;
    TTF_Font* speedFont_;

    SDL_Rect startButtonDestination_;
    SDL_Rect pauseButtonDestination_;
    SDL_Rect restartButtonDestination_;
    SDL_Rect speedSliderDestination_;
    SDL_Rect algorithmSliderDestination_;
    SDL_Rect startButtonRect_;
    SDL_Rect pauseButtonRect_;
    SDL_Rect restartButtonRect_;
    SDL_Rect speedSliderRect_;
    std::array<SDL_Rect, 3> algorithmButtonRects_;

    int selectedSpeedIndex_;
    bool speedDragging_;
    RoutingAlgorithm selectedAlgorithm_;

    inline static constexpr std::array<double, 5> SPEED_MULTIPLIERS_ = {
        0.5, 1.0, 2.0, 4.0, 8.0
    };

    PanelCommand updateSpeedFromMouseX(int mouseX);
    PanelCommand updateAlgorithmFromMouseX(int mouseX, int mouseY);
    void renderText(RenderWindow& window, TTF_Font* font, const std::string& text, int x, int y);
    std::string formatTime(double simulationTime) const;

public:
    StatisticsPanel();
    ~StatisticsPanel();

    bool loadAssets(RenderWindow& window);
    PanelCommand handleEvent(const SDL_Event& event);
    double getSelectedSpeedMultiplier() const;
    RoutingAlgorithm getSelectedAlgorithm() const;
    void render(RenderWindow& window, double simulationTime);
    void cleanUp(RenderWindow& window);
};

#endif
