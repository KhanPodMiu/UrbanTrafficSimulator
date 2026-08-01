#include "render/VehicleRenderer.hpp"
#include "render/TextureManager.hpp"
#include "render/RenderHelpers.hpp"
#include "core/Constants.hpp"
#include "core/renderWindow.hpp"
#include "visualization/camera.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"
#include <cmath>

namespace {
constexpr float VEHICLE_RENDER_LENGTH = 100.0f;
constexpr float VEHICLE_RENDER_WIDTH = 50.0f;
}

void VehicleRenderer::renderVehicles(
    RenderWindow& window,
    const Camera& camera,
    const std::vector<std::shared_ptr<Vehicle>>& vehicles,
    const TextureManager& textureManager)
{
    SDL_Renderer* renderer = window.getRenderer();
    float zoom = camera.getZoom();

    for (const auto& vehicle : vehicles)
    {
        if (!vehicle || vehicle->isFinished())
            continue;

        Vector2 worldPos = vehicle->getPosition();
        Vector2 screenPos = RenderHelpers::applyCamera(worldPos, camera);
        screenPos.x += Config::PANEL_WIDTH;

        SDL_Texture* texture = textureManager.getVehicleTexture(vehicle->getType());

        int screenLength = static_cast<int>(VEHICLE_RENDER_LENGTH * zoom);
        int screenWidth  = static_cast<int>(VEHICLE_RENDER_WIDTH * zoom);

        SDL_Rect dst;
        dst.w = screenWidth;
        dst.h = screenLength;
        dst.x = static_cast<int>(screenPos.x - screenWidth / 2);
        dst.y = static_cast<int>(screenPos.y - screenLength / 2);

        double angle = vehicle->getHeadingAngle() - 90.0;

        SDL_RenderCopyEx(
            renderer,
            texture,
            nullptr,
            &dst,
            angle,
            nullptr,
            SDL_FLIP_NONE);
    }
}

