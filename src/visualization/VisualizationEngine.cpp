#include "visualization/VisualizationEngine.hpp"
#include "core/Constants.hpp"
#include "core/renderWindow.hpp"
#include "visualization/camera.hpp"
#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>
#include <limits>
#include <limits>

namespace {
    // Trả về đúng texture (Green/Yellow/Red) tương ứng với trạng thái đèn hiện tại.
    SDL_Texture* trafficLightTexture(TrafficLightState state, SDL_Texture* green, SDL_Texture* yellow, SDL_Texture* red)
    {
        switch (state) {
            case TrafficLightState::GREEN:  return green;
            case TrafficLightState::YELLOW: return yellow;
            case TrafficLightState::RED:    return red;
        }
        return red;
    }

    // Simple color-coding per vehicle type until dedicated sprites are added.
    SDL_Color colorForVehicleType(VehicleType type)
    {
        switch (type) {
            case VehicleType::BUS:       return SDL_Color{ 240, 180, 40, 255 };  // vàng cam - bus
            case VehicleType::EMERGENCY: return SDL_Color{ 235, 45, 45, 255 };   // đỏ - xe ưu tiên
            case VehicleType::CAR:
            default:                     return SDL_Color{ 60, 160, 250, 255 }; // xanh dương - xe con
        }
    }
}

