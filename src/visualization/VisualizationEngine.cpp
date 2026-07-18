#include "visualization/VisualizationEngine.hpp"
#include "core/Constants.hpp"
#include "core/renderWindow.hpp"
#include "visualization/camera.hpp"
#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"

#include <iostream>
#include <cmath>

VisualizationEngine::VisualizationEngine() = default;
VisualizationEngine::~VisualizationEngine() = default;

Vector2 VisualizationEngine::applyCamera(const Vector2& worldPos, const Camera& camera) const
{
    float zoom = camera.getZoom();
    float screenX = (worldPos.x - camera.getX()) * zoom;
    float screenY = (worldPos.y - camera.getY()) * zoom;
    return Vector2(screenX, screenY);
}

bool VisualizationEngine::loadAssets(RenderWindow& window)
{
    mapBackground_ = window.loadTexture("assets/textures/BG.png");
    if (mapBackground_ == nullptr) {
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return false;
    }

    intersectionTexture_ = window.loadTexture("assets/textures/Intersection/Roundabout.png");
    if (intersectionTexture_ == nullptr) {
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return false;
    }

    roadTexture_ = window.loadTexture("assets/textures/Roads/road_striped.png");
    if (roadTexture_ == nullptr) {
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return false;
    }

    return true;
}

void VisualizationEngine::buildRenderCache(const Graph& graph)
{
    intersectionsLocation_.clear();
    roadsLocation_.clear();

    for (const auto& [intersectionID, intersection] : graph.getIntersections()) {
        Vector2 temp(intersection->getX() - Config::ROUNDABOUT_RADIUS,
                     intersection->getY() - Config::ROUNDABOUT_RADIUS);
        intersectionsLocation_.push_back(temp);
    }

    for (const auto& [roadID, road] : graph.getRoads()) {
        const Intersection* src = road->getSourceIntersection();
        const Intersection* dst = road->getDestinationIntersection();

        RoadRenderData temp;

        temp.start = Vector2(src->getX(), src->getY());
        temp.end   = Vector2(dst->getX(), dst->getY());

        float dx = temp.end.x - temp.start.x;
        float dy = temp.end.y - temp.start.y;

        temp.length = std::sqrt(dx * dx + dy * dy);
        temp.angle  = std::atan2(dy, dx) * 180.0f / static_cast<float>(M_PI);

        float angle_rad = std::atan2(dy, dx);
        float halfWidth = Config::ROAD_WIDTH * 0.5f;
        temp.offsetX = -std::sin(angle_rad) * halfWidth;
        temp.offsetY =  std::cos(angle_rad) * halfWidth;

        roadsLocation_.push_back(temp);
    }
}

void VisualizationEngine::render(RenderWindow& window, const Camera& camera)
{
    float zoom = camera.getZoom();

    Vector2 worldOrigin(0, 0);
    Vector2 bgPos = applyCamera(worldOrigin, camera);
    bgPos.x += Config::PANEL_WIDTH;
    window.render(mapBackground_, bgPos, zoom);

    for (const auto& road : roadsLocation_) {
        Vector2 shiftedStart(road.start.x + road.offsetX,
                              road.start.y + road.offsetY);

        Vector2 renderPos = applyCamera(shiftedStart, camera);
        renderPos.x += Config::PANEL_WIDTH;

        window.renderRoad(roadTexture_, renderPos, road.length * zoom, Config::ROAD_WIDTH * zoom, road.angle);
    }

    // Intersection
    for (const auto& intersection : intersectionsLocation_) {
        Vector2 renderPos = applyCamera(intersection, camera);
        renderPos.x += Config::PANEL_WIDTH;
        window.render(intersectionTexture_, renderPos, zoom, Config::ROUNDABOUT_RADIUS * zoom);
    }

    // UI Panel
    SDL_SetRenderDrawColor(window.getRenderer(), 40, 40, 40, 255);
    SDL_Rect panel = {0, 0, Config::PANEL_WIDTH, Config::WINDOW_HEIGHT};
    SDL_RenderFillRect(window.getRenderer(), &panel);
}

void VisualizationEngine::cleanUp(RenderWindow& window)
{
    window.cleanUpTexture(mapBackground_);
    window.cleanUpTexture(intersectionTexture_);
    window.cleanUpTexture(roadTexture_);
}
