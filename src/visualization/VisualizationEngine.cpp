#include "visualization/VisualizationEngine.hpp"
#include "core/Constants.hpp"
#include "core/renderWindow.hpp"
#include "visualization/camera.hpp"
#include "graph/Graph.hpp"
#include "graph/Road.hpp"
#include "graph/Intersection.hpp"
#include "render/TrafficHeatMap.hpp"

#include <iostream>
#include <cmath>
#include <algorithm>
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
constexpr Uint8 HEAT_MAP_ROAD_ALPHA = 178;

constexpr SDL_Rect HEAT_MAP_BUTTON_RECT{
    Config::PANEL_WIDTH + 18,
    18,
    154,
    42};

constexpr SDL_Rect HEAT_MAP_LEGEND_RECT{
    Config::PANEL_WIDTH + 18,
    68,
    218,
    124};

SDL_Texture* createTextTexture(
    SDL_Renderer* renderer,
    TTF_Font* font,
    const char* text,
    SDL_Color color)
{
    SDL_Surface* surface = TTF_RenderText_Blended(font, text, color);
    if (surface == nullptr)
        return nullptr;

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    return texture;
}

SDL_Texture* createWhitePixelTexture(SDL_Renderer* renderer)
{
    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(
        0,
        1,
        1,
        32,
        SDL_PIXELFORMAT_RGBA32);
    if (surface == nullptr)
        return nullptr;

    SDL_FillRect(
        surface,
        nullptr,
        SDL_MapRGBA(surface->format, 255, 255, 255, 255));
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);

    if (texture != nullptr)
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    return texture;
}

void renderTextureAt(
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    int x,
    int y)
{
    if (texture == nullptr)
        return;

    SDL_Rect destination{x, y, 0, 0};
    SDL_QueryTexture(
        texture,
        nullptr,
        nullptr,
        &destination.w,
        &destination.h);
    SDL_RenderCopy(renderer, texture, nullptr, &destination);
}

void fillCircle(
    SDL_Renderer* renderer,
    int centerX,
    int centerY,
    int radius)
{
    for (int y = -radius; y <= radius; ++y)
    {
        const int halfWidth = static_cast<int>(std::sqrt(
            static_cast<double>(radius * radius - y * y)));
        SDL_RenderDrawLine(
            renderer,
            centerX - halfWidth,
            centerY + y,
            centerX + halfWidth,
            centerY + y);
    }
}

void renderFloatingPanel(
    SDL_Renderer* renderer,
    const SDL_Rect& rectangle,
    SDL_Color background,
    SDL_Color border)
{
    const SDL_Rect shadow{
        rectangle.x + 3,
        rectangle.y + 4,
        rectangle.w,
        rectangle.h};

    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 3, 12, 18, 105);
    SDL_RenderFillRect(renderer, &shadow);
    SDL_SetRenderDrawColor(
        renderer,
        background.r,
        background.g,
        background.b,
        background.a);
    SDL_RenderFillRect(renderer, &rectangle);
    SDL_SetRenderDrawColor(
        renderer,
        border.r,
        border.g,
        border.b,
        border.a);
    SDL_RenderDrawRect(renderer, &rectangle);
}

void renderSolidRoadOverlay(
    SDL_Renderer* renderer,
    SDL_Texture* texture,
    const Vector2& start,
    float length,
    float width,
    double angle)
{
    if (length <= 0.0f || width <= 0.0f)
        return;

    SDL_Rect destination{
        static_cast<int>(std::lround(start.x)),
        static_cast<int>(std::lround(start.y - width * 0.5f)),
        std::max(1, static_cast<int>(std::ceil(length))),
        std::max(1, static_cast<int>(std::ceil(width)))};
    const SDL_Point pivot{0, destination.h / 2};
    SDL_RenderCopyEx(
        renderer,
        texture,
        nullptr,
        &destination,
        angle,
        &pivot,
        SDL_FLIP_NONE);
}

// Retrieves the correct texture for a given vehicle type
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

// Rotates a point around a center by a given angle in degrees
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

