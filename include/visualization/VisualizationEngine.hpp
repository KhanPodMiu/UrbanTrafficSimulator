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

    /// Loads all texture assets (map background, roads, intersections, vehicles, traffic lights).
    bool loadAssets(RenderWindow& window);

    /// Pre-computes render data for roads and intersections (called once after map load).
    void buildRenderCache(const Graph& graph);

    /// Renders the static map elements: background, roads, intersections, and traffic lights.
    void render(RenderWindow& window, const Camera& camera);

    /// Draws the current dynamic vehicle list. Kept separate from render()
    /// since vehicle positions change every frame while roads/intersections
    /// are cached once in buildRenderCache().
    void renderVehicles(RenderWindow& window, const Camera& camera, const std::vector<std::shared_ptr<Vehicle>>& vehicles);

    /// Returns the vehicle under the mouse cursor, or nullptr if none is hit.
    std::shared_ptr<Vehicle> pickVehicle(
        const Camera& camera,
        const std::vector<std::shared_ptr<Vehicle>>& vehicles,
        int mouseX,
        int mouseY) const;

    /// Returns the intersection under the mouse cursor, or nullptr if none is hit.
    std::shared_ptr<Intersection> pickIntersection(
        const Camera& camera,
        int mouseX,
        int mouseY) const;

    /// Draws a highlight outline around the currently selected vehicle or intersection.
    void renderSelectionHighlight(
        RenderWindow& window,
        const Camera& camera,
        const std::shared_ptr<Vehicle>& selectedVehicle,
        const std::shared_ptr<Intersection>& selectedIntersection) const;

    /// Releases all loaded textures.
    void cleanUp(RenderWindow& window);

private:
    /// Cached render data for a single intersection.
    struct IntersectionRenderData
    {
        Vector2 center;
        std::shared_ptr<Intersection> intersection;
    };

    /// Cached render data for a single road segment.
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

    /// Converts a world-space position to screen-space using the camera transform.
    Vector2 applyCamera(const Vector2& worldPos, const Camera& camera) const;

    // ---- Texture handles ----
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

    // ---- Pre-computed render caches ----
    std::vector<IntersectionRenderData> intersectionsLocation_;
    std::vector<RoadRenderData> roadsLocation_;
};