namespace {

constexpr float VEHICLE_RENDER_LENGTH = 100.0f;
constexpr float VEHICLE_RENDER_WIDTH = 50.0f;
constexpr float MIN_VEHICLE_HIT_HALF_WIDTH = 8.0f;
constexpr float MIN_VEHICLE_HIT_HALF_LENGTH = 10.0f;
constexpr float MIN_INTERSECTION_HIT_RADIUS = 10.0f;

constexpr float VEHICLE_RENDER_LENGTH = 100.0f;
constexpr float VEHICLE_RENDER_WIDTH = 50.0f;
constexpr float MIN_VEHICLE_HIT_HALF_WIDTH = 8.0f;
constexpr float MIN_VEHICLE_HIT_HALF_LENGTH = 10.0f;
constexpr float MIN_INTERSECTION_HIT_RADIUS = 10.0f;

SDL_Texture* vehicleTexture(
    VehicleType type,
    SDL_Texture* car,
    SDL_Texture* bus,
    SDL_Texture* emergency)
{
    switch(type)
    {
        case VehicleType::BUS:
            return bus;

        case VehicleType::EMERGENCY:
            return emergency;

        case VehicleType::CAR:
        default:
            return car;
    }
}

SDL_Point rotatePoint(
    float localX,
    float localY,
    float centerX,
    float centerY,
    double angleDegrees)
{
    const double radians = angleDegrees * M_PI / 180.0;
    const double cosAngle = std::cos(radians);
    const double sinAngle = std::sin(radians);

    return SDL_Point{
        static_cast<int>(std::lround(
            centerX + localX * cosAngle - localY * sinAngle)),
        static_cast<int>(std::lround(
            centerY + localX * sinAngle + localY * cosAngle))
    };
}

void drawCircle(
    SDL_Renderer* renderer,
    int centerX,
    int centerY,
    int radius)
{
    constexpr int SEGMENTS = 64;
    SDL_Point points[SEGMENTS + 1];

    for (int i = 0; i <= SEGMENTS; ++i)
    {
        const double angle =
            2.0 * M_PI * static_cast<double>(i) /
            static_cast<double>(SEGMENTS);
        points[i] = SDL_Point{
            centerX + static_cast<int>(
                std::lround(radius * std::cos(angle))),
            centerY + static_cast<int>(
                std::lround(radius * std::sin(angle)))
        };
    }

    SDL_RenderDrawLines(renderer, points, SEGMENTS + 1);
}

SDL_Point rotatePoint(
    float localX,
    float localY,
    float centerX,
    float centerY,
    double angleDegrees)
{
    const double radians = angleDegrees * M_PI / 180.0;
    const double cosAngle = std::cos(radians);
    const double sinAngle = std::sin(radians);

    return SDL_Point{
        static_cast<int>(std::lround(
            centerX + localX * cosAngle - localY * sinAngle)),
        static_cast<int>(std::lround(
            centerY + localX * sinAngle + localY * cosAngle))
    };
}

void drawCircle(
    SDL_Renderer* renderer,
    int centerX,
    int centerY,
    int radius)
{
    constexpr int SEGMENTS = 64;
    SDL_Point points[SEGMENTS + 1];

    for (int i = 0; i <= SEGMENTS; ++i)
    {
        const double angle =
            2.0 * M_PI * static_cast<double>(i) /
            static_cast<double>(SEGMENTS);
        points[i] = SDL_Point{
            centerX + static_cast<int>(
                std::lround(radius * std::cos(angle))),
            centerY + static_cast<int>(
                std::lround(radius * std::sin(angle)))
        };
    }

    SDL_RenderDrawLines(renderer, points, SEGMENTS + 1);
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

    // Load ảnh Đèn giao thông
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

    return true;
}


void VisualizationEngine::buildRenderCache(const Graph& graph)
{
    // Không đổi gì trong hàm này
    intersectionsLocation_.clear();
    roadsLocation_.clear();

    for (const auto& [intersectionID, intersection] : graph.getIntersections()) {
        IntersectionRenderData temp;
        temp.center = Vector2(intersection->getX(), intersection->getY());
        temp.intersection = intersection;
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

void VisualizationEngine::render(RenderWindow& window, const Camera& camera)
{
    float zoom = camera.getZoom();

    Vector2 worldOrigin(0, 0);
    Vector2 bgPos = applyCamera(worldOrigin, camera);
    bgPos.x += Config::PANEL_WIDTH;
    //window.render(mapBackground_, bgPos, zoom);

    // Background
    SDL_Rect backgroundDestination = {
    static_cast<int>(std::lround(bgPos.x)),
    static_cast<int>(std::lround(bgPos.y)),
    static_cast<int>(std::lround(Config::MAP_WIDTH * zoom)),
    static_cast<int>(std::lround(Config::MAP_HEIGHT * zoom))
    };

    SDL_RenderCopy(
    window.getRenderer(),
    mapBackground_,
    nullptr,
    &backgroundDestination
    );

    for (const auto& road : roadsLocation_) {
        Vector2 shiftedStart(road.start.x + road.offsetX, road.start.y + road.offsetY);

        Vector2 renderPos = applyCamera(shiftedStart, camera);
        renderPos.x += Config::PANEL_WIDTH;

        SDL_Texture* currentRoadTex = (road.road && road.road->isVIPExclusive()) 
                                      ? bannedRoadTexture_ 
                                      : roadTexture_;

        window.renderRoad(currentRoadTex, renderPos, road.length * zoom, Config::ROAD_WIDTH * zoom, road.angle);
    }

    // Intersection
    for (const auto& intersection : intersectionsLocation_) {
        Vector2 topLeft(
            intersection.center.x - Config::ROUNDABOUT_RADIUS,
            intersection.center.y - Config::ROUNDABOUT_RADIUS);
        Vector2 renderPos = applyCamera(topLeft, camera);
        Vector2 topLeft(
            intersection.center.x - Config::ROUNDABOUT_RADIUS,
            intersection.center.y - Config::ROUNDABOUT_RADIUS);
        Vector2 renderPos = applyCamera(topLeft, camera);
        renderPos.x += Config::PANEL_WIDTH;
        window.render(intersectionTexture_, renderPos, zoom, Config::ROUNDABOUT_RADIUS * zoom);
    }

    for (const auto& road : roadsLocation_) {
        if (road.road && road.road->isTrafficLightEnabled()) {
            Vector2 lightScreenPos = applyCamera(road.lightPos, camera);
            lightScreenPos.x += Config::PANEL_WIDTH;

            SDL_Texture* lightTex = trafficLightTexture(
                road.road->getTrafficLightState(),
                greenLightTexture_,
                yellowLightTexture_,
                redLightTexture_
            );

            int size = static_cast<int>(Config::TRAFFIC_LIGHT_MARKER_SIZE * zoom);
            window.renderTrafficLight(lightTex, (int)lightScreenPos.x, (int)lightScreenPos.y, size);
        }
    }

}

void VisualizationEngine::renderVehicles(
    RenderWindow& window,
    const Camera& camera,
    const std::vector<std::shared_ptr<Vehicle>>& vehicles)
{
    SDL_Renderer* renderer = window.getRenderer();
    float zoom = camera.getZoom();

    for (const auto& vehicle : vehicles)
    {
        if (!vehicle || vehicle->isFinished())
            continue;

        Vector2 worldPos = vehicle->getPosition();
        Vector2 screenPos = applyCamera(worldPos, camera);
        screenPos.x += Config::PANEL_WIDTH;

        SDL_Texture* texture = vehicleTexture(
            vehicle->getType(),
            carTexture_,
            busTexture_,
            emergencyTexture_);

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

std::shared_ptr<Vehicle> VisualizationEngine::pickVehicle(
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

        Vector2 screenPosition = applyCamera(vehicle->getPosition(), camera);
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

std::shared_ptr<Intersection> VisualizationEngine::pickIntersection(
    const Camera& camera,
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

    for (const auto& renderData : intersectionsLocation_)
    {
        if (!renderData.intersection)
            continue;

        Vector2 screenPosition = applyCamera(renderData.center, camera);
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

void VisualizationEngine::renderSelectionHighlight(
    RenderWindow& window,
    const Camera& camera,
    const std::shared_ptr<Vehicle>& selectedVehicle,
    const std::shared_ptr<Intersection>& selectedIntersection) const
{
    SDL_Renderer* renderer = window.getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 214, 48, 255);

    if (selectedVehicle && !selectedVehicle->isFinished())
    {
        Vector2 center = applyCamera(selectedVehicle->getPosition(), camera);
        center.x += Config::PANEL_WIDTH;

        const float halfWidth = std::max(
            5.0f,
            VEHICLE_RENDER_WIDTH * camera.getZoom() * 0.5f + 3.0f);
        const float halfLength = std::max(
            7.0f,
            VEHICLE_RENDER_LENGTH * camera.getZoom() * 0.5f + 3.0f);
        const double angle = selectedVehicle->getHeadingAngle() - 90.0;

        SDL_Point outline[5] = {
            rotatePoint(-halfWidth, -halfLength, center.x, center.y, angle),
            rotatePoint(halfWidth, -halfLength, center.x, center.y, angle),
            rotatePoint(halfWidth, halfLength, center.x, center.y, angle),
            rotatePoint(-halfWidth, halfLength, center.x, center.y, angle),
            rotatePoint(-halfWidth, -halfLength, center.x, center.y, angle)
        };
        SDL_RenderDrawLines(renderer, outline, 5);
        return;
    }

    if (selectedIntersection)
    {
        Vector2 center(
            selectedIntersection->getX(),
            selectedIntersection->getY());
        center = applyCamera(center, camera);
        center.x += Config::PANEL_WIDTH;

        const int radius = std::max(
            12,
            static_cast<int>(std::lround(
                Config::ROUNDABOUT_RADIUS * camera.getZoom() + 5.0f)));
        drawCircle(
            renderer,
            static_cast<int>(std::lround(center.x)),
            static_cast<int>(std::lround(center.y)),
            radius);
    }
}

std::shared_ptr<Vehicle> VisualizationEngine::pickVehicle(
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

        Vector2 screenPosition = applyCamera(vehicle->getPosition(), camera);
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

std::shared_ptr<Intersection> VisualizationEngine::pickIntersection(
    const Camera& camera,
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

    for (const auto& renderData : intersectionsLocation_)
    {
        if (!renderData.intersection)
            continue;

        Vector2 screenPosition = applyCamera(renderData.center, camera);
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

void VisualizationEngine::renderSelectionHighlight(
    RenderWindow& window,
    const Camera& camera,
    const std::shared_ptr<Vehicle>& selectedVehicle,
    const std::shared_ptr<Intersection>& selectedIntersection) const
{
    SDL_Renderer* renderer = window.getRenderer();
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 255, 214, 48, 255);

    if (selectedVehicle && !selectedVehicle->isFinished())
    {
        Vector2 center = applyCamera(selectedVehicle->getPosition(), camera);
        center.x += Config::PANEL_WIDTH;

        const float halfWidth = std::max(
            5.0f,
            VEHICLE_RENDER_WIDTH * camera.getZoom() * 0.5f + 3.0f);
        const float halfLength = std::max(
            7.0f,
            VEHICLE_RENDER_LENGTH * camera.getZoom() * 0.5f + 3.0f);
        const double angle = selectedVehicle->getHeadingAngle() - 90.0;

        SDL_Point outline[5] = {
            rotatePoint(-halfWidth, -halfLength, center.x, center.y, angle),
            rotatePoint(halfWidth, -halfLength, center.x, center.y, angle),
            rotatePoint(halfWidth, halfLength, center.x, center.y, angle),
            rotatePoint(-halfWidth, halfLength, center.x, center.y, angle),
            rotatePoint(-halfWidth, -halfLength, center.x, center.y, angle)
        };
        SDL_RenderDrawLines(renderer, outline, 5);
        return;
    }

    if (selectedIntersection)
    {
        Vector2 center(
            selectedIntersection->getX(),
            selectedIntersection->getY());
        center = applyCamera(center, camera);
        center.x += Config::PANEL_WIDTH;

        const int radius = std::max(
            12,
            static_cast<int>(std::lround(
                Config::ROUNDABOUT_RADIUS * camera.getZoom() + 5.0f)));
        drawCircle(
            renderer,
            static_cast<int>(std::lround(center.x)),
            static_cast<int>(std::lround(center.y)),
            radius);
    }
}

void VisualizationEngine::cleanUp(RenderWindow& window)
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
}
