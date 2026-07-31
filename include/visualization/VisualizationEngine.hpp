#pragma once

#include "SDL2/SDL.h"
#include "utils/vector2i.hpp"
#include "vehicles/Vehicle.hpp"
#include <filesystem>
#include <vector>
#include <memory>

class Graph;
class RenderWindow;
class Camera;
class Road;
class Vehicle;
class Intersection;

class VisualizationEngine
{
public:
    VisualizationEngine();
    ~VisualizationEngine();

    bool loadAssets(RenderWindow& window);

    void buildRenderCache(const Graph& graph);

    void render(RenderWindow& window, const Camera& camera);

    // Draws the current dynamic vehicle list. Kept separate from render()
    // since vehicle positions change every frame while roads/intersections
    // are cached once in buildRenderCache().
    void renderVehicles(RenderWindow& window, const Camera& camera, const std::vector<std::shared_ptr<Vehicle>>& vehicles);

    std::shared_ptr<Vehicle> pickVehicle(
        const Camera& camera,
        const std::vector<std::shared_ptr<Vehicle>>& vehicles,
        int mouseX,
        int mouseY) const;

    std::shared_ptr<Intersection> pickIntersection(
        const Camera& camera,
        int mouseX,
        int mouseY) const;

    void renderSelectionHighlight(
        RenderWindow& window,
        const Camera& camera,
        const std::shared_ptr<Vehicle>& selectedVehicle,
        const std::shared_ptr<Intersection>& selectedIntersection) const;

    void cleanUp(RenderWindow& window);

private:
    struct IntersectionRenderData
    {
        Vector2 center;
        std::shared_ptr<Intersection> intersection;
    };

    struct RoadRenderData
    {
        Vector2 start;
        Vector2 end;
        float length;
        float angle;
        float offsetX;
        float offsetY;

        std::shared_ptr<Road> road;

        Vector2 lightPos;
    };

    Vector2 applyCamera(const Vector2& worldPos, const Camera& camera) const;

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

    std::vector<IntersectionRenderData> intersectionsLocation_;
    std::vector<RoadRenderData> roadsLocation_;
};