void VehicleRenderer::renderVehicleRoute(
    RenderWindow& window,
    const Camera& camera,
    const Vehicle& vehicle) const
{
    const auto& route = vehicle.getRoute();
    if (route.empty())
        return;

    SDL_Renderer* renderer = window.getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    const size_t routeIndex = vehicle.getRouteIndex();
    const float zoom = camera.getZoom();

    auto computeRoadLaneOffset = [](const Intersection* src, const Intersection* dst) -> Vector2 {
        if (!src || !dst) return Vector2(0.0f, 0.0f);
        float dx = static_cast<float>(dst->getX() - src->getX());
        float dy = static_cast<float>(dst->getY() - src->getY());
        float len = std::sqrt(dx * dx + dy * dy);
        if (len < 0.001f) return Vector2(0.0f, 0.0f);
        float halfWidth = Config::ROAD_WIDTH * 0.5f;
        return Vector2(-dy / len * halfWidth, dx / len * halfWidth);
    };

    auto toScreen = [&camera](const Vector2& worldPos) -> Vector2 {
        Vector2 screen = RenderHelpers::applyCamera(worldPos, camera);
        screen.x += Config::PANEL_WIDTH;
        return screen;
    };

    // 1. Draw Past / Traveled Roads (roads before routeIndex)
    for (size_t i = 0; i < routeIndex && i < route.size(); ++i)
    {
        const auto& road = route[i];
        if (!road) continue;
        const Intersection* src = road->getSourceIntersection();
        const Intersection* dst = road->getDestinationIntersection();
        if (!src || !dst) continue;

        Vector2 laneOffset = computeRoadLaneOffset(src, dst);
        Vector2 startW(src->getX() + laneOffset.x, src->getY() + laneOffset.y);
        Vector2 endW(dst->getX() + laneOffset.x, dst->getY() + laneOffset.y);

        Vector2 startS = toScreen(startW);
        Vector2 endS = toScreen(endW);

        SDL_SetRenderDrawColor(renderer, 100, 140, 180, 110);
        RenderHelpers::drawThickLine(renderer, startS.x, startS.y, endS.x, endS.y, std::max(2.0f, 4.0f * zoom));
    }

    // 2. Draw Remaining Route (from current vehicle position onwards)
    Vector2 lastScreenPos{0.0f, 0.0f};
    bool hasLastPos = false;

    if (routeIndex < route.size() && route[routeIndex])
    {
        Vector2 vehiclePos = vehicle.getPosition();
        Vector2 vehicleS = toScreen(vehiclePos);
        lastScreenPos = vehicleS;
        hasLastPos = true;

        const auto& currentRoad = route[routeIndex];
        const Intersection* currentSrc = currentRoad->getSourceIntersection();
        const Intersection* currentDst = currentRoad->getDestinationIntersection();

        if (currentDst)
        {
            Vector2 laneOffset = computeRoadLaneOffset(currentSrc, currentDst);
            Vector2 roadEndW(currentDst->getX() + laneOffset.x, currentDst->getY() + laneOffset.y);
            Vector2 roadEndS = toScreen(roadEndW);

            // Glowing cyan path from car to current road end
            SDL_SetRenderDrawColor(renderer, 0, 229, 255, 110);
            RenderHelpers::drawThickLine(renderer, vehicleS.x, vehicleS.y, roadEndS.x, roadEndS.y, std::max(4.0f, 9.0f * zoom));
            SDL_SetRenderDrawColor(renderer, 0, 229, 255, 255);
            RenderHelpers::drawThickLine(renderer, vehicleS.x, vehicleS.y, roadEndS.x, roadEndS.y, std::max(2.0f, 4.0f * zoom));

            // Directional arrow along current segment
            float dx = roadEndS.x - vehicleS.x;
            float dy = roadEndS.y - vehicleS.y;
            if (std::sqrt(dx * dx + dy * dy) > 20.0f * zoom)
            {
                float midX = (vehicleS.x + roadEndS.x) * 0.5f;
                float midY = (vehicleS.y + roadEndS.y) * 0.5f;
                SDL_SetRenderDrawColor(renderer, 0, 229, 255, 255);
                RenderHelpers::drawArrowHead(renderer, midX, midY, dx, dy, std::max(8.0f, 14.0f * zoom));
            }

            lastScreenPos = roadEndS;
        }
    }

    // Draw upcoming road segments in remaining route
    for (size_t i = routeIndex + 1; i < route.size(); ++i)
    {
        const auto& road = route[i];
        if (!road) continue;
        const Intersection* src = road->getSourceIntersection();
        const Intersection* dst = road->getDestinationIntersection();
        if (!src || !dst) continue;

        Vector2 laneOffset = computeRoadLaneOffset(src, dst);
        Vector2 startW(src->getX() + laneOffset.x, src->getY() + laneOffset.y);
        Vector2 endW(dst->getX() + laneOffset.x, dst->getY() + laneOffset.y);

        Vector2 startS = toScreen(startW);
        Vector2 endS = toScreen(endW);

        // Junction transition
        if (hasLastPos)
        {
            SDL_SetRenderDrawColor(renderer, 0, 229, 255, 110);
            RenderHelpers::drawThickLine(renderer, lastScreenPos.x, lastScreenPos.y, startS.x, startS.y, std::max(4.0f, 9.0f * zoom));
            SDL_SetRenderDrawColor(renderer, 0, 229, 255, 255);
            RenderHelpers::drawThickLine(renderer, lastScreenPos.x, lastScreenPos.y, startS.x, startS.y, std::max(2.0f, 4.0f * zoom));
        }

        // Road segment
        SDL_SetRenderDrawColor(renderer, 0, 229, 255, 110);
        RenderHelpers::drawThickLine(renderer, startS.x, startS.y, endS.x, endS.y, std::max(4.0f, 9.0f * zoom));
        SDL_SetRenderDrawColor(renderer, 0, 229, 255, 255);
        RenderHelpers::drawThickLine(renderer, startS.x, startS.y, endS.x, endS.y, std::max(2.0f, 4.0f * zoom));

        // Waypoint dot at road start
        int wayRadius = std::max(3, static_cast<int>(std::lround(5.0f * zoom)));
        SDL_SetRenderDrawColor(renderer, 0, 229, 255, 255);
        RenderHelpers::fillCircle(renderer, static_cast<int>(std::lround(startS.x)), static_cast<int>(std::lround(startS.y)), wayRadius);

        // Directional arrow at segment midpoint
        float dx = endS.x - startS.x;
        float dy = endS.y - startS.y;
        if (std::sqrt(dx * dx + dy * dy) > 20.0f * zoom)
        {
            float midX = (startS.x + endS.x) * 0.5f;
            float midY = (startS.y + endS.y) * 0.5f;
            SDL_SetRenderDrawColor(renderer, 0, 229, 255, 255);
            RenderHelpers::drawArrowHead(renderer, midX, midY, dx, dy, std::max(8.0f, 14.0f * zoom));
        }

        lastScreenPos = endS;
        hasLastPos = true;
    }

    // 3. Start Intersection Marker (Green Circle)
    if (!route.empty() && route[0])
    {
        const Intersection* startInt = route[0]->getSourceIntersection();
        if (startInt)
        {
            Vector2 startW(static_cast<float>(startInt->getX()), static_cast<float>(startInt->getY()));
            Vector2 startS = toScreen(startW);

            int outerR = std::max(8, static_cast<int>(std::lround(12.0f * zoom)));
            int innerR = std::max(4, static_cast<int>(std::lround(6.0f * zoom)));

            SDL_SetRenderDrawColor(renderer, 46, 204, 113, 200);
            RenderHelpers::drawCircle(renderer, static_cast<int>(std::lround(startS.x)), static_cast<int>(std::lround(startS.y)), outerR);
            RenderHelpers::drawCircle(renderer, static_cast<int>(std::lround(startS.x)), static_cast<int>(std::lround(startS.y)), outerR - 1);
            SDL_SetRenderDrawColor(renderer, 46, 204, 113, 255);
            RenderHelpers::fillCircle(renderer, static_cast<int>(std::lround(startS.x)), static_cast<int>(std::lround(startS.y)), innerR);
        }
    }

    // 4. Destination Intersection Marker (Red Target Crosshair Circle)
    const Intersection* destInt = vehicle.getDestination() ? vehicle.getDestination().get() : nullptr;
    if (!destInt && !route.empty() && route.back())
    {
        destInt = route.back()->getDestinationIntersection();
    }

    if (destInt)
    {
        Vector2 destW(static_cast<float>(destInt->getX()), static_cast<float>(destInt->getY()));
        Vector2 destS = toScreen(destW);

        int outerR = std::max(12, static_cast<int>(std::lround(16.0f * zoom)));
        int midR   = std::max(8,  static_cast<int>(std::lround(10.0f * zoom)));
        int innerR = std::max(4,  static_cast<int>(std::lround(5.0f * zoom)));

        int cx = static_cast<int>(std::lround(destS.x));
        int cy = static_cast<int>(std::lround(destS.y));

        SDL_SetRenderDrawColor(renderer, 255, 23, 68, 120);
        RenderHelpers::fillCircle(renderer, cx, cy, outerR);

        SDL_SetRenderDrawColor(renderer, 255, 23, 68, 255);
        RenderHelpers::drawCircle(renderer, cx, cy, outerR);
        RenderHelpers::drawCircle(renderer, cx, cy, midR);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        RenderHelpers::fillCircle(renderer, cx, cy, innerR);

        int tickLen = std::max(4, static_cast<int>(std::lround(6.0f * zoom)));
        SDL_SetRenderDrawColor(renderer, 255, 23, 68, 255);
        SDL_RenderDrawLine(renderer, cx - outerR - tickLen, cy, cx - outerR, cy);
        SDL_RenderDrawLine(renderer, cx + outerR, cy, cx + outerR + tickLen, cy);
        SDL_RenderDrawLine(renderer, cx, cy - outerR - tickLen, cx, cy - outerR);
        SDL_RenderDrawLine(renderer, cx, cy + outerR, cx, cy + outerR + tickLen);
    }
}
