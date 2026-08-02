#include "render/MapRenderer.hpp"
#include "render/TextureManager.hpp"
#include "render/RenderHelpers.hpp"
#include "core/Constants.hpp"
#include "core/renderWindow.hpp"
#include "visualization/camera.hpp"
#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"
#include <cmath>

void MapRenderer::buildRenderCache(const Graph& graph)
{
    intersectionsLocation_.clear();
    roadsLocation_.clear();

    for (const auto& [intersectionID, intersection] : graph.getIntersections()) {
        IntersectionRenderData temp;
        temp.center = Vector2(intersection->getX(), intersection->getY());
        temp.intersection = intersection;
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

void MapRenderer::render(
    RenderWindow& window,
    const Camera& camera,
    const TextureManager& textureManager)
{
    float zoom = camera.getZoom();

    Vector2 worldOrigin(0, 0);
    Vector2 bgPos = RenderHelpers::applyCamera(worldOrigin, camera);
    bgPos.x += Config::PANEL_WIDTH;

    // Background
    SDL_Rect backgroundDestination = {
        static_cast<int>(std::lround(bgPos.x)),
        static_cast<int>(std::lround(bgPos.y)),
        static_cast<int>(std::lround(Config::MAP_WIDTH * zoom)),
        static_cast<int>(std::lround(Config::MAP_HEIGHT * zoom))
    };

    SDL_RenderCopy(
        window.getRenderer(),
        textureManager.getMapBackground(),
        nullptr,
        &backgroundDestination
    );

    // Roads
    for (const auto& road : roadsLocation_) {
        Vector2 shiftedStart(road.start.x + road.offsetX, road.start.y + road.offsetY);

        Vector2 renderPos = RenderHelpers::applyCamera(shiftedStart, camera);
        renderPos.x += Config::PANEL_WIDTH;

        SDL_Texture* currentRoadTex = (road.road && road.road->isVIPExclusive()) 
                                       ? textureManager.getBannedRoadTexture() 
                                       : textureManager.getRoadTexture();

        window.renderRoad(currentRoadTex, renderPos, road.length * zoom, Config::ROAD_WIDTH * zoom, road.angle);
    }

    // Intersections
    for (const auto& intersection : intersectionsLocation_) {
        Vector2 topLeft(
            intersection.center.x - Config::ROUNDABOUT_RADIUS,
            intersection.center.y - Config::ROUNDABOUT_RADIUS);
        Vector2 renderPos = RenderHelpers::applyCamera(topLeft, camera);
        renderPos.x += Config::PANEL_WIDTH;
        window.render(textureManager.getIntersectionTexture(), renderPos, zoom, Config::ROUNDABOUT_RADIUS * zoom);
    }

    // Traffic Lights
    for (const auto& road : roadsLocation_) {
        if (road.road && road.road->isTrafficLightEnabled()) {
            Vector2 lightScreenPos = RenderHelpers::applyCamera(road.lightPos, camera);
            lightScreenPos.x += Config::PANEL_WIDTH;

            SDL_Texture* lightTex = textureManager.getTrafficLightTexture(
                road.road->getTrafficLightState()
            );

            int size = static_cast<int>(Config::TRAFFIC_LIGHT_MARKER_SIZE * zoom);
            window.renderTrafficLight(lightTex, (int)lightScreenPos.x, (int)lightScreenPos.y, size);
        }
    }
}
