#include "render/StatisticsPanel.hpp"

#include "core/renderWindow.hpp"
#include "core/Constants.hpp"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

StatisticsPanel::StatisticsPanel()
    : backgroundTexture_(nullptr),
      startTexture_(nullptr),
      pauseTexture_(nullptr),
      restartTexture_(nullptr),
      speedTextures_{},
      timeFont_(nullptr),
      speedFont_(nullptr),
      startButtonDestination_{59, 260, 110, 56},
      // Rendered size of the uploaded pause button image.
      pauseButtonDestination_{180, 260, 110, 56},
      restartButtonDestination_{301, 260, 110, 56},
      // The uploaded speed images are 780x240 and are rendered at half size.
      speedSliderDestination_{20, 365, 394, 121},
      // Slightly larger click areas than the visible images.
      startButtonRect_{49, 250, 115, 80},
      // Slightly larger click area than the visible image.
      pauseButtonRect_{170, 250, 125, 80},
      restartButtonRect_{296, 250, 125, 80},
      speedSliderRect_{20, 355, 394, 140},
      selectedSpeedIndex_(1),
      speedDragging_(false)
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

    const std::array<const char*, 5> speedAssetPaths = {
        "assets/textures/Panel/speed_0_5x.png",
        "assets/textures/Panel/speed_1x.png",
        "assets/textures/Panel/speed_2x.png",
        "assets/textures/Panel/speed_4x.png",
        "assets/textures/Panel/speed_8x.png"
    };

    for (size_t i = 0; i < speedAssetPaths.size(); ++i)
    {
        speedTextures_[i] = window.loadTexture(speedAssetPaths[i]);

        if (speedTextures_[i] == nullptr)
        {
            std::cerr << "Cannot load speed asset: "
                      << speedAssetPaths[i]
                      << ": "
                      << SDL_GetError()
                      << std::endl;
            cleanUp(window);
            return false;
        }
    }

    timeFont_ = TTF_OpenFont("assets/textures/fonts/Octarine-Bold.otf", 30);

    if (timeFont_ == nullptr){
        std::cerr << "Cannot load time font: " << TTF_GetError() << std::endl;
        cleanUp(window);
        return false;
    }

    speedFont_ = TTF_OpenFont("assets/textures/fonts/Octarine-Bold.otf", 18);

    if (speedFont_ == nullptr){
        std::cerr << "Cannot load speed font: " << TTF_GetError() << std::endl;
        cleanUp(window);
        return false;
    }

    return true;
}

PanelCommand StatisticsPanel::updateSpeedFromMouseX(int mouseX)
{
    const int trackLeft = speedSliderDestination_.x + 15;
    const int trackRight =
        speedSliderDestination_.x + speedSliderDestination_.w - 15;

    const int clampedMouseX = std::clamp(mouseX, trackLeft, trackRight);
    const double normalizedPosition =
        static_cast<double>(clampedMouseX - trackLeft) /
        static_cast<double>(trackRight - trackLeft);

    const int speedCount = static_cast<int>(SPEED_MULTIPLIERS_.size());
    const int newIndex = static_cast<int>(std::lround(
        normalizedPosition * static_cast<double>(speedCount - 1)));

    if (newIndex == selectedSpeedIndex_)
    {
        return PanelCommand::None;
    }

    selectedSpeedIndex_ = newIndex;
    return PanelCommand::SpeedChanged;
}

PanelCommand StatisticsPanel::handleEvent(const SDL_Event& event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT)
    {
        SDL_Point mousePoint{
            event.button.x,
            event.button.y
        };

        if (SDL_PointInRect(&mousePoint, &speedSliderRect_))
        {
            speedDragging_ = true;
            return updateSpeedFromMouseX(event.button.x);
        }

        return PanelCommand::None;
    }

    if (event.type == SDL_MOUSEMOTION && speedDragging_)
    {
        return updateSpeedFromMouseX(event.motion.x);
    }

    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT &&
        speedDragging_)
    {
        speedDragging_ = false;
        return updateSpeedFromMouseX(event.button.x);
    }

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

double StatisticsPanel::getSelectedSpeedMultiplier() const
{
    return SPEED_MULTIPLIERS_[selectedSpeedIndex_];
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

    if (speedTextures_[selectedSpeedIndex_] != nullptr)
    {
        SDL_RenderCopy(
            window.getRenderer(),
            speedTextures_[selectedSpeedIndex_],
            nullptr,
            &speedSliderDestination_);
    }

    std::string timeText = formatTime(simulationTime);

    /*
        Change x and y depending on the empty time box
        in your panel background.
    */
    renderText(window, timeFont_, timeText, 220, 218);
    renderText(window, speedFont_, "SIMULATION SPEED", 20, 335);
}

void StatisticsPanel::renderText(
    RenderWindow& window,
    TTF_Font* font,
    const std::string& text,
    int x,
    int y)
{
    if (font == nullptr) {
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
            font,
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

    if (speedFont_ != nullptr)
    {
        TTF_CloseFont(speedFont_);
        speedFont_ = nullptr;
    }

    for (SDL_Texture*& speedTexture : speedTextures_)
    {
        if (speedTexture != nullptr)
        {
            window.cleanUpTexture(speedTexture);
            speedTexture = nullptr;
        }
    }
}
