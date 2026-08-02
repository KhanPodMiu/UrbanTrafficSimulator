#include "render/EntityPicker.hpp"
#include "render/VehicleRenderer.hpp"
#include "render/RenderHelpers.hpp"
#include "core/Constants.hpp"
#include "core/renderWindow.hpp"
#include "visualization/camera.hpp"
#include <limits>
#include <cmath>
#include <algorithm>

namespace {
constexpr float VEHICLE_RENDER_LENGTH = 100.0f;
constexpr float VEHICLE_RENDER_WIDTH = 50.0f;
constexpr float MIN_VEHICLE_HIT_HALF_WIDTH = 8.0f;
constexpr float MIN_VEHICLE_HIT_HALF_LENGTH = 10.0f;
constexpr float MIN_INTERSECTION_HIT_RADIUS = 10.0f;
}

std::shared_ptr<Vehicle> EntityPicker::pickVehicle(
    const Camera& camera,
    const std::vector<std::shared_ptr<Vehicle>>& vehicles,
    int mouseX,
    int mouseY) const
{
    std::shared_ptr<Vehicle> closestVehicle;
    double closestDistanceSquared = std::numeric_limits<double>::max();
    const float zoom = camera.getZoom();
    const float halfWidth = std::max(
        MIN_VEHICLE_HIT_HALF_WIDTH,
        VEHICLE_RENDER_WIDTH * zoom * 0.5f);
    const float halfLength = std::max(
        MIN_VEHICLE_HIT_HALF_LENGTH,
        VEHICLE_RENDER_LENGTH * zoom * 0.5f);

    for (const auto& vehicle : vehicles)
    {
        if (!vehicle || vehicle->isFinished())
            continue;

        Vector2 screenPosition = RenderHelpers::applyCamera(vehicle->getPosition(), camera);
        screenPosition.x += Config::PANEL_WIDTH;

        const double dx = static_cast<double>(mouseX) - screenPosition.x;
        const double dy = static_cast<double>(mouseY) - screenPosition.y;
        const double angle =
            (vehicle->getHeadingAngle() - 90.0) * M_PI / 180.0;
        const double cosAngle = std::cos(angle);
        const double sinAngle = std::sin(angle);
        const double localX = dx * cosAngle + dy * sinAngle;
        const double localY = -dx * sinAngle + dy * cosAngle;

        if (std::abs(localX) > halfWidth ||
            std::abs(localY) > halfLength)
        {
            continue;
        }

        const double distanceSquared = dx * dx + dy * dy;
        if (distanceSquared < closestDistanceSquared)
        {
            closestDistanceSquared = distanceSquared;
            closestVehicle = vehicle;
        }
    }

    return closestVehicle;
}

std::shared_ptr<Intersection> EntityPicker::pickIntersection(
    const Camera& camera,
    const std::vector<MapRenderer::IntersectionRenderData>& intersections,
    int mouseX,
    int mouseY) const
{
    std::shared_ptr<Intersection> closestIntersection;
    double closestDistanceSquared = std::numeric_limits<double>::max();
    const float hitRadius = std::max(
        MIN_INTERSECTION_HIT_RADIUS,
        Config::ROUNDABOUT_RADIUS * camera.getZoom());
    const double hitRadiusSquared =
        static_cast<double>(hitRadius) * hitRadius;

    for (const auto& renderData : intersections)
    {
        if (!renderData.intersection)
            continue;

        Vector2 screenPosition = RenderHelpers::applyCamera(renderData.center, camera);
        screenPosition.x += Config::PANEL_WIDTH;

        const double dx = static_cast<double>(mouseX) - screenPosition.x;
        const double dy = static_cast<double>(mouseY) - screenPosition.y;
        const double distanceSquared = dx * dx + dy * dy;

        if (distanceSquared <= hitRadiusSquared &&
            distanceSquared < closestDistanceSquared)
        {
            closestDistanceSquared = distanceSquared;
            closestIntersection = renderData.intersection;
        }
    }

    return closestIntersection;
}

void EntityPicker::renderSelectionHighlight(
    RenderWindow& window,
    const Camera& camera,
    const std::shared_ptr<Vehicle>& selectedVehicle,
    const std::shared_ptr<Intersection>& selectedIntersection,
    const VehicleRenderer& vehicleRenderer) const
{
    SDL_Renderer* renderer = window.getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    if (selectedVehicle && !selectedVehicle->isFinished())
    {
        vehicleRenderer.renderVehicleRoute(window, camera, *selectedVehicle);

        SDL_SetRenderDrawColor(renderer, 255, 214, 48, 255);
        Vector2 center = RenderHelpers::applyCamera(selectedVehicle->getPosition(), camera);
        center.x += Config::PANEL_WIDTH;

        const float halfWidth = std::max(
            5.0f,
            VEHICLE_RENDER_WIDTH * camera.getZoom() * 0.5f + 3.0f);
        const float halfLength = std::max(
            7.0f,
            VEHICLE_RENDER_LENGTH * camera.getZoom() * 0.5f + 3.0f);
        const double angle = selectedVehicle->getHeadingAngle() - 90.0;

        SDL_Point outline[5] = {
            RenderHelpers::rotatePoint(-halfWidth, -halfLength, center.x, center.y, angle),
            RenderHelpers::rotatePoint(halfWidth, -halfLength, center.x, center.y, angle),
            RenderHelpers::rotatePoint(halfWidth, halfLength, center.x, center.y, angle),
            RenderHelpers::rotatePoint(-halfWidth, halfLength, center.x, center.y, angle),
            RenderHelpers::rotatePoint(-halfWidth, -halfLength, center.x, center.y, angle)
        };
        SDL_RenderDrawLines(renderer, outline, 5);
        return;
    }

    if (selectedIntersection)
    {
        Vector2 center(
            selectedIntersection->getX(),
            selectedIntersection->getY());
        center = RenderHelpers::applyCamera(center, camera);
        center.x += Config::PANEL_WIDTH;

        const int radius = std::max(
            12,
            static_cast<int>(std::lround(
                Config::ROUNDABOUT_RADIUS * camera.getZoom() + 5.0f)));
        RenderHelpers::drawCircle(
            renderer,
            static_cast<int>(std::lround(center.x)),
            static_cast<int>(std::lround(center.y)),
            radius);
    }
}
