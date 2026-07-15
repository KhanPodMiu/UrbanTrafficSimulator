#include "visualization/VisualizationEngine.hpp"
#include "core/Constants.hpp"
#include "core/renderWindow.hpp"
#include "visualization/camera.hpp"
#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"

#include <iostream>
#include <cmath>

namespace {
    void trafficLightColor(TrafficLightState state, Uint8& r, Uint8& g, Uint8& b) {
        switch (state) {
            case TrafficLightState::GREEN:  r = 0;   g = 200; b = 0;   break;
            case TrafficLightState::YELLOW: r = 230; g = 200; b = 0;   break;
            case TrafficLightState::RED:    r = 200; g = 0;   b = 0;   break;
        }
    }
}

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

    intersectionTexture_ = window.loadTexture("assets/textures/Intersection/rb1.png");
    if (intersectionTexture_ == nullptr) {
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return false;
    }

    roadTexture_ = window.loadTexture("assets/textures/Roads/road2.png");
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

        temp.road = road;

        if (temp.length > 0.0f) {
            float dirX = dx / temp.length;
            float dirY = dy / temp.length;
            temp.lightPos = Vector2(
                temp.end.x - dirX * Config::ROUNDABOUT_RADIUS + temp.offsetX,
                temp.end.y - dirY * Config::ROUNDABOUT_RADIUS + temp.offsetY
            );
        } else {
            temp.lightPos = temp.end;
        }

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

    for (const auto& road : roadsLocation_) {
        if (road.road && road.road->isTrafficLightEnabled()) {
            Vector2 lightScreenPos = applyCamera(road.lightPos, camera);
            lightScreenPos.x += Config::PANEL_WIDTH;

            Uint8 r, g, b;
            trafficLightColor(road.road->getTrafficLightState(), r, g, b);

            int size = static_cast<int>(Config::TRAFFIC_LIGHT_MARKER_SIZE * zoom);
            window.renderTrafficLight((int)lightScreenPos.x, (int)lightScreenPos.y, size, r, g, b);
        }
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

