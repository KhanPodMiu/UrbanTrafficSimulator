#pragma once

#include "SDL2/SDL.h"
#include "vehicles/Vehicle.hpp"
#include "graph/Road.hpp"

class RenderWindow;

class TextureManager
{
public:
    TextureManager() = default;
    ~TextureManager();

    /// Loads all texture assets (map background, roads, intersections, vehicles, traffic lights, heatmap font/labels).
    bool loadAssets(RenderWindow& window);

    /// Releases all loaded textures and fonts.
    void cleanUp(RenderWindow& window);

    // ---- Getters ----
    SDL_Texture* getMapBackground() const noexcept { return mapBackground_; }
    SDL_Texture* getIntersectionTexture() const noexcept { return intersectionTexture_; }
    SDL_Texture* getRoadTexture() const noexcept { return roadTexture_; }
    SDL_Texture* getBannedRoadTexture() const noexcept { return bannedRoadTexture_; }

    SDL_Texture* getGreenLightTexture() const noexcept { return greenLightTexture_; }
    SDL_Texture* getYellowLightTexture() const noexcept { return yellowLightTexture_; }
    SDL_Texture* getRedLightTexture() const noexcept { return redLightTexture_; }
    SDL_Texture* getTrafficLightTexture(TrafficLightState state) const noexcept;

    SDL_Texture* getCarTexture() const noexcept { return carTexture_; }
    SDL_Texture* getBusTexture() const noexcept { return busTexture_; }
    SDL_Texture* getEmergencyTexture() const noexcept { return emergencyTexture_; }
    SDL_Texture* getVehicleTexture(VehicleType type) const noexcept;

    SDL_Texture* getHeatMapOverlayTexture() const noexcept { return heatMapOverlayTexture_; }
    SDL_Texture* getHeatMapOnButtonTexture() const noexcept { return heatMapOnButtonTexture_; }
    SDL_Texture* getHeatMapOffButtonTexture() const noexcept { return heatMapOffButtonTexture_; }

private:
    SDL_Texture* mapBackground_ = nullptr;
    SDL_Texture* intersectionTexture_ = nullptr;
    SDL_Texture* roadTexture_ = nullptr;
    SDL_Texture* bannedRoadTexture_ = nullptr;

    SDL_Texture* greenLightTexture_ = nullptr;
    SDL_Texture* redLightTexture_ = nullptr;
    SDL_Texture* yellowLightTexture_ = nullptr;

    SDL_Texture* carTexture_ = nullptr;
    SDL_Texture* busTexture_ = nullptr;
    SDL_Texture* emergencyTexture_ = nullptr;

    SDL_Texture* heatMapOverlayTexture_ = nullptr;
    SDL_Texture* heatMapOnButtonTexture_ = nullptr;
    SDL_Texture* heatMapOffButtonTexture_ = nullptr;
};
