#include "render/StatisticsPanel.hpp"

#include "core/renderWindow.hpp"
#include "core/Constants.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

StatisticsPanel::StatisticsPanel()
    : backgroundTexture_(nullptr),
      timeFont_(nullptr)
{
}

StatisticsPanel::~StatisticsPanel()
{
}

bool StatisticsPanel::loadAssets(RenderWindow& window)
{
    backgroundTexture_ = window.loadTexture("assets/textures/Panel/panel_background.png");

    if (backgroundTexture_ == nullptr)
    {
        std::cerr << "Cannot load control panel background: "<< SDL_GetError() << std::endl;
        return false;
    }

    timeFont_ = TTF_OpenFont("assets/textures/fonts/Octarine-Bold.otf", 42);

    if (timeFont_ == nullptr){
        std::cerr << "Cannot load time font: " << TTF_GetError() << std::endl;
        return false;
    }

    return true;
}

void StatisticsPanel::render(
    RenderWindow& window,
    double simulationTime)
{
    if (backgroundTexture_ == nullptr) {
        return;
    }

    SDL_Rect destination = {0,0,Config::PANEL_WIDTH,Config::WINDOW_HEIGHT};
    SDL_RenderCopy(window.getRenderer(),backgroundTexture_,nullptr,&destination);
    std::string timeText = formatTime(simulationTime);

    /*
        Change x and y depending on the empty time box
        in your panel background.
    */
    renderText(window,timeText,150,200);
}

void StatisticsPanel::renderText(
    RenderWindow& window,
    const std::string& text,
    int x,
    int y)
{
    if (timeFont_ == nullptr) {
        return;
    }

    SDL_Color textColor = {
        31,
        72,
        93,
        255
    };

    SDL_Surface* textSurface =
        TTF_RenderText_Blended(
            timeFont_,
            text.c_str(),
            textColor
        );

    if (textSurface == nullptr)
    {
        std::cerr << "Cannot create time surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(window.getRenderer(), textSurface);

    if (textTexture == nullptr)
    {
        std::cerr
            << "Cannot create time texture: "
            << SDL_GetError()
            << std::endl;

        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect textDestination = {
        x,
        y,
        textSurface->w,
        textSurface->h
    };

    SDL_FreeSurface(textSurface);

    SDL_RenderCopy(
        window.getRenderer(),
        textTexture,
        nullptr,
        &textDestination
    );

    SDL_DestroyTexture(textTexture);
}

std::string StatisticsPanel::formatTime(
    double simulationTime) const
{
    int totalSeconds = static_cast<int>(simulationTime);
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    std::ostringstream output;

    output << std::setfill('0') << std::setw(2) << hours << ":" << std::setw(2) << minutes << ":" << std::setw(2) << seconds;
    return output.str();
}

void StatisticsPanel::cleanUp(RenderWindow& window)
{
    if (backgroundTexture_ != nullptr) {
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
    }

    if (timeFont_ != nullptr)
    {
        TTF_CloseFont(timeFont_);
        timeFont_ = nullptr;
    }
}