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

    greenLightTexture_ = window.loadTexture("assets/textures/TrafficLight/Green_light.png");
    if (greenLightTexture_ == nullptr) {
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return false;
    }

    yellowLightTexture_ = window.loadTexture("assets/textures/TrafficLight/Yellow_light.png");
    if (yellowLightTexture_ == nullptr) {
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return false;
    }

    redLightTexture_ = window.loadTexture("assets/textures/TrafficLight/Red_light.png");
    if (redLightTexture_ == nullptr) {
        std::cerr << "\nHey.. recheck the IMG PATH" << SDL_GetError();
        return false;
    }

    carTexture_ = window.loadTexture("assets/textures/Vehicles/car.png");
    busTexture_ = window.loadTexture("assets/textures/Vehicles/bus.png");
    emergencyTexture_ = window.loadTexture("assets/textures/Vehicles/emergency.png");

    return true;
}


void VisualizationEngine::buildRenderCache(const Graph& graph)
{
    // Không đổi gì trong hàm này
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

        float vehicleLength = 100;
        float vehicleWidth  = 50;

        int screenLength = static_cast<int>(vehicleLength * zoom);
        int screenWidth  = static_cast<int>(vehicleWidth * zoom);

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

void VisualizationEngine::cleanUp(RenderWindow& window)
{
    window.cleanUpTexture(mapBackground_);
    window.cleanUpTexture(intersectionTexture_);
    window.cleanUpTexture(roadTexture_);

    window.cleanUpTexture(greenLightTexture_);
    window.cleanUpTexture(yellowLightTexture_);
    window.cleanUpTexture(redLightTexture_);

    window.cleanUpTexture(carTexture_);
    window.cleanUpTexture(busTexture_);
    window.cleanUpTexture(emergencyTexture_);
}