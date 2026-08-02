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
      startHoverTexture_(nullptr),
      pauseHoverTexture_(nullptr),
      restartHoverTexture_(nullptr),
      speedTextures_{},
      algorithmTextures_{},
      timeFont_(nullptr),
      speedFont_(nullptr),
      isStartHovered_(false),
      isPauseHovered_(false),
      isRestartHovered_(false),
      startButtonDestination_{59, 260, 110, 56},
      // Rendered size of the uploaded pause button image.
      pauseButtonDestination_{180, 260, 110, 56},
      restartButtonDestination_{301, 260, 110, 56},
      speedSliderDestination_{59, 330, 340, 110},
      algorithmSliderDestination_{59, 450, 340, 110},
      // Slightly larger click areas than the visible images.
      startButtonRect_{49, 250, 115, 80},
      // Slightly larger click area than the visible image.
      pauseButtonRect_{170, 250, 125, 80},
      restartButtonRect_{296, 250, 125, 80},
      speedSliderRect_{59, 330, 340, 110},
      algorithmButtonRects_{
          SDL_Rect{59, 450, 121, 121},
          SDL_Rect{153, 450, 121, 121},
          SDL_Rect{285, 450, 121, 121},
      },
      selectedSpeedIndex_(1),
      speedDragging_(false),
      selectedAlgorithm_(RoutingAlgorithm::Dijkstra)
{
}

StatisticsPanel::~StatisticsPanel()
{
}