// Draws a hollow circle using SDL renderer lines
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

    SDL_Renderer* renderer = window.getRenderer();
    heatMapOverlayTexture_ = createWhitePixelTexture(renderer);
    if (heatMapOverlayTexture_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot create traffic heat-map overlay: "
                  << SDL_GetError();
        return false;
    }

    heatMapFont_ = TTF_OpenFont(
        "assets/textures/fonts/Octarine-Bold.otf",
        14);
    if (heatMapFont_ == nullptr) {
        std::cerr << "\n[ERROR] Cannot load traffic heat-map font: "
                  << TTF_GetError();
        return false;
    }

    const SDL_Color primaryText{238, 246, 248, 255};
    const SDL_Color secondaryText{190, 207, 212, 255};
    heatMapButtonLabelTexture_ = createTextTexture(
        renderer, heatMapFont_, "HEAT MAP", primaryText);
    heatMapLegendTitleTexture_ = createTextTexture(
        renderer, heatMapFont_, "TRAFFIC LEVEL", primaryText);
    heatMapLowLabelTexture_ = createTextTexture(
        renderer, heatMapFont_, "LOW       0 - 39%", secondaryText);
    heatMapModerateLabelTexture_ = createTextTexture(
        renderer, heatMapFont_, "MEDIUM  40 - 69%", secondaryText);
    heatMapHeavyLabelTexture_ = createTextTexture(
        renderer, heatMapFont_, "HIGH     70 - 100%", secondaryText);

    if (heatMapButtonLabelTexture_ == nullptr ||
        heatMapLegendTitleTexture_ == nullptr ||
        heatMapLowLabelTexture_ == nullptr ||
        heatMapModerateLabelTexture_ == nullptr ||
        heatMapHeavyLabelTexture_ == nullptr)
    {
        std::cerr << "\n[ERROR] Cannot create traffic heat-map labels: "
                  << TTF_GetError();
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

        const bool closurePending =
            road.road && road.road->isVIPClosurePending();
        if (closurePending)
        {
            // Amber marks the advance-warning/clearance phase. The road is
            // already excluded from routing, but cars currently inside may
            // still leave before the striped active closure appears.
            SDL_SetTextureColorMod(currentRoadTex, 255, 185, 70);
        }

        window.renderRoad(currentRoadTex, renderPos, road.length * zoom, Config::ROAD_WIDTH * zoom, road.angle);

        if (closurePending)
            SDL_SetTextureColorMod(currentRoadTex, 255, 255, 255);
    }

    if (heatMapEnabled_)
        renderTrafficHeatMapRoads(window, camera);

    // Intersection
    for (const auto& intersection : intersectionsLocation_) {
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

void VisualizationEngine::renderTrafficHeatMapRoads(
    RenderWindow& window,
    const Camera& camera) const
{
    if (heatMapOverlayTexture_ == nullptr)
        return;

    SDL_Renderer* renderer = window.getRenderer();
    const SDL_Rect viewport{
        Config::PANEL_WIDTH,
        0,
        Config::VIEW_PORT_WIDTH,
        Config::VIEW_PORT_HEIGHT};
    SDL_RenderSetClipRect(renderer, &viewport);

    const float zoom = camera.getZoom();
    const float overlayWidth = std::max(
        3.0f,
        Config::ROAD_WIDTH * zoom * 0.72f);
    SDL_SetTextureAlphaMod(heatMapOverlayTexture_, HEAT_MAP_ROAD_ALPHA);

    for (const auto& road : roadsLocation_)
    {
        // Closed/VIP roads retain their dedicated striped texture so users do
        // not mistake an unavailable route for ordinary heavy traffic.
        // if (!road.road || road.road->isVIPExclusive()) // Legacy: active-only closure.
        if (!road.road || road.road->isUnavailableForRouting())
            continue;

        const SDL_Color color = TrafficHeatMapScale::colorFor(
            road.road->getCongestionLevel());
        SDL_SetTextureColorMod(
            heatMapOverlayTexture_, color.r, color.g, color.b);

        const Vector2 shiftedStart(
            road.start.x + road.offsetX,
            road.start.y + road.offsetY);
        Vector2 screenStart = applyCamera(shiftedStart, camera);
        screenStart.x += Config::PANEL_WIDTH;

        renderSolidRoadOverlay(
            renderer,
            heatMapOverlayTexture_,
            screenStart,
            road.length * zoom,
            overlayWidth,
            road.angle);
    }

    SDL_SetTextureColorMod(heatMapOverlayTexture_, 255, 255, 255);
    SDL_SetTextureAlphaMod(heatMapOverlayTexture_, 255);
    SDL_RenderSetClipRect(renderer, nullptr);
}

bool VisualizationEngine::handleTrafficHeatMapEvent(const SDL_Event& event)
{
    if (event.type == SDL_KEYDOWN &&
        event.key.repeat == 0 &&
        event.key.keysym.sym == SDLK_h)
    {
        heatMapEnabled_ = !heatMapEnabled_;
        return true;
    }

    if (event.type == SDL_MOUSEBUTTONDOWN &&
        event.button.button == SDL_BUTTON_LEFT)
    {
        const SDL_Point point{event.button.x, event.button.y};
        if (SDL_PointInRect(&point, &HEAT_MAP_BUTTON_RECT) == SDL_TRUE)
        {
            heatMapButtonPressed_ = true;
            return true;
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP &&
        event.button.button == SDL_BUTTON_LEFT &&
        heatMapButtonPressed_)
    {
        const SDL_Point point{event.button.x, event.button.y};
        if (SDL_PointInRect(&point, &HEAT_MAP_BUTTON_RECT) == SDL_TRUE)
            heatMapEnabled_ = !heatMapEnabled_;

        heatMapButtonPressed_ = false;
        return true;
    }

    return false;
}

void VisualizationEngine::renderTrafficHeatMapUi(RenderWindow& window) const
{
    SDL_Renderer* renderer = window.getRenderer();
    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    const SDL_Point mousePoint{mouseX, mouseY};
    const bool isHovered =
        SDL_PointInRect(&mousePoint, &HEAT_MAP_BUTTON_RECT) == SDL_TRUE;

    const SDL_Color buttonBackground = heatMapEnabled_
        ? SDL_Color{18, 65, 69, 238}
        : (isHovered
            ? SDL_Color{28, 58, 70, 238}
            : SDL_Color{18, 42, 53, 232});
    const SDL_Color buttonBorder = heatMapEnabled_
        ? SDL_Color{45, 212, 191, 255}
        : (isHovered
            ? SDL_Color{111, 147, 158, 255}
            : SDL_Color{70, 101, 111, 235});
    renderFloatingPanel(
        renderer,
        HEAT_MAP_BUTTON_RECT,
        buttonBackground,
        buttonBorder);

    // Three bars make the control recognizable even before reading its label.
    const SDL_Color scaleColors[] = {
        TrafficHeatMapScale::colorFor(0),
        TrafficHeatMapScale::colorFor(50),
        TrafficHeatMapScale::colorFor(100)};
    for (int i = 0; i < 3; ++i)
    {
        SDL_SetRenderDrawColor(
            renderer,
            scaleColors[i].r,
            scaleColors[i].g,
            scaleColors[i].b,
            255);
        const SDL_Rect bar{
            HEAT_MAP_BUTTON_RECT.x + 13 + i * 5,
            HEAT_MAP_BUTTON_RECT.y + 13 - i * 2,
            3,
            15 + i * 2};
        SDL_RenderFillRect(renderer, &bar);
    }

    renderTextureAt(
        renderer,
        heatMapButtonLabelTexture_,
        HEAT_MAP_BUTTON_RECT.x + 37,
        HEAT_MAP_BUTTON_RECT.y + 13);

    const SDL_Rect switchTrack{
        HEAT_MAP_BUTTON_RECT.x + HEAT_MAP_BUTTON_RECT.w - 33,
        HEAT_MAP_BUTTON_RECT.y + 14,
        22,
        14};
    SDL_SetRenderDrawColor(
        renderer,
        heatMapEnabled_ ? 19 : 72,
        heatMapEnabled_ ? 112 : 91,
        heatMapEnabled_ ? 102 : 98,
        255);
    SDL_RenderFillRect(renderer, &switchTrack);
    SDL_SetRenderDrawColor(renderer, 238, 246, 248, 255);
    fillCircle(
        renderer,
        heatMapEnabled_ ? switchTrack.x + 15 : switchTrack.x + 7,
        switchTrack.y + 7,
        5);

    if (!heatMapEnabled_)
        return;

    renderFloatingPanel(
        renderer,
        HEAT_MAP_LEGEND_RECT,
        SDL_Color{12, 34, 43, 226},
        SDL_Color{63, 91, 100, 225});
    renderTextureAt(
        renderer,
        heatMapLegendTitleTexture_,
        HEAT_MAP_LEGEND_RECT.x + 14,
        HEAT_MAP_LEGEND_RECT.y + 11);

    SDL_SetRenderDrawColor(renderer, 70, 101, 111, 180);
    SDL_RenderDrawLine(
        renderer,
        HEAT_MAP_LEGEND_RECT.x + 14,
        HEAT_MAP_LEGEND_RECT.y + 35,
        HEAT_MAP_LEGEND_RECT.x + HEAT_MAP_LEGEND_RECT.w - 14,
        HEAT_MAP_LEGEND_RECT.y + 35);

    struct LegendRow
    {
        SDL_Color color;
        SDL_Texture* label;
    };
    const LegendRow rows[] = {
        {TrafficHeatMapScale::colorFor(0), heatMapLowLabelTexture_},
        {TrafficHeatMapScale::colorFor(50), heatMapModerateLabelTexture_},
        {TrafficHeatMapScale::colorFor(100), heatMapHeavyLabelTexture_}};

    for (int i = 0; i < 3; ++i)
    {
        const int rowY = HEAT_MAP_LEGEND_RECT.y + 53 + i * 23;
        SDL_SetRenderDrawColor(
            renderer,
            rows[i].color.r,
            rows[i].color.g,
            rows[i].color.b,
            255);
        fillCircle(renderer, HEAT_MAP_LEGEND_RECT.x + 20, rowY + 6, 5);
        renderTextureAt(
            renderer,
            rows[i].label,
            HEAT_MAP_LEGEND_RECT.x + 34,
            rowY);
    }
}

bool VisualizationEngine::isTrafficHeatMapEnabled() const noexcept
{
    return heatMapEnabled_;
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

    window.cleanUpTexture(heatMapOverlayTexture_);
    window.cleanUpTexture(heatMapButtonLabelTexture_);
    window.cleanUpTexture(heatMapLegendTitleTexture_);
    window.cleanUpTexture(heatMapLowLabelTexture_);
    window.cleanUpTexture(heatMapModerateLabelTexture_);
    window.cleanUpTexture(heatMapHeavyLabelTexture_);

    if (heatMapFont_ != nullptr)
    {
        TTF_CloseFont(heatMapFont_);
        heatMapFont_ = nullptr;
    }
}
