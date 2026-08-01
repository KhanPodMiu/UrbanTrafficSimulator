#pragma once

#include "SDL2/SDL.h"
#include "utils/event_handling.hpp"
#include "simulation/BannedRouteManager.hpp"
#include "utils/vector2i.hpp"

class Graph;
class VehicleManager;

class InputHandler
{
public:
    InputHandler() = default;
    ~InputHandler() = default;

    void handleInput(SDL_Event& event, AppContext& Game);

    bool wasDragAction() const noexcept { return mouseMoved_; }

    void spawnVehicleAt(const Vector2& clickPos, Graph& graph, VehicleManager& vehicleManager);

    bool switchBannedRoute(int routeID, AppContext& Game);

    BannedRouteManager& getBannedRouteManager() noexcept { return bannedRouteManager_; }
    const BannedRouteManager& getBannedRouteManager() const noexcept { return bannedRouteManager_; }

private:
    static constexpr int DRAG_THRESHOLD = 5;

    bool isDragging_ = false;
    int lastMouseX_ = 0;
    int lastMouseY_ = 0;
    int dragStartX_ = 0;
    int dragStartY_ = 0;
    bool mouseMoved_ = false;

    BannedRouteManager bannedRouteManager_;
};