bool StatisticsPanel::loadAssets(RenderWindow& window)
{
    backgroundTexture_ = window.loadTexture(
        "assets/textures/Panel/panel_background.png");

    if (backgroundTexture_ == nullptr)
    {
        std::cerr << "Cannot load control panel background: "
                  << SDL_GetError()
                  << std::endl;
        return false;
    }

    startTexture_ = window.loadTexture("assets/textures/Panel/control_button/start.png");

    if (startTexture_ == nullptr)
    {
        std::cerr << "Cannot load start button: " << SDL_GetError() << std::endl;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    startHoverTexture_ = window.loadTexture("assets/textures/Panel/control_button/start_hover.png");

    if (startHoverTexture_ == nullptr)
    {
        std::cerr << "Cannot load start hover button: " << SDL_GetError() << std::endl;
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    pauseTexture_ = window.loadTexture("assets/textures/Panel/control_button/pause.png");

    if (pauseTexture_ == nullptr)
    {
        std::cerr << "Cannot load pause button: " << SDL_GetError() << std::endl;
        window.cleanUpTexture(startHoverTexture_);
        startHoverTexture_ = nullptr;
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    pauseHoverTexture_ = window.loadTexture("assets/textures/Panel/control_button/pause_hover.png");

    if (pauseHoverTexture_ == nullptr)
    {
        std::cerr << "Cannot load pause hover button: " << SDL_GetError() << std::endl;
        window.cleanUpTexture(pauseTexture_);
        pauseTexture_ = nullptr;
        window.cleanUpTexture(startHoverTexture_);
        startHoverTexture_ = nullptr;
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    restartTexture_ = window.loadTexture("assets/textures/Panel/control_button/restart.png");

    if (restartTexture_ == nullptr)
    {
        std::cerr << "Cannot load restart button: " << SDL_GetError() << std::endl;
        window.cleanUpTexture(pauseHoverTexture_);
        pauseHoverTexture_ = nullptr;
        window.cleanUpTexture(pauseTexture_);
        pauseTexture_ = nullptr;
        window.cleanUpTexture(startHoverTexture_);
        startHoverTexture_ = nullptr;
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    restartHoverTexture_ = window.loadTexture("assets/textures/Panel/control_button/restart_hover.png");

    if (restartHoverTexture_ == nullptr)
    {
        std::cerr << "Cannot load restart hover button: " << SDL_GetError() << std::endl;
        window.cleanUpTexture(restartTexture_);
        restartTexture_ = nullptr;
        window.cleanUpTexture(pauseHoverTexture_);
        pauseHoverTexture_ = nullptr;
        window.cleanUpTexture(pauseTexture_);
        pauseTexture_ = nullptr;
        window.cleanUpTexture(startHoverTexture_);
        startHoverTexture_ = nullptr;
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
        return false;
    }

    const std::array<const char*, 5> speedAssetPaths = {
        "assets/textures/Panel/speed/speed_0_5x.png",
        "assets/textures/Panel/speed/speed_1x.png",
        "assets/textures/Panel/speed/speed_2x.png",
        "assets/textures/Panel/speed/speed_4x.png",
        "assets/textures/Panel/speed/speed_8x.png",
    };

    for (std::size_t i = 0; i < speedAssetPaths.size(); ++i)
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

    const std::array<const char*, 3> algorithmAssetPaths = {
        "assets/textures/Panel/algorithms_button/BFS.png",
        "assets/textures/Panel/algorithms_button/DIJKSTRA.png",
        "assets/textures/Panel/algorithms_button/AStar.png"
    };

    for (std::size_t i = 0; i < algorithmAssetPaths.size(); ++i)
    {
        algorithmTextures_[i] = window.loadTexture(algorithmAssetPaths[i]);

        if (algorithmTextures_[i] == nullptr)
        {
            std::cerr << "Cannot load algorithm asset: "
                      << algorithmAssetPaths[i]
                      << ": "
                      << SDL_GetError()
                      << std::endl;
            cleanUp(window);
            return false;
        }
    }

    timeFont_ = TTF_OpenFont("assets/textures/fonts/Octarine-Bold.otf", 20);

    if (timeFont_ == nullptr)
    {
        std::cerr << "Cannot load time font: " << TTF_GetError() << std::endl;
        cleanUp(window);
        return false;
    }

    speedFont_ = TTF_OpenFont("assets/textures/fonts/Octarine-Bold.otf", 18);

    if (speedFont_ == nullptr)
    {
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

PanelCommand StatisticsPanel::updateAlgorithmFromMouseX(int mouseX, int mouseY)
{
    const SDL_Point mousePoint{mouseX, mouseY};

    for (std::size_t i = 0; i < algorithmButtonRects_.size(); ++i)
    {
        if (!SDL_PointInRect(&mousePoint, &algorithmButtonRects_[i]))
        {
            continue;
        }

        const RoutingAlgorithm algorithm =
            static_cast<RoutingAlgorithm>(i);

        if (algorithm == selectedAlgorithm_)
        {
            return PanelCommand::None;
        }

        selectedAlgorithm_ = algorithm;
        return PanelCommand::AlgorithmChanged;
    }

    return PanelCommand::None;
}

PanelCommand StatisticsPanel::handleEvent(const SDL_Event& event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT)
    {
        const SDL_Point mousePoint{event.button.x, event.button.y};

        isStartHovered_   = SDL_PointInRect(&mousePoint, &startButtonRect_);
        isPauseHovered_   = SDL_PointInRect(&mousePoint, &pauseButtonRect_);
        isRestartHovered_ = SDL_PointInRect(&mousePoint, &restartButtonRect_);

        if (SDL_PointInRect(&mousePoint, &speedSliderRect_))
        {
            speedDragging_ = true;
            return updateSpeedFromMouseX(event.button.x);
        }

        return PanelCommand::None;
    }

    if (event.type == SDL_MOUSEMOTION)
    {
        if (event.motion.state & SDL_BUTTON_LMASK)
        {
            const SDL_Point mousePoint{event.motion.x, event.motion.y};
            isStartHovered_   = SDL_PointInRect(&mousePoint, &startButtonRect_);
            isPauseHovered_   = SDL_PointInRect(&mousePoint, &pauseButtonRect_);
            isRestartHovered_ = SDL_PointInRect(&mousePoint, &restartButtonRect_);
        }

        if (speedDragging_)
        {
            return updateSpeedFromMouseX(event.motion.x);
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT && speedDragging_)
    {
        speedDragging_ = false;
        return updateSpeedFromMouseX(event.button.x);
    }

    if (event.type != SDL_MOUSEBUTTONUP ||
        event.button.button != SDL_BUTTON_LEFT)
    {
        return PanelCommand::None;
    }

    const SDL_Point mousePoint{event.button.x, event.button.y};

    bool wasStart = isStartHovered_;
    bool wasPause = isPauseHovered_;
    bool wasRestart = isRestartHovered_;

    isStartHovered_ = false;
    isPauseHovered_ = false;
    isRestartHovered_ = false;

    if (wasStart && SDL_PointInRect(&mousePoint, &startButtonRect_))
    {
        return PanelCommand::Start;
    }

    if (wasPause && SDL_PointInRect(&mousePoint, &pauseButtonRect_))
    {
        return PanelCommand::Pause;
    }

    if (wasRestart && SDL_PointInRect(&mousePoint, &restartButtonRect_))
    {
        return PanelCommand::Restart;
    }

    const PanelCommand algorithmCommand = updateAlgorithmFromMouseX(
        event.button.x,
        event.button.y);

    if (algorithmCommand != PanelCommand::None)
    {
        return algorithmCommand;
    }

    return PanelCommand::None;
}

double StatisticsPanel::getSelectedSpeedMultiplier() const
{
    return SPEED_MULTIPLIERS_[selectedSpeedIndex_];
}

RoutingAlgorithm StatisticsPanel::getSelectedAlgorithm() const
{
    return selectedAlgorithm_;
}

void StatisticsPanel::render(RenderWindow& window,double simulationTime,int vehicleCount)
{
    if (backgroundTexture_ == nullptr)
    {
        return;
    }

    SDL_Rect destination = {
        0,
        0,
        Config::PANEL_WIDTH,
        Config::WINDOW_HEIGHT,
    };
    SDL_RenderCopy(
        window.getRenderer(),
        backgroundTexture_,
        nullptr,
        &destination);

    SDL_Texture* currentStartTexture = (isStartHovered_ && startHoverTexture_ != nullptr) 
                                        ? startHoverTexture_ 
                                        : startTexture_;
    if (currentStartTexture != nullptr)
    {
        SDL_RenderCopy(
            window.getRenderer(),
            currentStartTexture,
            nullptr,
            &startButtonDestination_);
    }

    SDL_Texture* currentPauseTexture = (isPauseHovered_ && pauseHoverTexture_ != nullptr) 
                                        ? pauseHoverTexture_ 
                                        : pauseTexture_;
    if (currentPauseTexture != nullptr)
    {
        SDL_RenderCopy(
            window.getRenderer(),
            currentPauseTexture,
            nullptr,
            &pauseButtonDestination_);
    }

    SDL_Texture* currentRestartTexture = (isRestartHovered_ && restartHoverTexture_ != nullptr) 
                                          ? restartHoverTexture_ 
                                          : restartTexture_;
    if (currentRestartTexture != nullptr)
    {
        SDL_RenderCopy(
            window.getRenderer(),
            currentRestartTexture,
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

    const std::size_t algorithmIndex =
        static_cast<std::size_t>(selectedAlgorithm_);

    if (algorithmTextures_[algorithmIndex] != nullptr)
    {
        SDL_RenderCopy(
            window.getRenderer(),
            algorithmTextures_[algorithmIndex],
            nullptr,
            &algorithmSliderDestination_);
    }

    std::string timeText = formatTime(simulationTime);

    /*
        Change x and y depending on the empty time box
        in your panel background.
    */
    renderText(window, timeFont_, timeText, 280, 218);
    renderText(window, timeFont_, std::to_string(vehicleCount), 300, 570);
}

void StatisticsPanel::renderText(
    RenderWindow& window,
    TTF_Font* font,
    const std::string& text,
    int x,
    int y)
{
    if (font == nullptr)
    {
        return;
    }

    const SDL_Color textColor{31, 72, 93, 255};

    SDL_Surface* textSurface = TTF_RenderText_Blended(
        font,
        text.c_str(),
        textColor);

    if (textSurface == nullptr)
    {
        std::cerr << "Cannot create time surface: " << TTF_GetError() << std::endl;
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(
        window.getRenderer(),
        textSurface);

    if (textTexture == nullptr)
    {
        std::cerr
            << "Cannot create time texture: "
            << SDL_GetError()
            << std::endl;

        SDL_FreeSurface(textSurface);
        return;
    }

    const SDL_Rect textDestination{
        x,
        y,
        textSurface->w,
        textSurface->h,
    };

    SDL_FreeSurface(textSurface);

    SDL_RenderCopy(
        window.getRenderer(),
        textTexture,
        nullptr,
        &textDestination);

    SDL_DestroyTexture(textTexture);
}

std::string StatisticsPanel::formatTime(double simulationTime) const
{
    int totalSeconds = static_cast<int>(simulationTime);
    int hours = totalSeconds / 3600;
    int minutes = (totalSeconds % 3600) / 60;
    int seconds = totalSeconds % 60;
    std::ostringstream output;

    output << std::setfill('0') << std::setw(2) << hours
           << ":" << std::setw(2) << minutes
           << ":" << std::setw(2) << seconds;
    return output.str();
}

void StatisticsPanel::cleanUp(RenderWindow& window)
{
    if (backgroundTexture_ != nullptr)
    {
        window.cleanUpTexture(backgroundTexture_);
        backgroundTexture_ = nullptr;
    }

    if (startTexture_ != nullptr)
    {
        window.cleanUpTexture(startTexture_);
        startTexture_ = nullptr;
    }

    if (startHoverTexture_ != nullptr) { 
        window.cleanUpTexture(startHoverTexture_); 
        startHoverTexture_ = nullptr; 
    }

    if (pauseTexture_ != nullptr)
    {
        window.cleanUpTexture(pauseTexture_);
        pauseTexture_ = nullptr;
    }

    if (pauseHoverTexture_ != nullptr) { 
        window.cleanUpTexture(pauseHoverTexture_); 
        pauseHoverTexture_ = nullptr; 
    }

    if (restartTexture_ != nullptr)
    {
        window.cleanUpTexture(restartTexture_);
        restartTexture_ = nullptr;
    }
    
    if (restartHoverTexture_ != nullptr) { 
        window.cleanUpTexture(restartHoverTexture_); 
        restartHoverTexture_ = nullptr; 
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

    for (SDL_Texture*& algorithmTexture : algorithmTextures_)
    {
        if (algorithmTexture != nullptr)
        {
            window.cleanUpTexture(algorithmTexture);
            algorithmTexture = nullptr;
        }
    }
}
