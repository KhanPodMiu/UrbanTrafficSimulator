#include "render/StatisticsPanel.hpp"

#include "core/renderWindow.hpp"
#include "core/Constants.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>

StatisticsPanel::StatisticsPanel()
    : backgroundTexture_(nullptr),
      startTexture_(nullptr),
      pauseTexture_(nullptr),
      restartTexture_(nullptr),
      timeFont_(nullptr),
      startButtonDestination_{59, 260, 110, 56},
      // Rendered size of the uploaded pause button image.
      pauseButtonDestination_{180, 260, 110, 56},
      restartButtonDestination_{301, 260, 110, 56},
      // Slightly larger click areas than the visible images.
      startButtonRect_{49, 250, 115, 80},
      // Slightly larger click area than the visible image.
      pauseButtonRect_{170, 250, 125, 80},
      restartButtonRect_{296, 250, 125, 80}
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

    startTexture_ = window.loadTexture("assets/textures/Panel/start.png");

    if (startTexture_ == nullptr)
    {
        std::cerr << "Cannot load start button: " << SDL_GetError() << std::endl;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    pauseTexture_ = window.loadTexture("assets/textures/Panel/pause.png");

    if (pauseTexture_ == nullptr)
    {
        std::cerr << "Cannot load pause button: " << SDL_GetError() << std::endl;
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    restartTexture_ = window.loadTexture("assets/textures/Panel/restart.png");

    if (restartTexture_ == nullptr)
    {
        std::cerr << "Cannot load restart button: " << SDL_GetError() << std::endl;
        window.cleanUpTexture(pauseTexture_);
        pauseTexture_ = nullptr;
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    timeFont_ = TTF_OpenFont("assets/textures/fonts/Octarine-Bold.otf", 30);

    if (timeFont_ == nullptr){
        std::cerr << "Cannot load time font: " << TTF_GetError() << std::endl;
        window.cleanUpTexture(restartTexture_);
        restartTexture_ = nullptr;
        window.cleanUpTexture(pauseTexture_);
        pauseTexture_ = nullptr;
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    return true;
}

PanelCommand StatisticsPanel::handleEvent(const SDL_Event& event) const
{
    if (event.type != SDL_MOUSEBUTTONUP ||
        event.button.button != SDL_BUTTON_LEFT)
    {
        return PanelCommand::None;
    }

    SDL_Point mousePoint{
        event.button.x,
        event.button.y
    };

    if (SDL_PointInRect(&mousePoint, &startButtonRect_))
    {
        return PanelCommand::Start;
    }

    if (SDL_PointInRect(&mousePoint, &pauseButtonRect_))
    {
        return PanelCommand::Pause;
    }

    if (SDL_PointInRect(&mousePoint, &restartButtonRect_))
    {
        return PanelCommand::Restart;
    }

    return PanelCommand::None;
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

    if (startTexture_ != nullptr)
    {
        SDL_RenderCopy(
            window.getRenderer(),
            startTexture_,
            nullptr,
            &startButtonDestination_);
    }

    if (pauseTexture_ != nullptr)
    {
        SDL_RenderCopy(
            window.getRenderer(),
            pauseTexture_,
            nullptr,
            &pauseButtonDestination_);
    }

    if (restartTexture_ != nullptr)
    {
        SDL_RenderCopy(
            window.getRenderer(),
            restartTexture_,
            nullptr,
            &restartButtonDestination_);
    }

    std::string timeText = formatTime(simulationTime);

    /*
        Change x and y depending on the empty time box
        in your panel background.
    */
    renderText(window,timeText,220,218);
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

    if (startTexture_ != nullptr) {
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
    }

    if (pauseTexture_ != nullptr) {
        window.cleanUpTexture(pauseTexture_);
        pauseTexture_ = nullptr;
    }

    if (restartTexture_ != nullptr) {
        window.cleanUpTexture(restartTexture_);
        restartTexture_ = nullptr;
    }

    if (timeFont_ != nullptr)
    {
        TTF_CloseFont(timeFont_);
        timeFont_ = nullptr;
    }
}
