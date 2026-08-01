#include "render/TextureManager.hpp"
#include "render/RenderHelpers.hpp"
#include "core/renderWindow.hpp"
#include <iostream>

TextureManager::~TextureManager() = default;

bool TextureManager::loadAssets(RenderWindow& window)
{
    mapBackground_ = window.loadTexture("assets/textures/map.png");
    if (mapBackground_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load map background: " << SDL_GetError();
        return false;
    }

    intersectionTexture_ = window.loadTexture("assets/textures/Intersection/intersection.png");
    if (intersectionTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load intersection texture: " << SDL_GetError();
        return false;
    }

    roadTexture_ = window.loadTexture("assets/textures/Roads/road.png");
    if (roadTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load road texture: " << SDL_GetError();
        return false;
    }

    bannedRoadTexture_ = window.loadTexture("assets/textures/Roads/road_banned.png");
    if (bannedRoadTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load banned road texture: " << SDL_GetError();
        return false;
    }

    greenLightTexture_ = window.loadTexture("assets/textures/TrafficLight/Green_light.png");
    if (greenLightTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load green light texture: " << SDL_GetError();
        return false;
    }

    yellowLightTexture_ = window.loadTexture("assets/textures/TrafficLight/Yellow_light.png");
    if (yellowLightTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load yellow light texture: " << SDL_GetError();
        return false;
    }

    redLightTexture_ = window.loadTexture("assets/textures/TrafficLight/Red_light.png");
    if (redLightTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load red light texture: " << SDL_GetError();
        return false;
    }

    carTexture_ = window.loadTexture("assets/textures/Vehicles/car.png");
    if (carTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load car texture: " << SDL_GetError();
        return false;
    }

    busTexture_ = window.loadTexture("assets/textures/Vehicles/bus.png");
    if (busTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load bus texture: " << SDL_GetError();
        return false;
    }

    emergencyTexture_ = window.loadTexture("assets/textures/Vehicles/emergency.png");
    if (emergencyTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load emergency vehicle texture: " << SDL_GetError();
        return false;
    }

    SDL_Renderer* renderer = window.getRenderer();
    heatMapOverlayTexture_ = RenderHelpers::createWhitePixelTexture(renderer);
    if (heatMapOverlayTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot create traffic heat-map overlay: "
                  << SDL_GetError();
        return false;
    }

    heatMapFont_ = TTF_OpenFont(
        "assets/textures/fonts/Octarine-Bold.otf",
        14);
    if (heatMapFont_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load traffic heat-map font: "
                  << TTF_GetError();
        return false;
    }

    const SDL_Color primaryText{238, 246, 248, 255};
    const SDL_Color secondaryText{190, 207, 212, 255};
    heatMapButtonLabelTexture_ = RenderHelpers::createTextTexture(
        renderer, heatMapFont_, "HEAT MAP", primaryText);
    heatMapLegendTitleTexture_ = RenderHelpers::createTextTexture(
        renderer, heatMapFont_, "TRAFFIC LEVEL", primaryText);
    heatMapLowLabelTexture_ = RenderHelpers::createTextTexture(
        renderer, heatMapFont_, "LOW       0 - 39%", secondaryText);
    heatMapModerateLabelTexture_ = RenderHelpers::createTextTexture(
        renderer, heatMapFont_, "MEDIUM  40 - 69%", secondaryText);
    heatMapHeavyLabelTexture_ = RenderHelpers::createTextTexture(
        renderer, heatMapFont_, "HIGH     70 - 100%", secondaryText);

    if (heatMapButtonLabelTexture_ == nullptr ||
        heatMapLegendTitleTexture_ == nullptr ||
        heatMapLowLabelTexture_ == nullptr ||
        heatMapModerateLabelTexture_ == nullptr ||
        heatMapHeavyLabelTexture_ == nullptr)
    {
        std::cerr << "\n[ERROR] Cannot create traffic heat-map labels: "
                  << TTF_GetError();
        return false;
    }

    return true;
}

SDL_Texture* TextureManager::getTrafficLightTexture(TrafficLightState state) const noexcept
{
    switch (state) {
        case TrafficLightState::GREEN:  return greenLightTexture_;
        case TrafficLightState::YELLOW: return yellowLightTexture_;
        case TrafficLightState::RED:    return redLightTexture_;
    }
    return redLightTexture_;
}

SDL_Texture* TextureManager::getVehicleTexture(VehicleType type) const noexcept
{
    switch (type) {
        case VehicleType::BUS:       return busTexture_;
        case VehicleType::EMERGENCY: return emergencyTexture_;
        case VehicleType::CAR:
        default:                     return carTexture_;
    }
}

void TextureManager::cleanUp(RenderWindow& window)
{
    window.cleanUpTexture(mapBackground_);
    window.cleanUpTexture(intersectionTexture_);
    window.cleanUpTexture(roadTexture_);
    window.cleanUpTexture(bannedRoadTexture_);

    window.cleanUpTexture(greenLightTexture_);
    window.cleanUpTexture(yellowLightTexture_);
    window.cleanUpTexture(redLightTexture_);

    window.cleanUpTexture(carTexture_);
    window.cleanUpTexture(busTexture_);
    window.cleanUpTexture(emergencyTexture_);

    window.cleanUpTexture(heatMapOverlayTexture_);
    window.cleanUpTexture(heatMapButtonLabelTexture_);
    window.cleanUpTexture(heatMapLegendTitleTexture_);
    window.cleanUpTexture(heatMapLowLabelTexture_);
    window.cleanUpTexture(heatMapModerateLabelTexture_);
    window.cleanUpTexture(heatMapHeavyLabelTexture_);

    if (heatMapFont_ != nullptr)
    {
        TTF_CloseFont(heatMapFont_);
        heatMapFont_ = nullptr;
    }
}
